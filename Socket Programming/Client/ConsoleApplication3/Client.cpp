#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include<string>
#include <time.h>
#include<iostream>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512



struct FunctionMessage
{
	char type=0;
	int x=0;
	int y=0;
	int z=0;
	char message[BUFSIZE];
};

struct LoginMessage
{
		char type;
		char id[100];
	char password[100];
	bool success = false;
};


class ClientAgent
{
	private:
		WSADATA wsa;
		SOCKET sock;
		SOCKADDR_IN serveraddr;
		char buf[BUFSIZE + 1];
		int retval;
		int len;
		FunctionMessage functionMessage;
		LoginMessage loginMessage;
	public:
		ClientAgent();
		~ClientAgent();

		void err_quit(char *msg);
		void err_display(char *msg);
		void SetReadyState();
		void Communicate();
		int recvn(SOCKET s, char *buf, int len, int flags);

		void IsLogin();
		const void currentDateTime();
};

const void ClientAgent::currentDateTime() {
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y년%m월%d일.%X ", &tstruct);

	send(sock, buf, strlen(buf), 0);

	//return buf;
}

ClientAgent::ClientAgent()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;
}

ClientAgent::~ClientAgent()
{
	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	return ;
}

void ClientAgent::err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void ClientAgent::err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int ClientAgent::recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}
void ClientAgent::SetReadyState()
{
	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	//IsLogin();

	serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	IsLogin();

	//std::cout << "현재 시각 : " << currentDateTime() << std::endl;
}

void ClientAgent::Communicate()
{
	printf("기능을 선택하세요. 3 : 캐릭터이동 4: 대화하기");
	while (1) {
		scanf("%d", &functionMessage.type);
		
		//();
		switch (functionMessage.type)
		{
		case 3:

			for (int i = 0;i < sizeof(buf);i++)
				buf[i] = '\0';

			printf("x축 이동 : ");
			scanf("%d", &functionMessage.x);
			printf("y축 이동 : ");
			scanf("%d", &functionMessage.y);
			printf("z축 이동 : ");
			scanf("%d", &functionMessage.z);

			buf[0] = functionMessage.type;
			buf[1] = functionMessage.x;
			buf[2] = functionMessage.y;
			buf[3] = functionMessage.z;
			buf[4] = '\0';

			send(sock, buf, strlen(buf), 0);

			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}

			break;
		case 4:
			// 데이터 입력
			for (int i = 0;i <  sizeof(buf);i++)
			{
				buf[i] = '\0';
			}

			printf("\n[대화 메시지] ");
			if (fgets(functionMessage.message, BUFSIZE + 1, stdin) == "/bye")
				break;

			len = strlen(functionMessage.message);
			if (functionMessage.message[len - 1] == '\n')
				functionMessage.message[len - 1] = '\0';

			buf[0] = functionMessage.type;

			for (int i = 0;i < strlen(functionMessage.message);i++)
				buf[i + 1] = functionMessage.message[i];

			if (!strcmp(functionMessage.message, "/bye"))
			{
				//for (int i = 0;i < strlen(functionMessage.message);i++)
					//buf[i + 1] = functionMessage.message[i];
				retval = send(sock, buf, strlen(buf), 0);

				//for (int i = 1;i < strlen(loginMessage.id) + 1;i++)
				//	buf[i] = loginMessage.id[i - 1];

				//retval = send(sock, buf, strlen(buf), 0);



				return;
				//break;	
			}

			currentDateTime();
			
			// '\n' 문자 제거

			//		if (strlen(buf) == 0)
			//			break;

			// 데이터 보내기
			retval = send(sock, buf, strlen(buf), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			//printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

			// 데이터 받기
			/*retval = recvn(sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			//		else if (retval == 0)
			//			break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
			printf("[받은 데이터] %s\n", buf);*/
			break;
		}
	}
}

void main()
{
	ClientAgent Clinet;
	Clinet.SetReadyState();
	Clinet.Communicate();
}

void ClientAgent::IsLogin()
{
	bool isin = false;
	int choice = 0;
	char* inPassWord;
	while (!loginMessage.success)
	{
		printf("1 -> sign up\n");
		printf("2 -> sign in\n");
		scanf("%d", &loginMessage.type);
		switch (loginMessage.type)
		{
		case 1:
			printf("back -> /back\n");
			printf("ID\n");
			buf[0] = loginMessage.type;
			scanf("%s", loginMessage.id);
			buf[0] = loginMessage.type;
			for (int i = 0;i < strlen(loginMessage.id);i++)
			{
				buf[i + 1] = loginMessage.id[i];
			}
			buf[strlen(loginMessage.id) + 1] = '\0';
			retval = send(sock, buf, strlen(buf), 0);

			printf("Password\n");
			scanf("%s", loginMessage.password);

			for (int i = 0;i < strlen(loginMessage.id);i++)
			{
				buf[i + 1] = loginMessage.password[i];
			}

			buf[strlen(loginMessage.password) + 1] = '\0';
			retval = send(sock, buf, strlen(buf), 0);

			loginMessage.type = 0;
			break;
		case 2:
			printf("back -> /back\n");
			printf("ID\n");
			scanf("%s", loginMessage.id);
			//			printf("PassWord");
			//			scanf("%s", *inPassWord);
			//retval = send(sock,
			//	itoa(loginMessage.type, buf,2),strlen(buf), 0);
			buf[0] = loginMessage.type;
			for (int i = 0;i < strlen(loginMessage.id);i++)
			{
				buf[i + 1] = loginMessage.id[i];
			}
			buf[strlen(loginMessage.id) + 1] = '\0';

			currentDateTime();
			retval = send(sock, buf, strlen(buf), 0);

			printf("Password\n");
			scanf("%s", loginMessage.password);

			for (int i = 0;i < strlen(loginMessage.password);i++)
			{
				buf[i + 1] = loginMessage.password[i];
			}

			buf[strlen(loginMessage.password) + 1] = '\0';
			retval = send(sock, buf, strlen(buf), 0);

			loginMessage.type = 0;

//			printf("retval %d (char *)&loginMessage %s", retval, (char*)&loginMessage);
			loginMessage.success = true;
			loginMessage.type = 0;
			break;
		default:
			scanf("%d", &choice);
		}
	}
}