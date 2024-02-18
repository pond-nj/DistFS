
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_BUF_LEN PATH_MAX + 10

enum Operation { READ = 0, WRITE = 1, OK = 2 };

struct message {
  enum Operation op;
  size_t size;
  char* name;
};

struct message new_message(enum Operation op, size_t size, char* name) {
  struct message m;
  m.op = op;
  m.size = size;
  m.name = name;
  return m;
}

struct message message_ok() {
  struct message m;
  m.op = OK;
  m.size = 0;
  m.name = "";
  return m;
}

// message only lived as long as buf because msg.name points to buf
struct message decrypt(char* buf) {
  struct message msg;
  msg.op = (enum Operation)buf[0];
  msg.size = *((size_t*)&buf[1]);
  msg.name = &buf[9];

  return msg;
}

// buf should be char[PATH_MAX + 9 + 1]
void encrypt(struct message m, char* buf) {
  buf[0] = (char)m.op;
  char* size_addr = (char*)&m.size;
  for (int i = 0; i < 8; i += 1) {
    buf[1 + i] = size_addr[i];
  }
  strcpy(&buf[9], m.name);
}