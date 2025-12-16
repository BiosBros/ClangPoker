#include "room_process.h"
#include "protocol.h"

void room_process(int room_id, int parent_fd, int max_participants) {
  printf("[Комната %d PID=%d] Запущена. Максимум участников: %d\n", room_id,
         getpid(), max_participants);

  set_nonblocking(parent_fd);

  int epoll_fd = epoll_create1(0);
  if (epoll_fd < 0) {
    perror("epoll_create1");
    exit(1);
  }

  struct epoll_event ev;
  ev.events = EPOLLIN; // Edge-triggered режим
  ev.data.fd = parent_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, parent_fd, &ev) < 0) {
    perror("epoll_ctl parent");
    exit(1);
  }

  int participants[max_participants];
  int count = 0;
  char buf[1024];

  struct epoll_event events[10];

  while (1) {
    int n = epoll_wait(epoll_fd, events, 10, -1);
    if (n < 0) {
      perror("epoll_wait");
      continue;
    }

    for (int i = 0; i < n; i++) {
      int fd = events[i].data.fd;

      if (fd == parent_fd) {
        // сервер прислал JSON + клиентский FD
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0)
          continue;
        buf[n] = 0;

        Message msg;
        if (!parse_json_message(buf, &msg))
          continue;

        if (msg.type == MSG_JOIN_ROOM) {

          int client_fd = receive_fd(parent_fd);

          participants[count++] = client_fd;

          // уведомить клиента
          Message ready = {.type = MSG_ROOM_READY};
          strcpy(ready.text, "joined room");
          char *out = build_json_message(ready.type, &ready);
          send_all(client_fd, out, strlen(out));
          free(out);

          // слушать клиента в epoll
          ev.events = EPOLLIN;
          ev.data.fd = client_fd;
          epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
        }
      } else {
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0)
          continue;
        buf[n] = 0;

        Message msg;
        if (!parse_json_message(buf, &msg))
          continue;

        // чат в комнате → форвард всем
        if (msg.type == MSG_CHAT) {
          for (int k = 0; k < count; k++) {
            char *out = build_json_message(MSG_ROOM_FORWARD, &msg);
            send_all(participants[k], out, strlen(out));
            free(out);
          }
        }
      }
    }
  }
}
