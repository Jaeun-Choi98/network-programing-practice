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

	// UDP�� ���� ������ ���� �����Ƿ� listen(), accept() �Լ��� ������� �ʴ´�.
	// �ϳ��� �������θ� �ټ��� Ŭ���̾�Ʈ�� ó���Ѵ�. 
	sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE];

	// recvfrom() �Լ��� ���� IP �ּ�, Port ��ȣ�� �� �� �ִ�.
	// sendto() �Լ��� �޸�, �� ������ �ݵ�� ���� �ּҰ� �����Ǿ� �־�� �Ѵ�.
	while (1) {
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,(sockaddr*)&clientaddr, &addrlen);
		if (sock == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		// ���� ����
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