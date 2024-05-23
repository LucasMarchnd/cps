#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "fonction.h"

#define PORT 9090
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	int client_socket;
	int sock_serveur;
	int client_id;
} argument_t;

void *handle_client(void *arg) {
	argument_t *args = (argument_t *)arg;
	char buffer[BUFFER_SIZE];
	int bytes_read;
	unsigned int seed;

	printf("Client %d connecté.\n", args->client_id);

    while ((bytes_read = read(args->client_socket, buffer, 1)) > 0) {
        char buff[2];

		uint8_t fcs =  frame_check_sequence(buffer[0]);
		// je veux avec une chance sur 2 brouiller un bit dans l'octet reçu
        if (rand_r(&seed) % 2 == 0) {
            // brouiller un bit dans l'octet reçu par son opposé
            buffer[0] ^= 1 << (rand() % 8);
        }
        printf("frame check sequence:");
        print_bits8(fcs);
        
		buff[0] = fcs;
		buff[1] = buffer[0];

		print_bits16(*(uint16_t*)buff);

        printf("Reçu du client %d: %c\n", args->client_id, buff[1]);

        // Verrouiller le mutex pour éviter les conflits d'écriture simultanée au serveur
        pthread_mutex_lock(&mutex);

        // Envoyer l'information au serveur
        if (send(args->sock_serveur, buff, 2, 0) < 0) {
            perror("Erreur d'écriture au serveur");
            break;
        }

        char reponse;
        // Réponse du serveur
        int bytes_received = read(args->sock_serveur, &reponse, 1);
        if (bytes_received <= 0) {
            perror("Erreur de lecture du serveur");
            break;
        }


		printf("Reçu du serveur: %c\n", reponse);

        // Envoyer la réponse du serveur au client
        if (send(args->client_socket, &reponse, 1, 0) < 0) {
            perror("Erreur d'écriture au client");
            break;
        }

        pthread_mutex_unlock(&mutex);
    }

	printf("Client %d déconnecté.\n", args->client_id);
	close(args->client_socket);
	close(args->sock_serveur);
	free(args);
	return NULL;
}

int main() {
	int proxy_socket, sock_serveur, *new_client_socket, nb_clients = 0;
	struct sockaddr_in proxy_addr, client_addr, serveur_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	pthread_t thread_id;

	// Créer le socket du proxy
	proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (proxy_socket == -1) {
		perror("Erreur de création du socket");
		exit(EXIT_FAILURE);
	}

	// Configurer l'adresse du proxy
	proxy_addr.sin_family = AF_INET;
	proxy_addr.sin_addr.s_addr = INADDR_ANY;
	proxy_addr.sin_port = htons(PORT);

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

	while (1) {
		new_client_socket = malloc(sizeof(int));
		*new_client_socket = accept(proxy_socket, (struct sockaddr *)&client_addr, &client_addr_len);
		if (*new_client_socket == -1) {
			perror("Erreur d'acceptation de connexion");
			free(new_client_socket);
			continue;
		}

		// Allouer de la mémoire pour les arguments du thread
		argument_t *args = malloc(sizeof(argument_t));
		args->client_socket = *new_client_socket;
		args->sock_serveur = sock_serveur;
		args->client_id = nb_clients;

		if (pthread_create(&thread_id, NULL, handle_client, args) != 0) {
			perror("Erreur de création du thread");
			free(new_client_socket);
			free(args);
			continue;
		}

		nb_clients++;

		// Détacher le thread pour permettre la gestion indépendante
		pthread_detach(thread_id);
	}

	close(proxy_socket);
	return 0;
}
