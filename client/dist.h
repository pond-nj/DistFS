#include <string.h>

#define NUM_REMOTE 4

struct server {
  char* address;
  int port;
};

// TODO(Pond): read server info from .env
struct server* get_remotes_list() {
  struct server* servers =
      (struct server*)malloc(sizeof(struct server) * NUM_REMOTE);

  for (int i = 0; i < NUM_REMOTE) {
    servers[i].address = "127.0.0.1";
    servers[i].port = 5000 + i;
  }

  Not implemented

      return servers;
}