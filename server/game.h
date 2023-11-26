#ifndef GAME_H
#define GAME_H

#include "../options.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "moves.h"

typedef struct Game {
  int *board;
  Moves *moves;
  char player1[USERNAME_SIZE];
  char player2[USERNAME_SIZE];
  int score_player1;
  int score_player2;
  int rotation;
  char winner[10];
} Game;

Game * init_game(char *username1, char *username2);

void delete_game(Game * g);

void display_board(Game *g, int player);

void copy_board(int *board1, int *board2);

bool is_in_player_side(int joueur, int case_parcourue);

void make_a_move(Game *g,int case_jeu, int joueur);

void display_score(Game * g);
#endif
