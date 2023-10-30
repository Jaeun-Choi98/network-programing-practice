#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

int main(int argc, char* argv[]) {
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	// 넌블로킹 소켓으로 전환
	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if (listen_sock == INVALID_SOCKET) err_quit("ioctlsocket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN); 
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZ + 1];

	while (1) {
		
	ACCEPT_AGAIN:
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) goto ACCEPT_AGAIN;
			err_display("accept()");
			break;
		}

		//접속한 클라이언트 데이터 송수신에 사용할 변수
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트번호=%d\n", addr, ntohs(clientaddr.sin_port));

		
		while (1) {
		RECEIVE_AGAIN:
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAEWOULDBLOCK) goto RECEIVE_AGAIN;
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
		SEND_AGAIN:
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAEWOULDBLOCK) goto SEND_AGAIN;
				err_display("send()");
				break;
			}
		}
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
	}
	closesocket(listen_sock);
	WSACleanup();
	return 0;
}