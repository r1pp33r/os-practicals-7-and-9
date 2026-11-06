#include "net.h"
static int listener=-1,connection=-1;
static void stop(int sig){if(connection>=0)close(connection);if(listener>=0)close(listener);_exit(128+sig);}
int main(int argc,char**argv){
 if(argc!=2){fprintf(stderr,"Usage: server PORT\n");return 2;}
 int port=number(argv[1]);setbuf(stdout,NULL);signal(SIGTERM,stop);signal(SIGINT,stop);signal(SIGPIPE,SIG_IGN);
 listener=socket(AF_INET,SOCK_STREAM,0);if(listener<0){perror("socket");return 1;}
 int on=1;if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&on,sizeof on)<0){perror("setsockopt");return 1;}
 struct sockaddr_in addr={.sin_family=AF_INET,.sin_port=htons((uint16_t)port),.sin_addr.s_addr=htonl(INADDR_LOOPBACK)};
 if(bind(listener,(struct sockaddr*)&addr,sizeof addr)<0||listen(listener,8)<0){perror("bind/listen");close(listener);return 1;}
 printf("127.0.0.1:LISTEN port=%d\n",port);srand((unsigned)getpid());
 for(;;){struct sockaddr_in remote;socklen_t size=sizeof remote;connection=accept(listener,(struct sockaddr*)&remote,&size);if(connection<0){if(errno==EINTR)continue;perror("accept");break;}
 char ip[INET_ADDRSTRLEN];if(!inet_ntop(AF_INET,&remote.sin_addr,ip,sizeof ip)){perror("inet_ntop");close(connection);connection=-1;continue;}
 printf("%s:CONNECTED port=%u\n",ip,ntohs(remote.sin_port));int secret=1+rand()%14,n=14,attempts=0;
 if(packet(connection,&n,1)==0)for(;;){int guess;if(packet(connection,&guess,0)<0)break;attempts++;int result=guess==secret;printf("%s:guess=%d result=%s\n",ip,guess,result?"YES":"NO");if(guess<1||guess>14){result=-1;packet(connection,&result,1);break;}if(packet(connection,&result,1)<0)break;if(result){printf("%s:WIN attempts=%d secret=%d\n",ip,attempts,secret);break;}}
 if(close(connection)<0)perror("close client");connection=-1;printf("%s:DISCONNECTED\n",ip);
 }
 close(listener);return 1;
}
