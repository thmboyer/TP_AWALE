#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "game.h"

typedef struct Client Client;

typedef struct Invite {
  Client *recipient;
  struct Invite *next;
} Invite;

typedef struct Invites {
  Invite *first;
} Invites;

struct Client {
  int socket;
  char username[USERNAME_SIZE];
  char bio[100];
  Game *game;
  Invites *invites;
  struct Client *opponent;
  struct Client *next;
  struct Client *previous;
  int connected; // To know if the client has entered his password
                 // correctly yet (-1 if disconnected, 0 if new username, 1 if
                 // we need the password, 2 if connected), should become an enum
  char expected_password[20]; // expected hashed password of client if he is in
                              // the database.
};

typedef struct ActiveClients {
  Client *first;
  Client *last;
  int nb;
} ActiveClients;

int add_client(ActiveClients *, Client *);
void remove_client(ActiveClients *, Client *);
void write_client(SOCKET sock, const char *buffer);
void send_message_to_all_clients(ActiveClients clients, Client client,
                                 const char *buffer, char from_server);
Client *find_client_by_username(const ActiveClients, const char *username);
int add_invite(Client *, Client *);
void remove_invite(Client *, Invite *);

#endif
