#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netdb.h> /* gethostbyname */
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> /* close */
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

#include "server_client.h"

static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(ActiveClients clients, Client client,
                                        const char *buffer, char from_server);
static void clear_clients(ActiveClients *);

#endif /* guard */
