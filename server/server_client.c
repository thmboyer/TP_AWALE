#include "server_client.h"
#include "options.h"
#include <stdio.h>

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
    ;
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
