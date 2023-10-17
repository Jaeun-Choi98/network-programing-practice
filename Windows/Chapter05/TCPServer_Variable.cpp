#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

// ���� ������ �Լ�
int _recv_ahead(SOCKET s, char* p)
{
	__declspec(thread) static int nbytes = 0;
	__declspec(thread) static char buf[1024];
	__declspec(thread) static char* ptr;

	if (nbytes == 0 || nbytes == SOCKET_ERROR) {
		nbytes = recv(s, buf, sizeof(buf), 0);
		if (nbytes == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		else if (nbytes == 0)
			return 0;
		ptr = buf;
	}

	--nbytes;
	*p = *ptr++;
	return 1;
}

// ����� ���� ������ ���� �Լ�
int recvline(SOCKET s, char* buf, int maxlen)
{
	int n, nbytes;
	char c, * ptr = buf;

	// '\n'�� ������ n�� ��ȯ, 0('\0'/NUL, NULL)�� ������ n-1 ��ȯ
	for (n = 1; n < maxlen; n++) {
		nbytes = _recv_ahead(s, &c);
		if (nbytes == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (nbytes == 0) {
			*ptr = 0;
			return n - 1;
		}
		else
			return SOCKET_ERROR;
	}

	*ptr = 0;
	return n;
}

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

	// ������ ����� Ŭ���̾�Ʈ ����
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
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, clientaddr.sin_port);

		while (1) {
			retval = recvline(client_sock, buf, BUFSIZE+1);
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
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr.sin_port));
	}

	closesocket(listen_socket);
	WSACleanup();
	return 0;
}