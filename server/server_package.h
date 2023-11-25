#ifndef SERVER_PACKAGE_H
#define SERVER_PACKAGE_H
#include "server_client.h"

typedef struct ServerPackage {
  int nb_args;
  char **args;
} ServerPackage;

void handle_incomming_package(const ActiveClients clients, Client *client,
                              char *message);
static ServerPackage parse_incomming_package(char *message);
#endif
