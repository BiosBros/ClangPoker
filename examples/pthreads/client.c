#include "client.h"
#include <string.h>

int client_init(Client *c, int fd) {
  c->socket = fd;
  c->state = CLIENT_UNKNOWN;
  c->room_id = -1;
  memset(c->username, 0, sizeof(c->username));
  return 0;
}

void client_reset(Client *c) {
  c->socket = 0;
  c->state = CLIENT_UNKNOWN;
  c->room_id = -1;
}
