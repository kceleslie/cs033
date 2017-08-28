/* CS033 HW 01 - Maze Generator
 
 Generates a maze. Each room is represented in hexadecimal form per the
 specifications of the assignment. Output is written to a file. 
 https://github.com/eheimark/cs33
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "common.h"

/* Directions of maze */
#define EAST 0
#define WEST 1
#define SOUTH 2
#define NORTH 3

/* Connections of a room */
#define UNINITIALIZED -1
#define OPENING 0
#define WALL 1

/* Generates a maze, writing maze to a file with given fileName */
void generateMaze(char *fileName);

/* Calculates the hexadecimal value for a given room */
int calculateHexValue(struct room r);

/* Stores a connection of type connectionType for a room in a given direction */
void storeRoomConnection(struct room *r, int connectionType, int direction);

/* Determines whether a room has an initialized connection in a given direction */
int roomHasConnection(struct room r, int direction);

/* Determines the type of connection a room has in a given direction */
int connectionType(struct room r, int direction);

/* Creates a room a row and column and uninitialized connections */
struct room createRoom(int row, int column);

/* Creates a maze of the correct specifications with uninitialized connections */
void createMaze(int rows, int columns, struct room maze [MAZE_ROWS][MAZE_COLUMNS]);

/* Performs drunken walk algorithm on a maze, setting connection borders */
void drunkenWalkAlgorithm (int row, int column, struct room maze [MAZE_ROWS][MAZE_COLUMNS]);

/* Prints maze in hexadecimal form to an output file */
void printMaze(int rows, int columns, struct room maze [MAZE_ROWS][MAZE_COLUMNS], char *fileName);

/* Shuffles an array of directions */
void shuffleDirections(int arr[4]);

/* Outputs reverse direction */
int findOppositeDirection(int a);

/* Function main

   This function is where the program begins. Calls generateMaze to generate a hexadecimal
   maze.

   Input: int    argc - The number of program arguments, including the executable name
          char **argv - An array of strings containing the program arguments
 
   Output: 0 upon completion of the program
 */
int main(int argc, char **argv) {
    char *fileName = NULL;
    if(argc >= 4)
    {
        fileName = strdup(argv[1]);
        MAZE_ROWS = atoi(argv[2]);
        MAZE_COLUMNS = atoi(argv[3]);
    }
    else
    {
        fprintf(stderr,"Usage %s <fileName> <Rows> <Columns>\n", argv[0]);
        exit(0);
    }
    if(fileName == NULL || MAZE_ROWS == 0 || MAZE_COLUMNS == 0)
        return 0;
    srand(time(NULL));          //seed for random number generator
    generateMaze(fileName);
    return 0;
}

/* Function generateMaze

   This function is where the program begins. Initializes a maze, performs drunken walk 
   algorithm, and then prints the maze.

   Input: char *fileName- The fileName of where to print the maze
 
   Output: Void
 */
void generateMaze(char *fileName) {
    struct room maze[MAZE_ROWS][MAZE_COLUMNS];
    createMaze(MAZE_ROWS, MAZE_COLUMNS, maze);
    drunkenWalkAlgorithm(0, 0, maze);
    printMaze(MAZE_ROWS, MAZE_COLUMNS, maze, fileName);
}

/* Function calculateHexValue

   This function outputs the integer representation of a room

   Input: r - A room
 
   Output: The integer (decimal) conversion of a room
 */
int calculateHexValue(struct room r) {
    int hexValue = 0;
    
    if(r.east)
        hexValue |=8;
    if(r.west)
        hexValue |=4;
    if(r.south)
        hexValue |=2;
    if(r.north)
        hexValue |=1;
    return hexValue;
}

/* Function storeRoomConnection

   Stores a connection in a given room.

   Input: r - the room to modify the connection in
          connectionType - the type of connection (-1 if uninitialized, 0 if opening, and 1 if wall)
          direction - the direction of the connection
 
   Output: Void. 
*/
void storeRoomConnection(struct room *r, int connectionType, int direction) {
    switch(direction)
    {
        case EAST:
            r->east = connectionType;
            break;
        case WEST:
            r->west = connectionType;
            break;
        case SOUTH:
            r->south = connectionType;
            break;
        case NORTH:
            r->north = connectionType;
            break;
    }
}

/* Function roomHasConnection

   Determines whether a room has a connection in a given direction

   Input: r - the room to check connection in
          direction - the direction of connection
 
   Output: Returns 1 if room has initialized connection and 0 if no connection 
*/
int roomHasConnection(struct room r, int direction) {
    switch(direction)
    {
        case EAST:
            return (r.east == 1 ? 1 : 0);
        case WEST:
            return (r.west == 1 ? 1 : 0);
        case SOUTH:
            return (r.south == 1 ? 1 : 0);
        case NORTH:
            return (r.north == 1 ? 1 : 0);
    }
    return -1;
}

/* Function connectionType

   Determines the type of connection a room has in a given direction

   Input: r - the room to check connection in
          direction - the direction of connection
 
   Output: Returns the integer representation of the connection in room r in direction
*/
int connectionType(struct room r, int direction) {
    int dir = -1;
    switch(direction)
    {
        case EAST:
            dir = r.east;
        case WEST:
            dir = r.west;
        case SOUTH:
            dir = r.south;
        case NORTH:
            dir = r.north;
    }
    return dir;
}

/* Function createRoom

   Creates a room with uninitialized connections

   Input: row - the row of the room
          column - the column of the room
 
   Output: A struct room storing (row, column) and uninitialized connections
*/
struct room createRoom(int row, int column) {
    /* the row and column of the room */
    struct room r;
    r.east = UNINITIALIZED;
    r.west = UNINITIALIZED;
    r.south = UNINITIALIZED;
    r.north = UNINITIALIZED;
    r.visited = 0;
    r.row = row;
    r.column = column;
    return r;
    /* whether or not the room has been visited, where 0 is unvisited, 1 visited */

    /* whether that connection is a wall, opening, or is uninitialized, where -1 is uninitialized, 0 opening, 1 wall */
}

/* Function createMaze

   Creates a maze with rooms with uninitialized connections

   Input: rows - the number of rows of maze
          columns - the number of columns of maze
          room maze [][] - the maze to create rooms in
 
   Output: Void
*/
void createMaze(int rows, int columns, struct room maze [MAZE_ROWS][MAZE_COLUMNS]) {
    int r,c;
    for(r = 0; r < rows; r++)
    {
        for(c = 0; c < columns; c++)
        {
            maze[r][c] = createRoom(r,c);
        }
    }
}

/* Function printMaze

   Prints maze to an output file.

   Input: rows - the number of rows of maze
          columns - the number of columns of maze
          room maze [][] - the maze
          *fileName - pointer to fileName of output file
 
   Output: Void
*/
void printMaze(int rows, int columns, struct room maze [MAZE_ROWS][MAZE_COLUMNS], char *fileName) {
    FILE *fp;       //Pointer to file pointer
    fp = fopen(fileName, "w+");
    assert(fp != NULL);
    int r,c;
    for(r = 0; r < rows; r++)
    {
        for(c = 0; c < columns; c++)
        {
            fprintf(fp, "%x", calculateHexValue(maze[r][c]));
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

/* Function shuffleDirections

   Shuffles array of directions by randomizing order

   Input: arr[4] - Array of north, south, east, west
 
   Output: Void
*/
void shuffleDirections(int arr[4]) { 
    int i = 0;
    int temp;
    int randomNumber;
    for(i = 0; i < 4; i++)
    {
        randomNumber = rand() % 4;
        temp = arr[randomNumber];
        arr[randomNumber] = arr[i];
        arr[i] = temp;
    }
}

/* Function findOppositeDirection

   Returns opposite direction.

   Input: dir - an integer representation of direction
 
   Output: The integer representation of the opposite direction
*/
int findOppositeDirection(int dir) {
    switch(dir)
    {
        case EAST:
            return WEST;
        case WEST:
            return EAST;
        case SOUTH:
            return NORTH;
        case NORTH:
            return SOUTH;
    }
    return -1;
}

/* Function drunkenWalkAlgorithm

   Performs the drunken walk algorithm on a maze.

   Input: row - integer row of current room
          column - integer column of current room
          struct room maze [][] - the maze to perform drunken walk on

   Output: Void
*/
void drunkenWalkAlgorithm(int row, int column, struct room maze [MAZE_ROWS][MAZE_COLUMNS]){
    struct room *r = &maze[row][column];
    struct room neighbor;
    r->visited = 1;

    /* create random order of directions */
    int direction[4] = {EAST, WEST, SOUTH, NORTH};
    shuffleDirections(direction);
    /* for each direction dir in random order: */
    int i = 0;
    int tempR = 0, tempC = 0;
    for(i = 0; i < 4; i++)
    {
        tempR = row + SouthNorthOffset[direction[i]];
        tempC = column + EastWestOffset[direction[i]];
        /* if neighbor is out of bounds */
        /* store a wall in r at direction dir */
        
        if(roomOutOfBounds(tempR, tempC, MAZE_ROWS, MAZE_COLUMNS))
        {
            storeRoomConnection(r, WALL, direction[i]);
        }
        else
        {       
            neighbor = maze[tempR][tempC];
            if(!neighbor.visited)
            {
                storeRoomConnection(r, OPENING, direction[i]);
                drunkenWalkAlgorithm(tempR, tempC, maze);
            }
            else
            {

                int opposite_Direction = findOppositeDirection(direction[i]);
                if(roomHasConnection(maze[tempR][tempC], opposite_Direction))
                {

                    storeRoomConnection(r, connectionType(maze[tempR][tempC], opposite_Direction), direction[i]);
                }   
                else
                {

                    storeRoomConnection(r, WALL, direction[i]);
                }
            }
        }

        /* else if neighbor has not yet been visited */
        /* store an opening in r at direction dir */
        /* drunken_walk(neighbor.x, neighbor.y) */
        /* oppositeDirection = the opposite direction of dir */
        /* if neighbor has a connection c (door or wall) in direction oppositeDirection: */
        /* store c in r at direction dir */
        /* store a wall in r at direction dir */
    }
}
