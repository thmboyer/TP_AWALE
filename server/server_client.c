#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server_client.h"

int add_client(ActiveClients *clients, Client *client) {
  if (clients->nb == MAX_CLIENTS) {
    return 0;
  }
  if (!clients->first) {
    clients->first = client;
    clients->last = client;
    ++clients->nb;
    return 1;
  } else {
    clients->last->next = client;
    client->previous = clients->last;
    clients->last = client;
    ++clients->nb;
    return 1;
  }
}

void remove_client(ActiveClients *clients, Client *client) {
  if (clients->first == client) {
    clients->first = clients->first->next;
    --clients->nb;
  } else if (clients->last == client) {
    clients->last = clients->last->previous;
    clients->last->next = NULL;
    --clients->nb;
  } else {
    client->previous->next = client->next;
    client->next->previous = client->previous;
    --clients->nb;
  }
  remove_invites_from_client(client);
  if (client->opponent) {
    assert(client->game &&
           "If a client has an opponent, he should have a game");
    client->opponent->opponent = NULL;
    client->opponent->game = NULL;
    write_client(client->opponent->socket,
                 "\nYour opponent left, you won the game.");
    char message_to_observers[200];
    strcpy(message_to_observers, "\n");
    strcat(message_to_observers, client->opponent->username);
    strcat(message_to_observers, " won the game, his opponent left");
    send_message_to_all_observers(client->opponent->observers,
                                  message_to_observers);
    // free(client->game);
    delete_game(client->game);
  }
  if (client->watching) {
    remove_observer(client->watching->observers, client);
  }
  Observer *observer = client->observers->first;
  while (observer) {
    observer->watcher->watching = NULL;
    write_client(observer->watcher->socket,
                 "The client you were watching went offline, you are no longer "
                 "watching anyone.");
    Observer *temp = observer;
    observer = observer->next;
    free(temp);
  }
  Friend *friend_it = client->friends->first;
  while (friend_it) {
    remove_friend(friend_it->friend_of_client->friends, client);
    Friend *temp = friend_it;
    friend_it = friend_it->next;
    free(temp);
  }

  free(client->friends);
  free(client->observers);
  free(client);
}

int add_observer(Observers *observers, Observer *observer) {
  if (!observers->first) {
    observers->first = observer;
    observers->last = observer;
    return 1;
  } else {
    observers->last->next = observer;
    observer->previous = observers->last;
    observers->last = observer;
    return 1;
  }
}

void remove_observer(Observers *observers, Client *client) {
  Observer *observer = observers->first;
  while (observer) {
    if (!strcmp(observer->watcher->username, client->username)) {
      break;
    }
    observer = observer->next;
  }
  if (!observer) {
    printf("Client not found in observers\n");
    return;
  }
  if (observers->first == observer) {
    observers->first = observers->first->next;
  } else if (observers->last == observer) {
    observers->last = observers->last->previous;
    observers->last->next = NULL;
  } else {
    observer->previous->next = observer->next;
    observer->next->previous = observer->previous;
  }
  free(observer);
}

int add_friend(FriendList *friends, Friend *new_friend) {
  if (!friends->first) {
    friends->first = new_friend;
    friends->last = new_friend;
    return 1;
  } else {
    friends->last->next = new_friend;
    new_friend->previous = friends->last;
    friends->last = new_friend;
    return 1;
  }
}

void remove_friend(FriendList *friends, Client *client) {
  Friend *friend_it = friends->first;
  while (friend_it) {
    if (!strcmp(friend_it->friend_of_client->username, client->username)) {
      break;
    }
    friend_it = friend_it->next;
  }
  if (!friend_it) {
    printf("Client not found in friendlist\n");
    return;
  }
  if (friends->first == friend_it) {
    friends->first = friends->first->next;
  } else if (friends->last == friend_it) {
    friends->last = friends->last->previous;
    friends->last->next = NULL;
  } else {
    friend_it->previous->next = friend_it->next;
    friend_it->next->previous = friend_it->previous;
  }
  free(friend_it);
}

int add_invite(Invites *invites, Client *recipient) {
  if (invites->first == NULL) {
    Invite *invite = malloc(sizeof(Invite));
    invites->first = invite;
    invite->recipient = recipient;
    invite->next = NULL;
    return 1;
  } else {
    Invite *it_invite = invites->first;
    while (it_invite->next != NULL) {
      if (!strcmp(it_invite->recipient->username, recipient->username)) {
        return 0;
      }
      it_invite = it_invite->next;
    }
    if (!strcmp(it_invite->recipient->username, recipient->username)) {
      return 0;
    }
    Invite *invite = malloc(sizeof(Invite));
    it_invite->next = invite;
    invite->recipient = recipient;
    invite->next = NULL;
    return 1;
  }
}

void remove_invite(Invites *invites, Invite *invite) {
  Invite *it_invite = invites->first;
  if (it_invite == invite) {
    invites->first = it_invite->next;
    free(it_invite);
    return;
  }
  while (it_invite->next != invite) {
    it_invite = it_invite->next;
  }
  Invite *invite_to_remove = it_invite->next;
  it_invite->next = it_invite->next->next;
  free(invite_to_remove);
}

void remove_invites_from_client(Client *client) {
  Invite *invite = client->invites->first;
  while (invite) {
    Invite *previous_invite = invite;
    invite = invite->next;
    free(previous_invite);
  }
  client->invites->first = NULL;
}

int is_in_invites(const Invites *invites, const Client *recipient) {
  Invite *it_invite = invites->first;
  while (it_invite) {
    if (!strcmp(it_invite->recipient->username, recipient->username)) {
      return 1;
    }
    it_invite = it_invite->next;
  }
  return 0;
}

Client *find_client_by_username(const ActiveClients clients,
                                const char *username) {
  Client *client_iterator = clients.first;
  while (client_iterator != NULL) {
    if (!strcmp(client_iterator->username, username)) {
      break;
    }
    client_iterator = client_iterator->next;
  }
  return client_iterator;
}

void send_message_to_all_clients(ActiveClients clients, Client sender,
                                 const char *buffer, char from_server) {
  char message[BUF_SIZE];
  Client *client_iterator = clients.first;
  while (client_iterator) {
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
    client_iterator = client_iterator->next;
  }
}

void send_message_to_all_observers(Observers *observers, const char *message) {
  Observer *observer_iterator = observers->first;
  while (observer_iterator) {
    write_client(observer_iterator->watcher->socket, message);
    observer_iterator = observer_iterator->next;
  }
}

void write_client(SOCKET sock, const char *buffer) {
  if (send(sock, buffer, strlen(buffer), 0) < 0) {
    perror("send()");
    exit(errno);
  }
}

int is_in(Client *client, ActiveClients list_of_clients) {
  Client *client_iterator = list_of_clients.first;
  while (client_iterator) {
    if (!strcmp(client_iterator->username, client->username)) {
      return 1;
    }
    client_iterator = client_iterator->next;
  }
  return 0;
}

void remove_invite_to_new_friend(Client *client, Client *new_friend_client) {
  Invite *invite_it = client->friend_requests_sent->first;
  while (invite_it) {
    if (!strcmp(invite_it->recipient->username, new_friend_client->username)) {
      break;
    }
    invite_it = invite_it->next;
  }
  if (!invite_it) {
    printf("Invite not found in friendlist, should not be happening\n");
    return;
  }
  remove_invite(client->friend_requests_sent, invite_it);
}
