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

//Game * init_game(char *player_a, char *player_b);
Game * init_game(char *username1, char *username2, int *game_id);

void display_board(Game *g, int player);

char* create_board(Game *g, int player);

void copy_board(int *board1, int *board2);

/**
 * Renvoie 1 si la case est dans le camp du joueur
 * Renvoie 0 sinon
*/
bool is_in_player_side(int joueur, int case_parcourue);

int make_a_move(Game *g,int case_jeu, int joueur);

void display_score(Game * g);

void make_famine(Game *g, int player);


/**
 * Renvoie 1 si le joueur a au moins 1 pion dans son camp
 * Renvoie 0 sinon
*/
bool have_a_seed(Game * g, int player);

/**
 * Renvoie le nombre de cases entre la case passée en paramètre et la fin du camp du joueur (cela dépend
 * du sens de rotation de la partie)
*/
int nb_of_pits(Game *g, int select_tile, int player);

/**
 * Appelée après un coup pour savoir si la partie est terminée, et quel joueur l'emporte
 * Valeurs de retour : 
 *  - 0 : la partie continue
 *  - 1 : le joueur 1 gagne
 *  - 2 : le joueur 2 gagne
 *  - 3 : aucun des joueurs ne gagne
*/
int end_of_game_test(Game *g, int player);

void delete_game(Game * g);

void clean_board(Game * g);

int sum_seeds_left(Game *g, int player);

char * username_by_player(int player);

void add_play_to_moves(Game *g, int player, int selected_pit);

char* replay_game(Game *g);


#endif