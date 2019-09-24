#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>


void *handle_msg(void *ptr)
{
    int fd = *(int *)ptr;
    printf("fd = %d\n", fd);

    unsigned char recv_str[100];
    unsigned char send_str[] = "i am server\n";

    if(recv(fd, recv_str, sizeof(recv_str), 0) > 0)
    {
        printf("recv: %s\n", recv_str);
    }

    if(send(fd, send_str, sizeof(send_str), 0) < 0)
    {
        printf("send failed\n");
    }
    close(fd);
}

int main()
{
    int sockfd = 0;

    struct sockaddr_in client_addr;



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

    if(listen(sockfd, 1) < 0)
    {
        printf("listen failed\n");
        return -1;
    }

    pthread_t thread;
    unsigned int client_addr_len = sizeof(struct sockaddr_in);
    while(1){
        int newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
        if( newsockfd < 0)
        {
            printf("accept failed\n");
            return -1;      
        }

        // printf("get a connect from %s\n", inet_ntoa(client_addr.sin_addr));

        pthread_create(&thread, NULL, handle_msg, (void *)&newsockfd);
    }
    

    close(sockfd);

    return 0;
}