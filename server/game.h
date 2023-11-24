#if not defined GAME_H
#define GAME_H

#include "moves.h";

typedef struct Game {
  int board[12];
  Moves *moves;
  char player1[10];
  char player2[10];
  int score_player1;
  int score_player2;
  char winner[10];
} Game;

#endif
