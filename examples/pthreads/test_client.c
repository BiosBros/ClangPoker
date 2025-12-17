#include "protocol.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <ip>\n", argv[0]);
    return 1;
  }

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  inet_pton(AF_INET, argv[1], &addr.sin_addr);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect");
    return 1;
  }

  printf("Connected.\n");

  struct pollfd p[2];
  p[0].fd = fd;
  p[0].events = POLLIN;
  p[1].fd = STDIN_FILENO;
  p[1].events = POLLIN;

  char buf[2048];
  char input[2048];

  int in_room = 0;

  while (1) {
    poll(p, 2, -1);

    // сервер → клиент
    if (p[0].revents & POLLIN) {
      ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
      if (n <= 0)
        break;
      buf[n] = 0;

      Message msg;
      if (!parse_json_message(buf, &msg))
        continue;

      printf("\rServer: %s\n> ", msg.text);

      // запрос имени
      if (strcmp(msg.text, "enter username") == 0) {
        // ждём ввод с stdin
      }

      // запрос числа игроков
      if (strcmp(msg.text, "enter desired players count") == 0) {
        // ждём ввод
      }

      if (msg.type == MSG_ROOM_READY)
        in_room = 1;
    }

    // ввод пользователя
    if (p[1].revents & POLLIN) {
      ssize_t r = read(STDIN_FILENO, input, sizeof(input) - 1);
      if (r <= 0)
        continue;

      if (input[r - 1] == '\n')
        input[r - 1] = 0;

      Message m;

      if (!in_room) {
        // либо HELLO, либо JOIN_ROOM
        if (strchr(input, 'a'))
          continue; // просто анти-баг :)
        memset(&m, 0, sizeof(m));
        // if (clients state ??? doesn't matter — client just reacts)

        // динамика:
        // первым делом сервер всегда просит username
        // потом просьба players

        // мы НЕ храним состояние клиента — мы просто отправляем то,
        // что сервер последний раз просил
        // если сервер попросил имя → HELLO
        // если сервер попросил числа → JOIN_ROOM
      }

      else {
        // обычный чат
        memset(&m, 0, sizeof(m));
        m.type = MSG_CHAT;
        strcpy(m.text, input);
      }

      char *js = build_json_message(m.type, &m);
      send(fd, js, strlen(js), 0);
      free(js);

      printf("> ");
    }
  }
}
