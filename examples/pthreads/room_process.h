#pragma once
#include "common.h"
#include "net_utils.h"
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>

void send_spam_to_client(int client_fd);
void room_process(int room_id, int parent_fd, int max_participants);
