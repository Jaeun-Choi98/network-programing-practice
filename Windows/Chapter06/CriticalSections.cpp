#include <windows.h>
#include <stdio.h>

#define MAXCNT 100000000
int count = 0;
// 사용자 메모리 영역에 존재하는 단순한 구조체로, 전역 변수 형태로 생성한다.
CRITICAL_SECTION cs; 

DWORD WINAPI MyThread1(LPVOID arg)
{
	for (int i = 0; i < MAXCNT; i++) {
		// 공유 자원에 접근하기전에 호출한다. 공유 자원을 사용하고 있는 스레드가 없다면 곧바로 리턴한다.
		// 만약 공유 자원을 사용하고 있는 스레드가 있다면, 바로 리턴하지 못하고 해당 스레드는 대기 상태가 된다.
		EnterCriticalSection(&cs);
		count += 2;
		// 공유 자원 사용을 마치면 호출한다. 대기 중인 다른 스레드가 있다면 하나만 선택되어 깨어난다.
		LeaveCriticalSection(&cs);
	}
	return 0;
}

DWORD WINAPI MyThread2(LPVOID arg)
{
	for (int i = 0; i < MAXCNT; i++) {
		EnterCriticalSection(&cs);
		count -= 2;
		LeaveCriticalSection(&cs);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	// 임계 영역 초기화
	InitializeCriticalSection(&cs);

	// 스레드 두 개 생성
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);

	// 스레드 두 개 종료 대기
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// 임계 영역 삭제
	DeleteCriticalSection(&cs);

	// 결과 출력
	printf("count = %d\n", count);
	return 0;
}