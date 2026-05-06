#pragma once
#include "CTimerWheel.h"

class CTimerWheelManager
{
public:
	CTimerWheelManager();
	~CTimerWheelManager();

	void Update();

public:
	void Push(TWHEEL* data);
	void Pop(TWHEEL* data);

	void AddTimerWheel(int tick, int wheelsize);
	virtual void PostProcessing();			// 타이머 다돈거 후처리

	int GetCompleteData() { return m_vecTestCompleteData.size(); }
	std::vector<TWHEEL*> m_vecTestCompleteData;
private:
	unsigned int m_iLastUpdateTime;
	int m_iTick = 1;
	double m_dRemainTime;

	std::vector< CTimerWheel*> m_vecTimerWheel;
	int	m_vecTimerWheelSize;

	LARGE_INTEGER m_LStartTime;
	LARGE_INTEGER m_LEndTime;
	LARGE_INTEGER m_Lfreq;

	CTimerWheel m_CTimerWheel_1 = CTimerWheel(1,1000);		// tick == 1ms,			wheelsize == 1000,		1000ms		TimerWheel			== 1S
	CTimerWheel m_CTimerWheel_2 = CTimerWheel(1000,60);		// tick == 1000ms,		wheelsize == 60,		60000ms		TimerWheel			== 1Min
	CTimerWheel m_CTimerWheel_3 = CTimerWheel(60000, 60);	// tick == 60000ms,		wheelsize == 60,		3600000ms	TImerWheel			== 1Hour
	CTimerWheel m_CTimerWheel_4 = CTimerWheel(3600000, 24);	// tick == 3600000ms,	wheelsize == 24						TimerWheel			== 1Day
};

