#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "server_client.h"
#include "server_package.h"

void handle_incomming_package(ActiveClients clients, Client client,
                              char message[200]) {
  char *it = message;
  if (*(it++) != '/') {
    send_message_to_all_clients(clients, client, message, 0);
  } else {
    char command_str[4]; // Command enconded on 3 chars
    strncpy(command_str, it, 3);
    command_str[3] = '\0';
    int command = atoi(command_str);
    switch (command) {
    case 0:
      get_client_list(clients, message);
      write_client(client.socket, message);
      break;
    default:
      break;
    }
  }
}
