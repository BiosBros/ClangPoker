CC = gcc
CFLAGS = -Wall

all: uno_server uno_client

uno_server: ser_pizdec.c uno_rules.c server_utils.c card_utils.c
	$(CC) $(CFLAGS) -o uno_server.out ser_pizdec.c uno_rules.c server_utils.c card_utils.c -lcjson -lpthread

uno_client: cli_pizdec.c client_net.c client_utils.c card_utils.c
	$(CC) $(CFLAGS) -o uno_client.out cli_pizdec.c client_net.c client_utils.c card_utils.c -lcjson -lncurses

clean:
	rm -f uno_server uno_client

run-server: uno_server
	./uno_server

run-client: uno_client
	@if [ -z "$(IP)" ]; then \
		echo "Usage: make run-client IP=127.0.0.1"; \
	else \
		./uno_client $(IP); \
	fi

.PHONY: all clean run-server run-client
