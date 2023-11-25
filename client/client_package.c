#include "client_package.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
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
      if ((end_of_username - beginning_of_username) >= USERNAME_SIZE) {
        puts("Username's length has to be inferior than USERNAME_SIZE");
        return 0;
      }
      strcpy(buffer, "/001 ");
      char username[end_of_username - beginning_of_username + 1];
      strncpy(username, beginning_of_username,
              end_of_username - beginning_of_username);
      username[end_of_username - beginning_of_username] = '\0';
      strcat(buffer, username);
      return 1;
    } else {
      puts("Command does not exist");
      return 0;
    }
  }
  return 0;
}
