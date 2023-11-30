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
    if (client_iterator->priv) {
      strcat(message, " (private)");
    }
    if (client_iterator != active_clients.last) {
      strcat(message, "\n");
    }
    client_iterator = client_iterator->next;
  }
  write_client(client->socket, message);
}

void get_games_history(Client *client, Games *games,
                     char *message) {
  Game *game_iterator = games->first;
  message[0] = '\0';
  if(!game_iterator){
    strcat(message, "There is no game in the record, go ahead and challenge a friend !\n");
  } else {
    strcat(message, "History of games:\n\n");
    while (game_iterator) {
    strcat(message, "- Game_id : ");
    char intStr[20]; 
    sprintf(intStr, "%d", game_iterator->game_id);
    strcat(message, intStr);
    strcat(message, " ");
    strcat(message, game_iterator->player1);
    strcat(message, " vs. ");
    strcat(message, game_iterator->player2);
    strcat(message, " and the winner was : ");
    strcat(message, game_iterator->winner);
    strcat(message, "\n");
    game_iterator = game_iterator->next;
  }

  }
  
  write_client(client->socket, message);
}

void send_invite(ActiveClients clients, Client *sender,
                 const char *recipient_username, char *message, int *current_gm_id) {
  if (sender->opponent) {
    strcpy(message, "You can't send an invite while in a game");
    write_client(sender->socket, message);
    return;
  }
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
  } else if (!is_in_invites(recipient->invites, sender)) {
    if (add_invite(sender->invites, recipient)) {
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
    Game *game = init_game(sender->username, recipient->username,current_gm_id);

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
    if (first == 1) {
      write_client(sender->socket, "You start, type /plays <pit_num>");
      sender->turn = 1;

    } else {
      write_client(recipient->socket, "You start, type /plays <pit_num>");
      recipient->turn = 1;
    }
  }
}

void play_game(Client *sender, int num, Games *games) {
  if (!sender->turn) {

    write_client(sender->socket, "It's not your turn.");

  } else {
    Game *game = sender->game;
    int player = (!strcmp(sender->username, game->player1)) ? (1) : (2);
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
        end_game(sender, games);
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
  } else if (client_to_watch->priv && !friendship(client, client_to_watch)) {
    strcpy(buffer, username);
    strcat(buffer, " is in private mode and you are not friends with them");
    write_client(client->socket, buffer);
  } else {
    if (client->watching) {
      remove_observer(client->watching->observers, client);
    }
    client->watching = client_to_watch;
    Observer *observer = malloc(sizeof(Observer));
    observer->watcher = client;
    observer->previous = NULL;
    observer->next = NULL;
    add_observer(client_to_watch->observers, observer);
    strcpy(buffer, "you are now watching ");
    strcat(buffer, username);
    write_client(client->socket, buffer);
  }
}

void change_bio(Client *client, char *bio) {
  strcpy(client->bio, bio);
  char message[200];
  strcpy(message, "Your bio was changed to: ");
  strcat(message, client->bio);
  write_client(client->socket, message);
}

void get_bio(ActiveClients clients, Client *client, char *username) {
  Client *client_to_get_bio_from = find_client_by_username(clients, username);
  if (client_to_get_bio_from) {
    write_client(client->socket, client_to_get_bio_from->bio);
  } else {
    char message[200];
    strcpy(message, username);
    strcat(message, " was not found.");
    write_client(client->socket, message);
  }
}

void send_friend_request(ActiveClients clients, Client *sender,
                         const char *recipient_username) {
  Client *recipient = find_client_by_username(clients, recipient_username);
  char message[500];
  if (recipient == NULL) {
    strcpy(message, "The user you asked for is not connected.");
    write_client(sender->socket, message);
  } else if (!strcmp(recipient->username, sender->username)) {
    strcpy(message, "You cannot send a friend request to yourself.");
    write_client(sender->socket, message);
  } else if (!is_in_invites(recipient->friend_requests_sent, sender)) {
    if (add_invite(sender->friend_requests_sent, recipient)) {
      strcpy(message, "You got a friend request from ");
      strcat(message, sender->username);
      strcat(message, " type /addfr ");
      strcat(message, sender->username);
      strcat(message, " to accept the request");
      write_client(recipient->socket, message);
      strcpy(message, "Friend request sent to ");
      strcat(message, recipient->username);
      write_client(sender->socket, message);
    } else {
      strcpy(message, "You already sent a friend request to this player");
      write_client(sender->socket, message);
    }
  } else {
    Friend *friendship_1 = malloc(sizeof(Friend));
    friendship_1->next = NULL;
    friendship_1->friend_of_client = recipient;
    add_friend(sender->friends, friendship_1);
    strcpy(message, "You are now friends with ");
    strcat(message, recipient->username);
    write_client(sender->socket, message);

    Friend *friendship_2 = malloc(sizeof(Friend));
    friendship_2->next = NULL;
    friendship_2->friend_of_client = sender;
    add_friend(recipient->friends, friendship_2);
    remove_invite_to_new_friend(recipient, sender);
    strcpy(message, "You are now friends with ");
    strcat(message, sender->username);
    write_client(recipient->socket, message);
  }
}

void send_friend_list(Client *client) {
  char message[1024];
  strcpy(message, "FriendList:");
  Friend *friend_it = client->friends->first;
  while (friend_it) {
    strcat(message, "\n");
    strcat(message, friend_it->friend_of_client->username);
    friend_it = friend_it->next;
  }
  write_client(client->socket, message);
}

void toggle_private_mode(Client *client) {
  client->priv = (client->priv + 1) % 2;
  char message[200];
  switch (client->priv) {
  case 0:
    strcpy(message, "Private mode off.");
    write_client(client->socket, message);
    break;
  case 1:
    strcpy(message, "Private mode on.");
    write_client(client->socket, message);
    Observer *observer = client->observers->first;
    while (observer) {
      if (!friendship(client, observer->watcher)) {
        strcpy(message, client->username);
        strcat(message, " went private, only their friends can watch them");
        write_client(observer->watcher->socket, message);
        Observer *temp = observer;
        observer->watcher->watching = NULL;
        observer = observer->next;
        remove_observer(client->observers, temp->watcher);
      } else {
        observer = observer->next;
      }
    }
    break;
    }
}

void end_game(Client * client, Games * games){
  Game *game = client->game;
  client->game = NULL;
  client->opponent->game = NULL;
  if (!games->first) {
    games->first = game;
    games->last = game;
  } else {
    games->last->next = game;
    game->previous = games->last;
    games->last = game;
  }
  game->next = NULL;
}

void leave_game(Client *client, Games *games) {
  char message[200];
  if (!client->game) {
    strcpy(message, "You are not in a game.");
    write_client(client->socket, message);
    return;
  }
  Game *game = client->game;
  strcpy(game->winner, client->opponent->username);
  end_game(client,games);
  client->opponent->opponent = NULL;
  strcpy(message, "Your opponent left the game, you won.");
  write_client(client->opponent->socket, message);

  client->opponent = NULL;
  strcpy(message, "You left the game.");
  write_client(client->socket, message);

}
