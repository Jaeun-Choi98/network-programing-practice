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

	// SO_REUSEADDR 소켓 옵션 설정
	// 서버를 IP 주소별로 운용할 수 있다.
	// 멀티캐스트(UDP) 응용 프로그램이 같은 포트 번호를 사용할 수 있도록 한다.
	// 서버 종료 후 재실행 시 bind() 함수에서 오류가 발생하는 것을 방지한다.
	// -> 리눅스/유닉스 운영체제에서는 fork() 시스템 콜 함수가 자식 프로세스를 생성해서 처리한다.
	// 그래서 강제 종료 시 자식 프로세스가 남아 포트 번호를 계속 사용하므로 bind() 함수에서 오류가 난다.
	// **코드 이식성을 위해 서버 작성시 항상 SO_REUSEADDR 옵션을 작성하는 것이 좋다.**
	DWORD optval = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// SO_RCVTIMEO/SO_SNDTIMEO 옵션은 통신의 교착 상태를 방지하기 위해 사용된다.
	// socket() 함수로 생성 된 소켓은 기본적으로 블로킹 소켓이다.
	optval = 3500;
	retval = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
		(const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 수신 버퍼의 크기를 늘린다. 윈도우 10에서 수신 버퍼의 기본 크기는 64K바이트이다.
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