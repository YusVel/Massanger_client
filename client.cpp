#include "Multiplatformheader.h"

int main()
{
#if defined(_WIN32)
	WSADATA d;
	if(WSAStartup(MAKEWORD(2,2),&d))
	{
		fprintf(stderr,"Faild to initialize adapters!!!(%d)\n",GETSOCKETERRNO());
		return 1;
	}
#endif
printf("Configuring client ...\n");

struct addrinfo net_settings;
memset(&net_settings,0,sizeof(net_settings));
net_settings.ai_family = AF_INET;
net_settings.ai_socktype = SOCK_STREAM;
net_settings.ai_protocol = 0;

struct addrinfo *serveraddr;

printf("Введите адрес сервера(ddd.ddd.ddd.ddd): ");
char address[64] = {0};
gets(address);
printf("Введите порт: ");
char port[8] = {0};
gets(port);

if(getaddrinfo(address,port,&net_settings,&serveraddr))
{
	fprintf(stderr,"getaddrinfo() ERROR!",GETSOCKETERRNO());
	return 1;
}



SOCKET client_sock = socket(AF_INET,SOCK_STREAM,0);
if(!ISVALIDSOCKET(client_sock))
{
	fprintf(stderr,"Faild  socket()!!!(%d)\n",GETSOCKETERRNO());
	return 1;
}

/*
struct sockaddr_in serveraddr;
memset(&serveraddr,0,sizeof(serveraddr));
serveraddr.sin_family = AF_INET;
serveraddr.sin_port = htons(8080);

if (inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr) < 1)
{
	fprintf(stderr, "Faild  inet_pton()!!!(%d)\n", GETSOCKETERRNO());
	return 1;
}

*/
if(connect(client_sock,(sockaddr*)&serveraddr,sizeof(serveraddr))==-1)
{
	fprintf(stderr,"Faild  connect()!!!(%d)\n",GETSOCKETERRNO());
	return 1;
}
printf("Connected!\n");
char massage[MSGSIZE] = {'\0'};

#if defined(_WIN32)
sprintf_s(massage, "Hello world!");
#else
sprintf(massage, "Hello world!");
#endif

int send_bytes = send(client_sock,massage,strlen(massage),0);
printf("Send: %s (%d bytes)\n",massage,send_bytes);
memset(massage,0,MSGSIZE);
int recv_bytes  = recv(client_sock,massage,MSGSIZE,0);
printf("Recive: %s (%d bytes)\n",massage,recv_bytes);

memset(massage,0,MSGSIZE);
recv_bytes  = recv(client_sock,massage,MSGSIZE,0);
printf("Recive: %s (%d bytes)\n",massage,recv_bytes);

#if defined(_WIN32)
WSACleanup();
#endif


	return 0;
}
