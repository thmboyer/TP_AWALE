#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "client_package.h"

static void app(const char *address, const char *name) {
  SOCKET sock = init_connection(address);
  char buffer[BUF_SIZE];

  fd_set rdfs; // set of file descriptors

  assert(strlen(name) < USERNAME_SIZE &&
         "username length must be <= USERNAME_SIZE");

  write_server(sock, name); // Sending username to the server

  while (1) {
    FD_ZERO(&rdfs); // Clears the set

    FD_SET(STDIN_FILENO, &rdfs); // Adds stdin to the set

    FD_SET(sock, &rdfs); // Adds the socket to the set

    if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1) {
      perror("select()");
      exit(errno);
    }

    if (FD_ISSET(STDIN_FILENO, &rdfs)) { // If something from the keyboard
      if (fgets(buffer, BUF_SIZE - 1, stdin)) {
        if (!process(buffer)) {
          continue;
        }
      } else {
        break;
      }
      write_server(sock, buffer);
    } else if (FD_ISSET(sock, &rdfs)) { // If something from the socket
      int n = read_server(sock, buffer);
      if (n == 0) { // server down
        printf("Server disconnected !\n");
        break;
      }
      puts(buffer);
    }
  }

  end_connection(sock);
}

static int init_connection(const char *address) {
  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
  SOCKADDR_IN sin = {0};
  struct hostent *hostinfo;

  if (sock == INVALID_SOCKET) {
    perror("socket()");
    exit(errno);
  }

  hostinfo = gethostbyname(address);
  if (hostinfo == NULL) {
    fprintf(stderr, "Unknown host %s.\n", address);
    exit(EXIT_FAILURE);
  }

  sin.sin_addr = *(IN_ADDR *)hostinfo->h_addr;
  sin.sin_port = htons(PORT);
  sin.sin_family = AF_INET;

  if (connect(sock, (SOCKADDR *)&sin, sizeof(SOCKADDR)) == SOCKET_ERROR) {
    perror("connect()");
    exit(errno);
  }

  return sock;
}

static void end_connection(int sock) { closesocket(sock); }

static int read_server(SOCKET sock, char *buffer) {
  int n = 0;

  if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
    perror("recv()");
    exit(errno);
  }

  buffer[n] = 0;

  return n;
}

static void write_server(SOCKET sock, const char *buffer) {
  if (send(sock, buffer, strlen(buffer), 0) < 0) {
    perror("send()");
    exit(errno);
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage : %s [address] [pseudo]\n", argv[0]);
    return EXIT_FAILURE;
  }

  app(argv[1], argv[2]);

  return EXIT_SUCCESS;
}
