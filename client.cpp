#include "Multiplatformheader.h" //Все заголовки для работы в разных  OS (linux/win)

int main()
{
#if defined(_WIN32)
	WSADATA d;				// структура данных, содержащая информацию о версии сокетов в ОС
	if(WSAStartup(MAKEWORD(2,2),&d)) //инициализация струртуры WSADATA
	{
		fprintf(stderr,"Faild to initialize adapters!!!(%d)\n",GETSOCKETERRNO());
		return 1;
	}
#endif
char address[ADDRLEN] = { 0 };  //строка с адресом в формате "127.0.0.1"
char port[128] = {0};
printf("***** Configuring client ...\n");

get_yourIP(address); // процедура записывающая IP в формате "127.0.0.1" в строку address

struct addrinfo net_settings;					//структура содержащая информацию о интернет соединении: транспортный протокол, порт, размер адреса, тип сокета и т.д Подходит для всех типов алдресов
memset(&net_settings,0,sizeof(net_settings));
net_settings.ai_family = AF_INET;				//
net_settings.ai_socktype = SOCK_STREAM;			//Настройки будущего соединения
net_settings.ai_protocol = 0;					//

struct addrinfo *serveraddr = NULL;				// указатель на структуру, с нашими настройками.



printf("Enter server's address (ddd.ddd.ddd.ddd): ");

#if defined(_WIN32)
scanf_s("%s",address,sizeof(address));
#else
scanf("%s",address);
#endif

printf("Enter port: ");

#if defined(_WIN32)
scanf_s("%s",port,sizeof(port));
#else
scanf("%s",port);
#endif
getchar();

/*
//////////////////////////// тестирование
#if defined(_WIN32)
sprintf_s(address,ADDRLEN, "192.168.0.7");
sprintf_s(port,PORTLEN, "5000");
#else
sprintf(address,"192.168.10.93");
sprintf(port,"5000");
#endif
///////////////////////////
*/
if(getaddrinfo(address,port,&net_settings,&serveraddr))		//функция заполняет поля
{
	fprintf(stderr,"##### getaddrinfo() (%d) #####\n",GETSOCKETERRNO());
	show_error(GETSOCKETERRNO());
	return 1;
}

if (getnameinfo(serveraddr->ai_addr, serveraddr->ai_addrlen, address, sizeof(address), port, sizeof(port), NI_NUMERICHOST)) // получаем адрес и порт из нашей структуры
{
	fprintf(stderr, "##### Error: getnameinfo() (%d) #####", GETSOCKETERRNO());						// NI_NUMERICHOST - флаг для получения адреса и порта в формате "127.0.0.1:5000"
	show_error(GETSOCKETERRNO());
	return 1;
}
printf("***** Remote addess: %s:%s\n", address, port); //"127.0.0.1:5000"



SOCKET client_sock = socket(AF_INET,SOCK_STREAM,0);  //создаем сокет, через который будем общатся с сервером по указанному адресу
if(!ISVALIDSOCKET(client_sock))
{
	fprintf(stderr,"##### Faild  socket()!!!(%d) #####\n",GETSOCKETERRNO());
	show_error(GETSOCKETERRNO());
	return 1;
}

if(connect(client_sock,serveraddr->ai_addr,serveraddr->ai_addrlen)==-1) // пытаемся присоединиться к серверу
{
	fprintf(stderr,"##### Faild  connect()!!!(%d) #####\n",GETSOCKETERRNO());
	show_error(GETSOCKETERRNO());
	return 1;
}
printf("***** Connected!******\n");

char massage[MSGSIZE] ;

////////////////пробуем функцию select()/////////////////////////

while(1)
{
	memset(massage,'\0',MSGSIZE);
	fd_set sockets_set; // создаем пустой набор сокетов
	FD_ZERO(&sockets_set); // зануляем поля (обязательно!!!)
	FD_SET(client_sock,&sockets_set); //добавляем сокет клиента
#if !defined(_WIN32)
	FD_SET(fileno(stdin),&sockets_set); //добавляем в набор стандартный поток ввода (он имеет номер 0). Работает только в Linux 
#endif
	struct timeval timeout;		// специальная структура ХраНЯЩАЯ ВРЕМЯ В ДВУХ ПОЛЯХ, СЕКУНДЫ И МИЛИСЕКУНДЫ, устанавливаем  100000 милисекунд
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
	
	if(select(client_sock+1,&sockets_set,0,0, &timeout)<0)//////////////
	{
		fprintf(stderr,"##### select() ERROR! (%d) #####\n",GETSOCKETERRNO());
		show_error(GETSOCKETERRNO());
		return 1;
	}
	if(FD_ISSET(client_sock,&sockets_set))
	{
		int recv_bytes = recv(client_sock,massage,MSGSIZE,0);
		if(recv_bytes<1)
		{
			printf("\n***** Connection close by server!\n");
			break;
		}
		
		printf("*****SERVER RESPONSE (%d bytes recive): ",recv_bytes);
		printf("%.*s",recv_bytes,massage);  

	}
	
	
#if defined (_WIN32)
	if(_kbhit())
	{
#else
	if(FD_ISSET(0,&sockets_set))
	{
#endif
	memset(massage,0,MSGSIZE);
	if(!fgets(massage,MSGSIZE,stdin)){break;}
	//printf("We sending: %s",massage);
	int send_bytes = send(client_sock,massage,strlen(massage),0);
	//printf("We sending(%d bytes): %s",send_bytes,massage);
	}
	fflush(stdout);
}




//////////////////////////////////////////////////////////////


/*

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
*/
	CLOSESOCKET(client_sock);
#if defined(_WIN32)
	WSACleanup();
#endif

	printf("***FINISH***");
	return 0;
}
