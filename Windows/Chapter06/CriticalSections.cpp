#include <windows.h>
#include <stdio.h>

#define MAXCNT 100000000
int count = 0;
// ����� �޸� ������ �����ϴ� �ܼ��� ����ü��, ���� ���� ���·� �����Ѵ�.
CRITICAL_SECTION cs; 

DWORD WINAPI MyThread1(LPVOID arg)
{
	for (int i = 0; i < MAXCNT; i++) {
		// ���� �ڿ��� �����ϱ����� ȣ���Ѵ�. ���� �ڿ��� ����ϰ� �ִ� �����尡 ���ٸ� ��ٷ� �����Ѵ�.
		// ���� ���� �ڿ��� ����ϰ� �ִ� �����尡 �ִٸ�, �ٷ� �������� ���ϰ� �ش� ������� ��� ���°� �ȴ�.
		EnterCriticalSection(&cs);
		count += 2;
		// ���� �ڿ� ����� ��ġ�� ȣ���Ѵ�. ��� ���� �ٸ� �����尡 �ִٸ� �ϳ��� ���õǾ� �����.
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
	// �Ӱ� ���� �ʱ�ȭ
	InitializeCriticalSection(&cs);

	// ������ �� �� ����
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);

	// ������ �� �� ���� ���
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// �Ӱ� ���� ����
	DeleteCriticalSection(&cs);

	// ��� ���
	printf("count = %d\n", count);
	return 0;
}