#include <asm-generic/socket.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "env.h"
#include "meta.h"
#include "protocol.h"
#include "storage.h"

int N;
int PORT;
int server_sd;

int accept_client() {
  struct sockaddr_in address;
  int opt = 1;
  socklen_t addrlen = sizeof(address);
  char buffer[1024] = {0};
  char* hello = "Hello from server";

  // Creating socket file descriptor
  if ((server_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Forcefully attaching socket to the port 8080
  if (bind(server_sd, (struct sockaddr*)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_sd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  int new_socket;
  if ((new_socket = accept(server_sd, (struct sockaddr*)&address, &addrlen)) <
      0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return new_socket;
}

int main(int argc, char** argv) {
  // ./server n
  // n is zero indexed
  asssert(argc == 2);
  N = atoi(argv[1]);

  struct server* remotes = get_remotes_list();
  PORT = remotes[N].port;

  server_meta = init_meta();

  int in_socket = accept_client();

  while (1) {
    // default mode is blocking
    char msg_buf[MSG_BUF_LEN];
    read(in_socket, msg_buf, MSG_BUF_LEN);
    struct message m = decrypt(msg_buf);

    size_t size = m.size;
    char name[PATH_MAX];
    strcpy(name, m.name);

    if (m.op == READ) {
      assert(has_this_file(name));

      assert(size == get_file_size(name));

      char buf[size];
      // TODO(Pond): how to do zero copy
      ssize_t read_bytes = read_file(name, buf, size);
      send(in_socket, buf, size, 0);

      read(in_socket, msg_buf, MSG_BUF_LEN);
      m = decrypt(msg_buf);
      assert(m.op == OK);

    } else {
      assert(m.op == WRITE);

      encrypt(message_ok(), msg_buf);
      send(in_socket, msg_buf, MSG_BUF_LEN, 0);

      char buf[size];
      // TODO(Pond): how to do zero copy
      read(in_socket, buf, size);
      write_file(name, buf, size);

      encrypt(message_ok(), msg_buf);
      send(in_socket, msg_buf, MSG_BUF_LEN, 0);
    }
  }
}