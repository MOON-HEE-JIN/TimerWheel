#pragma once
#include <vector>
#include <list>
#include <Windows.h>

class CTimerWheel;

typedef struct st_TimerWheel
{
	unsigned int Time;				// Update 시간
	
	int Round;
	int Index;
	
	unsigned int PushTime;

	LARGE_INTEGER DebugTime;
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

