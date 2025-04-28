#include <iostream>
#include <cctype>
#include <ctime>   // for time(NULL)
#include "mineseeker_lib.h"
#include "mineseeker.h"



int main()
{
 

  srand(time(NULL));
  game();
  return 0;
}
// main game function
bool game()
{
  std::size_t x_dim {0};
  std::size_t y_dim {0};

  unsigned int num_mines {0};
  char * game_Board {NULL};

  start_game(game_Board, x_dim, y_dim, num_mines);

  char curr_action {0};
  while(curr_action != 'Q')
  {
    switch (curr_action)
    {
      case 'S' :  // show
      {
        action_show(game_Board, x_dim, y_dim, num_mines);
        break;
      }
      case 'M' :  // mark
      {
        action_mark(game_Board, x_dim, y_dim);
        break;
      }
      case 'R' :  // restart
      {
        std::cout << "Restarting the game." << std::endl;
        start_game(game_Board, x_dim, y_dim, num_mines);
        break;
      }
    }
    print_board(game_Board, x_dim, y_dim);

    if (is_game_won(game_Board, x_dim, y_dim))
    {
      std::cout << "You have revealed all the fields without stepping on a mine!" << std::endl;
      std::cout << "YOU WON!!!" << std::endl;

      for (std::size_t reveal_row {0}; reveal_row < y_dim; reveal_row++)
      {
        for (std::size_t reveal_col {0}; reveal_col < x_dim; reveal_col++)
        {
          game_Board[reveal_row*x_dim + reveal_col] = (game_Board[reveal_row*x_dim + reveal_col] & value_mask());
        }
      }

      print_board(game_Board, x_dim, y_dim);
      std::cout << "Resetting the game board." << std::endl;
      start_game(game_Board, x_dim, y_dim, num_mines);
      print_board(game_Board, x_dim, y_dim);
    }

    curr_action = get_action();
  }

  clean_board(game_Board);
  return true;
}

// gets initial conditions of the board
void start_game(char * & board, std::size_t & x_dim, std::size_t & y_dim, unsigned int & num_mines)
{
  std::cout << "Welcome to Mineseeker!" << std::endl;
  do {
    std::cout << "Please enter the desired x dimension of the playing board (max " << x_dim_max() << "): ";
    std::cin >> x_dim;
  } while(x_dim < 1 || x_dim > x_dim_max());

  do {
    std::cout << "Please enter the desired y dimension of the playing board (max " << y_dim_max() << "): ";
    std::cin >> y_dim;
  } while(y_dim < 1 || y_dim > y_dim_max());

  std::cout << "Please enter the desired number of mines: ";
  std::cin >> num_mines;
  while (num_mines > x_dim * y_dim)
  {
    std::cout << "That's too many mines!" << std::endl;
    std::cout << "Please enter the number of mines: ";
    std::cin >> num_mines;
  }

  //calls functions to create the board
  clean_board(board);
  board = create_board(x_dim, y_dim);
  spread_mines(board, x_dim, y_dim, num_mines);
  compute_neighbours(board, x_dim, y_dim);
  hide_board(board, x_dim, y_dim);

  return;
}

char get_action()
{
  char action {0};

  std::cout << "Please enter your desired action ([S]how, [M]ark, [R]estart, [Q]uit): ";
  std::cin >> action;

  if (islower(action))
    action = toupper(action);

  return action;
}

void action_show(char * & board, std::size_t & x_dim, std::size_t & y_dim, unsigned int & num_mines)
{
  std::size_t x_reveal {0};
  std::size_t y_reveal {0};
  std::cout << "Please enter the x location to reveal: ";
  std::cin >> x_reveal;
  std::cout << "Please enter the y location to reveal: ";
  std::cin >> y_reveal;

  if (x_reveal >= x_dim || y_reveal >= y_dim)
  {
    std::cout << "The entered location is not on the board." << std::endl;
  }
  else if (board[x_dim*y_reveal + x_reveal] & marked_mask())
  {
    std::cout << "This location is marked, and therefore cannot be revealed." << std::endl;
    std::cout << "Use [M]ark on this location to unmark." << std::endl;
  }
  else if (reveal(board, x_dim, y_dim, x_reveal, y_reveal) == 9)
  {
    std::cout << "BOOM! You stepped on a mine! GAME OVER." << std::endl;
    print_board(board, x_dim, y_dim);
    std::cout << "Starting a new game." << std::endl;
    start_game(board, x_dim, y_dim, num_mines);
  }

  return;
}

void action_mark(char * board, std::size_t x_dim, std::size_t y_dim)
{
  std::size_t x_mark {0};
  std::size_t y_mark {0};
  std::cout << "Please enter the x location to mark: ";
  std::cin >> x_mark;
  std::cout << "Please enter the y location to mark: ";
  std::cin >> y_mark;

  if (x_mark >= x_dim || y_mark >= y_dim)
  {
    std::cout << "The entered location is not on the board." << std::endl;
  }
  else if (mark(board, x_dim, y_dim, x_mark, y_mark) == 2)
  {
    std::cout << "This position is already revealed, and therefore cannot be marked." << std::endl;
  }
  return;
}

std::size_t x_dim_max()
{
  return 60;
}

std::size_t y_dim_max()
{
  return 20;
}

char marked_mask()
{
 //Returns a bitwise mask: A field with bit 0x10 set is a marked field (0b00010000)
  return 0x10;
}

char hidden_mask()
{
  //Returns a bitwise mask: A field with bit 0x20 set is a hidden field (0b00100000
  return 0x20;
}

char value_mask()
{
  /*Returns a bitwise mask: The mask 0x0F can be used to easily remove the marked and hidden bits from a 
    field value (0b00001111)*/
  return 0x0F;
}

void spread_mines(char * board, std::size_t x_dim, std::size_t y_dim, unsigned int num_mines)
{
  if (board != NULL)
  {
    for (unsigned int gen_mines {0}; gen_mines < num_mines; gen_mines++)
    {
      std::size_t try_position {0};
      do {
         try_position = rand() % (x_dim * y_dim);
      } while (board[try_position] != 0);

      board[try_position] = 9;
    }
  }
}
