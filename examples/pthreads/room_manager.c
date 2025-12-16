#include "room_manager.h"

RoomProcess *find_room_by_participants(int desired_participants) {
  for (int i = 0; i < MAX_ROOMS; i++) {
    if (rooms[i].pid > 0 && rooms[i].max_participants == desired_participants &&
        rooms[i].client_count < desired_participants) {
      printf("[DEBUG] Найдена комната %d с лимитом %d (сейчас %d/%d)\n",
             rooms[i].room_id, desired_participants, rooms[i].client_count,
             rooms[i].max_participants);
      return &rooms[i];
    }
  }
  printf("[DEBUG] Не найдена комната с лимитом %d\n", desired_participants);
  return NULL;
}

int create_room_process(int room_id, int max_participants) {
  int socket_pair[2];
  if (create_socket_pair(socket_pair) < 0) {
    perror("socketpair");
    return -1;
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return -1;
  }

  if (pid == 0) {
    close(socket_pair[0]);
    printf("[DEBUG] Дочерний процесс запущен для комнаты %d\n", room_id);

    room_process(room_id, socket_pair[1], max_participants);
    exit(0);
  }

  close(socket_pair[1]);

  for (int i = 0; i < MAX_ROOMS; i++) {
    if (rooms[i].pid == 0) {
      rooms[i].pid = pid;
      rooms[i].control_fd = socket_pair[0];
      rooms[i].room_id = room_id;
      rooms[i].client_count = 0;
      rooms[i].max_participants = max_participants;

      printf("Создана комната %d (PID=%d) с лимитом %d участников\n", room_id,
             pid, max_participants);
      return i;
    }
  }

  return -1;
}

void transfer_to_room(int client_fd, int desired_players) {
  RoomProcess *room = find_room_by_participants(desired_players);
  if (!room)
    return;

  // отправляем JSON
  Message m = {0};
  m.type = MSG_JOIN_ROOM;

  char *json = build_json_message(m.type, &m);
  send_all(room->control_fd, json, strlen(json));
  free(json);

  // отправляем FD
  send_fd(room->control_fd, client_fd);
}

void remove_client(int client_fd) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].socket == client_fd) {
      clients[i].socket = 0;
      clients[i].state = CLIENT_UNKNOWN;
      client_count--;
      break;
    }
  }
}
