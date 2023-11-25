#include "commands.h"
#include "server_client.h"
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
  Invite *invite_from_receiver;
  if (recipient == NULL) {
    strcpy(message, "The user you asked for is not connected.");
    write_client(sender->socket, message);
  } else if (recipient->opponent != NULL) {
    strcpy(message, "The user you asked for is currently in a game.");
    write_client(sender->socket, message);
  } else if ((invite_from_receiver = get_invite(recipient, sender)) == NULL) {
    if (add_invite(sender, recipient)) {
      strcpy(message, "You got an invite for a game against ");
      strcat(message, sender->username);
      strcat(message, " type /accept to accept the request");
      write_client(recipient->socket, message);
      strcpy(message, "Invite sent to ");
      strcat(message, recipient->username);
      write_client(sender->socket, message);
    } else {
      strcpy(message, "You already sent an invite to this player");
      write_client(sender->socket, message);
    }
  } else {
    Game *game = malloc(sizeof(Game));
    init_game(sender->username, recipient->username);
    sender->opponent = recipient;
    sender->game = game;
    // WARNING: BOTH PLAYERS SHARE THE SAME GAME OBJECT
    recipient->game = game;
    recipient->opponent = sender;

    remove_invite(recipient, invite_from_receiver);
    strcpy(message, "The game against ");
    strcat(message, sender->username);
    strcat(message, " has started.");
    write_client(recipient->socket, message);

    strcpy(message, "The game against ");
    strcat(message, recipient->username);
    strcat(message, " has started.");
    write_client(sender->socket, message);
  }
}
