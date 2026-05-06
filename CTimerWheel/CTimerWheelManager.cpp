#include "CTimerWheelManager.h"
#include <algorithm>

CTimerWheelManager::CTimerWheelManager()
{
	QueryPerformanceFrequency(&m_Lfreq);
	m_LStartTime = { 0 };
	m_dRemainTime = 0;

	m_vecTimerWheelSize = 0;

	AddTimerWheel(1, 1000);
	AddTimerWheel(1000, 60);
	AddTimerWheel(60000, 60);
}

CTimerWheelManager::~CTimerWheelManager()
{
}

void CTimerWheelManager::Update()
{
	double ms = 1;
	if (m_LStartTime.QuadPart != 0)
	{
		LARGE_INTEGER temp = m_LStartTime;
		QueryPerformanceCounter(&m_LEndTime);
		m_LStartTime = m_LEndTime;
		ms = (double)(m_LEndTime.QuadPart - temp.QuadPart) * 1000 / (double)m_Lfreq.QuadPart;
	}
	else
		QueryPerformanceCounter(&m_LStartTime);

	ms += m_dRemainTime;
	int nLoop = static_cast<int>(ms);
	m_dRemainTime = ms - nLoop;
	
	for (int i = 0; i < nLoop; i++)
	{
		++(*m_vecTimerWheel[0]);
		for (int j = 1; j < m_vecTimerWheelSize; j++)
		{
			++(*m_vecTimerWheel[j]);
		}
		
	}

	PostProcessing();
	
}

void CTimerWheelManager::Push(TWHEEL* data)
{
	for (int i = 0; i < m_vecTimerWheelSize; i++)
	{
		if (data->Time > m_vecTimerWheel[i]->GetMaxCheckTimer())
			continue;
		
		m_vecTimerWheel[i]->Push(data);
		break;
	}
}

void CTimerWheelManager::Pop(TWHEEL* data)
{
	for (int i = 0; i < m_vecTimerWheelSize; i++)
	{
		if (data->Time > m_vecTimerWheel[i]->GetMaxCheckTimer())
			continue;

		m_vecTimerWheel[i]->Pop(data);
	}
}

void CTimerWheelManager::AddTimerWheel(int tick, int wheelsize)
{
	CTimerWheel* pTimerWheel = new CTimerWheel(tick, wheelsize);

	m_vecTimerWheel.push_back(pTimerWheel);
	std::sort(m_vecTimerWheel.begin(), m_vecTimerWheel.end(),
		[](CTimerWheel* p1, CTimerWheel* p2) 
		{
			return p1->GetRoundTick() < p2->GetRoundTick();
		} 
	);
	m_vecTimerWheelSize++;

	m_vecTimerWheel[m_vecTimerWheelSize - 1]->SetMaxCheckTimer(m_vecTimerWheel[m_vecTimerWheelSize - 1]->GetRoundTick() * 60);
	for (int i =0; i < m_vecTimerWheelSize-1; i++)
	{
		m_vecTimerWheel[i]->SetMaxCheckTimer(m_vecTimerWheel[i + 1]->GetRoundTick());	// 최대 측정 값 지정
	}

}

void CTimerWheelManager::PostProcessing()
{
	std::list<TWHEEL*>::iterator biter;
	std::list<TWHEEL*>::iterator eiter;
	for (int i = 1; i < m_vecTimerWheelSize; i++)
	{
		biter = m_vecTimerWheel[i]->m_listCompleteTimeEvent.begin();
		eiter = m_vecTimerWheel[i]->m_listCompleteTimeEvent.end();
		for (biter; biter != eiter; ++biter)
			m_vecTestCompleteData.push_back(*biter);
	}
}
