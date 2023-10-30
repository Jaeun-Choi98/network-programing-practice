#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

// Select 모델은 동기 입출력 방식과 비동기 통지 방법을 이용해 단일 스레드에서 다중 처리를 제공한다.
// 그렇기 때문에, CPU 명령 수행과 입출력 작업이 병행할 수는 없다.

// Select 모델에서 select() 함수는 입출력이 준비된 소켓을 제외하고 나머지 소켓은 제거한다.
// select() 함수는 리턴한 후, 준비된 소켓을 반환해주지 않기 때문에 소켓 배열을 만들어 순회하면서 준비된 소켓이 있는지 확인해야한다.
// SOCKETINFO 구조체와 배열은 이를 위한 선언이다.
struct SOCKETINFO {
	SOCKET sock;
	int recvbytes;
	int sendbytes;
	char buf[BUFSIZE + 1];
};

int nTotalSockets = 0;
SOCKETINFO* SocketInfoArray[FD_SETSIZE];

bool AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

int main(int argc, char* argv[]) {
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR) err_display("ioctlsocket()");

	// fd_set은 소켓 셋으로 읽기 셋, 쓰기 셋, 예외 셋이 있다.
	// 소켓을 알맞은 셋에 넣은 뒤, select() 함수를 호출하여 준비된 소켓을 찾는다.
	fd_set rset, wset;
	int nready;
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;

	while (1){
		// 소켓 셋 함수로 FD_ZERO, FD_ISSET, FD_SET이 있다.
		// 소켓 초기화
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listen_sock, &rset);
		for (int i = 0;i < nTotalSockets;i++) {
			if (SocketInfoArray[i]->recvbytes > SocketInfoArray[i]->sendbytes)
				FD_SET(SocketInfoArray[i]->sock, &rset);
			else FD_SET(SocketInfoArray[i]->sock, &wset);
		}
		
		// select() 함수의 네 번째 인자는 timeoval 구조체로 NULL을 인수로 주었을 시, 셋에 준비된 소켓이 적어도 하나 있을 때까지 대기한다.
		// 첫 번째 인수는 리눅스/유닉스 호환용으로 넣어주는 인수로 윈도우는 무시해도 된다.
		nready = select(0, &rset, &wset, NULL, NULL);
		if (nready == SOCKET_ERROR) err_quit("select()");

		if (FD_ISSET(listen_sock, &rset)) {
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}
			else {
				char addr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
				if (!AddSocketInfo(client_sock)) {
					closesocket(client_sock);
				}
			}
			// select() 리턴 값으로 listen_sock 하나일 경우 아래 작업은 필요가 없기 때문
			if (--nready <= 0) continue;
		}

		for (int i = 0;i < nTotalSockets;i++) {
			SOCKETINFO* ptr = SocketInfoArray[i];
			if (FD_ISSET(ptr->sock,&rset)) {
				retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
				if (retval == SOCKET_ERROR) {
					err_display("recv()");
					RemoveSocketInfo(i);
				}// 소켓이 정상 종료 하면 recv() 반환 값으로 0을 받는다.
				else if (retval == 0) {
					RemoveSocketInfo(i);
				}
				else {
					ptr->recvbytes = retval;
					addrlen = sizeof(clientaddr);
					getpeername(ptr->sock, (struct sockaddr*)&clientaddr, &addrlen);
					ptr->buf[ptr->recvbytes] = '\0';
					char addr[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
					printf("[TCP/%s:%d] %s\n", addr,
						ntohs(clientaddr.sin_port), ptr->buf);
				}
			}
			else if (FD_ISSET(ptr->sock, &wset)) {
				// 논블로킹 소켓이라 send() 함수에서 송신 버퍼에 데이터 전부가 복사가 안되었을 수도 있음.
				// 그래서 이전에 보냈던 sendbytes만큼 버퍼 주소값에 더해줌
				retval = send(ptr->sock, ptr->buf + ptr->sendbytes,
					ptr->recvbytes - ptr->sendbytes, 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					RemoveSocketInfo(i);
				}
				else {
					ptr->sendbytes += retval;
					// send() 함수에서 데이터가 전부 송신됨.
					if (ptr->recvbytes == ptr->sendbytes) {
						ptr->recvbytes = ptr->sendbytes = 0;
					}
				}
			}
		}
	}

	closesocket(listen_sock);
	WSACleanup();
	return 0;
}


bool AddSocketInfo(SOCKET sock)
{
	if (nTotalSockets >= FD_SETSIZE) {
		printf("[오류] 소켓 정보를 추가할 수 없습니다!\n");
		return false;
	}
	SOCKETINFO* ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("[오류] 메모리가 부족합니다!\n");
		return false;
	}
	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	SocketInfoArray[nTotalSockets++] = ptr;
	return true;
}

void RemoveSocketInfo(int nIndex)
{
	SOCKETINFO* ptr = SocketInfoArray[nIndex];

	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (struct sockaddr*)&clientaddr, &addrlen);

	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		addr, ntohs(clientaddr.sin_port));

	closesocket(ptr->sock);
	delete ptr;

	if (nIndex != (nTotalSockets - 1))
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];
	--nTotalSockets;
}