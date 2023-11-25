#ifndef GAME_H
#define GAME_H

#include "../options.h"
#include "moves.h"

typedef struct Game {
  int board[12];
  Moves *moves;
  char player1[USERNAME_SIZE];
  char player2[USERNAME_SIZE];
  int score_player1;
  int score_player2;
  char winner[10];
} Game;

void init_game(char *player_a, char *player_b);
#endif
