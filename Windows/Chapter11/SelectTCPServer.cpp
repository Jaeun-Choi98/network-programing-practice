#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

// Select ���� ���� ����� ��İ� �񵿱� ���� ����� �̿��� ���� �����忡�� ���� ó���� �����Ѵ�.
// �׷��� ������, CPU ��� ����� ����� �۾��� ������ ���� ����.

// Select �𵨿��� select() �Լ��� ������� �غ�� ������ �����ϰ� ������ ������ �����Ѵ�.
// select() �Լ��� ������ ��, �غ�� ������ ��ȯ������ �ʱ� ������ ���� �迭�� ����� ��ȸ�ϸ鼭 �غ�� ������ �ִ��� Ȯ���ؾ��Ѵ�.
// SOCKETINFO ����ü�� �迭�� �̸� ���� �����̴�.
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

	// fd_set�� ���� ������ �б� ��, ���� ��, ���� ���� �ִ�.
	// ������ �˸��� �¿� ���� ��, select() �Լ��� ȣ���Ͽ� �غ�� ������ ã�´�.
	fd_set rset, wset;
	int nready;
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;

	while (1){
		// ���� �� �Լ��� FD_ZERO, FD_ISSET, FD_SET�� �ִ�.
		// ���� �ʱ�ȭ
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listen_sock, &rset);
		for (int i = 0;i < nTotalSockets;i++) {
			if (SocketInfoArray[i]->recvbytes > SocketInfoArray[i]->sendbytes)
				FD_SET(SocketInfoArray[i]->sock, &rset);
			else FD_SET(SocketInfoArray[i]->sock, &wset);
		}
		
		// select() �Լ��� �� ��° ���ڴ� timeoval ����ü�� NULL�� �μ��� �־��� ��, �¿� �غ�� ������ ��� �ϳ� ���� ������ ����Ѵ�.
		// ù ��° �μ��� ������/���н� ȣȯ������ �־��ִ� �μ��� ������� �����ص� �ȴ�.
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
				printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr.sin_port));
				if (!AddSocketInfo(client_sock)) {
					closesocket(client_sock);
				}
			}
			// select() ���� ������ listen_sock �ϳ��� ��� �Ʒ� �۾��� �ʿ䰡 ���� ����
			if (--nready <= 0) continue;
		}

		for (int i = 0;i < nTotalSockets;i++) {
			SOCKETINFO* ptr = SocketInfoArray[i];
			if (FD_ISSET(ptr->sock,&rset)) {
				retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
				if (retval == SOCKET_ERROR) {
					err_display("recv()");
					RemoveSocketInfo(i);
				}// ������ ���� ���� �ϸ� recv() ��ȯ ������ 0�� �޴´�.
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
				// ����ŷ �����̶� send() �Լ����� �۽� ���ۿ� ������ ���ΰ� ���簡 �ȵǾ��� ���� ����.
				// �׷��� ������ ���´� sendbytes��ŭ ���� �ּҰ��� ������
				retval = send(ptr->sock, ptr->buf + ptr->sendbytes,
					ptr->recvbytes - ptr->sendbytes, 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					RemoveSocketInfo(i);
				}
				else {
					ptr->sendbytes += retval;
					// send() �Լ����� �����Ͱ� ���� �۽ŵ�.
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
		printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
		return false;
	}
	SOCKETINFO* ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("[����] �޸𸮰� �����մϴ�!\n");
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
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		addr, ntohs(clientaddr.sin_port));

	closesocket(ptr->sock);
	delete ptr;

	if (nIndex != (nTotalSockets - 1))
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];
	--nTotalSockets;
}