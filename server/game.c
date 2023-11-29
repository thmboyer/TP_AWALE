#include "game.h"

#define PITS_NB 12

#include "game.h"
#include "moves.h"
#include <string.h>
#include <stdlib.h> 


Game * init_game(char *username1, char *username2) {
    Game * g = (Game*)malloc(sizeof(Game));
    g->board = (int*)malloc(sizeof(int)*PITS_NB);
    
    strncpy(g->player1, username1, USERNAME_SIZE - 1);
    g->player1[USERNAME_SIZE - 1] = '\0'; 

    strncpy(g->player2, username2, USERNAME_SIZE - 1);
    g->player2[USERNAME_SIZE - 1] = '\0'; 
    
    g->moves = (Moves*)malloc(sizeof(Moves));
    g->moves->move = (Move*)malloc(sizeof(Move)*100);
    g->moves->size = 100;
    
    
    for (int i=0; i<PITS_NB; ++i) {
        g->board[i] = 4; 
    }

    g->score_player1 = 0;
    g->score_player2 = 0;
    //g->rotation_sens = (rand()< 0.5) ? (-1) : (1); 
    g->rotation_sens = -1; 

    printf("Rotation way :%d",g->rotation_sens);
    return g;
}

char* create_board(Game *g, int player) {
    int idx_row1, idx_row2;
    char* opp;
    char* name;
    idx_row1 = PITS_NB / 2 * (2 - player);
    idx_row2 = PITS_NB / 2 * player - 1;

    if (player == 1) {
        opp = g->player2;
        name = g->player1;
    } else {
        opp = g->player1;
        name = g->player2;
    }

    char* board_display = (char*)malloc(200 * sizeof(char)); // Taille arbitraire

    sprintf(board_display, "%s's side\n\n", opp);

    for (int i = idx_row1; i < idx_row1 + 6; ++i) {
        sprintf(board_display + strlen(board_display), " | %d", g->board[i]);
    }
    sprintf(board_display + strlen(board_display), " |\n");
    
    for (int i = 0; i < PITS_NB / 2; ++i) {
        sprintf(board_display + strlen(board_display), "-----");
    }
    sprintf(board_display + strlen(board_display), "\n");
    
    for (int i = idx_row2; i > idx_row2 - 6; --i) {
        sprintf(board_display + strlen(board_display), " | %d", g->board[i]);
    }
    sprintf(board_display + strlen(board_display), " |\n\n%s's side\n\n", name);
    
    for (int j = 1; j <= 6; ++j) {
        sprintf(board_display + strlen(board_display), " | %d", j);
    }
    sprintf(board_display + strlen(board_display), " |\n\n");

    return board_display;
}

void display_board(Game *g, int player) {
    
    int idx_row1, idx_row2;
    char* opp;
    char* name;
    idx_row1 = PITS_NB/2 * (2 - player);
    idx_row2 = PITS_NB/2 * player - 1;

    if(player == 1){
        opp = g->player2;
        name = g->player1;

    } else {
        opp = g->player1;
        name = g->player2;

    }
    printf("%s'side\n\n", opp);
    
    for (int i=idx_row1; i<idx_row1+6; ++i){
        printf(" | %d", g->board[i]);
    }
    printf(" |\n");
    for (int i=0; i<PITS_NB/2; ++i){
        printf("-----");
    }
    printf("\n");
    for (int i=idx_row2; i>idx_row2-6; --i){
        printf(" | %d", g->board[i]);
    }
    printf(" |\n\n");
    printf("%s'side\n\n", name);
    for (int j=1; j<=6; ++j){
        printf(" | %d", j);
    }
    printf(" |\n\n");

}

void copy_board(int *board1, int *board2){

    for (int i=0; i<PITS_NB; ++i) {
        board2[i] = board1[i];
    }

}

void clean_board(Game *g){

    for (int i=0; i<PITS_NB; ++i) {
        g->board[i] = 0;
    }

}

bool is_in_player_side(int joueur, int visited_pit) {
    return (PITS_NB/2*joueur > visited_pit) && (PITS_NB/2*(joueur-1) <= visited_pit);
}

int make_a_move(Game *g, int selected_pit, int player) {
    int seed_nbs;
    int res = 1;
    int visited_pit, starting_pit;
    int *scoreplayer = (player == 1 ) ? (&g->score_player1): (&g->score_player2) ;
    int temp_score = *scoreplayer;
    int *tmp_board;
    int *tmp_board2;
    int opp = (player == 1) ? (2) : (1);

    tmp_board2 = (int*)malloc(PITS_NB*sizeof(int));
    copy_board(g->board, tmp_board2);

    if (selected_pit > 6 || selected_pit < 1)
    {
        printf("Case choisie non valide\n");
        return 0;
    }

    
    starting_pit = (PITS_NB/2 * player) - selected_pit;
    visited_pit = starting_pit;

    
    seed_nbs = g->board[visited_pit];
    g->board[visited_pit] = 0;

    //printf("case sélec : %d | nb pions = %d\n", visited_pit, seed_nbs);

    
    while (seed_nbs > 0)
    {
        visited_pit = (visited_pit + g->rotation_sens)%12;

        if(visited_pit == -1) {
            visited_pit = 11;
        } else if(visited_pit == 12) {
            visited_pit = 0;
        }


        if (visited_pit == starting_pit) { continue; }

        //printf("case sélec : %d | nb pions = %d\n", visited_pit, g->board[visited_pit]);

        g->board[visited_pit] = g->board[visited_pit] + 1;
        seed_nbs--;
    }

    if(!have_a_seed(g,opp)){
        //printf("Illegal move; you must feed you opponent.\n");
        copy_board(tmp_board2, g->board);
        return 0;
        
    }
   
    tmp_board = (int*)malloc(PITS_NB*sizeof(int));
    copy_board(g->board, tmp_board);

    //display_board(g, 1);
  

    
    while (!is_in_player_side(player, visited_pit)
            && (g->board[visited_pit] == 2 || g->board[visited_pit] == 3))
    {
        printf("Player takes this seeds : %d\n", g->board[visited_pit]);
        *scoreplayer += g->board[visited_pit];
        g->board[visited_pit] = 0;
        visited_pit -= g->rotation_sens;

        if (!have_a_seed(g,opp)) {
            copy_board(tmp_board, g->board);
            *scoreplayer = temp_score;
            res = 2;
    }
        
    }

    //Move* move = (Move*)malloc(sizeof(Move));

    //char* name;
    //if(player == 1){
    //    name = g->player1;

    //} else {
    //    name = g->player2;

    //}
    // strcpy(move->player,name);
    // move->value = selected_pit;
    // g->moves->next_index = 

    return res;

    

}

void display_score(Game * g) {

    printf("Score of player 1 : %d\n", g->score_player1);

    printf("Score of player 2 : %d\n", g->score_player2);


}

bool have_a_seed(Game * g, int player){
    int visited_pit = PITS_NB/2*(player-1);
    while (g->board[visited_pit] == 0
            && visited_pit < PITS_NB/2*player)
    {
        visited_pit++;
    }
    return (visited_pit == PITS_NB/2*player) ? false : true;
}

int nb_of_tiles(Game *g, int select_tile, int player) {
    if (g->rotation_sens == 1) {
        return (PITS_NB/2*(player) - select_tile);
    } else {
        return select_tile + 1 - 6*(player-1);
    }
}

bool can_feed(Game * g, int player) {

     int visited_pit = PITS_NB/2*(player-1);
     while (g->board[visited_pit] < nb_of_tiles(g, visited_pit, player))
     {
         visited_pit++;
     }
     return (visited_pit == PITS_NB/2*player) ? false : true;
 }

int nbCasesDansLeCamp(int selected_pit, int player, Game *game) {
    if (game->rotation_sens == 1) {
      return (PITS_NB / 2 * (player)-selected_pit);
    } else {
      return selected_pit + 1 - 6 * (player - 1);
    }
  }

bool force_feed(Game *game, int *play, int *indexPlay, int player) {

  if (have_a_seed(game,player)) {
    return false;
  }

  int visited_pit = PITS_NB / 2 * (player - 1);
  *indexPlay = 0;
  while (visited_pit < PITS_NB / 2 * (player)) {
    if (game->board[visited_pit] <
        nbCasesDansLeCamp(visited_pit,player, game)) {
      play[*indexPlay] = visited_pit % (PITS_NB / 2);
      (*indexPlay)++;
    }
    visited_pit++;
  }

  return true;
}

int end_of_game_test(Game *g, int player){
    int opp = (player == 1) ? (2) : (1);
    int *scoreplayer = (player == 1 ) ? (&g->score_player1): (&g->score_player2) ;
    int *scoreopp = (player == 1 ) ? (&g->score_player2): (&g->score_player1) ;
   
    if (g->score_player1 >= 25) {
        return 1;
    }

    if (g->score_player2 >= 25) {
        return 2;
    }

    
    if(!have_a_seed(g, player))
    {
        int play[PITS_NB / 2];
        int indexPlay = 0;
        if (force_feed(g, play, &indexPlay,opp)) {
            return 0;
        
        } else {
        *scoreopp += sum_seeds_left(g,opp);
        (player == 1 ) ? (&g->score_player2): (&g->score_player1);
        return (*scoreopp > *scoreplayer ) ? (opp): (player);      
        }
    }

    return 0;
}

void make_famine(Game *g, int player){
    int deb;
    int fin;
    if(player == 1){
        deb = 0;
        fin = 5;
    } else {
        deb = 6;
        fin = 11;
    }
    for(int i = deb;i<= fin; i++){
        g->board[i]=0;
    }
    display_board(g,player);
}

int sum_seeds_left(Game *g, int player){
    int start;
    int end;
    int sum = 0;
    if(player == 1){
        start = 0;
        end = 5;
    } else {
        start = 6;
        end = 11;
    }
    for(int i = start;i<= end; i++){
        sum+=g->board[i];
    }
    return sum;
}

void delete_game(Game * g){
    free(g->board);
    free(g->moves->move);
    free(g->moves);
    free(g);
}
