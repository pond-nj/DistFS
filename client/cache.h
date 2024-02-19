
#pragma once
#include <stddef.h>
#include <stdio.h>
#include <sys.h>

#include "params.h"
#include "util.h"

struct cache_meta {
  int fd;
  char* name;
}

struct caches {
  struct cache_meta* meta;
  int num_open_caches;
}

int has_cache(const char* path);
int remember_open_cache(const char* name, int fd);
int open_cache_in_mem(const char* name);

int new_cache_file(const char* name);
int cache_in_mem(const char* name);

int read_from_cache(const char* name, char* buf, size_t size, off_t offset);

int write_to_cache(const char* name, char* buf, size_t size, off_t offset);
void remove_cache(const char* name);