/*
To compile paste following in terminal: 
g++ -std=c++11 mineseeker.cpp mineseeker_lib.cpp -o main
*/

#include <iostream>
#include "mineseeker.h"
#include "mineseeker_lib.h"


char * create_board(std::size_t x_dim, std::size_t y_dim){

/*
Allocate a 1-dimensional char array with x_dim * y_dim elements and initialize each element with zero.
*/

    char *array{new char [x_dim * y_dim] {}};

    for (int k{}; k <= x_dim*y_dim; k++){
        array[k] = 0;
    }

return array;
}


void clean_board(char * board){

/*
Deallocate the given board - clears that board from memory.
*/

    delete[] board;
    board = nullptr;

return;
}


void hide_board(char * board, std::size_t x_dim, std::size_t y_dim){

/*
Hide all the field values. The hide_board function is called after a board has been created, mines spread (by
mineseeker_lib.cpp), and neighbours calculated.
*/

    for (int k{}; k <= (x_dim*y_dim) - 1; k++){
        board[k] |= 0x20;  
    }

return;
}


void display6Bits(char c);
void display6Bits(char c) {

/*
Helper function that prints out binary values of every field in the board. Take out of multi-line comment in print_board
to use. 
*/

    std::cout << " ["; // leading space -helps readability
    for (char mask{0b100000}; mask > 0; mask = mask >> 1) {
        std::cout << ((mask & c) != 0); // 1 if non zero
    }
    std::cout << "] "; // trailing space -helps readability
}


void print_board(char * board, std::size_t x_dim, std::size_t y_dim){

/* display6Bits HELPER FUNCTION

    for (int k{}; k < y_dim; k++){ 
        for (int i{}; i < x_dim; i++){
        
            display6Bits(board[k*x_dim + i]);
        }      
    std::cout << std::endl;
    }
*/


/*
Prints the content of the board to std::cout. Each field is represented with a single character. If a field is
hidden this function prints an asterisk ‘*’, if a field is marked an ‘M’ is printed, and otherwise the field value
is displayed ('0' to '9').
*/

for (int k{}; k < y_dim; k++){ //to iterate through rows
    for (int i{}; i < x_dim; i++){ //to iterate through individual fields and print corresponding symbol
         
        if((board[(k*x_dim) + i] & marked_mask()) && (board[(k*x_dim) + i] & hidden_mask())){ //if hidden and marked 
            std::cout << 'M'; 
        }
        else if(board[(k*x_dim) + i] & hidden_mask()){ //if just hidden
            std::cout << '*';
        }
        else{ //if neither marked nor hidden, print value of field
            std::cout << static_cast <int> (board[(k*x_dim) + i]);
        }

    }
    std::cout << std::endl;
}

return;
}


int mark(char * board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc){

/*
This function is called when the player attempts to mark a field. If the field is already revealed, its state
should not be changed and the function returns the value 2. Otherwise, the Marked bit for the field is
toggled and the function returns 0. If the Marked bit is on, then this function turns it off. If the Marked bit
is off, then this function turns it on.
*/

if (board[(x_dim * y_loc) + x_loc] & hidden_mask() ){ //if field is already revealed, don't do anything and reutrn 2;
     
    if (board[(x_dim * y_loc) + x_loc] & marked_mask()){ //if marked bit is 1
    
        board[(x_dim * y_loc) + x_loc] &= ~marked_mask(); // change to 0
        return 0;
    }
    else { // if marked bit is 0
        
        board[(x_dim * y_loc) + x_loc] |= marked_mask(); // change to 1
        return 0;
    }
}

else { // if field is hidden, check if marked or unmarked and toggle to opposite bit. 
return 2;
}

return 0;
}


void compute_neighbours(char * board, std::size_t x_dim, std::size_t y_dim){

/*
This function receives a board array with all elements set to the values 0 or 9. The function updates the
board array such that all fields that do not have a mine (value 9) have their value set to the number of
mines adjacent to that field.
*/

for (int k{}; k < y_dim; k++){ // iterate through board
for (int i{}; i < x_dim; i++){ // iterate through board


        if ((board[(k*x_dim) + i] & 0x0F) != 0x09){ // if come across that does not have a mine, compute amount of neighbouring mines

            for (int row{-1}; row <= 1; row++){ // iterate through neighbour fields
            for (int col{-1}; col <= 1; col++){ // iterate through neighbour fields

                    int neighbour_i = k + row; // initialize row and col position of neighbouring fields based on current field iteration
                    int neighbour_k = i + col;

                    if (neighbour_i >= 0 && neighbour_i < y_dim && neighbour_k >= 0 && neighbour_k < x_dim) { // if within bounds and contains a mine, incremett field value by 1
                        if (board[(neighbour_i*x_dim) + neighbour_k] == 0x09) {
                            board[(k*x_dim) + i] += 0x01;
                        }
                    }
            }
            }
        }

        else if((board[(k*x_dim) + i] & 0x0F) == 0x09){ // if come across field that contains mines, skip to next field in board
        continue;
        }
}
}

return;
}


bool is_game_won(char * board, std::size_t x_dim, std::size_t y_dim){

/*
Determine whether the board is in a won state. The game is won when all fields that do not have a mine
have been revealed. Returns true if the game is won and false otherwise. 
*/

for (int k{}; k < y_dim; k++){
for (int i{}; i < x_dim; i++){
        
        if((board[(k*x_dim) + i] & 0x0F) != 0x09 && (((board[(k*x_dim) + i]) & hidden_mask()))){ // if a field does not contain a mine AND is hidden
            
            return false; 
        }

}
}

return true;
}


int reveal(char * board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc){

/*
This function reveals a field if it is hidden and unmarked. Revealing a field means
to set the hiddenBit to zero for the specific field. If the field is marked, the function returns the value 1
without modifying the field. If the field is already revealed, the function returns the value 2 without modifying the field. 
If there is a mine in the field the function reveals the field and then returns the value 9. For all other cases, the 
function returns the value 0. If an empty field is revealed (it contains the value zero), also reveal the values of all 
unmarked neighboring fields.
*/

if ((board[(y_loc * x_dim) + x_loc]) & marked_mask()){ // if board is marked
    return 1;
}
else if ((board[(y_loc * x_dim) + x_loc] & hidden_mask()) == 0x00){ // or if board is already revealed
    return 2; 
}

if ((board[(y_loc * x_dim) + x_loc] & value_mask()) == 0x09){ // field contains a mine, meaning 0x09
    board[(y_loc * x_dim) + x_loc] ^= hidden_mask(); // reveal mine, meaning reveal number 9
    return 0x09;
}

if ((board[(y_loc * x_dim) + x_loc] & value_mask()) == 0x00){ // if field contains value of zero

    for (int k{-1}; k <= 1; k++){ // iterate through neighbour fields
    for (int i{-1}; i <= 1; i++){ // iterate through neighbour fields
            
            int neighbour_i = x_loc + k;  // initialize row and col position of neighbouring fields based on current field iteration
            int neighbour_k = y_loc + i;

            if (neighbour_i >= 0 && neighbour_i < y_dim && neighbour_k >= 0 && neighbour_k < x_dim) { // if within bounds, reveal neighbouring field value
                board[(neighbour_k*x_dim) + neighbour_i] &= value_mask();
            }

    }
    }

return 0;
}

else{ // for every other number, print the number

    board[(y_loc * x_dim) + x_loc] &= value_mask();

return 0;
}

return 0;
}