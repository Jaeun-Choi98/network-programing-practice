#include "..\Common.h"

int main(int argc, char* argv[]) {
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	
	/* ipv4 ��ȯ ���� */
	const char* ipv4Test = "147.46.114.70";
	printf("IPv4 �ּ�(��ȯ ��): %s\n", ipv4Test);

	// ���� �ּ� ����ü(sockaddr,sockaddr_in,sockaddr_in6)���� �ּ�ü��, ��Ʈ, �ּ� ���� �����Ѵ�.
	// in_addr�� ip�ּҸ� �ǹ��ϸ�, ipv4�� 32��Ʈ, ipv6�� 128��Ʈ ����ü Ÿ���̴�. 
	// ���� �ּ� ����ü ipv4�� sockaddr_in(16����Ʈ)�� ipv6 sockaddr_in6(28����Ʈ)�� 96��Ʈ(12����Ʈ) ���̰� ����.
	// **�׷��� ���� ���α׷����� �ݵ�� sockaddr ������ Ÿ������ ��ȯ�ؾ��ϰ�, ���� �ּ� ����ü�� ũ�� ������ ���� �����ؾ��Ѵ�.**
	struct in_addr ipv4num;
	
	//inet_pton(): presentation to numeric���� ���ڿ� ���¸� �Է¹޾� 32/128 ��Ʈ(��Ʈ��ũ ����Ʈ ����) ���·� ����.
	//inet_ntop(): numeric to presentation
	inet_pton(AF_INET, ipv4Test, &ipv4num);
	printf("IPv4 �ּ�(��ȯ ��): %#x\n", ipv4num);

	char ipv4str[INET_ADDRSTRLEN]; //ipv4 �ּ��� �ִ� ���̸� ������ ���� ����
	inet_ntop(AF_INET, &ipv4num, ipv4str, sizeof(ipv4str));
	printf("IPv4 �ּ�(�ٽ� ��ȯ ��): %s\n", ipv4str);

	/* ipv6 ��ȯ ���� */
	struct in6_addr ipv6num;
	const char* ipv6test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 �ּ�(��ȯ ��): %s\n", ipv6test);

	inet_pton(AF_INET6, ipv6test, &ipv6num);
	printf("IPv6 �ּ�(��ȯ ��): %#x\n", ipv6num);

	char ipv6str[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &ipv6num, ipv6str, sizeof(ipv6str));
	printf("IPv6 �ּ�(�ٽ� ��ȯ ��): %s\n", ipv6str);

	WSACleanup();
	return 0;
}