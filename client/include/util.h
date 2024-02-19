#pragma once
#include <stdarg.h>
#include <unistd.h>

int file_exist(const char* fname);
char* concat_path(int path_num, char* tmp_path, ...);
