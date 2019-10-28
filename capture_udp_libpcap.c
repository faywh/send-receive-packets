#include <pcap.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
typedef unsigned char u_char; 
typedef unsigned short u_short; 
#define ETHER_ADDR_LEN 6
#define SIZE_ETHERNET 14

 struct sniff_ethernet
 {
  u_char ether_dhost[ETHER_ADDR_LEN];	/* destination host address */
  u_char ether_shost[ETHER_ADDR_LEN];	/* source host address */
  u_short ether_type;		/* IP? ARP? RARP? etc */
 };

/* IP header */
 struct sniff_ip
 {
  u_char ip_vhl;		/* version << 4 | header length >> 2 */
  u_char ip_tos;		/* type of service */
  u_short ip_len;		/* total length */
  u_short ip_id;		/* identification */
  u_short ip_off;		/* fragment offset field */
  u_char ip_ttl;		/* time to live */
  u_char ip_p;			/* protocol */
  u_short ip_sum;		/* checksum */
  struct in_addr ip_src, ip_dst;	/* source and dest address */
 };


void getPacket(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
	 const struct sniff_ip *ip;	
	 if(pkthdr->len >= 14)
	 {
		 ip = (struct sniff_ip *)(packet+ SIZE_ETHERNET);
		 if(ip->ip_p == 17)
		 {
			   printf ("From: %s\n", inet_ntoa(ip->ip_src));
		 	   printf ("To: %s\n", inet_ntoa(ip->ip_dst));
		 		 int *id = (int*)userarg;
    		 printf("Get %dth UDP packet\n", ++(*id));
     		 printf("packet length: %d\n", pkthdr->len);
	   		 printf("number of bytes: %d\n", pkthdr->caplen);
	   		 printf("Received time: %s, %6ld\n", ctime((const time_t*)&pkthdr->ts.tv_sec), pkthdr->ts.tv_usec);
				 //printf("Received time: %ld us\n", (pkthdr->ts.tv_sec)*1000000 + pkthdr->ts.tv_usec);
				 FILE *fpt;
				 fpt = fopen("timestamp.txt", "a");
				 fprintf(fpt, "%s %s %ld\n", inet_ntoa(ip->ip_src), inet_ntoa(ip->ip_dst), (pkthdr->ts.tv_sec*1000000 + pkthdr->ts.tv_usec));
				 fclose(fpt);
		 }
	 }
}


void capture_packet2(pcap_t* device)
{
	int id = 0;
	pcap_loop(device, 5, getPacket, (u_char*)&id);
}


int main()
{
	char errBuf[PCAP_ERRBUF_SIZE], *devStr;
	// 获取设备
	// devStr = pcap_lookupdev(errBuf);
	devStr = "wlp3s0";

	if(devStr)
	{
	  printf("success: device: %s\n", devStr);
	} else {
	  printf("error: %s\n", errBuf);
		exit(1);
	}

	// 打开一个设备，等待直到数据包到来
	/* 
	参数：1.获取的接口 
			 2.对每个数据包从开头要抓多少字节（任何协议的数据包都要小于65535）
			 3.是否打开混杂模式（0非混杂),如果是混杂模式，对应的网卡也要打开混杂模式，ifconfig eth0 promisc
			 4.指定需要等待的毫秒数，0表示一直等待直到数据包到来
			 5.存放出错信息的数组
	*/
	pcap_t *device = pcap_open_live(devStr, 65535, 1, 0, errBuf);
  if(!device)
	{
		printf("error: pcap_open_live(): %s\n", errBuf);
		exit(1);
	}

  struct bpf_program fp;
  if(pcap_compile(device, &fp, "src host 192.168.31.166", 1, 0) == -1)
	{
		printf("error in fliter\n");
		exit(1);
	}

  if(pcap_setfilter(device, &fp) == -1)
	{
		exit(1);
	}

	// 等待数据包
	capture_packet2(device);
	// 关闭设备

	pcap_close(device);
	return 0;
}
