#ifndef OPTIONS_H
#define OPTIONS_H
#define USERNAME_SIZE 10
#define MAX_CLIENTS 25
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#define CRLF "\r\n"
#define PORT 7777

#define BUF_SIZE 1024
#endif
