#include "..\Common.h"

int main(int argc, char* argv[]) {
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	/* 바이트 정렬 함수 */
	// 바이트 정렬이 되지 않은 상태에서 정보를 전달할 경우, 주소 또는 포트, 데이터의 해석이 달라진다.
	// 빅엔디언(최상위 바이트부터), 리틀엔디언(최하위 바이트부터) 정렬이 있고, 소켓 주소 구조체에서는 빅엔디언 정렬로 통일한다.
	// 빅엔디언을 네트워크 용어로 네트워크 바이트 정렬이라고 부른다.

	u_short x1 = 0x1234;
	u_long y1 = 0x1234567;
	u_short x2 = htons(x1); // s는 short 2바이트를 말함.
	u_long y2 = htonl(y1); // l은 long 4바이트를 말함.

	// 호스트 바이트 -> 네트워크 바이트
	printf("호스트 바이트 -> 네트워크 바이트\n");
	printf("%#x -> %#x\n", x1, x2);
	printf("%#x -> %#x\n", y1, y2);

	// 네트워크 바이트  -> 호스트 바이트
	printf("네트워크 바이트 -> 호스트 바이트\n");
	printf("%#x -> %#x\n", x2, ntohs(x2));
	printf("%#x -> %#x\n", y2, ntohl(y2));
	WSACleanup();
	return 0;
}