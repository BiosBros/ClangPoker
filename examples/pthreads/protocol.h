#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>

typedef enum {
  MSG_HELLO,
  MSG_JOIN_ROOM,
  MSG_CHAT,
  MSG_SYSTEM,
  MSG_ERROR,
  MSG_UNKNOWN
} MessageType;

typedef struct {
  MessageType type;
  char username[32];
  int players;
  char text[512];
} Message;

bool parse_json_message(const char *json, Message *out);
char *build_json_message(MessageType type, const Message *msg);

#endif

