#include <stddef.h>
#include <sys.h>

#include "params.h"
#include "util.h"

int has_cache(const char* path) {
  char cache_path[PATH_MAX] = FS_DATA->cache_dir;
  cache_path = concat_path(2, cache_path, path);
  return file_exist(cache_path);
}

// use this to get FILE* and write into that to save in cache
FILE* f new_cache_file(const char* name) {
  char cache_path[PATH_MAX] = FS_DATA->cache_dir;
  cache_path = concat_path(2, cache_path, name);

  // TODO(Pond): error handling
  FILE* f = fopen(cache_path, "w");
  int num_cache = FS_DATA->caches->num_cache;
  FS_DATA->caches->meta[count_cache]->f = f;
  FS_DATA->caches->meta[count_cache]->name = name;
  FS_DATA->caches->num_cache += 1;

  return f;
}

int in_cache(const char* name) {
  int count_cache = FS_DATA->caches->num_cache;
  for (int i = 0; i < count_cache; i += 1) {
    if (strcmp(FS_DATA->caches->meta[i]->name, name) == 0) {
      return 1;
    }
  }
  return 0;
}

FILE* find_cache(const char* name) {
  int count_cache = FS_DATA->caches->num_cache;
  for (int i = 0; i < count_cache; i += 1) {
    if (strcmp(FS_DATA->caches->meta[i]->name, name) == 0) {
      return FS_DATA->caches->meta[i]->f;
    }
  }

  assert(0);  // not found
}

int read_from_cache(const char* name, char* buf, size_t size, off_t offset) {
  char cache_path[PATH_MAX] = FS_DATA->cache_dir;
  cache_path = concat_path(2, cache_path, name);

  FILE* f = find_cache(name);
  return pread(f, buf, size, offset);
}