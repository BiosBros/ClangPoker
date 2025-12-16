#include "server.h"
#include <signal.h>
#include <time.h>

RoomProcess rooms[MAX_ROOMS] = {0};
Client clients[MAX_CLIENTS] = {0};
int client_count = 0;

int main() {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  srand(time(NULL));

  printf("Запуск TCP сервера с процессами...\n");
  printf("Порт: %d\n", SERVER_PORT);
  printf("Отладка включена\n");

  return run_server();
}
