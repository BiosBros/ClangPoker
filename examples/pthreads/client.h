#pragma once
#include "common.h"

int client_init(Client *c, int fd);
void client_reset(Client *c);
