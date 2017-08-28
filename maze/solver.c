/* CS033 HW 02 - Maze Solver
 
Solves a maze. A path from the starting coordinate to the ending coordinate is determined
using depth first search. FULL or PRUNED Output is written to a file. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "common.h"

/* A linked list that internally stores the row and column of rooms */
struct linkedlist {
    int row, column;
    struct linkedlist *next;
};

/* Traverses and prints the linked list to a file */
void printLL(struct linkedlist *alos, FILE *fileName);

/* Solves a maze from input file for the requested coordinates and outputs to file */
void solveMaze(char *mazeFileName, char *outputFileName, int startColumn, int startRow, int endColumn, int endRow);

/* Reads in a maze from a file */
int readMaze(struct room maze [MAZE_ROWS][MAZE_COLUMNS], char *fileName);

/* Performs depth first search and outputs to a file with FULL output */
int fullDFS(int row, int column, struct room maze [MAZE_ROWS][MAZE_COLUMNS], int targetRow, int targetCol, struct linkedlist *last);

/* Performs depth first search and outputs to a file with PRUNED output */
int prunedDFS(int row, int column, struct room maze [MAZE_ROWS][MAZE_COLUMNS], int targetRow, int targetCol, struct linkedlist *previous);

/* Determines whether a room has an open connection in a given direction */
int roomHasOpenConnection(struct room r, int direction);

/* Creates and initializes a room from hexValue at a given location */
struct room createRoomFromHexValue(unsigned int hexValue, int row, int column);

/* Function main

   This function is where the program begins. Calls solveMaze to solve the maze or outputs
   an error message if parameters are invalid.

   Input: int argc - The number of program arguments, including the executable name
          char **argv - An array of strings containing the program arguments
 
   Output: 0 upon completion of the program
 */
int main(int argc, char **argv) {
    if(argc < 7)
    {
        printf("Usage: %s <input maze file> <number of rows> <number of columns> <output solution file> <starting row> <starting column> <ending row> <ending column>\n", argv[0]);
        exit(0);
    }
    
    char *inputFile = strdup(argv[1]);
    char *outputFile = strdup(argv[4]);
    MAZE_ROWS = atoi(argv[2]);
    MAZE_COLUMNS = atoi(argv[3]);
    int startingRow = atoi(argv[5]);
    int startingColumn = atoi(argv[6]);
    int endingRow = atoi(argv[7]);
    int endingColumn = atoi(argv[8]);
    
    if(inputFile == NULL || outputFile == NULL)
    {
        fprintf(stderr,"Input/Output files must be provided\n");
        exit(0);
    }
    if(MAZE_ROWS == 0 || MAZE_COLUMNS == 0)
    {
        fprintf(stderr, "Maze Rows/Columns must be non-zero\n");
        exit(0);
    }
    if(startingRow < 0 || startingColumn < 0 || startingRow > endingRow || startingColumn > endingColumn)
    {
        fprintf(stderr, "Error with strting row/column\n");
        exit(0);
    }
    if(endingRow <= 0 || endingColumn <=0)
    {
        fprintf(stderr, "Error with ending row/column\n");
        exit(0);
    }
    
    #ifdef DEBUG
        printf("main: starting row = %d, sc = %d, er = %d ec = %d\n", startingRow, startingColumn, endingRow, endingColumn);
    #endif
    solveMaze(inputFile, outputFile, startingColumn, startingRow, endingColumn, endingRow);
    
    return 0;
}

/* Function solveMaze

   This function solves the maze by reading in a maze and then determining and print the FULL path or
   PRUNED path.

   Input: *mazeFileName - file name of maze to read in
          *outputFileName - file name of where to write maze solution
          startColumn, startRow - coordinates of room location to start solving path from
          endColumn, endRow - coordinates of room location to solve path to

   Output: Void
 */
void solveMaze(char *mazeFileName, char *outputFileName, int startColumn, int startRow, int endColumn, int endRow) {
    /* reads a maze into memory and returns true if no error reading file */
    #ifdef DEBUG
        printf("solveMaze: inside solveMaze\n");
    #endif
    struct room maze[endRow][endColumn];
        /* outputs PRUNED or FULL solution */
    if(readMaze(maze, mazeFileName) == 0)
    {
        fprintf(stderr, "Error reading maze\n");
        exit(0);
    }
    
    struct linkedlist *solution = (struct linkedlist*)malloc(sizeof(struct linkedlist)); 
    #ifdef DEBUG
        printf("solveMaze: Starting DFS with paramters: Maze File = %s Output File = %s Start Column=%d Start Row = %d End Column = %d End Row = %d\n", mazeFileName, outputFileName, startColumn, startRow, endColumn, endRow);
    #endif
    #ifdef FULL
        fullDFS(startRow, startColumn, maze, endRow, endColumn, solution);
    #else
        prunedDFS(startRow, startColumn, maze, endRow, endColumn, solution);
    #endif
    FILE *fp = fopen(outputFileName, "w+");
    assert(fp != NULL);
    #ifdef DEBUG
        printf("solveMaze: starting printLL\n");
    #endif  
    printLL(solution, fp);
    fclose(fp);
    #ifdef DEBUG
        printf("solveMaze: End solveMaze\n");
    #endif
}

/* Function roomHasOpenConnection

   Determines whether a room has open connection in direction

   Input: r - a room to evaluate
          direction - the direction of the connection
 
   Output: Int 0 if room does not have open connection in direction and 1 if it does
*/
int roomHasOpenConnection(struct room r, int direction) {
    switch(direction)
    {
        case EAST:
            #ifdef DEBUG
                printf("roomHasOpenConnection: r=%d, c=%d, east = %d\n", r.row, r.column, r.east);
            #endif
            return (r.east == OPENING ? 1 : 0);
        case WEST:
            return (r.west == OPENING ? 1 : 0);
        case SOUTH:
            return (r.south == OPENING ? 1 : 0);
        default:
            return (r.north == OPENING ? 1 : 0);
    }
}

/* Function createRoomFromHexValue

   Creates a room with connections initialized based on hex value

   Input: hexValue - unsigned int that stores sum of room locations
          row, column - the coordinates of the room to be initialized

   Output: A room with directions read in from hex sum and the given coordinates
*/
struct room createRoomFromHexValue(unsigned int hexValue, int row, int column) {
    struct room rm;
    
    rm.row = row;
    rm.column = column;
    rm.visited = 0;
    rm.east = OPENING;
    rm.west = OPENING;
    rm.south = OPENING;
    rm.north = OPENING;
    
    if((hexValue & EASTHEX) == EASTHEX)
        rm.east = WALL;
    if((hexValue & WESTHEX) == WESTHEX)
        rm.west = WALL;
    if((hexValue & SOUTHHEX) == SOUTHHEX)
        rm.south = WALL;
    if((hexValue & NORTHHEX) == NORTHHEX)
        rm.north = WALL;
    
    //temp debug code
    //if(row == 0 && column == 0)
        printf("createRoomFromHex: row = %d, column = %d, HexValue = %x, east = %d\n", row, column, hexValue, rm.east);
    
    return rm;
}

/* Function readMaze

   Reads in a maze from an input file and stores it internally in the room maze

   Input: maze[][] - internal representation of all the rooms in maze
          fileName - the file name of the maze to read in

   Output: 0 if failed to read maze properly and 1 if maze was read properly
*/
int readMaze(struct room maze [MAZE_ROWS][MAZE_COLUMNS], char *fileName) {
    FILE *fp = fopen(fileName, "r");
    if(fp == NULL)
        return 0;
    unsigned int hexValue;
    int r,c;
    r = 0;
    c = 0;
    while(fscanf(fp,"%1x", &hexValue) &&  r < MAZE_ROWS)
    {
        #ifdef DEBUG
            printf("readMaze: hexValue = %x R = %d, C = %d\n", hexValue, r, c);
        #endif
        maze[r][c] = createRoomFromHexValue(hexValue, r, c);
        if(c == MAZE_COLUMNS - 1)
        {
            c = 0;
            r++;
        }
        else
        {
            c++;
        }
    }
    fclose(fp);
    return 1;
}

/* Function printLL

   Prints linkedlist to file

   Input: *p - pointer to a linkedlist
          *filename - file of where to write contents of linkedlist

   Output: Void
*/
void printLL(struct linkedlist *p, FILE *fileName) {
    assert(fileName != NULL);
    assert(p != NULL);
    #ifdef FULL
        fprintf(fileName, "FULL\n");
    #else
        fprintf(fileName, "PRUNED\n");
    #endif
    
    struct linkedlist *temp;
    #ifdef DEBUG
        printf("printLL: starting loop\n");
    #endif  
    for(temp = p; temp != NULL;)
    {
        fprintf(fileName, "%d, %d\n", temp->row, temp->column);
        p = temp;
        temp = temp->next;
        free(p);
    }
    #ifdef DEBUG
        printf("printLL: end loop\n");
    #endif  
}

/* Function fullDFS

   Performs a depth first search and writes FULL output path

   Input: row, column - coordinates of current location
          targetRow, targetCol - coordinates of the goal
          maze[][] - internal representation of all the rooms in maze
          *fileName - file name of output file

   Output: 0 representing false and 1 representing true per the requirements of dfs
*/
int fullDFS(int row, int column, struct room maze [MAZE_ROWS][MAZE_COLUMNS], int targetRow, int targetCol, struct linkedlist *last){
    if(row == (targetRow-1) && column == (targetCol-1))
    {
        struct linkedlist *temp = (struct linkedlist *)malloc(sizeof(struct linkedlist));
        temp->row = row;
        temp->column = column;
        temp->next = NULL;
        last->next = temp;
        return 1;
    }

    maze[row][column].visited = 1;
    
    last->row = row;
    last->column = column;
    
    int d = 0;
    struct room *neighbor;
    int tempR, tempC;
    for(d = 0; d < 4; d++)
    {
        //Checking to see if maze has opening at direction D. roomHasOpenConnection returns 1 for open connection
        neighbor = &maze[row][column];
        #ifdef DEBUG
            printf("fullDFS: roomHasOpenConnnection r=%d, c=%d, direction = %d, result=%d\n", row, column, d, roomHasOpenConnection(*neighbor, d));
        #endif
        if(roomHasOpenConnection(*neighbor, d))
        {
            tempR = row + SouthNorthOffset[d];
            tempC = column + EastWestOffset[d];
            neighbor = &maze[tempR][tempC];
            #ifdef DEBUG
                printf("fullDFS: roomHasOpenConnection (Neighbor): row=%d, col = %d, Direction = %d, result = %d, visited = %d\n", tempR, tempC, d,roomHasOpenConnection(*neighbor, d),     (*neighbor).visited);
            #endif
            if(roomOutOfBounds(tempR, tempC, targetCol, targetRow) == 0 &&(*neighbor).visited == 0) // && roomHasOpenConnection(*neighbor, d) == 1)
            {
                struct linkedlist *temp = (struct linkedlist *)malloc(sizeof(struct linkedlist));
                temp->row = tempR;
                temp->column = tempC;
                #ifdef DEBUG
                    printf("fullDFS: calling pruneDFS R=%d C=%d targetR=%d targetC=%d\n", (*neighbor).row, neighbor->column, targetRow, targetCol);
                #endif          
                if(fullDFS((*neighbor).row, (*neighbor).column, maze, targetRow, targetCol, temp) == 1)
                {
                    #ifdef DEBUG
                        printf("fullDFS: linking list r=%d c=%d targetR=%d targetC=%d\n", neighbor->row, neighbor->column, targetRow, targetCol);
                    #endif              
                    last->next = temp;
                    return 1;
                }
                else
                {
                    last->next = temp;
                }
            }
        }
    }
    #ifdef DEBUG
        printf("fullDFS: end pruneDFS R=%d C=%d targetR=%d targetC=%d\n", row, column, targetRow, targetCol);
    #endif
    
    return 0;

}


/* Function prunedDFS

   Performs a depth first search and writes PRUNED output path

   Input: row, column - coordinates of current location
          targetRow, targetCol - coordinates of the goal
          maze[][] - internal representation of all the rooms in maze
          *last - pointer to end of linked list that we can accumulate from for pruned lists

   Output: 0 representing false and 1 representing true per the requirements of dfs
*/
int prunedDFS(int row, int column, struct room maze [MAZE_ROWS][MAZE_COLUMNS], int targetRow, int targetCol, struct linkedlist *last) {
    if(row == (targetRow-1) && column == (targetCol-1))
    {
        struct linkedlist *temp = (struct linkedlist *)malloc(sizeof(struct linkedlist));
        temp->row = row;
        temp->column = column;
        temp->next = NULL;
        last->next = temp;
        return 1;
    }

    maze[row][column].visited = 1;
    
    last->row = row;
    last->column = column;
    
    int d = 0;
    struct room *neighbor;
    int tempR, tempC;
    for(d = 0; d < 4; d++)
    {
        //Checking to see if maze has opening at direction D. roomHasOpenConnection returns 1 for open connection
        neighbor = &maze[row][column];
        #ifdef DEBUG
            printf("pruneDFS: roomHasOpenConnnection r=%d, c=%d, direction = %d, result=%d\n", row, column, d, roomHasOpenConnection(*neighbor, d));
        #endif
        if(roomHasOpenConnection(*neighbor, d))
        {
            tempR = row + SouthNorthOffset[d];
            tempC = column + EastWestOffset[d];
            neighbor = &maze[tempR][tempC];
            #ifdef DEBUG
                printf("pruneDFS: roomHasOpenConnection (Neighbor): row=%d, col = %d, Direction = %d, result = %d, visited = %d\n", tempR, tempC, d,roomHasOpenConnection(*neighbor, d),    (*neighbor).visited);
            #endif
            if(roomOutOfBounds(tempR, tempC, targetCol, targetRow) == 0 &&(*neighbor).visited == 0) // && roomHasOpenConnection(*neighbor, d) == 1)
            {
                struct linkedlist *temp = (struct linkedlist *)malloc(sizeof(struct linkedlist));
                temp->row = tempR;
                temp->column = tempC;
                #ifdef DEBUG
                    printf("prunedDFS: calling pruneDFS R=%d C=%d targetR=%d targetC=%d\n", (*neighbor).row, neighbor->column, targetRow, targetCol);
                #endif          
                if(prunedDFS((*neighbor).row, (*neighbor).column, maze, targetRow, targetCol, temp) == 1)
                {
                    #ifdef DEBUG
                        printf("prunedDFS: linking list r=%d c=%d targetR=%d targetC=%d\n", neighbor->row, neighbor->column, targetRow, targetCol);
                    #endif              
                    last->next = temp;
                    return 1;
                }
                else
                {
                    #ifdef DEBUG
                        printf("pruneDFS: freeing memory\n");
                    #endif
                    free(temp);
                }
            }
        }
    }
    #ifdef DEBUG
        printf("prunedDFS: end pruneDFS R=%d C=%d targetR=%d targetC=%d\n", row, column, targetRow, targetCol);
    #endif
    
    return 0;
}
