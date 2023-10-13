#include "..\Common.h"

int main(int argc, char* argv[]) {
	//윈도우에서는 윈속 초기화를 먼저해야하지만, 리눅스에서는 별도의 초기화 과정이 필요하지 않는다.
	WSADATA wsa;
	//1번째 인수는 프로그램이 요구하는 최상위 원속 버전(MAKEWORD(3,2)를 기입해도 된다. 3.2 버전임을 나타낸다.
	//2번째 인수는 wsa 기입함으로써 윈속 구현에 관한 정보를 얻을 수 있음.
	if (WSAStartup(0X0203, &wsa) != 0) return 1;
	printf("[알림]초기화 성공\n");

	//소켓 생성
	//1번째 인수는 adress family로 ipv4,ipv6,블루투스가 있음
	//2번째 인수는 socket type으로 SOCK_STREAM(신뢰성 제공), SOCK_DGRAM(신뢰성 제공x)가 있음
	//3번째 인수는 사용할 프로토콜을 적는다.
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[알림]소켓 생성 성공\n");

	//소켓 종료
	closesocket(sock);
	//윈속 종료
	WSACleanup();
	return 0;
}