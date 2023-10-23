#include "..\Common.h"

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE    512

int main(int argc, char* argv[])
{
	int retval;
	if (argc > 1) SERVERIP = argv[1];
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);

	struct sockaddr_in peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	while (1) {
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n'제거
		len = (int)strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// 이때, if(지역 주소와 포트 번호가 결정되지 않았다면) 원격 주소 뿐만 아니라, 지역 주소, 포트 번호도 결정된다.
		// TCP의 bind() 함수 역할을 대신한다.
		retval = sendto(sock, buf, (int)strlen(buf), 0,
			(struct sockaddr*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			break;
		}
		printf("[UDP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		// 데이터 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		// 송신자의 주소 체크
		// recvfrom() 함수는 UDP 서버가 보낸 데이터 뿐만 아니라, 다른 UDP 응용 프로그램이 보낸 데이터도 수신할 수 있다.
		// UDP 소켓에 대해 connect() 함수를 호출하면 통신할 상대의 주소가 내부적으로 저장됨으로, send()/recv() 함수를 사용하여
		// 특정 UDP 서버와 통신할 수 있다.
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
			printf("[오류] 잘못된 데이터입니다!\n");
			break;
		}

		buf[retval] = '\0';
		printf("[UDP 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", buf);
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}