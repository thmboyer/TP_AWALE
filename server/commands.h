#ifndef COMMANDS_H
#define COMMANDS_H
#include "server_client.h"

void get_client_list(ActiveClients, Client *, char *);
void send_invite(ActiveClients, Client *, const char *, char *);
void play_game(Client *, int);
void get_game_list(ActiveClients clients, Client *client, char *buffer);
void watch_user(ActiveClients clients, Client *client, char *username,
                char *buffer);
void change_bio(Client *client, char *bio);
void get_bio(ActiveClients clients, Client *client, char *username);
void send_friend_request(ActiveClients clients, Client *client,
                         const char *recipient_username);
void send_friend_list(Client *client);
void toggle_private_mode(Client *client);

#endif
