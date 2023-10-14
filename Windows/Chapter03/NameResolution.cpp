#include "..\Common.h"
#define TESTNAME "www.google.com"

// gethostbyname(), gethostbyaddr()를 사용하여 도메인과 주소를 상호 변환할 수 있다.
// 두 함수 모두 구조체 hostent{} 를 반환한다. hostent 필드로는 호스트 이름, 호스트 별칭들, 주소 체계, 주소 길이, 주소 값들이 있다.
// h_name, h_aliases, h_addrtype, h_length, h_addr_list
// IPv6의 경우 getaddrinfo()와 getnameinfo()함수로 각각 대체해서 사용하면 된다. 

// 도메인 이름 -> IPv4 주소
bool getIPAddr(const char* name, struct in_addr* addr) {
	struct hostent* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display("gethostname()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET)
		return false;
	//h_addr 은 h_addr_list[0]으로 매크로 되어있다.
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return true;
}

// IPv4 주소 -> 도메인 이름
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
	printf("도메인 이름(변환 전): %s\n", TESTNAME);

	// 도메인 이름 -> IPv4
	struct in_addr addr;
	if (getIPAddr(TESTNAME, &addr)) {
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr, str, sizeof(str));
		printf("IP 주소(변환 후): %s\n", str);

		// IP 주소 -> 도메인 이름
		char name[256];
		if (getDomainName(addr, name, sizeof(name))) {
			printf("도메인 이름(다시 변환 후): %s\n",name);
		}
	}
	WSACleanup();
	return 0;
}