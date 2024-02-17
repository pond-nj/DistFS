#include <stdio.h>

struct cache_meta {
  int fd;
  char* name;
}

struct caches {
  struct cache_meta* meta;
  int num_open_caches;
}