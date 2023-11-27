#include "commands.h"
#include "server_client.h"
#include "game.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

void get_client_list(ActiveClients active_clients, Client *client,
                     char *message) {
  Client *client_iterator = active_clients.first;
  message[0] = '\0';
  strcat(message, "Active users:\n");
  while (client_iterator) {
    strcat(message, client_iterator->username);
    if (client_iterator != active_clients.last) {
      strcat(message, "\n");
    }
    client_iterator = client_iterator->next;
  }
  write_client(client->socket, message);
}

void send_invite(ActiveClients clients, Client *sender,
                 const char *recipient_username, char *message) {
  Client *recipient = find_client_by_username(clients, recipient_username);
  if (recipient == NULL) {
    strcpy(message, "The user you asked for is not connected.");
    write_client(sender->socket, message);
  } else if (recipient->opponent != NULL) {
    strcpy(message, "The user you asked for is currently in a game.");
    write_client(sender->socket, message);
  } else if (!strcmp(recipient->username, sender->username)) {
    strcpy(message, "You cannot challenge yourself");
    write_client(sender->socket, message);
  } else if (!has_sent_invite(recipient, sender)) {
    if (add_invite(sender, recipient)) {
      strcpy(message, "You got an invite for a game against ");
      strcat(message, sender->username);
      strcat(message, " type /fight ");
      strcat(message, sender->username);
      strcat(message, " to accept the request");
      write_client(recipient->socket, message);
      strcpy(message, "Invite sent to ");
      strcat(message, recipient->username);
      write_client(sender->socket, message);
    } else {
      strcpy(message, "You already sent an invite to this player");
      write_client(sender->socket, message);
    }
  } else {
    Game *game = init_game(sender->username, recipient->username);
    
    sender->opponent = recipient;
    sender->game = game;
    // WARNING: BOTH PLAYERS SHARE THE SAME GAME OBJECT
    recipient->game = game;
    recipient->opponent = sender;

    remove_invites_from_client(recipient);
    remove_invites_from_client(sender);

    strcpy(message, "The game against ");
    strcat(message, sender->username);
    strcat(message, " has started.\n");
    write_client(recipient->socket, message);

    strcpy(message, "The game against ");
    strcat(message, recipient->username);
    strcat(message, " has started.\n");
    write_client(sender->socket, message);

    write_client(sender->socket, create_board(game,1) );

    write_client(recipient->socket, create_board(game,2));

    srand(time(NULL));
    int first = (rand() % 2) + 1;
    printf("first: %d\n", first);
    if(first == 1){
      write_client(sender->socket, "You start, type /play <pit_num>" );
      sender->turn = 1;

    } else {
    write_client(recipient->socket, "You start, type /play <pit_num>" );
    recipient->turn = 1;
   }

  }

  
}

void play_game(Client * sender, int num){
  if(!sender->turn){

    write_client(sender->socket, "It's not your turn." );
    
  } else {
    Game * game = sender->game;
    int player = (strcmp(sender->username,game->player1)) ? (1) : (2);
    int opp = (player == 1) ? (2) : (1);
    make_a_move(game,num,player);
    sender->turn = 0;
    sender->opponent->turn = 1;

    write_client(sender->socket, create_board(game,player));

    write_client(sender->opponent->socket, "Opponent's play :\n");
    write_client(sender->opponent->socket, create_board(game,opp));


  }

}
