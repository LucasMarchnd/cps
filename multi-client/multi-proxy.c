#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 9090
#define SERVER_PORT 8080

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 4


typedef struct{
    int client_socket;
    int sock_serveur;
    int client_id;
}argument_t;


void *handle_client(void *arg) {

    argument_t *args = (argument_t *)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    int bytes_read;

    printf("Client connecté.\n");

    while ((bytes_read = read(args->client_socket, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Reçu du client: %s\n", buffer);
        write(args->client_socket, buffer, bytes_read);

        // envoyer l'information au serveur par le bias du proxy
        write(args->sock_serveur, buffer, bytes_read);

    }

    printf("Client déconnecté.\n");
    close(args->client_socket);
    return NULL;
}

int main() {
    int proxy_socket, *new_client_socket,sock_serveur,nb_clients = 0;
    struct sockaddr_in proxy_addr, client_addr,serveur_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    pthread_t thread_id;

    // Créer le socket
    proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy_socket == -1) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_addr.s_addr = INADDR_ANY;
    proxy_addr.sin_port = htons(PORT);



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
    printf("Serveur connecté\n");




    // Attacher le socket à l'adresse
    if (bind(proxy_socket, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) == -1) {
        perror("Erreur de liaison du socket");
        close(proxy_socket);
        exit(EXIT_FAILURE);
    }

    // Écouter les connexions entrantes
    if (listen(proxy_socket, MAX_CLIENTS) == -1) {
        perror("Erreur de mise en écoute");
        close(proxy_socket);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    argument_t *arg;
    arg = malloc(sizeof(argument_t)*MAX_CLIENTS);

    while (1) {
        new_client_socket = malloc(sizeof(int));
        *new_client_socket = accept(proxy_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (*new_client_socket == -1) {
            perror("Erreur d'acceptation de connexion");
            free(new_client_socket);
            continue;
        }

        // Créer un thread pour gérer la nouvelle connexion

        
        arg[nb_clients].client_socket = *new_client_socket;
        arg[nb_clients].sock_serveur = sock_serveur;
        arg[nb_clients].client_id = nb_clients;
        if (pthread_create(&thread_id, NULL, handle_client, &arg[nb_clients]) != 0) {
            perror("Erreur de création du thread");
            free(new_client_socket);
            continue;
        }

        nb_clients++;

        // Détacher le thread pour permettre la gestion indépendante
        pthread_detach(thread_id);
    }

    close(proxy_socket);
    return 0;
}
