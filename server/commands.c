#include "commands.h"
#include <string.h>

// WARNING: buffer size may be too small if there are too many clients, reduce
// nb of clients in options according to BUF_SIZE
void get_client_list(ActiveClients clients, char *buffer) {
  Client *client_iterator = clients.first;
  buffer[0] = '\0';
  while (client_iterator) {
    strcat(buffer, client_iterator->username);
    if (client_iterator != clients.last) {
      strcat(buffer, "\n");
    }
    client_iterator = client_iterator->next;
  }
}
