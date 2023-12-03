#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netdb.h> /* gethostbyname */
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> /* close */

#include "server_client.h"

static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void clear_clients(ActiveClients *);
void load_games(Games *games);
int get_last_id(Games *games);

#endif /* guard */
