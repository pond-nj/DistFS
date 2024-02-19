// TODO(Pond): might be better to cache to use mmap?

#include "cache.h"

int has_cache(const char* path) {
  char cache_path[PATH_MAX] = FS_DATA->cache_dir;
  cache_path = concat_path(2, cache_path, path);
  return file_exist(cache_path);
}

// remember in mem that is already open
int remember_open_cache(const char* name, int fd) {
  int num_cache = FS_DATA->caches->num_cache;
  FS_DATA->caches->meta[count_cache]->fd = fd;
  FS_DATA->caches->meta[count_cache]->name = name;
  FS_DATA->caches->num_cache += 1;
}

// there must exist cache file, but might not yet be open in memory
int open_cache_in_mem(const char* name) {
  assert(has_cache(name));

  int fd;
  fd = cache_in_mem(name);  // try to see if it's already open first
  if (fd != -1) {
    return fd;
  }

  assert(fd == -1);

  char cache_path[PATH_MAX] = FS_DATA->cache_dir;
  cache_path = concat_path(2, cache_path, name);

  fd = open(cache_path, O_RDONLY);
  remember_open_cache(fd, name);

  return fd;
}

// use this to get fd and write into that to save in cache
// will create new file
int new_cache_file(const char* name) {
  assert(!has_cache(name));

  char cache_path[PATH_MAX] = FS_DATA->cache_dir;
  cache_path = concat_path(2, cache_path, name);

  // TODO(Pond): error handling
  int fd = open(cache_path, O_WRONLY);
  remember_open_cache(f, name);
  return fd;
}

// check if this cache file name is already open, so no need to re-open
int cache_in_mem(const char* name) {
  int count_cache = FS_DATA->caches->num_open_caches;
  for (int i = 0; i < count_cache; i += 1) {
    if (strcmp(FS_DATA->caches->meta[i]->name, name) == 0) {
      return FS_DATA->caches->meta[i]->fd;
    }
  }

  return -1;
}

int read_from_cache(const char* name, char* buf, size_t size, off_t offset) {
  assert(has_cache(name));
  char cache_path[PATH_MAX] = FS_DATA->cache_dir;
  cache_path = concat_path(2, cache_path, name);

  int fd = cache_in_mem(name);
  if (fd == null) {
    fd = open_cache_in_mem(name);
  }

  return pread(fd, buf, size, offset);
}

int write_to_cache(const char* name, char* buf, size_t size, off_t offset) {
  int fd;

  if (!has_cache(name)) {
    fd = new_cache_file(name);
  } else {
    fd = open_cache_in_mem(name);
  }

  return pwrite(fd, buf, size, offset);
}

int remove_cache(const char* name) {
  char cache_path[PATH_MAX] = FS_DATA->cache_dir;
  cache_path = concat_path(2, cache_path, path);

  return unlink(cache_path);
}