#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/socket.h>

#include "utils/error_msg.h"

#define PORT 18000
#define MAX_DATA 4096

int main(int argc, char* argv[]) {
    int socket_fd, n;
    int send_bytes;
    struct sockaddr_in server_addr;
    char send_buff[MAX_DATA];
    char receive_buff[MAX_DATA];

    if (argc < 2) {
        printf("Missing server address\r\n");
        return 1;
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error_msg("Create socket error");
    
    memset(&server_addr, 0, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) error_msg("Convert address error");

    if (connect(socket_fd, (sockaddr*) &server_addr, sizeof(server_addr))) error_msg("Error on connect");

    sprintf(send_buff, "GET / HTTP/1.1\r\n\r\n");
    send_bytes = strlen(send_buff);

    if (write(socket_fd, send_buff, send_bytes) != send_bytes) error_msg("Error on send message");

    memset(receive_buff, 0, sizeof(receive_buff));
    while ((n = read(socket_fd, receive_buff, MAX_DATA - 1)) > 0) printf("%s\r\n", receive_buff);

    if (n < 0) error_msg("Error on receive message");

    return 0;
}