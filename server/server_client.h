#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "game.h"

typedef struct Client Client;
typedef struct ActiveClients ActiveClients;

typedef struct Friend {
  struct Friend *next;
  struct Friend *previous;
  Client *friend_of_client;
} Friend;

typedef struct Games {
  Game *first;
  Game *last;
} Games;

typedef struct FriendList {
  Friend *first;
  Friend *last;
} FriendList;

typedef struct Observer {
  struct Observer *next;
  struct Observer *previous;
  Client *watcher;
} Observer;

typedef struct Observers {
  Observer *first;
  Observer *last;
} Observers;

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
  char bio[BIO_SIZE];
  Game *game;
  Invites *invites;
  Invites *friend_requests_sent;
  struct Client *opponent;
  struct Client *next;
  struct Client *previous;
  struct Client *watching;
  Observers *observers;
  FriendList *friends;
  int connected; // To know if the client has entered his password
                 // correctly yet (-1 if disconnected, 0 if new username, 1 if
                 // we need the password, 2 if connected), should become an enum
  char expected_password[20]; // expected hashed password of client if he is in
                              // the database.
  int turn;
  int priv;
};

typedef struct ActiveClients {
  Client *first;
  Client *last;
  int nb;
} ActiveClients;

int add_client(ActiveClients *, Client *);
void remove_client(ActiveClients *, Client *);
int add_observer(Observers *observers, Observer *observer);
void remove_observer(Observers *observers, Client *client);
int add_friend(FriendList *friends, Friend *new_friend);
void remove_friend(FriendList *friends, Client *client);
void write_client(SOCKET sock, const char *buffer);
void send_message_to_all_clients(ActiveClients clients, Client client,
                                 const char *buffer, char from_server);
void send_message_to_all_observers(Observers *observers, const char *message);
Client *find_client_by_username(const ActiveClients, const char *username);
int add_invite(Invites *, Client *);
void remove_invite_to_new_friend(Client *client, Client *new_friend_client);
void remove_invite(Invites *invites, Invite *);
void remove_invites_from_client(Client *client);
int is_in_invites(const Invites *invites, const Client *recipient);
int is_in(Client *client, ActiveClients list_of_clients);
int friendship(Client *client1, Client *client2);
int is_already_used(ActiveClients clients, char *username);

#endif
