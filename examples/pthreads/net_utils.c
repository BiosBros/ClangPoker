#include "net_utils.h"

ssize_t send_all(int fd, const void *buf, size_t len) {
  size_t total = 0;
  const char *p = buf;

  while (total < len) {
    ssize_t n = send(fd, p + total, len - total, 0);

    if (n > 0) {
      total += n;
    } else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      return total; // отправили сколько смогли
    } else {
      return -1; // ошибка или клиент умер
    }
  }

  return total;
}

int send_fd(int sock, int fd_to_send) {
  struct msghdr msg = {0};

  char buf = 0; // dummy payload
  struct iovec iov = {.iov_base = &buf, .iov_len = sizeof(buf)};
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  char cmsgbuf[CMSG_SPACE(sizeof(int))];
  msg.msg_control = cmsgbuf;
  msg.msg_controllen = sizeof(cmsgbuf);

  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN(sizeof(int));

  memcpy(CMSG_DATA(cmsg), &fd_to_send, sizeof(int));

  if (sendmsg(sock, &msg, 0) < 0) {
    perror("sendmsg");
    return -1;
  }

  return 0;
}

int set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0)
    return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int receive_fd(int sock) {
  struct msghdr msg = {0};

  char buf;
  struct iovec iov = {.iov_base = &buf, .iov_len = sizeof(buf)};
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  char cmsgbuf[CMSG_SPACE(sizeof(int))];
  msg.msg_control = cmsgbuf;
  msg.msg_controllen = sizeof(cmsgbuf);

  ssize_t n = recvmsg(sock, &msg, 0);
  if (n < 0) {
    return -1; // errno снаружи
  }

  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
  if (!cmsg) {
    errno = EPROTO;
    return -1;
  }

  if (cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS) {
    errno = EPROTO;
    return -1;
  }

  int fd;
  memcpy(&fd, CMSG_DATA(cmsg), sizeof(int));
  return fd;
}

int create_server_socket(int port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return -1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(server_fd);
    return -1;
  }

  if (listen(server_fd, 10) < 0) {
    perror("listen");
    close(server_fd);
    return -1;
  }

  printf("Сервер слушает порт %d\n", port);
  return server_fd;
}

int create_socket_pair(int pair[2]) {
  int result = socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
  printf("[DEBUG] socketpair создан: [%d, %d]\n", pair[0], pair[1]);
  return result;
}
