CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wno-misleading-indentation

.PHONY: all check clean
all: server client

server: server.c net.h
	$(CC) $(CFLAGS) server.c -o $@

client: client.c net.h
	$(CC) $(CFLAGS) client.c -o $@

check:
	bash -n sysinfo.sh

clean:
	rm -f server client
