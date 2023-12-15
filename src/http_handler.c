#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <unistd.h>
#include "http_handler.h"

int startUp(unsigned short port)
{
    int serverSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        handleError("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        handleError("Error binding");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        handleError("Error listening");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    return serverSocket;
}

void received_request(void *arg)
{
    int clientSocket = (intptr_t)arg;
    char buf[1024];
    int numChars; //unsigned int
    char method[256];
    char url[256];
    char path[256];
    struct stat st;


    //GET / Http1.1
    numChars = get_line(clientSocket, buf);
    int i, j = 0;
    //buf[i] is not a space
    while (buf[i] != ' ' && (i < sizeof(buf) - 1))
    {
        method[j] = buf[i];
        i++;
        j++;
    }
    method[j] = '\0';  //method string

    if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
    {
        //return a html 501 page
        not_implemented(clientSocket);
        return;
    }

    i++;
    j = 0;
    while(buf[i] != ' ' && (i < sizeof(buf) - 1) && j < numChars)
    {
        url[j] = buf[i];
        i++;
        j++;
    }
    url[j] = '\0'; //url string


    sprintf(path, "../htdocs%s", url);
    if (path[strlen(path) - 1] == '/')
        strcat(path, "index.html");
    if (stat(path, &st) == -1) {
        while ((numChars > 0) && strcmp("\n", buf))  // read & discard headers
            numChars = get_line(clientSocket, buf);
        not_found(clientSocket);
    }
    else
    {
        index_file(clientSocket, path);
    }
    close(clientSocket);
}

 int get_line(int clientSocket, char *buf)
{
    char c = '0';
    int i = 0;
    int num;

    //receive from socket buffer until get '\n'
    while(c != '\n' && i < 1024)
    {
        num = recv(clientSocket, &c, 1, 0);
        if(num > 0)
        {
          if (c == '\r')
            {
                num = recv(clientSocket, &c, 1, MSG_PEEK);
                // DEBUG printf("%02X\n", c);
                if ((num > 0) && (c == '\n'))
                    recv(clientSocket, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';
    return i;
}

void index_file(int clientSocket, char *fileName)
{

    int numChars = 1;
    char buf[1024];

    buf[0] = 'A'; buf[1] = '\0';
    while ((numChars > 0) && strcmp("\n", buf))  // read & discard headers
        numChars = get_line(clientSocket, buf);


    FILE *file = fopen(fileName, "r");
    if(file == NULL)
        not_found(clientSocket);
    else
    {
        successful_headers(clientSocket, file, "html");
        file_text(clientSocket, file);
    }
    fclose(file);
}

void successful_headers(int clientSocket, FILE *file, char *type)
{
    char buf[1024];
    (void)file;
    // get local time
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = gmtime(&rawTime);

    // format string as "Sat, 01 Jan 2022 12:00:00 GMT"
    char timeString[50];
    strftime(timeString, sizeof(timeString), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "Date: %s\r\n", timeString);
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "%s\r\n",SERVER_STRING);
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/%s\r\n", type);
    send(clientSocket, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(clientSocket, buf, strlen(buf), 0);

}

void file_text(int clientSocket, FILE *file)
{
    char buf[1024];
    //read a line into buf
    fgets(buf, sizeof(buf), file);
    while (!feof(file))
    {
        send(clientSocket, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), file);
    }
}

void not_implemented(int clientSocket)
{
    char buf[1024];
    // get local time
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = gmtime(&rawTime);

    // format string as "Sat, 01 Jan 2022 12:00:00 GMT"
    char timeString[50];
    strftime(timeString, sizeof(timeString), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "Date: %s\r\n", timeString);
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "%s\r\n",SERVER_STRING);
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(clientSocket, buf, strlen(buf), 0);

    sprintf(buf, "<!DOCTYPE>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<html>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<head><title>Method Not Implemented</title></head>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<body>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<h1>501 Not Implemented</h1>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<p>The server does not support the functionality required to fulfill the request.</p>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "</body>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "</html>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
}

void not_found(int clientSocket)
{
    char buf[1024];
    // get local time
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = gmtime(&rawTime);

    // format string as "Sat, 01 Jan 2022 12:00:00 GMT"
    char timeString[50];
    strftime(timeString, sizeof(timeString), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

    sprintf(buf, "HTTP/1.1 404 Not Found\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "Date: %s\r\n", timeString);
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "%s\r\n",SERVER_STRING);
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(clientSocket, buf, strlen(buf), 0);

    sprintf(buf, "<!DOCTYPE html>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<html>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<head><title>404 Not Found</title></head>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<body>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<h1>404 Not Found</h1>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "<p>The requested URL was not found on this server.</p>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "</body>\r\n");
    send(clientSocket, buf, strlen(buf), 0);
    sprintf(buf, "</html>\r\n");
    send(clientSocket, buf, strlen(buf), 0);

}

void handleError(char *error)
{
    perror(error);
}