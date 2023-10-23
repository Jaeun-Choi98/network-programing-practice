#include "..\Common.h"

#define REMOTEPORT 9000
#define BUFSIZE 512
// 지역 브로드캐스트
#define REMOTEIP "255.255.255.255" 

int main(int argc, char* argv[])
{
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// 브로드캐스팅 활성화
	DWORD bEnable = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(const char*)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	struct sockaddr_in remoteaddr;
	memset(&remoteaddr, 0, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	inet_pton(AF_INET, REMOTEIP, &remoteaddr.sin_addr);
	remoteaddr.sin_port = htons(REMOTEPORT);

	char buf[BUFSIZE + 1];
	int len;

	while (1) {
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		len = (int)strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		retval = sendto(sock, buf, (int)strlen(buf), 0,
			(struct sockaddr*)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			break;
		}
		printf("[UDP] %d바이트를 보냈습니다.\n", retval);
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}