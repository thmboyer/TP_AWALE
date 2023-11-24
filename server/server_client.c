#include "server_client.h"
#include "options.h"

int add_client(ActiveClients *clients, Client *client) {
  if (clients->nb == MAX_CLIENTS) {
    return 0;
  }
  if (!clients->first_client) {
    clients->first_client = client;
    ++clients->nb;
    return 1;
  } else {
    Client *client_iterator = clients->first_client;
    while (client_iterator->next) {
      client_iterator = client_iterator->next;
    }
    client_iterator->next = client;
    client->previous = client_iterator;
    ++clients->nb;
    return 1;
  }
}

void remove_client(ActiveClients *clients, Client *client) {
  if (clients->first_client == client) {
    clients->first_client = clients->first_client->next;
    --clients->nb;
    return;
  } else {
    client->previous->next = client->next;
    --clients->nb;
    return;
  }
}
