#ifndef mineseeker_LIB_H
#define mineseeker_LIB_H

#include <iostream>

int main();
bool game();
void start_game(char * & board, std::size_t & x_dim, std::size_t & y_dim, unsigned int & num_mines);
char get_action();
void action_show(char * & board, std::size_t & x_dim, std::size_t & y_dim, unsigned int & num_mines);
void action_mark(char * board, std::size_t x_dim, std::size_t y_dim);
std::size_t x_dim_max();
std::size_t y_dim_max();
char marked_mask();
char hidden_mask();
char value_mask();
void spread_mines(char * board, std::size_t x_dim, std::size_t y_dim, unsigned int num_mines);

#endif
