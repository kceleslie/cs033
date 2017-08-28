/* Function roomOutOfBounds

   Determines whether a location is out of bounds of the maze 

   Input: row - the row position
          column - the column position
          cols - the number of columns in maze
          rows - the number of rows in maze
 
   Output: Int 0 if location is in bounds and 1 if location is out of bounds of maze 
*/
int roomOutOfBounds(int row, int column, int cols, int rows) {
    if((row >= 0 && row < rows) && (column >= 0 && column < cols))
        return 0;
    return 1;
}
