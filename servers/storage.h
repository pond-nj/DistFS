struct file_data {
  char* name;
  size_t size;
};

struct meta {
  struct file_data* data;
  int count_files;
};