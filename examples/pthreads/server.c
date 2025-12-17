#include "server.h"
#include "common.h"
#include "protocol.h"

int run_server() {
  int server_fd = create_server_socket(SERVER_PORT);
  if (server_fd < 0) {
    return 1;
  }

  printf("Сервер запущен.\n");

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
    int players = handle_lobby(client_fd);

    transfer_to_room(client_fd, players);
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
  // Отправляем запрос имени
  Message ask = {.type = MSG_SYSTEM};
  strcpy(ask.text, "enter username");
  char *a = build_json_message(MSG_SYSTEM, &ask);
  send_all(client_fd, a, strlen(a));
  free(a);

  char buf[BUFFER_SIZE];
  ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
  if (n <= 0)
    return;
  buf[n] = 0;

  Message msg;
  if (!parse_json_message(buf, &msg) || msg.type != MSG_HELLO) {
    Message e = {.type = MSG_ERROR};
    strcpy(e.text, "expected HELLO");
    char *js = build_json_message(MSG_ERROR, &e);
    send_all(client_fd, js, strlen(js));
    free(js);
    return;
  }

  int slot = find_free_client_slot();
  clients[slot].socket = client_fd;
  strcpy(clients[slot].username, msg.username);
  clients[slot].state = CLIENT_IN_LOBBY;

  Message ok = {.type = MSG_SYSTEM};
  strcpy(ok.text, "hello accepted");
  char *js = build_json_message(MSG_SYSTEM, &ok);
  send_all(client_fd, js, strlen(js));
  free(js);
}

int handle_lobby(int client_fd) {
  Message ask = {.type = MSG_SYSTEM};
  strcpy(ask.text, "enter desired players count");
  char *js = build_json_message(MSG_SYSTEM, &ask);
  send_all(client_fd, js, strlen(js));
  free(js);

  char buf[BUFFER_SIZE];
  ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
  if (n <= 0)
    return 3;
  buf[n] = 0;

  Message msg;
  if (!parse_json_message(buf, &msg) || msg.type != MSG_JOIN_ROOM)
    return 3;

  return msg.players > 0 ? msg.players : 3;
}
