#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int board[12];

int main(int argc, char **argv) {
  int sockfd, newsockfd, clilen, chilpid, ok, nleft, nbwriten, pid;
  struct sockaddr_in cli_addr, serv_addr;
  char c;

  if (argc != 2) {
    printf("usage: socket_server port\n");
    exit(0);
  }

  printf("server starting...\n");

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("impossible d'ouvrir le socket\n");
    exit(0);
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("impossible de faire le bind\n");
    exit(0);
  }

  listen(sockfd, 2); // Le backlog correspond au nombre de sockets en file
                     // d'attente, pas au nombre de sockets acceptées.

  clilen = sizeof(cli_addr);

  while (1) {
    // Protection contre le ddos à faire avec un nombre max d'enfants qu'on peut
    // avoir, on le vérifie avant d'accepter une nouvelle demande.
    if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                            (socklen_t *)&clilen)) == -1) {
      printf("Could not accept a connexion !\n");
      continue;
    }
    pid = fork();
    if (pid == 0) {
      close(sockfd);

      if (newsockfd < 0) {
        printf("accept error\n");
        exit(0);
      }
      printf("connection accepted\n");

      while (1)
      // SMART !
      {
        int read_value;
        while ((read_value = read(newsockfd, &c, 1)) != 1) {
          if (!read_value) {
            printf("Client disconnected !\nClosing the server...\n");
            close(sockfd);
            close(newsockfd);
            exit(0);
          }
        }
        if (c == EOF) {
          printf("Client ended the exchange !\nClosing the socket...\n");
          close(sockfd);
          close(newsockfd);
          exit(0);
        }
        printf("%c", c);
      }
    } else {
      close(newsockfd);
    }
  }

  /*  attention il s'agit d'une boucle infinie
   *  le socket nn'est jamais ferme !
   */

  return 1;
}