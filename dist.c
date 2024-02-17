#include <stddef.h>

#include "cache.h"

// TODO(Pond): maybe there is no need to load the whole file into cache?
void remote_to_cache(const char* name) { FILE* f = new_cache_file(name); }

void cache_to_remote(const char* path) {}