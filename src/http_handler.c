#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>

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

void *received_request(void *arg)
{
    int clientSocket = (intptr_t)arg;
    char buf[1024];
    unsigned int numChars; //unsigned int
    char method[256];
    char url[256];
    char path[256];
    struct stat st;

    //GET / Http1.1
    numChars = get_line(clientSocket, buf, sizeof(clientSocket));
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
        //返回一个 html页面 501
        unimplemented(clientSocket);
        return NULL;
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

    if (strcasecmp(method, "GET") == 0)
    {

    }

    sprintf(path, "htdocs%s", url);
    if (path[strlen(path) - 1] == '/')
        strcat(path, "index.html");
    if (stat(path, &st) == -1) {  //检查指定路径的文件或目录是否存在。如果 stat() 返回 -1，可能是因为文件或目录不存在、权限不足或其他错误原因
        while ((numChars > 0) && strcmp("\n", buf))  /* read & discard headers */
            numChars = get_line(clientSocket, buf, sizeof(buf));
        not_found(clientSocket);
    }
    else
    {
        if ((st.st_mode & S_IFMT) == S_IFDIR)
            strcat(path, "/index.html");


        index_file(clientSocket, path);

    }
    close(clientSocket);
}


unsigned int get_line(int clientSocket, char *buf, int lenClientSocket)
{
    char c = '0';
    int i = 0;
    int num;

    //receive from socket buffer until get '\n'
    while(c != '\n' && i < lenClientSocket)
    {
        num = recv(clientSocket, &c, 1, 0);
        if(num > 0)
        {
//            if(c == '\r')
//            {
//                len = recv(clientSocket, &c, 1, 0);
//                if(c == '\n')
//
//            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    return i;
}



void handleError(char *error)
{
    perror(error);
}