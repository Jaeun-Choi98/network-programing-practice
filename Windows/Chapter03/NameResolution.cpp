#include "..\Common.h"
#define TESTNAME "www.google.com"

// gethostbyname(), gethostbyaddr()�� ����Ͽ� �����ΰ� �ּҸ� ��ȣ ��ȯ�� �� �ִ�.
// �� �Լ� ��� ����ü hostent{} �� ��ȯ�Ѵ�. hostent �ʵ�δ� ȣ��Ʈ �̸�, ȣ��Ʈ ��Ī��, �ּ� ü��, �ּ� ����, �ּ� ������ �ִ�.
// h_name, h_aliases, h_addrtype, h_length, h_addr_list
// IPv6�� ��� getaddrinfo()�� getnameinfo()�Լ��� ���� ��ü�ؼ� ����ϸ� �ȴ�. 

// ������ �̸� -> IPv4 �ּ�
bool getIPAddr(const char* name, struct in_addr* addr) {
	struct hostent* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display("gethostname()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET)
		return false;
	//h_addr �� h_addr_list[0]���� ��ũ�� �Ǿ��ִ�.
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return true;
}

// IPv4 �ּ� -> ������ �̸�
bool getDomainName(struct in_addr addr, char* name, int namelen) {
	struct hostent* ptr = gethostbyaddr((const char*)&addr,sizeof(addr),AF_INET);
	if (ptr == NULL) {
		err_display("gethostbyaddr()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET)
		return false;
	strncpy(name, ptr->h_name, namelen);
	return true;
}

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	printf("������ �̸�(��ȯ ��): %s\n", TESTNAME);

	// ������ �̸� -> IPv4
	struct in_addr addr;
	if (getIPAddr(TESTNAME, &addr)) {
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr, str, sizeof(str));
		printf("IP �ּ�(��ȯ ��): %s\n", str);

		// IP �ּ� -> ������ �̸�
		char name[256];
		if (getDomainName(addr, name, sizeof(name))) {
			printf("������ �̸�(�ٽ� ��ȯ ��): %s\n",name);
		}
	}
	WSACleanup();
	return 0;
}