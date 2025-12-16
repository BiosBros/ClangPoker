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

void transfer_to_room(int client_fd, int desired_participants) {
  printf("Поиск комнаты для клиента %d (желает %d участников)\n", client_fd,
         desired_participants);

  RoomProcess *room = find_room_by_participants(desired_participants);

  if (!room) {
    static int next_room_id = 1;
    int room_id = next_room_id++;

    printf("Создаем новую комнату %d с лимитом %d\n", room_id,
           desired_participants);

    int room_index = create_room_process(room_id, desired_participants);
    if (room_index < 0) {
      printf("Не удалось создать комнату\n");
      close(client_fd);
      return;
    }
    room = &rooms[room_index];
  }

  set_nonblocking(client_fd);

  printf("[DEBUG] Передаем клиента fd=%d в комнату %d\n", client_fd,
         room->room_id);

  if (send_fd(room->control_fd, client_fd) < 0) {
    perror("send_fd");
    close(client_fd);
    return;
  }

  room->client_count++;
  printf("Клиент %d передан в комнату %d (клиентов: %d/%d)\n", client_fd,
         room->room_id, room->client_count, room->max_participants);

  remove_client(client_fd);
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
