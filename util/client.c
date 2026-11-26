#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr = { .sin_family = AF_INET,
                                      .sin_port = htons(6379), 
                                    };

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid server address\n");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sock);
        return 1;
    }

    char buf[128];
    char msg[128] = "PING\nPING\n";
    for(int i = 0; i < 2; i++)
    {
        if(send(sock, msg, strlen(msg), 0) < 0) {
            printf("Pong failed: %s \n", strerror(errno));
        }
        ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
        if (n == -1) {
            perror("recv");
            close(sock);
            return 1;
        }
        
        if(n == 0) {
            printf("Server closed the connection without sending the data");
        } else {
            buf[n] = '\0';
            printf("Server replied with %s", buf);
        }
    }
    close(sock);

    return 0;
}