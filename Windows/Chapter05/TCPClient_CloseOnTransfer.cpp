#include "..\Common.h"

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 50

int main(int argc, char* argv[]) {
	int retval;

	if (argc > 1) SERVERIP = argv[1];

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	
	// 데이터 통신에 사용할 변수 1. 고정길이 데이터 전송
	char buf[BUFSIZE];
	const char* testdata[] = {
		"안녕하세요",
		"반가워요",
		"안녕히 계세요"
	};

	// 데이터를 보낼 때마다 소켓 생성/서버 접속/접속 종료를 반복한다.
	for (int i = 0;i < 3;i++) {
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
			err_quit("socket()");

		retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
		if (retval == INVALID_SOCKET)
			err_quit("connect()");
		printf("[TCP 클라이언트] 서버 접속: 서버 주소=%s, 포트 번호=%d\n", SERVERIP, SERVERPORT);
		
		// 데이터 보내기
		memset(buf, '#', sizeof(buf));
		strncpy(buf, testdata[i], strlen(testdata[i]));
		retval = send(sock, buf, strlen(testdata[i]), 0);
		if (retval == INVALID_SOCKET) {
			err_display("send()");
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
		closesocket(sock);
	}

	return 0;
}