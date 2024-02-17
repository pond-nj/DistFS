#include "params.h"

size_t find_find_size(const char* name) {
  struct file_meta* stored_files = FS_DATA->stored_files;
  int count_files = FS_DATA->count_files;

  for (int i = 0; i < count_files; i += 1) {
    if (strcmp(stored_files[i].name, name) == 0) {
      return stored_files[i].size;
    }
  }

  return 0;
}