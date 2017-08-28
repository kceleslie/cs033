/* Directions of maze */
#define EAST 0
#define WEST 1
#define SOUTH 2
#define NORTH 3

/* Connections of a room */
#define UNINITIALIZED -1
#define OPENING 0
#define WALL 1

/* Room Hex values */
#define EASTHEX 8
#define WESTHEX 4
#define SOUTHHEX 2
#define NORTHHEX 1

int MAZE_ROWS = 0;
int MAZE_COLUMNS = 0;

/* Represents a room */
struct room {
    int row, column, visited, east, west, south, north;
};


/* Determines whether a given room is out of bounds */
int roomOutOfBounds(int, int column, int cols, int rows);

const int EastWestOffset[4] = {1,-1,0,0};
const int SouthNorthOffset[4] = {0,0,1,-1};
