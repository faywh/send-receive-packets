#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


int main()
{
    unsigned char send_buf[] = "abcde";
    unsigned char recv_buf[100];

    struct sockaddr_in my_addr;
    // memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(6788);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("create socket failed\n");
        return -1;
    }

    // connect
    if (connect(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
    {
        printf("connect failed\n");
        return -1;
    }

    while (1)
    {
        // printf("%d\n", sizeof(send_buf));
        if (send(sockfd, send_buf, sizeof(send_buf), 0) < 0)
        {
            printf("send failed\n");
            return -1;
        }

        // recv
        if (recv(sockfd, recv_buf, sizeof(recv_buf), 0) > 0)
        {
            printf("recv: %s\n", recv_buf);
        }
    }
    close(sockfd);
    return 0;
}