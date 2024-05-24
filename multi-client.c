#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Créer le socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Adresse invalide ou non supportée");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Connecter au serveur
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur de connexion au serveur");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connecté au serveur. Tapez vos messages:\n");

    while (1) {
        // Lire le message de l'utilisateur
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        for (int i = 0; i < strlen(buffer); i++)
        {
            write(client_socket, &buffer[i], 1);
            // Lire la réponse du serveur
            
            // reponse 
            char reponse;
            
            bytes_read = read(client_socket, &reponse, 1);
            if (bytes_read <= 0) {
                perror("Erreur de lecture du serveur");
                break;
            }
            if (reponse == 'A')
            {
                printf("Reçu du serveur: %c\n", reponse);
            
            }
            else
            {
                printf("Reçu du serveur: %c\n", reponse);
                i--;
                continue;
            }
        }
    }

    close(client_socket);
    return 0;
}
