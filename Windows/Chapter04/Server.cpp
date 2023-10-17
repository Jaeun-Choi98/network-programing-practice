#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

int main(int argc, char* argv[]) {
	int retval;
	WSADATA wsa;
	// �������� �Ƚᵵ ��.
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	// ���� ����
	// ������ Ŀ�� ������Ʈ�� Ŀ�ο� ���� �����Ǿ����ϴ�. socket() ��ȯ���� int ������ Ŀ�� ������Ʈ���� ���� ��ȯ���� �����մϴ�.
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");
	
	// bind() �Լ��� ������ ���� IP�ּҿ� ���� ��Ʈ�� �����Ѵ�.
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY ���� ���� �ּҷ� �����ϸ�, Ŭ���̾�Ʈ�� ��� IP�� �����ϵ� �޾Ƶ��� �� �ִ�.
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen() �Լ��� ������ ���¸� LISTENING ���·� �ٲ۴�. Ŭ���̾�Ʈ�� ������ �� �� �ִ� ����.
	retval = listen(listen_sock, SOMAXCONN); // SOMAXCONN �Ϻ� �������ݿ��� ���� ������ ���� ���� Ŭ���̾�Ʈ�� �ִ��̴�.
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZ + 1];

	while (1){
		// accept() �Լ��� Ŭ���̾�Ʈ ������ �����ϰ�, ������ Ŭ���̾�Ʈ�� ����� ���ο� ���� ����.
		// ������ Ŭ���̾�Ʈ ���� ��� accept() �Լ��� ������ ��� ���·� �����.
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		//������ Ŭ���̾�Ʈ ������ �ۼ��ſ� ����� ����
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ��ȣ=%d\n", addr, ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		// recv(), send() ������ ���� �Լ��� Ŭ���̾�Ʈ�� ������ ��� ���.
		// ������ ��輱�� ������ ���� ��ȣ ����� �ʿ��ϴ�. �̴� ���� ���α׷� ���ؿ��� ó���ؾ� �Ѵ�.
		// ���� ����(��/���� ����) ������ �ִ�.
		while (1) {
			// 4 ��° �μ��� flags�� MSG_PEEK, MSG_WAITALL �� �ִ�. 
			// ��ȯ���� ���� �������� �����̴�.
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

			// ������ ������
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr.sin_port));
	}
	closesocket(listen_sock);
	WSACleanup();
	return 0;
}