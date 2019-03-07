#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include<tchar.h>
#include<iostream>
#include<fstream>
#include<string>

#define SERVERPORT 9000
#define BUFSIZE    512



class ServerAgent
{
private:
	WSADATA wsa;
	SOCKET listen_sock;
	SOCKADDR_IN serveraddr;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
public:
	ServerAgent();
	~ServerAgent();
	void err_quit(char *msg);
	void err_display(char *msg);
	void SetReadyState();
	void Communicate();
	static DWORD WINAPI ProcessClient(LPVOID lpParam);
};


ServerAgent::ServerAgent()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;
}

ServerAgent::~ServerAgent()
{
	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
}

void ServerAgent::err_quit(char *msg)
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

void ServerAgent::err_display(char *msg)
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

DWORD WINAPI ServerAgent::ProcessClient(LPVOID arg)
{
	ServerAgent* pServerAgent = (ServerAgent*)arg;
	SOCKET sock = pServerAgent->client_sock;
	INT retval;
	INT addressLen;
	SOCKADDR_IN threadSocketAddress;
	TCHAR Buffer[BUFSIZE + 1];

	addressLen = sizeof(threadSocketAddress);
	getpeername(sock, (SOCKADDR *)&threadSocketAddress, &addressLen);

	while (true)
	{
		retval = recv(sock, (CHAR *)Buffer, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			pServerAgent->err_display(_T("recv()"));
			break;
		}
		else if (retval == 0)
			break;

		Buffer[retval] = _T('\0');

		if (Buffer[0] == 1)
		{
			std::ofstream fout;
			fout.open("id.txt", std::ios_base::out | std::ios_base::app);
			Buffer[strlen(Buffer) + 1] = '\0';
			Buffer[strlen(Buffer)] = '\n';
			//Buffer[strlen(Buffer) + 1] = '\0';
			for (int i = 1;i<strlen(Buffer);i++)
				fout.put(Buffer[i]);
			fout.close();

			for (int i = 1;i < BUFSIZE;i++)
			{
				Buffer[i] = '\0';
			}
			retval = recv(sock, (CHAR *)Buffer, BUFSIZE, 0);

			fout.open("password.txt", std::ios_base::out | std::ios_base::app);
			Buffer[strlen(Buffer) + 1] = '\0';
			Buffer[strlen(Buffer)] = '\n';
			//Buffer[strlen(Buffer) + 1] = '\0';
			for (int i = 1;i<strlen(Buffer);i++)
				fout.put(Buffer[i]);
			fout.close();
		}
		else if (Buffer[0] == 2)
		{
			std::ifstream fin;
			fin.open("id.txt");
			char temp2[20];
			int lineCount = 0;
			while (true)
			{
				fin.getline(temp2, sizeof(temp2));
				lineCount++;
				char temp3[20];
				for (int i = 1;i < strlen(Buffer);i++)
				{
					temp3[i - 1] = Buffer[i];
				}
				if (strlen(temp2) > 0)
				{
					if (!strncmp(temp2, temp3, strlen(temp2)))
					{
						for (int i = 1;i < strlen(Buffer);i++)
							printf("%c", Buffer[i]);

						printf(" connection request success\n");

						std::ofstream fout;
						fout.open("Log.txt", std::ios_base::out | std::ios_base::app);
						for (int i = 1;i < strlen(Buffer);i++)
							fout.put(Buffer[i]);

						char temp[] = " connection request success\n";
						for (int i = 0;i < strlen(temp);i++)
							fout.put(temp[i]);
						fin.close();
						break;

					}
					if (temp2 == '\0')
					{
						printf("잘못된 아이디 입니다.");
						break;
					}
				}
				else
				{
					printf("잘못된 아이디 또는 비밀번호 입니다.");
					break;
				}
			}
		}
		else
		{
			//printf("\n[TCP/%s:%d] %s", inet_ntoa(pServerAgent->clientaddr.sin_addr), ntohs(pServerAgent->clientaddr.sin_port), Buffer);
			//printf("\n[TCP/%s:%d]", inet_ntoa(pServerAgent->clientaddr.sin_addr), ntohs(pServerAgent->clientaddr.sin_port));
			std::ofstream fout;

			fout.open("Log.txt", std::ios_base::out | std::ios_base::app);
			if (Buffer[0] == 4)
			{
				char temp[100];
				Buffer[strlen(Buffer) + 1] = '\0';
				Buffer[strlen(Buffer)] = '\n';
				for (int i = 1;i < strlen(Buffer);i++)
				{
					temp[i - 1] = Buffer[i];
				}
				if (!strncmp(temp, "/bye",4))
				{
					printf("\n[TCP/%s:%d]\n", inet_ntoa(pServerAgent->clientaddr.sin_addr), ntohs(pServerAgent->clientaddr.sin_port));
					for (int i = 0;i < sizeof(Buffer);i++)
						Buffer[0] = '\0';
					retval = recv(sock, (CHAR *)Buffer, BUFSIZE, 0);
					printf("%s", Buffer);
					continue;
				}

				for (int i = 1;i < strlen(Buffer);i++)
				{
					fout.put((int) Buffer[i]);
					printf("%c", Buffer[i]);
				}
			}
			else if (Buffer[0] == 3)
			{
				//Buffer[strlen(Buffer) + 1] = '\0';
				//Buffer[strlen(Buffer)] = '\n';
				for (int i = 1;i < strlen(Buffer);i++)
				{
					fout.put(Buffer[i]);
					printf("%d", Buffer[i]);
				}
			}
			else
				for (int i = 0;i < strlen(Buffer);i++)
					fout.put(Buffer[i]);



		}

		retval = send(pServerAgent->client_sock, Buffer, retval, 0);
		if (retval == SOCKET_ERROR) 
		{
			pServerAgent->err_display("send()");
			break;
		}
	}
}


void ServerAgent::SetReadyState()
{
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit(_T("Socket()"));

	INT Retval;

	//bind()
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	Retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (Retval == SOCKET_ERROR)
		err_quit(_T("Bind()"));

	//listen()
	Retval = listen(listen_sock, SOMAXCONN);
	if (Retval == SOCKET_ERROR)
		err_quit(_T("listen()"));
}

void ServerAgent::Communicate()
{
	HANDLE hThread[100];
	int ClinetCount = 0;
	while (true)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		if (client_sock == INVALID_SOCKET)
		{
			err_display(_T("Accept"));
			break;
		}
		
		//Displaying Client Display
		
		printf("[TCP 서버] 클라이언트 접속: IP주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//Create Thread

		hThread[ClinetCount] = CreateThread(NULL, 0, ProcessClient,
			this, 0, NULL);
		ClinetCount++;
		if (hThread[ClinetCount] == NULL)
			closesocket(client_sock);
		else
			CloseHandle(hThread[ClinetCount-1]);
	}
}

INT _tmain(INT argc, TCHAR *argv[])
{
	ServerAgent Server;
	Server.SetReadyState();
	Server.Communicate();

	return 0;
}