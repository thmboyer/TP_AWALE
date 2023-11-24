#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "server.h"
#include "server_client.h"

// INFO: See infos for places where the code will have to change because of our
// data strcture choices
static void app(void) {
  SOCKET sock = init_connection(); // On récupère le FD de la socket du server.

  char buffer[BUF_SIZE]; // Char buffer for a message.

  int max = sock; // The filedescriptor with the highest number, needed for the
                  // select() call

  ActiveClients clients; // INFO: We should not need it since we have a
                         // linked list with the Clients struct.
  clients.first_client = NULL;
  clients.nb = 0;

  fd_set rdfs; // Set of file descriptors. Before select : all the file
               // descriptors. After select: only the file descriptors that
               // are ready for reading.

  while (1)

  {
    Client *client_iterator = clients.first_client;
    FD_ZERO(&rdfs); // Clears the set

    FD_SET(STDIN_FILENO, &rdfs); // Adds STDIN to the set

    FD_SET(sock, &rdfs); // Adds the server socket to the set

    while (client_iterator) { // Adds the socket of every client
      FD_SET(client_iterator->socket, &rdfs);
      client_iterator = client_iterator->next;
    }

    if (select(max + 1, &rdfs, NULL, NULL, NULL) ==
        -1) { // Blocks until there is a readable socket, clears all the sockets
              // from the set except the readable ones
      perror("select()");
      exit(errno);
    }

    if (FD_ISSET(STDIN_FILENO,
                 &rdfs)) { // If stdin is readable, we stop the app
      break;
    } else if (FD_ISSET(sock, &rdfs)) { // If the server socket is readable,
                                        // there is a new client
      SOCKADDR_IN csin = {0};
      size_t sinsize = sizeof csin;
      int csock = accept(
          sock, (SOCKADDR *)&csin,
          (socklen_t *)&sinsize); // Accepts the connection from the new client
      if (csock == SOCKET_ERROR) {
        perror("accept()");
        continue;
      }

      if (read_client(csock, buffer) ==
          0) { // WARNING: I think it waits for the client to send his username
               // but I am not sure. If it is that way, the client app should
               // require the client to specify his username in execution
               // command

        continue; // He disconnected
      }

      Client *c = malloc(sizeof(Client));
      c->socket = csock;
      c->game = NULL;
      c->opponent = NULL;
      c->connected = 0;
      c->next = NULL;
      c->previous = NULL;

      strncpy(c->username, buffer, USERNAME_SIZE);

      if (!add_client(&clients, c)) {
        continue;
      }

      max = csock > max ? csock : max; // We update the max value if we need to

      FD_SET(csock, &rdfs); // We add the client socket to the set

    } else { // In this case at least a client socket is readable.
      client_iterator = clients.first_client;
      while ((client_iterator = client_iterator->next)) {
        if (FD_ISSET(client_iterator->socket, &rdfs)) {
          int c = read_client(client_iterator->socket, buffer);
          if (c == 0) { // The client disconnected
            closesocket(client_iterator->socket);
            remove_client(&clients, client_iterator);
            strncpy(buffer, client_iterator->username, BUF_SIZE - 1);
            strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
            send_message_to_all_clients(clients, *client_iterator, buffer, 1);
          } else { // INFO: This is where we go into handle_incomming_package();
            send_message_to_all_clients(clients, *client_iterator, buffer, 0);
          }
          break;
        }
      }
    }
  }

  clear_clients(&clients);
  end_connection(sock);
}

static void clear_clients(ActiveClients *clients) {
  clients->nb = 0;
  Client *client_iterator = clients->first_client;
  while (client_iterator) {
    closesocket(client_iterator->socket);
    Client *previous = client_iterator;
    client_iterator = client_iterator->next;
    free(previous);
  }
}

static void send_message_to_all_clients(ActiveClients clients, Client sender,
                                        const char *buffer, char from_server) {
  int i = 0;
  char message[BUF_SIZE];
  Client *client_iterator = clients.first_client;
  while ((client_iterator = client_iterator->next)) {
    message[0] = 0;
    {
      /* we don't send message to the sender */
      if (sender.socket != client_iterator->socket) {
        if (from_server == 0) {
          strncpy(message, sender.username, BUF_SIZE - 1);
          strncat(message, " : ", sizeof message - strlen(message) - 1);
        }
        strncat(message, buffer, sizeof message - strlen(message) - 1);
        write_client(client_iterator->socket, message);
      }
    }
  }
}

static int init_connection(void) {
  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); // Socket IPV4 TCP
  SOCKADDR_IN sin = {0};

  if (sock == INVALID_SOCKET) {
    perror("socket()");
    exit(errno);
  }

  sin.sin_addr.s_addr = htonl(INADDR_ANY); // Toutes les interfaces du PC
  sin.sin_port = htons(PORT);              // PORT définit dans server.h
  sin.sin_family = AF_INET;                // IPV4

  if (bind(sock, (SOCKADDR *)&sin, sizeof sin) ==
      SOCKET_ERROR) { // Bind la socket aux paramètres définits.
    perror("bind()");
    exit(errno);
  }

  if (listen(sock, MAX_CLIENTS) ==
      SOCKET_ERROR) { // Marks the socket as passive socket. This call is non
                      // blocking
    perror("listen()");
    exit(errno);
  }

  return sock;
}

static void end_connection(int sock) { closesocket(sock); }

static int read_client(SOCKET sock, char *buffer) {
  int n = 0;

  if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
    perror("recv()");
    n = 0;
  }

  buffer[n] = '\0';

  return n;
}

static void write_client(SOCKET sock, const char *buffer) {
  if (send(sock, buffer, strlen(buffer), 0) < 0) {
    perror("send()");
    exit(errno);
  }
}

int main(int argc, char **argv) {

  app();

  return EXIT_SUCCESS;
}
