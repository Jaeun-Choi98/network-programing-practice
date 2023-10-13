#include "..\Common.h"

int main(int argc, char* argv[]) {
	//�����쿡���� ���� �ʱ�ȭ�� �����ؾ�������, ������������ ������ �ʱ�ȭ ������ �ʿ����� �ʴ´�.
	WSADATA wsa;
	//1��° �μ��� ���α׷��� �䱸�ϴ� �ֻ��� ���� ����(MAKEWORD(3,2)�� �����ص� �ȴ�. 3.2 �������� ��Ÿ����.
	//2��° �μ��� wsa ���������ν� ���� ������ ���� ������ ���� �� ����.
	if (WSAStartup(0X0203, &wsa) != 0) return 1;
	printf("[�˸�]�ʱ�ȭ ����\n");

	//���� ����
	//1��° �μ��� adress family�� ipv4,ipv6,��������� ����
	//2��° �μ��� socket type���� SOCK_STREAM(�ŷڼ� ����), SOCK_DGRAM(�ŷڼ� ����x)�� ����
	//3��° �μ��� ����� ���������� ���´�.
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[�˸�]���� ���� ����\n");

	//���� ����
	closesocket(sock);
	//���� ����
	WSACleanup();
	return 0;
}