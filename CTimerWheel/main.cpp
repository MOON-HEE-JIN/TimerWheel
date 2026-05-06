#include <iostream>
#include <Windows.h>
#include <process.h>
#include <thread>

#pragma comment(lib, "winmm.lib")
#include "CTimerWheelManager.h"

HANDLE thread;
HANDLE g_hTimerWheelTestThreadHandle;
#define MILLISECOND 1
#define SECOND 1000
#define MIN		SECOND * 60
#define HOUR	MIN * 60
int g_Time;

unsigned __stdcall TimerWheelTestThread(void* arg)
{
	timeBeginPeriod(1);
	
	srand(100);

	CTimerWheelManager TestObject;	// 1s, 1m, 1h  TimerWheel 있음
	// test data 준비
	std::vector<TWHEEL*> vecTestData;
	for (int i = 0; i < 99; i++)
	{
		TWHEEL* data = new TWHEEL;
		vecTestData.push_back(data);
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 33; j++)
		{
			int TotalTime;
			switch (i)
			{
			case 0:
				TotalTime = rand() % 60 * SECOND + rand() % 1000 * MILLISECOND;
				break;
			case 1:
				TotalTime = rand() % 60 * MIN + (rand() % 60) * SECOND + (rand() % 1000) * MILLISECOND;
				if (TotalTime == 105000)
				{
					TotalTime = 1 * MIN + 10 * SECOND + 800;
				}
				break;
			case 2:
				TotalTime = rand() % 24 * HOUR + (rand() % 60) * MIN + (rand() % 60) * SECOND + (rand() % 1000) * MILLISECOND;
				break;
			default:
				TotalTime = 0;
				break;
			}
			vecTestData[i * 33 + j]->Time = TotalTime;
#ifdef __DEBUG_TIMERWHEEL__
			vecTestData[i * 33 + j]->OriginalTime = TotalTime;
#endif // __DEBUG_TIMERWHEEL__

		}
	}

	// 데이터 검증
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 33; j++)
		{
			int TotalTime;
			int MaxTime;
			int MaxTime2;
			switch (i)
			{
			case 0:
				TotalTime = vecTestData[i * 33 + j]->Time;
				MaxTime = 60 * SECOND;
				MaxTime2 = 60 * MIN;
				break;
			case 1:
				TotalTime = vecTestData[i * 33 + j]->Time;
				MaxTime = 60 * MIN;
				MaxTime2 = 24 * HOUR;
				break;
			case 2:
				TotalTime = vecTestData[i * 33 + j]->Time;
				MaxTime = 24 * HOUR;
				MaxTime2 = 0xffffffff;
				break;
			default:
				TotalTime = 0;
				break;
			}

			if (MaxTime <= TotalTime && TotalTime <= MaxTime2)
			{
				printf("Wrong Data Time %d\n", TotalTime);
				return 0;
			}
		}
	}
	int replaceIndex_1 = 1 * SECOND;
	int replaceIndex_2 = 1 * MIN;
	int replaceIndex_3 = 1 * HOUR;
	
	//넣기
	for (int i = 0; i < 99; i++)
		TestObject.Push(vecTestData[i]);

	while (1)
	{
		//WaitForSingleObject(hTimer, 1);
		int ret = WaitForSingleObject(g_hTimerWheelTestThreadHandle, 1);
		TestObject.Update();

		if (TestObject.GetCompleteData() >= 99)
			break;
	}

	int nLoop = TestObject.GetCompleteData();
	int compare = 0;
	std::vector<int> completeIndex;
	for (int i = 0; i < nLoop; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			if (TestObject.m_vecTestCompleteData[i] == vecTestData[j])
			{
				compare++;
				break;
			}
		}
	}

	if (compare == 99)
		printf("COMPLETE\n");
	else
	{

	}

	timeEndPeriod(1);
	return 0;
}

unsigned __stdcall test()
{
	timeBeginPeriod(1);

	CTimerWheelManager timer;

	std::vector<TWHEEL*> vec;
	for (int i = 0; i < 9; i++)
	{
		vec.push_back(new TWHEEL);
	}

	vec[0]->Time = 1 * SECOND + 500;
	vec[1]->Time = 5 * SECOND + 200;
	vec[2]->Time = 10 * SECOND + 800;
	vec[3]->Time = 1 * MIN + 45 * SECOND;
	vec[4]->Time = 2 * MIN;
	vec[5]->Time = 3 * MIN + 3 * SECOND;
	vec[6]->Time = 1 * HOUR;
	vec[7]->Time = 2 * HOUR;
	vec[8]->Time = 1 * HOUR + 20 * MIN;

	unsigned int CurTime = GetTickCount();
	unsigned int EndTime = GetTickCount() + vec[8]->Time + 500;
	int debugIndex = 0;
	g_Time = GetTickCount();

	static bool bInit = false;
	for (int i = 0; i < 9; i++)
		timer.Push(vec[i]);

	while (1)
	{
		WaitForSingleObject(thread, 1);
		
		timer.Update();
		CurTime = GetTickCount();
		
		if (++debugIndex == vec[8]->Time)
			break;

	}
	timeEndPeriod(1);
	return 0;
}

void main()
{
	g_hTimerWheelTestThreadHandle = (HANDLE)_beginthreadex(NULL, 0, TimerWheelTestThread, 0,0,NULL);
	WaitForSingleObject(g_hTimerWheelTestThreadHandle, INFINITE);
}