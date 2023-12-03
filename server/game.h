#ifndef GAME_H
#define GAME_H

#include "../options.h"
#include "moves.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  typedef struct Game {
    int *board;
    int game_id;
    Moves *moves;
    struct Game *next;
    struct Game *previous;
    char player1[USERNAME_SIZE];
    char player2[USERNAME_SIZE];
    int score_player1;
    int score_player2;
    int rotation_sens;
    char winner[10];
  } Game;


Game * init_game(char *username1, char *username2, int *game_id);

void display_board(Game *g, int player);

char* create_board(Game *g, int player);

void copy_board(int *board1, int *board2);


bool is_in_player_side(int joueur, int case_parcourue);

int make_a_move(Game *g,int case_jeu, int joueur);

void display_score(Game * g);

bool have_a_seed(Game * g, int player);

int nb_of_pits(int selected_pit, int player, Game *game);

int end_of_game_test(Game *g, int player);

void delete_game(Game * g);

int sum_seeds_left(Game *g, int player);

char * username_by_player(int player);

void add_play_to_moves(Game *g, int player, int selected_pit);

char* replay_game(Game *g);

void parseGameToCSV(Game *game, const char *filename);

Game* parseCSVToGame(FILE *file);


#endif