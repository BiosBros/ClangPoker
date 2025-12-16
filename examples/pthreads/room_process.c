#include "room_process.h"

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

  int client_fds[100] = {0};
  int client_count = 0;
  int spam_sent = 0;

  struct epoll_event events[10];

  char ready_msg[100];
  snprintf(ready_msg, sizeof(ready_msg),
           "Комната %d готова к работе (PID=%d)\n", room_id, getpid());
  send_all(parent_fd, ready_msg, strlen(ready_msg));

  while (1) {
    int n = epoll_wait(epoll_fd, events, 10, -1);
    if (n < 0) {
      perror("epoll_wait");
      continue;
    }

    for (int i = 0; i < n; i++) {
      int fd = events[i].data.fd;

      if (fd == parent_fd) {
        while (1) {
          int new_client_fd = receive_fd(parent_fd);
          if (new_client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
              break; // fd пока нет — это нормально
            }
            perror("receive_fd");
            break;
          }
          printf("[Комната %d] Получен клиент fd=%d\n", +room_id,
                 new_client_fd); // Проверяем лимит

          if (client_count >= max_participants) {
            printf("[Комната %d] Лимит достигнут! Отказ клиенту fd=%d\n",
                   room_id, new_client_fd);

            char *reject_msg = "Комната заполнена!\n";
            send_all(new_client_fd, reject_msg, strlen(reject_msg));
            close(new_client_fd);
            continue;
          }

          set_nonblocking(new_client_fd);

          struct epoll_event client_ev;
          client_ev.events = EPOLLIN | EPOLLET;
          client_ev.data.fd = new_client_fd;

          if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client_fd, &client_ev) <
              0) {
            perror("epoll_ctl client");
            close(new_client_fd);
            continue;
          }

          client_fds[client_count++] = new_client_fd;

          char welcome[BUFFER_SIZE];
          snprintf(welcome, sizeof(welcome),
                   "Добро пожаловать в комнату %d! Участников: %d/%d\n",
                   room_id, client_count, max_participants);

          ssize_t sent = send_all(new_client_fd, welcome, strlen(welcome));
          printf(
              "[Комната %d] Отправлено приветствие клиенту fd=%d: %ld байт\n",
              room_id, new_client_fd, sent);

          if (client_count == max_participants && !spam_sent) {
            printf("[Комната %d] Лимит достигнут! Отправляем спам\n", room_id);

            for (int j = 0; j < client_count; j++) {
              char *limit_msg = "\n=== ЛИМИТ УЧАСТНИКОВ ДОСТИГНУТ! ===\n";
              send_all(client_fds[j], limit_msg, strlen(limit_msg));
              send_spam_to_client(client_fds[j]);
            }
            spam_sent = 1;
          }
        }

      } else {
        char buffer[BUFFER_SIZE];
        ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes <= 0) {
          if (bytes == 0) {
            printf("[Комната %d] Клиент fd=%d отключился\n", room_id, fd);
          } else {
            printf("[Комната %d] Ошибка чтения от клиента fd=%d: %s\n", room_id,
                   fd, strerror(errno));
          }

          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);

          for (int j = 0; j < client_count; j++) {
            if (client_fds[j] == fd) {
              client_fds[j] = client_fds[client_count - 1];
              client_count--;
              break;
            }
          }

          if (client_count < max_participants) {
            spam_sent = 0;
          }

          close(fd);
        } else {
          buffer[bytes] = '\0';
          printf("[Комната %d] Сообщение от fd=%d: %s\n", room_id, fd, buffer);

          char response[BUFFER_SIZE];
          snprintf(response, sizeof(response), "[Комната %d] Эхо: %.900s",
                   room_id, buffer);

          ssize_t sent = send_all(fd, response, strlen(response));
          printf("[Комната %d] Отправлен ответ клиенту fd=%d: %ld байт\n",
                 room_id, fd, sent);
        }
      }
    }
  }
}

void send_spam_to_client(int client_fd) {
  printf("[DEBUG] Отправляем спам клиенту fd=%d\n", client_fd);

  char *spam_messages[] = {"SPAM: Бесплатные деньги!\n",
                           "SPAM: Вы выиграли iPhone!\n",
                           "SPAM: Срочно! Ваш аккаунт взломали!\n",
                           "SPAM: Инвестируйте в криптовалюту!\n",
                           "SPAM: Скидка 90% только сегодня!\n",
                           "SPAM: Ваша карта заблокирована!\n",
                           "SPAM: Наследство из Нигерии!\n",
                           "SPAM: Ваш компьютер заражен!\n",
                           "SPAM: Срочный звонок из банка!\n",
                           "SPAM: Активируйте бонусы!\n"};

  int num_messages = 5 + rand() % 6;

  for (int i = 0; i < num_messages; i++) {
    char msg[BUFFER_SIZE];
    snprintf(msg, sizeof(msg), "[%d/%d] %s", i + 1, num_messages,
             spam_messages[rand() % 10]);

    ssize_t sent = send_all(client_fd, msg, strlen(msg));
    printf("[DEBUG] Отправлено %ld байт спама клиенту fd=%d\n", sent,
           client_fd);

    if (sent <= 0) {
      printf("[DEBUG] Ошибка отправки спама: %s\n", strerror(errno));
    }

    usleep(200000);
  }

  char *limit_msg = "=== КОНЕЦ СПАМА ===\n";
  send_all(client_fd, limit_msg, strlen(limit_msg));
}
