// proxy.c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fonction.h"

#define PROXY_PORT 9090
#define SERVER_PORT 8080
#define BUFFER_SIZE 1 // une lettre par une lettre


int main()
{
    int sock_client,sock_serveur,sock_proxy;
    struct sockaddr_in local_addr, serveur_addr,client_addr;
    socklen_t client_len;

    // Mise en place de la connexion avec le client
    if ((sock_proxy = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(PROXY_PORT);
    if (bind(sock_proxy, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind failed");
        close(sock_proxy);
        exit(EXIT_FAILURE);
    }
    if (listen(sock_proxy, 3) < 0) {
        perror("listen");
        close(sock_proxy);
        exit(EXIT_FAILURE);
    }
    
    printf("Proxy is listening on port %d\n", PROXY_PORT);
    
    client_len = sizeof(client_addr);
    if ((sock_client = accept(sock_proxy, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("accept");
        close(sock_proxy);
        exit(EXIT_FAILURE);
    }
    printf("Client connected\n");

    // Mise en place de la connexion avec le serveur
    if ((sock_serveur = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;
    serveur_addr.sin_port = htons(SERVER_PORT);
    if (connect(sock_serveur, (struct sockaddr *)&serveur_addr, sizeof(serveur_addr)) < 0) {
        perror("connect");
        close(sock_serveur);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");

    srand(time(NULL));

    char buffer[BUFFER_SIZE];
    while (1) {
        recv(sock_client, buffer, BUFFER_SIZE, 0);
        
        //calculer le bit de parité
        int parity = calculate_parity(buffer[0]);
        printf("Received: %s\n", buffer);
        printf("Parity: %d\n", parity);

        // je veux avec une chance sur 2 brouiller un bit dans l'octet reçu
        if (rand() % 2 == 0) {
            // brouiller un bit dans l'octet reçu par son opposé
            buffer[0] ^= 1 << (rand() % 8);
        }
        // envoyer l'octet brouillé au serveur
        send(sock_serveur, buffer, BUFFER_SIZE, 0);
        // envoyer le bit de parité au serveur
        send(sock_serveur, &parity, 1, 0);
        
        // recevoir la réponse du serveur
        recv(sock_serveur, buffer, BUFFER_SIZE, 0);

        send(sock_client, buffer, BUFFER_SIZE, 0);
    }
    

    return 0;
}
