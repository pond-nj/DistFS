#include <unistd.h>

// shared metadata structure between client and server

struct file_data {
  char* name;
  size_t size;
};

struct meta {
  struct file_data* data;
  int count_files;
};

int MAX_FILES;
struct meta server_meta;

struct meta init_meta(MAX_FILES);
void add_meta(char* name, size_t size);
size_t get_file_size(const char* name);