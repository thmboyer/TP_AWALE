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
    return;
  } else if (clients->last == client) {
    clients->last = clients->last->previous;
    clients->last->next = NULL;
    --clients->nb;
    return;
  } else {
    client->previous->next = client->next;
    client->next->previous = client->previous;
    --clients->nb;
    return;
  }
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
