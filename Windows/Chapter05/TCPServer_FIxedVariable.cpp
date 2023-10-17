#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

int main(int argc, char* argv[]) {
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET) err_quit("socket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_socket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == INVALID_SOCKET) err_quit("listen()");

	retval = listen(listen_socket, SOMAXCONN);
	if (retval == INVALID_SOCKET) err_quit("listen()");

	// 데이터 통신할 클라이언트 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	int len; // 고정 길이 데이터
	char buf[BUFSIZE + 1]; //가변 길이 데이터

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_socket, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", addr, clientaddr.sin_port);

		while (1) {
			// 데이터 받기(고정길이)
			retval = recv(client_sock, (char*)&len, sizeof(int), MSG_WAITALL);
			if (retval == INVALID_SOCKET) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			// 데이터 받기(가변길이)
			retval = recv(client_sock, buf, len, MSG_WAITALL);
			if (retval == INVALID_SOCKET) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
		}

		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
	}

	closesocket(listen_socket);
	WSACleanup();
	return 0;
}