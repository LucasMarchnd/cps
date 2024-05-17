// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <poll.h>

#define PROXY_PORT 9090
#define BUFFER_SIZE 1025

int main(int argc, char const *argv[])
{
    int sock_proxy;
    struct sockaddr_in address, proxy_addr;

    // Créer un socket
    if ((sock_proxy = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    inet_aton("127.0.0.1", &address.sin_addr);
    address.sin_port = htons(PROXY_PORT);
    
    // Se connecter au proxy

    if (connect(sock_proxy, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("connect");
        close(sock_proxy);
        exit(EXIT_FAILURE);
    }
    printf("Connecté au proxy\n");

    struct pollfd fds[2];

    // Configure les file descriptors pour poll
    fds[0].fd = STDIN_FILENO;   // Entrée standard
    fds[0].events = POLLIN;     // Événements de lecture
    fds[1].fd = sock_proxy;     // Socket proxy
    fds[1].events = POLLIN;     // Événements de lecture


    char buffer[BUFFER_SIZE];
    while (1) {
        printf("Entrez un mot: ");
        fflush(stdout); // S'assurer que le texte est affiché avant de bloquer sur poll

        int poll_count = poll(fds, 2, -1);  // Attend indéfiniment

        if (poll_count < 0) {
            perror("Erreur avec poll");
            break;
        }

        // Vérifier si l'entrée standard est prête pour la lecture
        if (fds[0].revents & POLLIN) {
            ssize_t bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1); // Lire jusqu'à BUFFER_SIZE - 1 octets

            // Vérifier si la lecture est réussie
            if (bytes_read < 0) {
                perror("Erreur de lecture de l'entrée standard");
                break;
            }

            // Envoyer les données lues via le socket, une lettre à la fois
            for (ssize_t i = 0; i < bytes_read; i++) {
                ssize_t bytes_sent = send(sock_proxy, &buffer[i], 1, 0);

                // Vérifier si l'envoi est réussi
                if (bytes_sent < 0) {
                    perror("Erreur d'envoi via le socket");
                    break;
                }
            }
        }

        // Vérifier si le socket est prêt pour la lecture
        if (fds[1].revents & POLLIN) {
            ssize_t bytes_received = recv(sock_proxy, buffer, 1, 0);

            // Vérifier si la réception est réussie
            if (bytes_received < 0) {
                perror("Erreur de réception via le socket");
                break;
            }

            if (bytes_received == 0) {
                printf("Le serveur a fermé la connexion\n");
                break;
            }

            // Assurez-vous que le buffer est bien terminé par un null character
            buffer[bytes_received] = '\0';

            // Si la réponse du proxy est "A", continue
            if (buffer[0] == 'A') {
                printf("ACK pour cette lettre\n");
                continue;
            } else {
                // Si la réponse du proxy n'est pas "A", renvoyer la lettre
                ssize_t bytes_sent = send(STDOUT_FILENO, buffer, bytes_received, 0);
                if (bytes_sent < 0) {
                    perror("Erreur d'envoi vers la sortie standard");
                    break;
                }
            }
        }
    }


    close(sock_proxy);


    return 0;
}

