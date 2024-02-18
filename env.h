#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int NUM_REMOTE;

struct server {
  char* address;
  int port;
};

// TODO(Pond): read server info from .env
struct server* get_remotes_list() {
  char* line = NULL;
  size_t len = 0;
  ssize_t read;

  FILE* f = fopen("./.env", "r");
  if (f == NULL) exit(EXIT_FAILURE);

  read = getline(&line, &len, f);
  NUM_REMOTE = atoi(line);

  struct server* servers =
      (struct server*)malloc(sizeof(struct server) * NUM_REMOTE);

  int count_remote = 0;
  while ((read = getline(&line, &len, f)) != -1) {
    char* ip = (char*)malloc(sizeof(char) * 20);
    int idx = 0;
    while (line[idx] != ' ') {
      idx += 1;
    }
    line[idx++] = '\0';

    servers[count_remote].address = line;
    servers[count_remote++].port = atoi(&line[idx]);
  }
  fclose(f);

  return servers;
}