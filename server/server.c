#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

// INFO: See infos for places where the code will have to change because of our
// data strcture choices
static void app(void) {
  SOCKET sock = init_connection(); // On récupère le FD de la socket du server.

  char buffer[BUF_SIZE]; // Char buffer for a message.

  int actual = 0; // INFO: The number of clients, we should not need it since we
                  // have a linked list

  int max = sock; // The filedescriptor with the highest number, needed for the
                  // select() call

  Client clients[MAX_CLIENTS]; // INFO: We should not need it since we have a
                               // linked list with the Clients struct.

  fd_set rdfs; // Set of file descriptors. Before select : all the file
               // descriptors. After select: only the file descriptors that
               // are ready for reading.

  while (1)

  {
    int i = 0;
    FD_ZERO(&rdfs); // Clears the set

    FD_SET(STDIN_FILENO, &rdfs); // Adds STDIN to the set

    FD_SET(sock, &rdfs); // Adds the server socket to the set

    for (i = 0; i < actual; i++) { // Adds the socket of every client
      FD_SET(clients[i].fd, &rdfs);
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
          -1) { // WARNING: I think it waits for the client to send his username
                // but I am not sure. If it is that way, the client app should
                // require the client to specify his username in execution
                // command

        continue; // He disconnected
      }

      max = csock > max ? csock : max; // We update the max value if we need to

      // BUG: Breakpoint where I finished reading

      FD_SET(csock, &rdfs);

      Client c = {csock};
      // FIX: Needs to be changed, changed it to stop warning me but it was
      // c.username, buffer, BUF_SIZE
      strncpy(c.username, buffer, 9);
      clients[actual] = c;
      actual++;
    } else {
      int i = 0;
      for (i = 0; i < actual; i++) {
        /* a client is talking */
        if (FD_ISSET(clients[i].fd, &rdfs)) {
          Client client = clients[i];
          int c = read_client(clients[i].fd, buffer);
          /* client disconnected */
          if (c == 0) {
            closesocket(clients[i].fd);
            remove_client(clients, i, &actual);
            strncpy(buffer, client.username, BUF_SIZE - 1);
            strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
            send_message_to_all_clients(clients, client, actual, buffer, 1);
          } else {
            send_message_to_all_clients(clients, client, actual, buffer, 0);
          }
          break;
        }
      }
    }
  }

  clear_clients(clients, actual);
  end_connection(sock);
}

static void clear_clients(Client *clients, int actual) {
  int i = 0;
  for (i = 0; i < actual; i++) {
    closesocket(clients[i].fd);
  }
}

static void remove_client(Client *clients, int to_remove, int *actual) {
  /* we remove the client in the array */
  memmove(clients + to_remove, clients + to_remove + 1,
          (*actual - to_remove - 1) * sizeof(Client));
  /* number client - 1 */
  (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender,
                                        int actual, const char *buffer,
                                        char from_server) {
  int i = 0;
  char message[BUF_SIZE];
  message[0] = 0;
  for (i = 0; i < actual; i++) {
    /* we don't send message to the sender */
    if (sender.fd != clients[i].fd) {
      if (from_server == 0) {
        strncpy(message, sender.username, BUF_SIZE - 1);
        strncat(message, " : ", sizeof message - strlen(message) - 1);
      }
      strncat(message, buffer, sizeof message - strlen(message) - 1);
      write_client(clients[i].fd, message);
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
  // WARNING: Changed this function a bit because I think there was an errror
  int n = 0;

  if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
    perror("recv()");
  }

  buffer[n >= 0 ? n : 0] = '\0';

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
