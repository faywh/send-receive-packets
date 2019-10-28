/// \file mysynflood.c
/// \author Zuoru YANG (zryang@cse.cuhk.edu.hk)
/// \brief a simple program to send tcp packet to a given ip address and port
/// \version 0.1
/// \date 2019-04-09
///
/// \copyright Copyright (c) 2019
///
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> ///< getsockname
#include <stdlib.h>
#include <unistd.h>
//#include "tcp_pkt.h"

int main(int argc, char const *argv[])
{
    /* code */
    // parse the parameters
    if (argc != 5){
        printf("The amount of parameters is not correct\n");
        printf("Usage: %s <dstIP> <dstPort> <time interval(ms)> <count>.\n", argv[0]);
        printf("-------------------------help------------------------\n"
                "<dstIp>: the destination ip.\n"
                "<dstport>: the connection port in destination host.\n"
                "<time interval>: the time interval of sending packets.\n"
                "<count>: the count of sending packet.\n");
        exit(1);
    }
    /*
    * 1): dstIP
    * 2): dstPort
    * 3): time interval
    */
    const char* dstIp = argv[1];
    unsigned short dstPort = (unsigned short) atoi(argv[2]);  //atoi()把接收的字符串转换成整数
    int timeInterval = atoi(argv[3]);
    int count = atoi(argv[4]);
    struct sockaddr_in targetAdd;


    /**Initialize the local ip address*/
    struct sockaddr_in myAddr;
    memset(&myAddr, 0, sizeof(myAddr));  //地址所有位初始化为0
    myAddr.sin_family = AF_INET;  //设置协议家族为TCP/IP协议
    myAddr.sin_addr.s_addr = INADDR_ANY;  //设置IP地址
    // inet_aton("192.168.31.5", &myAddr.sin_addr);
    myAddr.sin_port = htons(8889); // default use 8889, htons()将主机字节转化为网络字节

    /**Initialize the dst host ip and port*/
    struct sockaddr_in dstAddr;
    memset(&dstAddr, 0, sizeof(dstAddr));
    dstAddr.sin_family = AF_INET;  // AF_INET represents the TCP/IP
    inet_aton(dstIp, &dstAddr.sin_addr); // inet_aton()将一个字符串ip转换成一个32位的网络序列ip地址
    dstAddr.sin_port = htons(dstPort);
    //printf("The sin_port: %x", targetAdd.sin_port);
    
    /**start to create socket */
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM是面向无连接的udp套接字名字，UPD packet，第一个参数是指明ipv4地址
    if (sockfd < 0) {
        printf("Error: socket creates wrong\n");
        exit(1);
    }

    /**bind to the given port */
    if (bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr)) == -1) {
        printf("Error: port binding fails.\n");
        exit(1);        
    }

    /**generate random data and pad into buffer*/
    unsigned char buf[16];
    for (int i = 0; i < 16; i++) {
        buf[i] = 0xff;
    }
    /**send UDP packet via socket */
    for (int i = 0; i < count; i++) {
        usleep(timeInterval);
        printf("Sending %dth packet to %s.\n", i, dstIp);
        if (sendto(sockfd, buf, sizeof(buf), 0, 
            (struct sockaddr*)&dstAddr, sizeof(dstAddr)) == -1) {
            printf("Error: send packet fails.\n");
            exit(1);
        }
    }

    close(sockfd);

    return 0;
}
           