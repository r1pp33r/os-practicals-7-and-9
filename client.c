#include "net.h"
int main(int argc,char**argv){
 if(argc!=2){fprintf(stderr,"Usage: client IPv4:PORT\n");return 2;}
 char address[128];if(strlen(argv[1])>=sizeof address)return 2;strcpy(address,argv[1]);char*colon=strrchr(address,':');if(!colon){fprintf(stderr,"Address must include :PORT\n");return 2;}*colon++=0;
 struct sockaddr_in remote={.sin_family=AF_INET,.sin_port=htons((uint16_t)number(colon))};if(inet_pton(AF_INET,address,&remote.sin_addr)!=1){fprintf(stderr,"Invalid IPv4 address\n");return 2;}
 int fd=socket(AF_INET,SOCK_STREAM,0);if(fd<0){perror("socket");return 1;}if(connect(fd,(struct sockaddr*)&remote,sizeof remote)<0){perror("connect");close(fd);return 1;}
 int n;if(packet(fd,&n,0)<0||n<1||n>10000){fprintf(stderr,"Invalid greeting or closed connection\n");close(fd);return 1;}
 for(int guess=1;guess<=n;guess++){int result=0;if(packet(fd,&guess,1)<0||packet(fd,&result,0)<0){fprintf(stderr,"Connection lost\n");close(fd);return 1;}printf("guess=%d result=%s\n",guess,result==1?"YES":"NO");if(result==1){printf("WIN attempts=%d\n",guess);if(close(fd)<0){perror("close");return 1;}return 0;}if(result!=0)break;}
 fprintf(stderr,"Protocol error\n");close(fd);return 1;
}
