#include "..\Common.h"

#define LOCALPORT 9000
#define BUFSIZE 512

int main(int argc, char* argv[]) {
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	sockaddr_in localaddr;
	memset(&localaddr, 0, sizeof(localaddr));
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(LOCALPORT);
	localaddr.sin_family = AF_INET;

	retval = bind(sock, (sockaddr*)&localaddr, sizeof(localaddr));
	if (retval == INVALID_SOCKET) err_quit("bind()");

	sockaddr_in peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZ, 0, (sockaddr*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		buf[retval] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &peeraddr.sin_addr, addr, sizeof(addr));
		printf("[UDP/%s:%d] %s\n", addr, ntohs(peeraddr.sin_port), buf);
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}