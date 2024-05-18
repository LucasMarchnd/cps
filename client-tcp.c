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

    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("Entrez un mot :");
        fflush(stdout);
        
        ssize_t bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0) {
            perror("read");
            break;
        }
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i++)
        {
            send(sock_proxy, &buffer[i], 1, 0);

            // j'attends la réponse du proxy
            char response;
            recv(sock_proxy, &response, 1, 0);
            if (response == 'A')
            {
                printf("ACK\n");
            }
            else
            {
                printf("NACK\n");
                i = i - 1;
                continue;
            }
        }
    }
    


    close(sock_proxy);


    return 0;
}

