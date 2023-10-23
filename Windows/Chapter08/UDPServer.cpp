#include "../Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

int main(int argc, char* argv[]) {
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if (sock == INVALID_SOCKET) err_quit("bind()");

	// UDP는 연결 설정을 하지 않으므로 listen(), accept() 함수를 사용하지 않는다.
	// 하나의 소켓으로만 다수의 클라이언트를 처리한다. 
	sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE];

	// recvfrom() 함수로 원격 IP 주소, Port 번호를 알 수 있다.
	// sendto() 함수와 달리, 이 소켓은 반드시 지역 주소가 결정되어 있어야 한다.
	while (1) {
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,(sockaddr*)&clientaddr, &addrlen);
		if (sock == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		// 에코 서버
		buf[retval] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr, addr, sizeof(addr));
		printf("[UDP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

		retval = sendto(sock, buf, retval, 0, (sockaddr*)&clientaddr, sizeof(clientaddr));
		if (sock == SOCKET_ERROR) {
			err_display("sendto()");
			break;
		}
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}