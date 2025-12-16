#include "protocol.h"
#include <cjson/cJSON.h>
#include <string.h>

static MessageType type_from_string(const char *s) {
  if (!s)
    return MSG_UNKNOWN;
  if (!strcmp(s, "hello"))
    return MSG_HELLO;
  if (!strcmp(s, "join_room"))
    return MSG_JOIN_ROOM;
  if (!strcmp(s, "chat"))
    return MSG_CHAT;
  if (!strcmp(s, "system"))
    return MSG_SYSTEM;
  if (!strcmp(s, "error"))
    return MSG_ERROR;
  return MSG_UNKNOWN;
}

bool parse_json_message(const char *json, Message *out) {
  cJSON *root = cJSON_Parse(json);
  if (!root)
    return false;

  memset(out, 0, sizeof(*out));

  cJSON *type = cJSON_GetObjectItem(root, "type");
  cJSON *payload = cJSON_GetObjectItem(root, "payload");

  out->type = type_from_string(type ? type->valuestring : NULL);

  if (payload) {
    cJSON *u = cJSON_GetObjectItem(payload, "username");
    cJSON *p = cJSON_GetObjectItem(payload, "players");
    cJSON *t = cJSON_GetObjectItem(payload, "text");

    if (cJSON_IsString(u))
      strncpy(out->username, u->valuestring, sizeof(out->username) - 1);

    if (cJSON_IsNumber(p))
      out->players = p->valueint;

    if (cJSON_IsString(t))
      strncpy(out->text, t->valuestring, sizeof(out->text) - 1);
  }

  cJSON_Delete(root);
  return true;
}

char *build_json_message(MessageType type, const Message *msg) {
  cJSON *root = cJSON_CreateObject();
  cJSON *payload = cJSON_CreateObject();

  const char *type_str = "unknown";
  if (type == MSG_SYSTEM)
    type_str = "system";
  if (type == MSG_ERROR)
    type_str = "error";
  if (type == MSG_CHAT)
    type_str = "chat";

  cJSON_AddStringToObject(root, "type", type_str);

  if (msg->text[0])
    cJSON_AddStringToObject(payload, "text", msg->text);

  cJSON_AddItemToObject(root, "payload", payload);

  char *out = cJSON_PrintUnformatted(root);
  cJSON_Delete(root);
  return out;
}
