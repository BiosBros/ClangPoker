#pragma once

#include "client.h"
#include "common.h"
#include "net_utils.h"
#include "room_manager.h"
#include <arpa/inet.h>
#include <unistd.h>

int find_free_client_slot();
int run_server();
void handle_unknown(int client_fd);
int handle_lobby(int client_fd);
