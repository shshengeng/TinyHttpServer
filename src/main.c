#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include "http_handler.h"

int main()
{
    int serverSocket, clientSocket = 0;
    unsigned short port = PORT;
    struct sockaddr_in clientAddr;
    pthread_t thid;

    serverSocket = startUp(port);
    printf("Server is listening on Port: %d\n", port);

    while(1)
    {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddr);
        if (clientSocket < 0) {
            handleError("Error accepting connection");
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&thid , NULL, (void *)received_request, (void *)(intptr_t)clientSocket) != 0)
        {
            handleError("Error pthread_create");
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
    }
    exit(0);
}