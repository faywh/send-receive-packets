#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


int connect_controller(int port)
{
    struct sockaddr_in my_addr;
    // memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");



    // create socket
    int sockfd  = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("create socket failed\n");
        return -1;
    }

    // connect
    if(connect(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0)
    {
        printf("connect failed\n");
        return -1;
    }
    return sockfd;
}

int talk_controller(int sockfd, unsigned char* send_msg)
{
    if(send(sockfd, send_msg, sizeof(send_msg), 0) < 0)
    {
        printf("send failed\n");
        return -1;
    }

    // recv
    unsigned char recv_msg[100];
    if(recv(sockfd, recv_msg, sizeof(recv_msg), 0) > 0)
    {
        printf("recv: %s\n", recv_msg);
    }else{
        printf("recv failed\n");
    }

    return 0;
}

int main() 
{
    unsigned char send_buf[] = "abcde\n";
    // unsigned char recv_buf[100];

    int sockfd = connect_controller(6788);
    if( sockfd < 0)
    {
        printf("connect controller failed\n");
    }

    while(1)
    {
        if(talk_controller(sockfd, send_buf) == 0)
        {
            printf("talk controller success\n");
        }
    }
    close(sockfd);
    return 0;
}