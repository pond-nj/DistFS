#pragma once

// need this to get pwrite().  I have to use setvbuf() instead of
// setlinebuf() later in consequence.
#define _XOPEN_SOURCE 500

#include "cache.h"

struct file_meta {
  char *name;
  size_t size;
};

struct fs_info {
  FILE *logfile;
  char *cache_dir;  // for aggregating file from remote when read, file should
                    // be closed when done reading
  struct file_meta *stored_files;
  int count_files;
  int *socket_connections;
};
#define FS_DATA ((struct fs_info *)(fuse_get_context()->private_data))