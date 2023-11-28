#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "server_client.h"
#include "server_package.h"

void handle_incomming_package(const ActiveClients clients, Client *client,
                              char *message) {
  char *it = message;
  if (*(it++) != '/') {
    send_message_to_all_clients(clients, *client, message, 0);
  } else {
    char command_str[4]; // Command enconded on 3 chars
    strncpy(command_str, it, 3);
    command_str[3] = '\0';
    int command = atoi(command_str);
    switch (command) {
    case 0:
      get_client_list(clients, client, message);
      break;
    case 1:
      it += 4;
      char username[USERNAME_SIZE];
      strncpy(username, it, USERNAME_SIZE);
      send_invite(clients, client, username, message);
      break;
    case 2:
      get_game_list(clients, client, message);
      break;
    case 3:
      it += 4;
      int num = atoi(it);
      play_game(client, num);
      break;
    default:
      break;
    }
  }
}
