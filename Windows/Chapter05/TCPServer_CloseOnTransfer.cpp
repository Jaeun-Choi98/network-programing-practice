#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE 1024

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
	char buf[BUFSIZE + 1];

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
			// BUFSIZE가 1024이고 실제로 받을 데이터는 이보다 작다고 가정한다.
			// 이때 MSG_WAITALL은 한 번에 최대한 데이터를 읽겠다는 뜻이다.
			// 하지만 여기서는 while루프를 돌면서 데이터를 계속 읽으므로 옵션과 상관없이 데이터를 모두 읽을 수 있다.
			retval = recv(client_sock, buf, BUFSIZE, MSG_WAITALL);
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