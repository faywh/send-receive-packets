#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

 
typedef struct
{
	uint8_t ip_hl:4;
	uint8_t ip_v:4;
	uint8_t ip_tos;
	uint16_t ip_len;
	uint16_t ip_id;
	uint16_t ip_off;
	uint8_t ip_ttl;
	uint8_t ip_p;
	uint16_t ip_sum;
	uint32_t ip_src;
	uint32_t ip_dst;
} __attribute__ ((packed)) IpHeader;
 
typedef struct
{
	uint16_t source;
	uint16_t dest;
	uint16_t len;
	uint16_t check;
} __attribute__ ((packed)) UdpHeader;
 
typedef struct
{
	uint32_t src_ip;
	uint32_t dst_ip;
	uint8_t zero;
	uint8_t protocol;
	uint16_t udp_len;
} __attribute__ ((packed)) PseudoUdpHeader;
 
static uint16_t CalcChecksum(void *data, size_t len)
{
	uint16_t *p = (uint16_t *)data;
	size_t left = len;
	uint32_t sum = 0;
	while (left > 1) {
		sum += *p++;
		left -= sizeof(uint16_t);
	}
	if (left == 1) {
		sum += *(uint8_t *)p;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return ~sum;
}
 
int SendFakeUdp(const void *msg, size_t len, const char *src_ip, 
					uint16_t src_port, const char *dst_ip, uint16_t dst_port)
{
	static int threadIndex = 1;
	printf("create %d thread\n", threadIndex);
	threadIndex++;
	if (!msg || len <= 0 || !src_ip || !dst_ip) {
		return -1;
	}
 
	static int fd = -1;
	if (fd == -1) {
		if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
			//add by wuhui
			printf("error: %s\n", strerror(errno));
			return -2;
		}
		int on = 1;
		if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) == -1) {
			close(fd);
			fd = -1;
			return -3;
		}
	}
 
	static char buf[65536];
	IpHeader *ip_header = (IpHeader *)buf;
	UdpHeader *udp_header = (UdpHeader *)(ip_header + 1);
	char *data = (char *)(udp_header + 1);
	PseudoUdpHeader *pseudo_header = (PseudoUdpHeader *)((char *)udp_header - sizeof(PseudoUdpHeader));
	if (sizeof(*ip_header) + sizeof(*udp_header) + len + len % 2 > sizeof(buf)) {
		return -4;
	}
 
	uint32_t src_ip_v = 0;
	if (inet_pton(AF_INET, src_ip, &src_ip_v) <= 0) {
		return -5;
	}
	uint32_t dst_ip_v = 0;
	if (inet_pton(AF_INET, dst_ip, &dst_ip_v) <= 0) {
		return -6;
	}
 
	uint16_t udp_len = sizeof(*udp_header) + len;
	uint16_t total_len = sizeof(*ip_header) + sizeof(*udp_header) + len;
 
	pseudo_header->src_ip = src_ip_v;
	pseudo_header->dst_ip = dst_ip_v;
	pseudo_header->zero = 0;
	pseudo_header->protocol = IPPROTO_UDP;
	pseudo_header->udp_len = htons(udp_len);
 
	udp_header->source = htons(src_port);
	udp_header->dest = htons(dst_port);
	udp_header->len = htons(sizeof(*udp_header) + len);
	udp_header->check = 0;
 
	memcpy(data, msg, len);
 
	size_t udp_check_len = sizeof(*pseudo_header) + sizeof(*udp_header) + len;
	if (len % 2 != 0) {
		udp_check_len += 1;
		*(data + len) = 0;
	}
	udp_header->check = CalcChecksum(pseudo_header, udp_check_len);
 
	ip_header->ip_hl = sizeof(*ip_header) / sizeof (uint32_t);
	ip_header->ip_v = 4;
	ip_header->ip_tos = 0;
	ip_header->ip_len = htons(total_len);
	ip_header->ip_id = htons(0); //为0，协议栈自动设置
	ip_header->ip_off = htons(0);
	ip_header->ip_ttl = 255;
	ip_header->ip_p = IPPROTO_UDP;
	ip_header->ip_src = src_ip_v;
	ip_header->ip_dst = dst_ip_v;
	ip_header->ip_sum = 0;
 
	//协议栈总是自动计算与填充
	//ip_header->ip_sum = CalcChecksum(ip_header, sizeof(*ip_header));
 
	struct sockaddr_in dst_addr;
	memset(&dst_addr, 0, sizeof(dst_addr));
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_addr.s_addr = dst_ip_v;
	dst_addr.sin_port = htons(dst_port);

	if (sendto(fd, buf, total_len, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) != total_len)  {
	return -7;
	}
	return 0;
}
 
int main(int argc, char *argv[])
{
	
	if (argc != 5) {
		printf("Usage: %s src_ip src_port dst_ip dst_port\n", argv[0]);
		return -1;
	}
	unsigned char buf[16];
    	for (int i = 0; i < 16; i++) {
       		buf[i] = 0xff;
   	 }
	int ret = SendFakeUdp(buf, sizeof(buf), argv[1], strtoul(argv[2], NULL, 0), argv[3], strtoul(argv[4], NULL, 0));
	if (ret != 0) {
		printf("SendFakeUdp fail. ret=%d\n", ret);
		return -1;
	}
	printf("SendFakeUdp succ.\n");
	return 0;
}
