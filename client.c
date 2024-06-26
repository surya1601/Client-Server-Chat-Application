#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <netdb.h>


void error(const char *msg){
    perror(msg);
    exit(1);
}

/*
argv[0] -> filename
argv[1] -> server_ipaddress
argv[2] -> portno
*/

int main(int argc, char *argv[]){
    char buffer[255];
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if(argc < 3){
        fprintf(stderr, "Usage %s hostname port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0) error("ERROR opening socket\n");

    printf("1. SockFD: %d\n", sockfd);

    server = gethostbyname(argv[1]);
    if(server == NULL) fprintf(stderr, "Error, no such host\n");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    bcopy((char *)server->h_addr_list[0], (char *) &serv_addr.sin_addr.s_addr, server->h_length);

    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("Connection Failed\n");
    printf("2. SockFD: %d\n", sockfd);
    while(1){
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if(n < 0) error("Error on writing\n");
        
        bzero(buffer, 255);
        n = read(sockfd, buffer, 255);
        if(n < 0) error("Error on reading\n");
        printf("Server: %s", buffer);

        int i = strncmp("Bye", buffer, 3);
        if(i == 0) break;
    }

    close(sockfd);


    return 0;
}