#include "meta.h"

// shared metadata structure between client and server

struct meta init_meta(MAX_FILES) {
  MAX_FILES = 10;
  struct meta meta;
  meta.count_files = 0;
  meta.data = (struct file_data*)malloc(sizeof(struct file_data) * MAX_FILES);
  return meta;
}

void add_meta(char* name, size_t size) {
  if (server_meta.count_files + 1 > MAX_FILES) {
    MAX_FILES = MAX_FILES * 2;
    struct file_data* new_data = malloc(sizeof(struct file_data) * MAX_FILES);

    for (int i = 0; i < server_meta.count_files; i += 1) {
      new_data[i] = server_meta.data[i];
    }

    free(server_meta.data);
    server_meta.data = new_data;
  }

  server_meta.data[server_meta.count_files].name = name;
  server_meta.data[server_meta.count_files].size = size;
  server_meta.count_files += 1;
}

size_t get_file_size(const char* name) {
  for (int i = 0; i < server_meta.count_files; i += 1) {
    if (strcmp(server_meta.data[i].name, name) == 0) {
      return server_meta.data[i].size;
    }
  }

  return 0;
}