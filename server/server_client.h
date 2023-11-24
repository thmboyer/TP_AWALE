#if not defined SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "game.h";

typedef struct Client {
  int fd;
  char username[10];
  char bio[100];
  Game *game;
  struct Client *opponent;
  struct Client *next;
} Client;

typedef struct Connected_Clients {
  Client *first_client;
  int nb;
} Connected_Clients;

#endif
