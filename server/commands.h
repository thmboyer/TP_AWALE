#ifndef COMMANDS_H
#define COMMANDS_H
#include "server_client.h"

void get_client_list(ActiveClients, Client *, char *);
void send_invite(ActiveClients, Client *, const char *, char *);

#endif
