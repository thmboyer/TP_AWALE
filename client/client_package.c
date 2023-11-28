#include "client_package.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../options.h"

int process(char *buffer) {
  char *it = buffer;

  if (*it++ != '/') {
    char *p = NULL;
    p = strstr(buffer, "\n"); // looks for \n
    if (p != NULL) {
      *p = 0; // Replaces it with '\0'
    } else {
      buffer[BUF_SIZE - 1] = 0;
    }
    return 1;
  } else {
    char command[6];
    strncpy(command, it, 5);
    command[5] = '\0';

    if (!strcmp(command, "users")) {
      strcpy(buffer, "/000");
      return 1;
    } else if (!strcmp(command, "fight")) {
      it += 5;
      if (*(it++) != ' ') {
        puts("Expected cmd /fight <username>");
        return 0;
      }
      char *beginning_of_username = it;
      char *end_of_username;
      while (!isspace(*it)) {
        ++it;
      }
      end_of_username = it;
      int length_of_username = end_of_username - beginning_of_username;
      if (length_of_username >= USERNAME_SIZE) {
        puts("Username's length has to be inferior than USERNAME_SIZE");
        return 0;
      }
      strcpy(buffer, "/001 ");
      char username[USERNAME_SIZE];
      strncpy(username, beginning_of_username, length_of_username);
      for (int i = length_of_username; i < USERNAME_SIZE; ++i) {
        username[i] = '\0';
      }
      strcat(buffer, username);
      return 1;
    } else if (!strcmp(command, "games")) {
      strcpy(buffer, "/002");
      return 1;
    } else if (!strcmp(command, "plays")) {
      int num;
      if (sscanf(buffer, "%s %d", command, &num) == 2) {
        if (num >= 1 && num <= 6) {
          sprintf(buffer, "%s %d", "/003", num);
          return 1;
        } else {
          printf("The number must be between 1 and 6.\n");
          return 0;
        }
      } else {
        printf("Expected cmd /plays <pit_num>. \n");
        return 0; // Retourne 0 si le format est invalide
      }
    } else if (!strcmp(command, "watch")) {
      it += 5;
      if (*(it++) != ' ') {
        puts("Expected cmd /watch <username>");
        return 0;
      }
      char *beginning_of_username = it;
      char *end_of_username;
      while (!isspace(*it)) {
        ++it;
      }
      end_of_username = it;
      int length_of_username = end_of_username - beginning_of_username;
      if (length_of_username >= USERNAME_SIZE) {
        puts("Username's length has to be inferior than USERNAME_SIZE");
        return 0;
      }
      strcpy(buffer, "/004 ");
      char username[USERNAME_SIZE];
      strncpy(username, beginning_of_username, length_of_username);
      for (int i = length_of_username; i < USERNAME_SIZE; ++i) {
        username[i] = '\0';
      }
      strcat(buffer, username);
      return 1;
    } else {
      puts("Command does not exist..");
      return 0;
    }
    return 0;
  }
}
