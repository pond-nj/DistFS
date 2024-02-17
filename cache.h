#include <stdio.h>

struct cache_meta {
  File* f;
  char* name;
}

struct caches {
  struct cache_meta* meta;
  int num_cache;
}