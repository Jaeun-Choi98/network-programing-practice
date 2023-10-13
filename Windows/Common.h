#define _CRT_SECURE_NO_WARNINGS // 구형 C 함수 사용 시 경고 끄기
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 구형 소켓 API 사용 시 경고 끄기

#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더

#include <tchar.h> // _T(), ...
#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strncpy(), ...

#pragma comment(lib, "ws2_32") // ws2_32.lib 링크

//오류 코드 얻기
//WSAGetLastError() 함수 사용(윈도우) / 전역 변수 errno 사용(리눅스)
//오류 메시지 얻기
//FormatMessage() 함수(윈도우) / steerror() 함수(리눅스)

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	//WASGetLastError() 값에 대응하는 에러 메시지를 얻을 수 있다.
	FormatMessageA(
		//dwFlags
		//ALLOCATE_BUFFER은 오류 메세지를 저장할 공간을 자동으로 할당
		//FROM_SYSTEM은 운영체제에서 오류 메시지를 가져온다는 의미
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		//2번째 인수는 lpSource 는 null 값(dwFlags의 인수로 인해 FormatMessage()에서 자동으로 할당되기 때문)
		//3번째 인수는 dwMessgeId로 오류 코드를 나타내며, WASGETLastError의 값을 여기에 넣는다.
		NULL, WSAGetLastError(),
		//4번째 인수는 오류를 표시할 언어를 설정한다.
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		//5번째 인수는 오류 메시지의 시작 주소를 담는다,
		//6번째와 7번째 인수는 nSize와 Arguments로 1번째 인수로 0과 null이 입력되어 있다.
		(char*)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	//현재 실행되고 프로세스 종료
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 소켓 함수 오류 출력
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[오류] %s\n", (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}