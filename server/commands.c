#include "commands.h"
#include "game.h"
#include "server_client.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void get_client_list(ActiveClients active_clients, Client *client,
                     char *message) {
  Client *client_iterator = active_clients.first;
  message[0] = '\0';
  strcat(message, "Active users:\n");
  while (client_iterator) {
    strcat(message, client_iterator->username);
    if (!strcmp(client_iterator->username, client->username)) {
      strcat(message, " (you)");
    }
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
    if (recipient->watching) {
      remove_observer(recipient->watching->observers, recipient);
      recipient->watching = NULL;
    }
    if (sender->watching) {
      remove_observer(sender->watching->observers, sender);
      sender->watching = NULL;
    }

    strcpy(message, "The game against ");
    strcat(message, sender->username);
    strcat(message, " has started.\n");
    write_client(recipient->socket, message);

    strcpy(message, "The game against ");
    strcat(message, recipient->username);
    strcat(message, " has started.\n");
    write_client(sender->socket, message);

    write_client(sender->socket, create_board(game, 1));

    write_client(recipient->socket, create_board(game, 2));

    srand(time(NULL));
    int first = (rand() % 2) + 1;
    printf("first: %d\n", first);
    if (first == 1) {
      write_client(sender->socket, "You start, type /plays <pit_num>");
      sender->turn = 1;

    } else {
      write_client(recipient->socket, "You start, type /plays <pit_num>");
      recipient->turn = 1;
    }
  }
}

void play_game(Client *sender, int num) {
  if (!sender->turn) {

    write_client(sender->socket, "It's not your turn.");

  } else {
    Game *game = sender->game;
    int player = (strcmp(sender->username, game->player1)) ? (1) : (2);
    int opp = (player == 1) ? (2) : (1);
    int check = make_a_move(game, num, player);
    if (!check) {

      write_client(sender->socket,
                   "Illegal move; you must feed you opponent.\n");

    } else {

      sender->turn = 0;
      sender->opponent->turn = 1;

      char board[200];
      strcpy(board, create_board(game, player));
      write_client(sender->socket, board);
      send_message_to_all_observers(sender->observers, board);

      strcpy(board, "Opponent's play :\n");
      strcat(board, create_board(game, opp));
      write_client(sender->opponent->socket, board);
      send_message_to_all_observers(sender->opponent->observers, board);

      if (end_of_game_test(game, player)) {
        write_client(sender->socket, "End of the game.\n");
        write_client(sender->opponent->socket, "End of the game.\n");
        send_message_to_all_observers(sender->opponent->observers,
                                      "End of the game.\n");
      }
    }
  }
}

void get_game_list(ActiveClients clients, Client *client, char *buffer) {
  Client *client_iterator = clients.first;
  ActiveClients clients_already_dealt_with;
  clients_already_dealt_with.first = NULL;
  clients_already_dealt_with.last = NULL;
  strcpy(buffer, "List of the games currently going on:");
  while (client_iterator) {
    if (client_iterator->game != NULL) {
      if (!is_in(client_iterator, clients_already_dealt_with)) {
        add_client(&clients_already_dealt_with, client_iterator);
        add_client(&clients_already_dealt_with, client_iterator->opponent);
        strcat(buffer, "\n");
        strcat(buffer, client_iterator->username);
        strcat(buffer, " vs ");
        strcat(buffer, client_iterator->opponent->username);
      }
    }
    client_iterator = client_iterator->next;
  }
  if (clients_already_dealt_with.first == NULL) {
    write_client(client->socket, "No games going on currently");
  } else {
    write_client(client->socket, buffer);
  }
}

void watch_user(ActiveClients clients, Client *client, char *username,
                char *buffer) {
  // printf("Watching user");
  if (!strcmp(client->username, username)) {
    if (client->watching) {
      remove_observer(client->watching->observers, client);
      client->watching = NULL;
    }
    strcpy(buffer, "You are not watching anyone anymore");
    write_client(client->socket, buffer);
    return;
  } else if (client->game) {
    strcpy(buffer, "You can't watch someone while you're in a game");
    write_client(client->socket, buffer);
    return;
  }
  Client *client_to_watch = find_client_by_username(clients, username);
  if (client_to_watch == NULL) {
    strcpy(buffer, username);
    strcat(buffer, " is not connected");
    write_client(client->socket, buffer);
  } else {
    if (client->watching) {
      remove_observer(client->watching->observers, client);
    }
    client->watching = client_to_watch;
    Observer *observer = malloc(sizeof(Observer));
    observer->watcher = client;
    observer->watched = client_to_watch;
    observer->previous = NULL;
    observer->next = NULL;
    add_observer(client_to_watch->observers, observer);
    strcpy(buffer, "you are now watching ");
    strcat(buffer, username);
    write_client(client->socket, buffer);
  }
}
