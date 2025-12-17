#include "protocol.h"
#include <cjson/cJSON.h>
#include <string.h>

static MessageType type_from_string(const char *s) {
  if (!s)
    return MSG_ERROR;
  if (!strcmp(s, "hello"))
    return MSG_HELLO;
  if (!strcmp(s, "join_room"))
    return MSG_JOIN_ROOM;

  if (!strcmp(s, "chat"))
    return MSG_CHAT;
  if (!strcmp(s, "system"))
    return MSG_SYSTEM;
  if (!strcmp(s, "room_ready"))
    return MSG_ROOM_READY;
  if (!strcmp(s, "room_forward"))
    return MSG_ROOM_FORWARD;
  if (!strcmp(s, "room_event"))
    return MSG_ROOM_EVENT;

  return MSG_ERROR;
}

bool parse_json_message(const char *json, Message *out) {
  if (!json || !out)
    return false;

  memset(out, 0, sizeof(*out));

  cJSON *root = cJSON_Parse(json);
  if (!root)
    return false;

  cJSON *type = cJSON_GetObjectItem(root, "type");
  if (!cJSON_IsString(type)) {
    cJSON_Delete(root);
    return false;
  }
  out->type = type_from_string(type->valuestring);

  cJSON *payload = cJSON_GetObjectItem(root, "payload");
  if (!cJSON_IsObject(payload)) {
    cJSON_Delete(root);
    return true; // допустимо иметь пустой payload
  }

  cJSON *u = cJSON_GetObjectItem(payload, "username");
  if (cJSON_IsString(u))
    strncpy(out->username, u->valuestring, sizeof(out->username) - 1);

  cJSON *p = cJSON_GetObjectItem(payload, "players");
  if (cJSON_IsNumber(p))
    out->players = p->valueint;

  cJSON *t = cJSON_GetObjectItem(payload, "text");
  if (cJSON_IsString(t))
    strncpy(out->text, t->valuestring, sizeof(out->text) - 1);

  cJSON_Delete(root);
  return true;
}

char *build_json_message(MessageType type, const Message *msg) {
  cJSON *root = cJSON_CreateObject();
  cJSON *payload = cJSON_CreateObject();

  const char *type_str = "error";
  switch (type) {
  case MSG_HELLO:
    type_str = "hello";
    break;
  case MSG_JOIN_ROOM:
    type_str = "join_room";
    break;
  case MSG_CHAT:
    type_str = "chat";
    break;
  case MSG_SYSTEM:
    type_str = "system";
    break;
  case MSG_ROOM_READY:
    type_str = "room_ready";
    break;
  case MSG_ROOM_FORWARD:
    type_str = "room_forward";
    break;
  case MSG_ROOM_EVENT:
    type_str = "room_event";
    break;
  default:
    break;
  }

  cJSON_AddStringToObject(root, "type", type_str);

  if (msg) {
    if (msg->username[0])
      cJSON_AddStringToObject(payload, "username", msg->username);
    if (msg->players > 0)
      cJSON_AddNumberToObject(payload, "players", msg->players);
    if (msg->text[0])
      cJSON_AddStringToObject(payload, "text", msg->text);
  }

  // Добавляем payload даже если пустой
  cJSON_AddItemToObject(root, "payload", payload);

  char *out = cJSON_PrintUnformatted(root);
  cJSON_Delete(root);

  return out; // вызывающий должен free()
}
