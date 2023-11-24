#ifndef SERVER_PACKAGE_H
#define SERVER_PACKAGE_H

typedef struct ServerPackage {
  int nb_args;
  char **args;
} ServerPackage;

void handle_incomming_package(char message[200]);
static ServerPackage parse_incomming_package(char message[200]);
#endif
