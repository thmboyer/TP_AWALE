#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "game.h"

typedef struct Client {
  int fd;
  char username[10];
  char bio[100];
  Game *game;
  struct Client *opponent;
  struct Client *next;
  int connected;              // Bool to know if the client is connected yet.
  char expected_password[20]; // expected password of client if he is in the
                              // database.
} Client;

typedef struct ConnectedClients {
  Client *first_client;
  int nb;
} Connected_Clients;

#endif
