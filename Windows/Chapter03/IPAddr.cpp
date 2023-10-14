#include "..\Common.h"

int main(int argc, char* argv[]) {
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	
	/* ipv4 변환 연습 */
	const char* ipv4Test = "147.46.114.70";
	printf("IPv4 주소(변환 전): %s\n", ipv4Test);

	// 소켓 주소 구조체(sockaddr,sockaddr_in,sockaddr_in6)에는 주소체계, 포트, 주소 등을 저장한다.
	// in_addr은 ip주소를 의미하며, ipv4는 32비트, ipv6는 128비트 구조체 타입이다. 
	// 소켓 주소 구조체 ipv4인 sockaddr_in(16바이트)과 ipv6 sockaddr_in6(28바이트)는 96비트(12바이트) 차이가 난다.
	// **그래서 응용 프로그램에서 반드시 sockaddr 포인터 타입으로 변환해야하고, 소켓 주소 구조체의 크기 정보도 같이 전달해야한다.**
	struct in_addr ipv4num;
	
	//inet_pton(): presentation to numeric으로 문자열 형태를 입력받아 32/128 비트(네트워크 바이트 정렬) 형태로 리턴.
	//inet_ntop(): numeric to presentation
	inet_pton(AF_INET, ipv4Test, &ipv4num);
	printf("IPv4 주소(변환 후): %#x\n", ipv4num);

	char ipv4str[INET_ADDRSTRLEN]; //ipv4 주소의 최대 길이를 저장한 전역 변수
	inet_ntop(AF_INET, &ipv4num, ipv4str, sizeof(ipv4str));
	printf("IPv4 주소(다시 변환 후): %s\n", ipv4str);

	/* ipv6 변환 연습 */
	struct in6_addr ipv6num;
	const char* ipv6test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 주소(변환 전): %s\n", ipv6test);

	inet_pton(AF_INET6, ipv6test, &ipv6num);
	printf("IPv6 주소(변환 후): %#x\n", ipv6num);

	char ipv6str[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &ipv6num, ipv6str, sizeof(ipv6str));
	printf("IPv6 주소(다시 변환 후): %s\n", ipv6str);

	WSACleanup();
	return 0;
}