#include "server.h"

int run_server() {
  int server_fd = create_server_socket(SERVER_PORT);
  if (server_fd < 0) {
    return 1;
  }

  printf("Сервер запущен. Ожидание подключений...\n");

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
      perror("accept");
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("Новое подключение: %s:%d (fd=%d)\n", client_ip,
           ntohs(client_addr.sin_port), client_fd);

    handle_unknown(client_fd);
    int desired_participants = handle_lobby(client_fd);
    transfer_to_room(client_fd, desired_participants);
  }

  close(server_fd);
  return 0;
}

int find_free_client_slot() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].socket == 0) {
      return i;
    }
  }
  return -1;
}

void handle_unknown(int client_fd) {
  printf("Обработка UNKNOWN для клиента %d\n", client_fd);

  char *welcome_msg = "Добро пожаловать! Введите ваше имя: ";
  ssize_t sent = send_all(client_fd, welcome_msg, strlen(welcome_msg));
  printf("[DEBUG] Отправлено приветствие: %ld байт\n", sent);

  char buffer[BUFFER_SIZE];
  ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

  if (bytes > 0) {
    buffer[bytes] = '\0';
    printf("Клиент %d представился как: %s\n", client_fd, buffer);

    int slot = find_free_client_slot();
    if (slot >= 0) {
      clients[slot].socket = client_fd;
      clients[slot].state = CLIENT_IN_LOBBY;
      strncpy(clients[slot].username, buffer,
              sizeof(clients[slot].username) - 1);
      client_count++;
    }
  }
}

int handle_lobby(int client_fd) {
  printf("Обработка LOBBY для клиента %d\n", client_fd);

  char *question =
      "Введите желаемое количество участников в комнате (от 1 до 10): ";
  send_all(client_fd, question, strlen(question));

  char buffer[BUFFER_SIZE];
  ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

  if (bytes > 0) {
    buffer[bytes] = '\0';
    int desired_participants = atoi(buffer);

    if (desired_participants >= 1 && desired_participants <= 10) {
      printf("Клиент %d хочет комнату с %d участниками\n", client_fd,
             desired_participants);
      return desired_participants;
    } else {
      send_all(client_fd,
               "Некорректный ввод. Используем значение по умолчанию (3).\n",
               60);
      return 3;
    }
  }

  return 3;
}
