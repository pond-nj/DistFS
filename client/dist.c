#include "dist.h"

#include <arpa/inet.h>
#include <protocol.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cache.h"
#include "env.h"
#include "meta.h"
#include "params.h"

void connect_remote() {
  struct server* servers = get_remotes_list();
  int* connections = FS_DATA->socket_connections;

  for (int i = 0; i < NUM_REMOTE; i += 1) {
    struct sockaddr_in serv_addr;
    int sd;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("\n Socket creation error \n");
      return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(servers[i].port);

    // convert IP in char* format to binary
    if (inet_pton(AF_INET, servers[i].address, &serv_addr.sin_addr) <= 0) {
      printf("\nInvalid address/ Address not supported \n");
      return -1;
    }

    if (connect(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      printf("\nConnection Failed \n");
      return -1;
    }

    connections[i] = sd;
  }
}

int is_server_down(int sd) {
  char* c;
  ssize_t x = recv(sd, &c, 1, MSG_PEEK);
  if (x <= 0)
    return 1;
  else
    return 0;
}

size_t get_fragment_size(size_t file_size) {
  int NUM_STORAGE = NUM_REMOTE - 1;
  return (file_size + NUM_STORAGE - 1) / NUM_STORAGE;
}

ssize_t read_fragment(const char* name, int sd, char* buf, size_t max_size) {
  // TODO(Pond): should we do loop to read??
  char msg_buf[MSG_BUF_LEN];
  encrypt(new_message(READ, max_size, name), msg_buf);
  send(sd, msg_buf, MSG_BUF_LEN, 0);
  // TODO(Pond): do assert check on what read returns
  ssize_t read_bytes = read(sd, buf, max_size);
  assert(read_bytes == max_size);

  encrypt(message_ok(), msg_buf);
  return send(sd, msg_buf, MSG_BUF_LEN, 0);
}

void send_fragment(const char* name, int sd, char* buf, size_t fragment_size) {
  // TODO(Pond): should we distinguish between read and write operation?
  // TODO(Pond): is send a blocking operation?
  char msg_buf[MSG_BUF_LEN];
  encrypt(new_message(READ, max_size, name), msg_buf);
  send(sd, msg_buf, MSG_BUF_LEN, 0);

  read(sd, msg_buf, MSG_BUF_LEN);
  struct message m = decrypt(msg_buf);
  assert(m.op == OK);

  // TODO(Pond): should we do checking if server is ready to receive file?
  send(sd, buf, fragment_size, 0);

  read(sd, msg_buf, MSG_BUF_LEN);
  m = decrypt(msg_buf);
  assert(m.op == OK);
}

// TODO(Pond): maybe there is no need to load the whole file into cache?
void remote_to_cache(const char* name) {
  int fd = new_cache_file(name);

  int down_server = -1;
  for (int i = 0; i < NUM_REMOTE; i += 1) {
    if (is_server_down(FS_DATA->socket_connections[i])) {
      down_server = i;
    }
  }

  size_t file_size = find_find_size(name);
  size_t one_fragment_size = get_fragment_size(file_size);
  char buf[one_fragment_size];
  if (down_server == -1 || down_server == NUM_REMOTE - 1) {
    // no server is down or the parity server is down

    for (int i = 0; i < NUM_REMOTE - 1; i += 1) {
      ssize_t read_bytes = read_fragment(name, FS_DATA->socket_connections[i],
                                         buf, one_fragment_size);
      write(fd, buf, read_bytes);
    }
  } else {
    // one server that is not parity server is down
    char restore_buf[one_fragment_size];
    // TODO(Pond): do we need memset here? or is it already zero?
    memset(restore_buf, 0, sizeof(char) * one_fragment_size);

    for (int i = 0; i < NUM_REMOTE; i += 1) {
      if (i != down_server) {
        ssize_t read_bytes = read_fragment(name, FS_DATA->socket_connections[i],
                                           buf, one_fragment_size);
        pwrite(fd, buf, read_bytes, i * one_fragment_size);
        for (int j = 0; j < read_bytes; j += 1) {
          restore_buf[j] = restore_buf[j] ^ buf[j];
        }
      }
    }

    // write down server data
    pwrite(fd, restore_buf, one_fragment_size, down_server * one_fragment_size);
  }

  close(fd);
}

void cache_to_remote(const char* name) {
  int fd = cache_in_mem(name);
  assert(fd != -1);

  ssize_t file_size = find_find_size(name);
  ssize_t one_fragment_size = get_fragment_size(file_size);
  char buf[one_fragment_size];
  char parity_buf[one_fragment_size];
  memset(parity_buf, 0, one_fragment_size * sizeof(char));
  for (int i = 0; i < NUM_REMOTE - 1; i += 1) {
    ssize_t read_bytes =
        pread(fd, buf, one_fragment_size, one_fragment_size * i);
    // TODO(Pond): do assert on fragment size (e.g. last one will not equal the
    // rest)?
    send_fragment(name, FS_DATA->socket_connections[i], buf, read_bytes);

    for (int j = 0; j < read_bytes; j += 1) {
      parity_buf[j] = parity_buf[j] ^ buf[j];
    }
  }

  send_fragment(name, FS_DATA->socket_connections[NUM_REMOTE - 1], parity_buf,
                one_fragment_size);

  // TODO(Pond): do file closing here?
}
