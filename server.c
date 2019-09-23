#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    int sockfd = 0;

    struct sockaddr_in client_addr;
    unsigned char recv_str[100]; 
    unsigned char send_str[] = "i am server\n";


    struct sockaddr_in my_addr;
    // memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(2000);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("create socket failed\n");
        return -1;
    }

    
    if(bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr_in)) < 0)
    {
        printf("bind failed\n");
        return -1;
    }

    if(listen(sockfd, 5) < 0)
    {
        printf("listen failed\n");
        return -1;
    }

    unsigned int client_addr_len = sizeof(struct sockaddr_in);
    int newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
    if( newsockfd < 0)
    {
        printf("accept failed\n");
        return -1;      
    }

    //send
    if(send(newsockfd, send_str, sizeof(send_str), 0) < 0)
    {
        printf("send failed\n");
        return -1;
    }

    // recv
    if(recv(newsockfd, recv_str, sizeof(recv_str), 0) > 0)
    {
        printf("recv: %s\n", recv_str);
    }

    close(sockfd);
    close(newsockfd);
    return 0;
}