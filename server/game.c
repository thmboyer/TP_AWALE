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
    g->rotation = (rand()< 0.5) ? (-1) : (1); 

    printf("Rotation way :%d",g->rotation);
    return g;
}

void delete_game(Game * g){
    
    free(g->board);
    free(g->moves->move);
    free(g->moves);
    free(g);
}

void display_board(Game *g, int player) {
    
    int idx_row1, idx_row2;

   
    idx_row1 = PITS_NB/2 * (2 - player);
    idx_row2 = PITS_NB/2 * player - 1;
    
    
    printf("Player %d's side\n", player);
    printf("Board :\n\n");
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
}

void copy_board(int *board1, int *board2){

    for (int i=0; i<PITS_NB; ++i) {
        board2[i] = board1[i];
    }

}

bool is_in_player_side(int joueur, int visited_pit) {
    return (PITS_NB/2*joueur > visited_pit) && (PITS_NB/2*(joueur-1) <= visited_pit);
}

void make_a_move(Game *g, int selected_pit, int player) {

    int seed_nbs;
    int visited_pit, starting_pit;
    int *scoreplayer = (player == 1 ) ? (&g->score_player1): (&g->score_player2) ;
    int *tmp_board;

    if (selected_pit > 6 || selected_pit < 1)
    {
        printf("Selected pit is not valid.\n");
        return;
    }

    
    starting_pit = (PITS_NB/2 * player) - selected_pit;
    visited_pit = starting_pit;

    
    seed_nbs = g->board[visited_pit];
    g->board[visited_pit] = 0;

    printf("Selected pit : %d | nb seeds = %d\n", visited_pit, seed_nbs);

    
    while (seed_nbs > 0)
    {
        visited_pit = (visited_pit + g->rotation)%12; 


        if (visited_pit == starting_pit) { continue; }

        g->board[visited_pit] = g->board[visited_pit] + 1;
        seed_nbs--;
    }

   
    tmp_board = (int*)malloc(PITS_NB*sizeof(int));
    copy_board(g->board, tmp_board);

    display_board(g, 1);
  

    
    while (!is_in_player_side(player, visited_pit)
            && (g->board[visited_pit] == 2 || g->board[visited_pit] == 3))
    {
        printf("Player takes this seeds : %d\n", g->board[visited_pit]);
        *scoreplayer += g->board[visited_pit];
        g->board[visited_pit] = 0;
        visited_pit -= g->rotation;

        
        if ( ((g->rotation == 1 && visited_pit == PITS_NB/2 * (2-player))
            || (g->rotation == 0 && visited_pit == PITS_NB/2 * (3-player) - 1))
            && g->board[visited_pit] == 0)
        {
            copy_board(tmp_board, g->board);
        }
    }

    

}

void display_score(Game * g) {

    printf("Score of player 1 : %d\n", g->score_player1);

    printf("Score of player 2 : %d\n", g->score_player2);


}
/*
 int main() {

     Game * g;

     g = init_game();

     display_board(g,2);

     make_a_move(g,3,2);

     make_a_move(g,4,1);

     make_a_move(g,4,1);

     display_score(g);

     return 0;
 }
*/
