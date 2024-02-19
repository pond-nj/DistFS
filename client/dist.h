
#pragma once
#include <string.h>

void connect_remote();

int is_server_down(int sd);
size_t get_fragment_size(size_t file_size);
ssize_t read_fragment(const char* name, int sd, char* buf, size_t max_size);
void send_fragment(const char* name, int sd, char* buf, size_t fragment_size);
void remote_to_cache(const char* name);
void cache_to_remote(const char* name);