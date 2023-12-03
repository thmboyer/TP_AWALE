#include "game.h"

#define PITS_NB 12

#include "game.h"
#include "moves.h"
#include <stdlib.h>
#include <string.h>

Game *init_game(char *username1, char *username2, int *current_gm_id) {
  Game *g = (Game *)malloc(sizeof(Game));
  g->board = (int *)malloc(sizeof(int) * PITS_NB);
  g->game_id = *current_gm_id;
  *current_gm_id = *current_gm_id+1;
  strncpy(g->player1, username1, USERNAME_SIZE - 1);
  g->player1[USERNAME_SIZE - 1] = '\0';

  strncpy(g->player2, username2, USERNAME_SIZE - 1);
  g->player2[USERNAME_SIZE - 1] = '\0';

  g->moves = (Moves *)malloc(sizeof(Moves));
  g->moves->first = NULL;
  g->moves->last = NULL;
  g->moves->size = 0;

  for (int i = 0; i < PITS_NB; ++i) {
    g->board[i] = 4;
  }

  g->score_player1 = 0;
  g->score_player2 = 0;

  g->rotation_sens = -1;

  return g;
}

char *create_board(Game *g, int player) {
  int idx_row1, idx_row2;
  char *opp;
  char *name;
  idx_row1 = PITS_NB / 2 * (2 - player);
  idx_row2 = PITS_NB / 2 * player - 1;

  if (player == 1) {
    opp = g->player2;
    name = g->player1;
  } else {
    opp = g->player1;
    name = g->player2;
  }

  char *board_display = (char *)malloc(200 * sizeof(char)); 
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
  char *opp;
  char *name;
  idx_row1 = PITS_NB / 2 * (2 - player);
  idx_row2 = PITS_NB / 2 * player - 1;

  if (player == 1) {
    opp = g->player2;
    name = g->player1;

  } else {
    opp = g->player1;
    name = g->player2;
  }
  printf("%s'side\n\n", opp);

  for (int i = idx_row1; i < idx_row1 + 6; ++i) {
    printf(" | %d", g->board[i]);
  }
  printf(" |\n");
  for (int i = 0; i < PITS_NB / 2; ++i) {
    printf("-----");
  }
  printf("\n");
  for (int i = idx_row2; i > idx_row2 - 6; --i) {
    printf(" | %d", g->board[i]);
  }
  printf(" |\n\n");
  printf("%s'side\n\n", name);
  for (int j = 1; j <= 6; ++j) {
    printf(" | %d", j);
  }
  printf(" |\n\n");
}

void copy_board(int *board1, int *board2) {

  for (int i = 0; i < PITS_NB; ++i) {
    board2[i] = board1[i];
  }
}

void clean_board(Game *g) {

  for (int i = 0; i < PITS_NB; ++i) {
    g->board[i] = 0;
  }
}

bool is_in_player_side(int joueur, int visited_pit) {
  return (PITS_NB / 2 * joueur > visited_pit) &&
         (PITS_NB / 2 * (joueur - 1) <= visited_pit);
}

int make_a_move(Game *g, int selected_pit, int player) {
  int seed_nbs;
  int res = 1;
  int visited_pit, starting_pit;
  int *scoreplayer = (player == 1) ? (&g->score_player1) : (&g->score_player2);
  int temp_score = *scoreplayer;
  int *tmp_board;
  int *tmp_board2;
  int opp = (player == 1) ? (2) : (1);

  tmp_board2 = (int *)malloc(PITS_NB * sizeof(int));
  copy_board(g->board, tmp_board2);

  starting_pit = (PITS_NB / 2 * player) - selected_pit;
  visited_pit = starting_pit;

  seed_nbs = g->board[visited_pit];
  g->board[visited_pit] = 0;


  if(!seed_nbs) {

    return -1;

  }


  while (seed_nbs > 0) {
    visited_pit = (visited_pit + g->rotation_sens) % 12;

    if (visited_pit == -1) {
      visited_pit = 11;
    } else if (visited_pit == 12) {
      visited_pit = 0;
    }

    if (visited_pit == starting_pit) {
      continue;
    }



    g->board[visited_pit] = g->board[visited_pit] + 1;
    seed_nbs--;
  }

  if (!have_a_seed(g, opp)) {
    
    copy_board(tmp_board2, g->board);
    return 0;
  }

  tmp_board = (int *)malloc(PITS_NB * sizeof(int));
  copy_board(g->board, tmp_board);



  while (!is_in_player_side(player, visited_pit) &&
         (g->board[visited_pit] == 2 || g->board[visited_pit] == 3)) {
    printf("Player takes this seeds : %d\n", g->board[visited_pit]);
    *scoreplayer += g->board[visited_pit];
    g->board[visited_pit] = 0;
    visited_pit -= g->rotation_sens;

    if (!have_a_seed(g, opp)) {
      copy_board(tmp_board, g->board);
      *scoreplayer = temp_score;
      res = 2;
    }
  }
  add_play_to_moves(g, player, selected_pit);

  return res;
}

void display_score(Game *g) {

  printf("Score of player 1 : %d\n", g->score_player1);

  printf("Score of player 2 : %d\n", g->score_player2);
}

bool have_a_seed(Game *g, int player) {
  int visited_pit = PITS_NB / 2 * (player - 1);
  while (g->board[visited_pit] == 0 && visited_pit < PITS_NB / 2 * player) {
    visited_pit++;
  }
  return (visited_pit == PITS_NB / 2 * player) ? false : true;
}

int nb_of_tiles(Game *g, int select_tile, int player) {
  if (g->rotation_sens == 1) {
    return (PITS_NB / 2 * (player)-select_tile);
  } else {
    return select_tile + 1 - 6 * (player - 1);
  }
}

bool can_feed(Game *g, int player) {

  int visited_pit = PITS_NB / 2 * (player - 1);
  while (g->board[visited_pit] < nb_of_tiles(g, visited_pit, player)) {
    visited_pit++;
  }
  return (visited_pit == PITS_NB / 2 * player) ? false : true;
}

int nb_of_pits(int selected_pit, int player, Game *game) {
  if (game->rotation_sens == 1) {
    return (PITS_NB / 2 * (player)-selected_pit);
  } else {
    return selected_pit + 1 - 6 * (player - 1);
  }
}

bool force_feed(Game *game, int *play, int *indexPlay, int player) {

  if (have_a_seed(game, player)) {
    return false;
  }

  int visited_pit = PITS_NB / 2 * (player - 1);
  *indexPlay = 0;
  while (visited_pit < PITS_NB / 2 * (player)) {
    if (game->board[visited_pit] <
        nb_of_pits(visited_pit, player, game)) {
      play[*indexPlay] = visited_pit % (PITS_NB / 2);
      (*indexPlay)++;
    }
    visited_pit++;
  }

  return true;
}

int end_of_game_test(Game *g, int player) {
  int opp = (player == 1) ? (2) : (1);
  int *scoreplayer = (player == 1) ? (&g->score_player1) : (&g->score_player2);
  int *scoreopp = (player == 1) ? (&g->score_player2) : (&g->score_player1);

  if (g->score_player1 >= 25) {
    return 1;
  }

  if (g->score_player2 >= 25) {
    return 2;
  }

  if (!have_a_seed(g, player)) {
    int play[PITS_NB / 2];
    int indexPlay = 0;
    if (force_feed(g, play, &indexPlay, opp)) {
      return 0;

    } else {
      *scoreopp += sum_seeds_left(g, opp);
      (player == 1) ? (&g->score_player2) : (&g->score_player1);
      return (*scoreopp > *scoreplayer) ? (opp) : (player);
    }
  }

  return 0;
}


int sum_seeds_left(Game *g, int player) {
  int start;
  int end;
  int sum = 0;
  if (player == 1) {
    start = 0;
    end = 5;
  } else {
    start = 6;
    end = 11;
  }
  for (int i = start; i <= end; i++) {
    sum += g->board[i];
  }
  return sum;
}

void add_play_to_moves(Game *g, int player, int selected_pit){
    Move* move = (Move*)malloc(sizeof(Move));
    char* name;
    if(player == 1){
       name = g->player1;
    } else {
      name = g->player2;
    }
  strcpy(move->player,name);
  move->value = selected_pit;
    if(g->moves->first == NULL){
        g->moves->first = move;
        g->moves->last = move;
    } else {
        g->moves->last->next = move;
        move->previous = g->moves->last;
        g->moves->last = move;
    }
    move->next = NULL;
    g->moves->size++;
}

char* replay_game(Game *g){
    int temp = 5;
    Game * game = init_game(g->player1,g->player2,&temp);
    game->rotation_sens = g->rotation_sens;
    char *board_display = (char *)malloc((200 *  (g->moves->size + 1)) * sizeof(char));
    int pov = 1;
    strcat(board_display,create_board(game,pov));
    Move * current = g->moves->first;
    while(current != NULL){
        int play = (!strcmp(current->player, game->player1)) ? (1) : (2);
        make_a_move(game, current->value, play);
        strcat(board_display,create_board(game,pov));
        current = current->next;
    }
    strcat(board_display,"\n\nThw winner is :");
    strcat(board_display,g->winner);
    strcat(board_display,"\n\n");
    return board_display;
}

void parseGameToCSV(Game *game, const char *filename) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    
    fprintf(file, "%d,%s,%s,%d,%d,%d,%s\n",
            game->game_id, game->player1, game->player2,
            game->score_player1, game->score_player2, game->rotation_sens,game->winner);

   
    Moves *movesList = game->moves;
    if (movesList != NULL) {
        Move *currentMove = movesList->first;
        while (currentMove != NULL) {
            fprintf(file, "%s,%d\n", currentMove->player, currentMove->value);
            currentMove = currentMove->next;
            movesList->size++;
        }
    }
    fprintf(file,"%s\n","end");
    fclose(file);
}

Game* parseCSVToGame(FILE *file) {
        if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return NULL;
    }
    Game *newGame = (Game*)malloc(sizeof(Game));
    if (newGame == NULL) {
        printf("Erreur d'allocation de mémoire pour Game.\n");
        fclose(file);
        return NULL;
    }
    char buffer[1024];
    fgets(buffer, sizeof(buffer), file);
   
    if(strstr(buffer,"end") ){
       fgets(buffer, sizeof(buffer), file);
      }
    if(!strcmp(buffer,"\n") || feof(file)){
      return NULL;
    }
    
    sscanf(buffer, "%d,%[^,],%[^,],%d,%d,%d,%s\n",
           &newGame->game_id, newGame->player1,newGame->player2,
           &newGame->score_player1, &newGame->score_player2, &newGame->rotation_sens, newGame->winner);
    
    newGame->moves = (Moves *)malloc(sizeof(Moves));
    newGame->moves->first = NULL;
    newGame->moves->last = NULL;
    newGame->moves->size = 0;
    char player[USERNAME_SIZE];
    int play;
    
     while (strcmp(fgets(buffer, sizeof(buffer), file),"end")) {
      if(strstr(buffer,"end")){
        break;
      }
        sscanf(buffer, "%[^,],%d\n", player, &play);
        if(!strcmp(newGame->player1,player)){
          add_play_to_moves(newGame,1,play);
        } else if(!strcmp(newGame->player2,player)){
          add_play_to_moves(newGame,2,play);
        }
    }
    
    return newGame;
} 

void delete_game(Game *g) {
  free(g->board);

  free(g->moves);
  free(g);
}
