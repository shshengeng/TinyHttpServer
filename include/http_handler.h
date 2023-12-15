#ifndef TINYHTTPSERVER_HTTP_HANDLER_H
#define TINYHTTPSERVER_HTTP_HANDLER_H

#define PORT 9090
#define SERVER_STRING "Server: tinyHttpServer/0.1"

int startUp(unsigned short port);
void received_request(void *arg);
int get_line(int clientSocket, char *buf);
void index_file(int clientSocket, char *fileName);
void successful_headers(int clientSocket, FILE *file, char *type);
void file_text(int clientSocket, FILE *file);
void handleError(char *error);
void not_implemented(int clientSocket);
void not_found(int clientSocket);


#endif //TINYHTTPSERVER_HTTP_HANDLER_H
