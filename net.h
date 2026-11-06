#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
static int number(const char*s){char*e;long n=strtol(s,&e,10);if(!*s||*e||n<1||n>65535){fprintf(stderr,"Invalid port\n");exit(2);}return (int)n;}
static int packet(int fd,int*value,int writing){uint32_t word=writing?htonl((uint32_t)*value):0;size_t pos=0;while(pos<sizeof word){ssize_t n=writing?send(fd,(char*)&word+pos,sizeof word-pos,MSG_NOSIGNAL):recv(fd,(char*)&word+pos,sizeof word-pos,0);if(n<0&&errno==EINTR)continue;if(n<=0)return -1;pos+=(size_t)n;}if(!writing)*value=(int)ntohl(word);return 0;}
