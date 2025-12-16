#pragma once

#include <unistd.h>

#define MAX_CLIENTS 1000
#define MAX_ROOMS 10
#define BUFFER_SIZE 1024
#define SERVER_PORT 8080

typedef enum { CLIENT_UNKNOWN, CLIENT_IN_LOBBY, CLIENT_IN_ROOM } ClientState;

typedef struct {
  pid_t pid;
  int control_fd;
  int room_id;
  int client_count;
  int max_participants;
} RoomProcess;

typedef struct {
  int socket;
  ClientState state;
  int room_id;
  char username[32];
} Client;

extern Client clients[MAX_CLIENTS];
extern int client_count;

extern RoomProcess rooms[MAX_ROOMS];
