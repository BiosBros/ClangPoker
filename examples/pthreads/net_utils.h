#pragma once
#include "protocol.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int send_fd(int socket, int fd_to_send);
int receive_fd(int socket);
ssize_t send_all(int fd, const void *buf, size_t len);
int set_nonblocking(int fd);
int create_server_socket(int port);
int create_socket_pair(int pair[2]);
