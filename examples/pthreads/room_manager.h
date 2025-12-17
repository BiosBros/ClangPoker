#pragma once
#include "common.h"
#include "net_utils.h"
#include "room_process.h"
#include <stdio.h>
#include <sys/types.h>

void remove_client(int client_fd);
RoomProcess *find_room_by_participants(int n);
int create_room_process(int room_id, int max_participants);
void transfer_to_room(int client_fd, int desired_participants);
RoomProcess *find_free_room(int needed_players);
