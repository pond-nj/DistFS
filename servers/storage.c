
#include <bits/fcntl-linux.h>
#include <stdarg.h>
#include <unistd.h>

char* DATA_PATH = "./data";

int file_exist(const char* fname) {
  if (access(fname, F_OK) == 0) {
    // file exists
    return 1;
  } else {
    // file doesn't exist
    return 0;
  }
}

void make_full_path(char* new_path, char* name) {
  int idx = 0;
  while (DATA_PATH[idx] != '\0') {
    new_path[idx] = DATA_PATH[idx];
    idx += 1;
  }
  new_path[idx++] = '/';
  strcpy(&new_path[idx], name);
}

int has_this_file(const char* name) {
  char full_path[PATH_MAX];
  make_full_path(full_path, name);

  return file_exist(concat_path(DATA_PATH, name));
};

ssize_t read_file(const char* name, char* buf, size_t size) {
  char full_path[PATH_MAX];
  make_full_path(full_path, name);
  int fd = open(full_path, O_RDONLY);

  ssize_t read_bytes = read(fd, buf, size);

  close(fd);

  return read_bytes;
}

ssize_t write_file(const char* name, char* buf, size_t size) {
  char full_path[PATH_MAX];
  make_full_path(full_path, name);

  int fd = open(full_path, O_WRONLY);

  ssize_t write_bytes = write(fd, buf, size);

  close(fd);

  return write_bytes;
}