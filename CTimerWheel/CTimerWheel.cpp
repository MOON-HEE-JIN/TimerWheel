#include "CTimerWheel.h"
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <process.h>

CTimerWheel::CTimerWheel(int tick, int wheelsize)
{
	m_iTick = 0;
	m_iUpdateTick = tick;

	m_iPassTick = 500;
	m_iWheelSize = wheelsize;
	m_iIndex = 0;

	m_vecTimerWheel.resize(wheelsize);
	QueryPerformanceFrequency(&freq);
}

CTimerWheel::~CTimerWheel()
{
}

void CTimerWheel::Update()
{
	if (++m_iIndex >= m_iWheelSize)
		m_iIndex = 0;

	std::list<TWHEEL*>::iterator biter = m_vecTimerWheel[m_iIndex].begin();
	std::list<TWHEEL*>::iterator eiter = m_vecTimerWheel[m_iIndex].end();
	for (biter; biter != eiter; )
	{
		bool ret = (*biter)->TimeCheck();

		if (ret) // 실행됨
		{
			if ((*biter)->RemainTime > m_iPassTick)	// m_iPassTick 보다 크면 시간을 다시 잰다
			{
				(*biter)->Time = (*biter)->RemainTime;
				m_listRemainTimeEvent.push_back((*biter));
			}
			else
			{
				(*biter)->Update(freq);
				m_listCompleteTimeEvent.push_back(*biter);
			}

			biter = m_vecTimerWheel[m_iIndex].erase(biter);
		}
		else // 실행 안됨
		{
			++biter;
		}
	}
}

void CTimerWheel::Push(TWHEEL* data)
{
	// 시간 측정의 최소 단위는 tick
	int Time = data->Time / m_iUpdateTick;	// tick 단위로 변환

	int round = (Time - 1) / m_iWheelSize + 1;	// 몇바퀴 돌아야 하는지 계산
	int offset = Time % m_iWheelSize; // 남은 시간 계산
	
	int index = (m_iIndex + offset) % m_iWheelSize; // 현재 인덱스 를 기준으로 offset 만큼 떨어진 위치

	data->Round = round;
	data->Index = index;
	data->RemainTime = data->Time % m_iUpdateTick;	// tick 단위로 나눈 나머지 시간

	m_vecTimerWheel[index].push_back(data);
	
#ifdef __DEBUG_TIMERWHEEL__
	if (data->DebugTime.QuadPart == 0)
		QueryPerformanceCounter(&data->DebugTime);
#endif // __DEBUG_TIMERWHEEL__
}

bool CTimerWheel::Pop(TWHEEL* data)
{
	std::list<TWHEEL*>::iterator biter = m_vecTimerWheel[data->Index].begin();
	std::list<TWHEEL*>::iterator eiter = m_vecTimerWheel[data->Index].end();
	for (biter; biter != eiter; ++biter)
	{
		if ((*biter) == data)
		{
			m_vecTimerWheel[data->Index].erase(biter);
			return true;
		}
	}
	return false;
}

CTimerWheel CTimerWheel::operator++()
{
	m_iTick++;
	if (m_iTick >= m_iUpdateTick)
	{
		m_iTick = 0;
		Update();
	}
	return *this;
}

bool st_TimerWheel::TimeCheck()
{
	return --Round <= 0;
}

bool st_TimerWheel::Update(LARGE_INTEGER freq)
{
#ifdef __DEBUG_TIMERWHEEL__
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	double ms = (double)(end.QuadPart - DebugTime.QuadPart) * 1000 / (double)freq.QuadPart;
	printf("UPDATE %d  DebugTime %.2f[S] \n", OriginalTime, ms/1000.f);
#endif // __DEBUG_TIMERWHEEL__
	return true;
}
