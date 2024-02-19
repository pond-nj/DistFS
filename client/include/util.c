#include "util.h"

int file_exist(const char* fname) {
  if (access(fname, F_OK) == 0) {
    // file exists
    return 1;
  } else {
    // file doesn't exist
    return 0;
  }
}

char* concat_path(int path_num, char* tmp_path, ...) {
  va_list args;
  va_start(args, tmp_path);

  for (int i = 0; i < path_num - 1; i++) {
    char* path = va_arg(args, char*);
    strcat(tmp_path, "/");
    strcat(tmp_path, path);
  }

  va_end(args);

  return tmp_path;
}