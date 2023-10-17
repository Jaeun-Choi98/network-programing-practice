#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

int main(int argc, char* argv[]) {
	int retval;
	WSADATA wsa;
	// 리눅스는 안써도 됨.
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	// 소켓 생성
	// 소켓은 커널 오브젝트로 커널에 의해 관리되어집니다. socket() 반환값은 int 형으로 커널 오브젝트들의 고유 반환값을 리턴합니다.
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");
	
	// bind() 함수는 소켓의 지역 IP주소와 지역 포트를 결정한다.
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY 값을 지역 주소로 설정하면, 클라이언트가 어느 IP로 접속하든 받아들일 수 있다.
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen() 함수는 소켓의 상태를 LISTENING 상태로 바꾼다. 클라이언트가 접속을 할 수 있는 상태.
	retval = listen(listen_sock, SOMAXCONN); // SOMAXCONN 하부 프로토콜에서 지원 가능한 접속 가능 클라이언트의 최댓값이다.
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZ + 1];

	while (1){
		// accept() 함수는 클라이언트 접속을 수용하고, 접속한 클라이언트와 통신할 새로운 소켓 생성.
		// 접속한 클라이언트 없을 경우 accept() 함수는 서버를 대기 상태로 만든다.
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		//접속한 클라이언트 데이터 송수신에 사용할 변수
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트번호=%d\n", addr, ntohs(clientaddr.sin_port));

		// 클라이언트와 데이터 통신
		// recv(), send() 데이터 전송 함수로 클라이언트와 서버에 모두 사용.
		// 데이터 경계선을 구분을 위한 상호 약속이 필요하다. 이는 응용 프로그램 수준에서 처리해야 한다.
		// 소켓 버퍼(송/수신 버퍼) 영역이 있다.
		while (1) {
			// 4 번째 인수는 flags로 MSG_PEEK, MSG_WAITALL 이 있다. 
			// 반환값은 받은 데이터의 길이이다.
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

			// 데이터 보내기
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
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