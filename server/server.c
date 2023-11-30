#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "server.h"
#include "server_client.h"
#include "server_package.h"

static void app(void) {
  SOCKET sock = init_connection(); // On récupère le FD de la socket du server.

  char buffer[BUF_SIZE]; // Char buffer for a message.

  int max = sock; // The filedescriptor with the highest number, needed for the
                  // select() call

  ActiveClients clients;

  clients.first = NULL;
  clients.last = NULL;
  clients.nb = 0;

  fd_set rdfs; // Set of file descriptors. Before select : all the file
               // descriptors. After select: only the file descriptors that
               // are ready for reading.

  while (1)

  {
    Client *client_iterator = clients.first;
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

      if (read_client(csock, buffer) == 0) {

        continue; // He disconnected
      }

      Client *c = malloc(sizeof(Client));
      Invites *invites = malloc(sizeof(Invites));
      Invites *friend_requests_sent = malloc(sizeof(Invites));
      Observers *observers = malloc(sizeof(Observers));
      FriendList *friendList = malloc(sizeof(FriendList));
      c->socket = csock;
      c->game = NULL;
      c->opponent = NULL;
      c->connected = 0;
      c->next = NULL;
      c->previous = NULL;
      c->watching = NULL;
      c->invites = invites;
      c->invites->first = NULL;
      c->friend_requests_sent = friend_requests_sent;
      c->friend_requests_sent->first = NULL;
      c->observers = observers;
      c->observers->first = NULL;
      c->observers->last = NULL;
      c->friends = friendList;
      c->friends->first = NULL;
      c->friends->last = NULL;

      strncpy(c->username, buffer, USERNAME_SIZE);
      strcpy(c->bio, "This user has yet to write his bio.");

      if (!add_client(&clients, c)) {
        continue;
      }

      max = csock > max ? csock : max; // We update the max value if we need to

      FD_SET(csock, &rdfs); // We add the client socket to the set

    } else { // In this case at least a client socket is readable.
      client_iterator = clients.first;
      while (client_iterator) {
        if (FD_ISSET(client_iterator->socket, &rdfs)) {
          int c = read_client(client_iterator->socket, buffer);
          if (c == 0) { // The client disconnected
            closesocket(client_iterator->socket);
            strncpy(buffer, client_iterator->username, BUF_SIZE - 1);
            strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
            send_message_to_all_clients(clients, *client_iterator, buffer, 1);
            remove_client(&clients, client_iterator);
          } else { // INFO: This is where we go into handle_incomming_package();
            handle_incomming_package(clients, client_iterator, buffer);
            // send_message_to_all_clients(clients, *client_iterator, buffer,
            // 0); write_client(client_iterator->socket, buffer);
          }
          break; // ISSUE: This can cause starvation if the first process keeps
                 // talking ?
        }
        client_iterator = client_iterator->next;
      }
    }
  }

  clear_clients(&clients);
  end_connection(sock);
}

static void clear_clients(ActiveClients *clients) {
  clients->nb = 0;
  Client *client_iterator = clients->first;
  while (client_iterator) {
    Invite *invite_it = client_iterator->invites->first;
    while (invite_it) {
      Invite *previous = invite_it;
      invite_it = invite_it->next;
      free(previous);
    }
    closesocket(client_iterator->socket);
    Client *previous = client_iterator;
    client_iterator = client_iterator->next;
    free(previous);
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

int main(int argc, char **argv) {

  app();

  return EXIT_SUCCESS;
}
