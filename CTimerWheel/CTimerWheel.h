#pragma once
#include <vector>
#include <list>
#include <Windows.h>

#define __DEBUG_TIMERWHEEL__

class CTimerWheel;

typedef struct st_TimerWheel
{
	unsigned int Time;				// Update 시간
	
	int Round;
	int Index;
	int RemainTime;

#ifdef __DEBUG_TIMERWHEEL__
	LARGE_INTEGER DebugTime;
	unsigned int OriginalTime;
#endif // __DEBUG_TIMERWHEEL__


	st_TimerWheel() : Time(0), Round(0), Index(0), RemainTime(0)
	{
#ifdef __DEBUG_TIMERWHEEL__
		DebugTime.QuadPart = 0;
		OriginalTime = 0;
#endif // __DEBUG_TIMERWHEEL__
	}
	bool TimeCheck();
	bool Update(LARGE_INTEGER freq);
}TWHEEL;

class CTimerWheel
{
public:
	CTimerWheel(int tick, int wheelsize);
	~CTimerWheel();

private:
	int m_iTick;
	int m_iUpdateTick;

	int m_iPassTick;
	int m_iWheelSize;
	int m_iIndex;
	int m_iMaxCheckTimer;// 측정 최대 시간
	std::vector<std::list<TWHEEL*>> m_vecTimerWheel;


	void Update();

	LARGE_INTEGER freq;

public:
	std::list<TWHEEL*> m_listCompleteTimeEvent;			// 처리가 완료된 리스트
	std::list<TWHEEL*> m_listRemainTimeEvent;			// 남은 시간 이벤트 리스트

	int GetTick() { return m_iUpdateTick; }
	int GetWheelSize() { return m_iWheelSize; }
	int GetRoundTick() { return m_iUpdateTick * m_iWheelSize; }
	int GetNowIndex() { return m_iIndex; }

	void SetMaxCheckTimer(int MaxTime) { m_iMaxCheckTimer = MaxTime; }
	int GetMaxCheckTimer() { return m_iMaxCheckTimer; }

	void Push(TWHEEL* data);
	bool Pop(TWHEEL* data);

	CTimerWheel operator++();
};

