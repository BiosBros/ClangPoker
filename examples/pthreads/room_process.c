#include "room_process.h"
#include "protocol.h"

void room_process(int room_id, int parent_fd, int max_participants) {
  printf("[Комната %d PID=%d] Старт. Ждем %d участников.\n", room_id, getpid(),
         max_participants);

  set_nonblocking(parent_fd);

  int ep = epoll_create1(0);
  if (ep < 0) {
    perror("epoll_create");
    exit(1);
  }

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = parent_fd;
  epoll_ctl(ep, EPOLL_CTL_ADD, parent_fd, &ev);

  int users[max_participants];
  int count = 0;

  struct epoll_event events[32];
  char buf[2048];

  while (1) {
    int n = epoll_wait(ep, events, 32, -1);
    if (n < 0)
      continue;

    for (int i = 0; i < n; i++) {
      int fd = events[i].data.fd;

      // Новый клиент засунут родителем
      if (fd == parent_fd) {
        int newfd = receive_fd(parent_fd);
        if (newfd < 0)
          continue;

        users[count++] = newfd;
        set_nonblocking(newfd);

        ev.events = EPOLLIN;
        ev.data.fd = newfd;
        epoll_ctl(ep, EPOLL_CTL_ADD, newfd, &ev);

        // Когда набрали всех — уведомляем
        if (count == max_participants) {
          Message m = {.type = MSG_ROOM_READY};
          strcpy(m.text, "room is ready");

          char *js = build_json_message(MSG_ROOM_READY, &m);
          for (int k = 0; k < count; k++)
            send_all(users[k], js, strlen(js));
          free(js);
        }
        continue;
      }

      // Сообщения от пользователей
      ssize_t r = recv(fd, buf, sizeof(buf) - 1, 0);
      if (r <= 0)
        continue;
      buf[r] = 0;

      Message msg;
      if (!parse_json_message(buf, &msg))
        continue;

      if (msg.type == MSG_CHAT) {
        char *js = build_json_message(MSG_ROOM_FORWARD, &msg);
        for (int k = 0; k < count; k++)
          send_all(users[k], js, strlen(js));
        free(js);
      }
    }
  }
}
