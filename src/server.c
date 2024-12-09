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
#define SOCKET_QUEUE 10

int main() {
    int server_fd, connection_fd, n;
    struct sockaddr_in server_addr;
    uint8_t send_buff[MAX_DATA + 1];
    uint8_t receive_buff[MAX_DATA + 1];
    char server_addr_string[MAX_DATA + 1];
    char connection_addr_string[MAX_DATA + 1];

    // Create a IPv4 STREAM TCP Socket 
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) error_msg("Create socket error");

    int opt = 1;
    int set_socket_status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (set_socket_status < 0) error_msg("Error on set socket");

    // Configuring the socket addrs
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(PORT);

    // Binding Socket to addrs
    int binding_status = bind(server_fd, (sockaddr*) &server_addr, sizeof(server_addr));
    if (binding_status < 0) error_msg("Binding socket error");

    // Turn socket able to receive connections
    int listen_status = listen(server_fd, SOCKET_QUEUE);
    if (listen_status < 0) error_msg("Set socket to listen error");

    inet_ntop(AF_INET, &server_addr, server_addr_string, MAX_DATA);
    printf("Listenning on %s\r\n\n", server_addr_string);
    
    fflush(stdout);

    while (true) {
        // Accept the first connection on the queue (wait if don't have connection on the queue) 
        struct sockaddr_in connection_addr;
        socklen_t connection_addr_len;
        
        printf("Waiting connection...\r\n");
        fflush(stdout);
        connection_fd = accept(server_fd, (sockaddr *) &connection_addr, &connection_addr_len);

        inet_ntop(AF_INET, &connection_addr, connection_addr_string, MAX_DATA);
        printf("Connected with %s\r\n", connection_addr_string);
        fflush(stdout);

        // Receive data from accepted connection
        memset(receive_buff, 0, MAX_DATA);
        while ((n = read(connection_fd, receive_buff, MAX_DATA - 1)) > 0) {
            printf("Received bytes %d \r\n", n);
            printf("Msg: %s", (char*)receive_buff);

            if (receive_buff[n - 1]  == '\n') break;
        }

        if (n < 0) error_msg("Read error");
        
        snprintf((char*)send_buff, sizeof(send_buff), "HTTP/1.1 200 OK  \r\n\r\nHello");

        write(connection_fd, (char*)send_buff, strlen((char*)send_buff));
        close(connection_fd);

        memset(receive_buff, 0, MAX_DATA);
    }

    close(server_fd);
}