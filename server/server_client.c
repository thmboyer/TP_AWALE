#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server_client.h"

int add_client(ActiveClients *clients, Client *client) {
  if (clients->nb == MAX_CLIENTS) {
    return 0;
  }
  if (!clients->first) {
    clients->first = client;
    clients->last = client;
    ++clients->nb;
    return 1;
  } else {
    clients->last->next = client;
    client->previous = clients->last;
    clients->last = client;
    ++clients->nb;
    return 1;
  }
}

void remove_client(ActiveClients *clients, Client *client) {
  if (clients->first == client) {
    clients->first = clients->first->next;
    --clients->nb;
  } else if (clients->last == client) {
    clients->last = clients->last->previous;
    clients->last->next = NULL;
    --clients->nb;
  } else {
    client->previous->next = client->next;
    client->next->previous = client->previous;
    --clients->nb;
  }
  remove_invites_from_client(client);
  if (client->opponent) {
    assert(client->game &&
           "If a client has an opponent, he should have a game");
    client->opponent->opponent = NULL;
    client->opponent->game = NULL;
    write_client(client->opponent->socket,
                 "\nYour opponent left, you won the game.");
    free(client->game);
  }
  free(client);
}

int add_invite(Client *sender, Client *recipient) {
  if (sender->invites->first == NULL) {
    Invite *invite = malloc(sizeof(Invite));
    sender->invites->first = invite;
    invite->recipient = recipient;
    invite->next = NULL;
    return 1;
  } else {
    Invite *it_invite = sender->invites->first;
    while (it_invite->next != NULL) {
      if (!strcmp(it_invite->recipient->username, recipient->username)) {
        return 0;
      }
    }
    if (!strcmp(it_invite->recipient->username, recipient->username)) {
      return 0;
    }
    Invite *invite = malloc(sizeof(Invite));
    it_invite->next = invite;
    invite->recipient = recipient;
    invite->next = NULL;
    return 1;
  }
}

void remove_invite(Client *client, Invite *invite) {
  Invite *it_invite = client->invites->first;
  if (it_invite == invite) {
    client->invites->first = it_invite->next;
    free(it_invite);
    return;
  }
  while (it_invite->next != invite) {
    it_invite = it_invite->next;
  }
  Invite *invite_to_remove = it_invite->next;
  it_invite->next = it_invite->next->next;
  free(invite_to_remove);
}

void remove_invites_from_client(Client *client) {
  Invite *invite = client->invites->first;
  while (invite) {
    Invite *previous_invite = invite;
    invite = invite->next;
    free(previous_invite);
  }
  client->invites->first = NULL;
}

Invite *get_invite(const Client *sender, const Client *recipient) {
  Invite *it_invite = sender->invites->first;
  while (it_invite) {
    if (!strcmp(it_invite->recipient->username, recipient->username)) {
      return it_invite;
    }
    it_invite = it_invite->next;
  }
  return NULL;
}

Client *find_client_by_username(const ActiveClients clients,
                                const char *username) {
  Client *client_iterator = clients.first;
  while (client_iterator != NULL) {
    if (!strcmp(client_iterator->username, username)) {
      break;
    }
    client_iterator = client_iterator->next;
  }
  return client_iterator;
}

void send_message_to_all_clients(ActiveClients clients, Client sender,
                                 const char *buffer, char from_server) {
  char message[BUF_SIZE];
  Client *client_iterator = clients.first;
  while (client_iterator) {
    message[0] = 0;
    {
      /* we don't send message to the sender */
      if (sender.socket != client_iterator->socket) {
        if (from_server == 0) {
          strncpy(message, sender.username, BUF_SIZE - 1);
          strncat(message, " : ", sizeof message - strlen(message) - 1);
        }
        strncat(message, buffer, sizeof message - strlen(message) - 1);
        write_client(client_iterator->socket, message);
      }
    }
    client_iterator = client_iterator->next;
  }
}

void write_client(SOCKET sock, const char *buffer) {
  if (send(sock, buffer, strlen(buffer), 0) < 0) {
    perror("send()");
    exit(errno);
  }
}
