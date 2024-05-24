// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "fonction.h"

#define PORT 8080
#define BUFFER_SIZE 1

int main(int argc, char const *argv[]) {
    // connexion au proxy
    int sock_proxy,sock_serveur, port_serveur;
    struct sockaddr_in address, proxy_addr;
    socklen_t proxy_len;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port_serveur>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    port_serveur = atoi(argv[1]);

    // Mise en place de la connexion avec le proxy (bind)
    if ((sock_serveur = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(port_serveur);
    if (bind(sock_serveur, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(sock_serveur);
        exit(EXIT_FAILURE);
    }
    if (listen(sock_serveur, 3) < 0) {
        perror("listen");
        close(sock_serveur);
        exit(EXIT_FAILURE);
    }
    printf("Serveur écoute le port %d\n", port_serveur);
    proxy_len = sizeof(proxy_addr);
    sock_proxy = accept(sock_serveur, (struct sockaddr *)&proxy_addr, &proxy_len);
    printf("Proxy connecté\n");

    char buffer[3];
    while (1) {
        recv(sock_proxy, buffer, 3, 0);

        if (crc8(*(uint16_t*)buffer) == 0) {
            fprintf(stdout,"Client id : %d :  %c\n", buffer[2] ,buffer[1]);
            send(sock_proxy, "A", 1, 0);
        } 
        else 
        {
            send(sock_proxy, "N", 1, 0);
        }
        
    }


    close(sock_proxy);
    close(sock_serveur);

    return 0;
}
