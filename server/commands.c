#include "commands.h"
#include "server_client.h"
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
    strcpy(message, "The user you asked for is not connected.\n");
    write_client(sender->socket, message);
  } else if (recipient->opponent != NULL) {
    strcpy(message, "The user you asked for is currently in a game.\n");
    write_client(sender->socket, message);
  } else {
    if (add_invite(sender, recipient)) {
      strcpy(message, "You got an invite for a game against ");
      strcat(message, sender->username);
      strcat(message, " type /accept to accept the request\n");
      write_client(recipient->socket, message);
      strcpy(message, "Invite sent to ");
      strcat(message, recipient->username);
      strcat(message, "\n");
      write_client(sender->socket, message);
    } else {
      strcpy(message, "You already sent an invite to this player");
      write_client(sender->socket, message);
    }
  }
}
