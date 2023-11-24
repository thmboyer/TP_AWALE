#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "game.h"
#include "options.h"

typedef struct Client {
  int socket;
  char username[USERNAME_SIZE];
  char bio[100];
  Game *game;
  struct Client *opponent;
  struct Client *next;
  struct Client *previous;
  int connected;              // Bool to know if the client is connected yet.
  char expected_password[20]; // expected password of client if he is in the
                              // database.
} Client;

typedef struct ActiveClients {
  Client *first_client;
  int nb;
} ActiveClients;

int add_client(ActiveClients *, Client *);
void remove_client(ActiveClients *, Client *);

#endif
