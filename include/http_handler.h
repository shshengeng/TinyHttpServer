#ifndef TINYHTTPSERVER_HTTP_HANDLER_H
#define TINYHTTPSERVER_HTTP_HANDLER_H

#define PORT 9090


int startUp(unsigned short port);
void *received_request(void *arg);
unsigned int get_line(int clientSocket, char *buf, int lenClientSocket);
void index_file(int clientSocket, char *path);
void handleError(char *error);
void unimplemented(int clientSocket);
void not_found(int clientSocket);










#endif //TINYHTTPSERVER_HTTP_HANDLER_H
