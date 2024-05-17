// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "fonction.h"

#define PORT 8080
#define BUFFER_SIZE 1

int main() {
    // connexion au proxy
    int sock_proxy,sock_serveur;
    struct sockaddr_in address, proxy_addr;
    socklen_t proxy_len;

    // Mise en place de la connexion avec le proxy (bind)
    if ((sock_serveur = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
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
    printf("Server is listening on port %d\n", PORT);
    proxy_len = sizeof(proxy_addr);
    sock_proxy = accept(sock_serveur, (struct sockaddr *)&proxy_addr, &proxy_len);
    printf("Proxy connected\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        recv(sock_proxy, buffer, BUFFER_SIZE, 0);
        //printf("Received: %s\n", buffer);

        int parity = 0;
        recv(sock_proxy, &parity, 1, 0);
        // printf("Received parity: %d\n", parity);
        
        int np = calculate_parity(buffer[0]);
        // printf("Calculated parity: %d\n", np);
        
        if (parity == np) {
            //printf("Parity is correct\n");
            printf("%c", buffer[0]);
            send(sock_proxy, "A", 1, 0);
        } else {
            //printf("Parity is incorrect\n");
            send(sock_proxy, "N", 1, 0);
        }

        
    }

    return 0;
}
