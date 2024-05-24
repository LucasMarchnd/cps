# Compiler and flags
CC = gcc
CFLAGS = -Wall
DEBUG_FLAGS = -g

PROXY_BIN = proxy
CLIENT_BIN = client
SERVEUR_BIN = serveur

PROXY_SRC = multi-proxy.c fonction.c
CLIENT_SRC = multi-client.c fonction.c
SERVEUR_SRC = multi-serveur.c fonction.c

PROXY_OBJS = $(PROXY_SRC:.c=.o)
CLIENT_OBJS = $(CLIENT_SRC:.c=.o)
SERVEUR_OBJS = $(SERVEUR_SRC:.c=.o)

all: $(PROXY_BIN) $(CLIENT_BIN) $(SERVEUR_BIN)


$(PROXY_BIN): $(PROXY_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SERVEUR_BIN): $(SERVEUR_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROXY_BIN) $(CLIENT_BIN) $(SERVEUR_BIN)
	rm -f $(PROXY_OBJS) $(CLIENT_OBJS) $(SERVEUR_OBJS)

