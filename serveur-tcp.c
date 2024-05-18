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

    char buffer[2*BUFFER_SIZE];
    while (1) {
        recv(sock_proxy, buffer, 2*BUFFER_SIZE, 0);

        uint8_t calculated_crc = crc8(buffer[0]);
        
        print_bits8(buffer[0]);
        printf("Received CRC: ");
        print_bits8(buffer[1]);
        printf("CRC: ");
        print_bits8(calculated_crc);

        if (calculated_crc == buffer[1]) {
            printf("Parity is correct\n");
        } else {
            printf("Parity is incorrect\n");
        }
        printf("--------------------\n");
        send(sock_proxy, "A", 1, 0);
        
    }

    return 0;
}
