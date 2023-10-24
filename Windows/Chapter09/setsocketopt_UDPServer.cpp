#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

int main(int argc, char* argv[])
{
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// SO_REUSEADDR ���� �ɼ� ����
	// ������ IP �ּҺ��� ����� �� �ִ�.
	// ��Ƽĳ��Ʈ(UDP) ���� ���α׷��� ���� ��Ʈ ��ȣ�� ����� �� �ֵ��� �Ѵ�.
	// ���� ���� �� ����� �� bind() �Լ����� ������ �߻��ϴ� ���� �����Ѵ�.
	// -> ������/���н� �ü�������� fork() �ý��� �� �Լ��� �ڽ� ���μ����� �����ؼ� ó���Ѵ�.
	// �׷��� ���� ���� �� �ڽ� ���μ����� ���� ��Ʈ ��ȣ�� ��� ����ϹǷ� bind() �Լ����� ������ ����.
	// **�ڵ� �̽ļ��� ���� ���� �ۼ��� �׻� SO_REUSEADDR �ɼ��� �ۼ��ϴ� ���� ����.**
	DWORD optval = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// SO_RCVTIMEO/SO_SNDTIMEO �ɼ��� ����� ���� ���¸� �����ϱ� ���� ���ȴ�.
	// socket() �Լ��� ���� �� ������ �⺻������ ���ŷ �����̴�.
	optval = 3500;
	retval = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
		(const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ���� ������ ũ�⸦ �ø���. ������ 10���� ���� ������ �⺻ ũ��� 64K����Ʈ�̴�.
	optval *= 2;
	retval = setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
		(const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		buf[retval] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("[UDP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

		retval = sendto(sock, buf, retval, 0,
			(struct sockaddr*)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			break;
		}
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}