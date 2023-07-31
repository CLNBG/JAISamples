#pragma once

class CCallbackLogger
{
	static const int MAX_ARRAY_SIZE = 4096;

	LONGLONG m_liPerformanceFrequency;
	LONGLONG m_liPreviousClock;
	LONGLONG m_liCurrentClock;

	LONGLONG	g_llTimeLog[MAX_ARRAY_SIZE];
	LONGLONG	g_llStartTime;
	int			g_iLogCount;
	int			m_iStop;
	double		m_dAverageFrameRate;
	int			m_iAverageCounter;

	double		m_dTimeSpan;
	double		m_dClocksPerSec;
	double		m_dFrameRate;

	bool		m_bSaved;

public:
	CCallbackLogger(void)
		: m_liPerformanceFrequency(0)
		, m_liPreviousClock(0)
		, m_liCurrentClock(0)
		, g_llStartTime(0)
		, g_iLogCount(0)
		, m_iStop(0)
		, m_dAverageFrameRate(0.0)
		, m_iAverageCounter(0)
		, m_dTimeSpan(0.0)
		, m_dClocksPerSec(0.0)
		, m_dFrameRate(0.0)
		, m_bSaved(true)
	{
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&m_liPerformanceFrequency));
		m_dClocksPerSec = static_cast<double>(m_liPerformanceFrequency);
	}

	~CCallbackLogger(void)
	{
	}

	void Start()
	{
		m_bSaved = false;

		g_iLogCount = 0;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&g_llStartTime));

		memset(g_llTimeLog, 0, MAX_ARRAY_SIZE * 8);
		m_iAverageCounter = 0;
		m_dAverageFrameRate = 0.0;
	}

	double Update()
	{
		static long	lFrames = 0;
		static int	iRunnFlag = 0;
		lFrames += 1;
		m_dTimeSpan = 0.0;

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&m_liCurrentClock));

		g_llTimeLog[g_iLogCount] = m_liCurrentClock;
		g_iLogCount = (g_iLogCount + 1) % MAX_ARRAY_SIZE;
		if(g_iLogCount == 2047)
		{
			//m_ctrlLogFull.SetCheck(1);
		}

		if(m_liPerformanceFrequency <= (m_liCurrentClock - m_liPreviousClock))
		{
			if(m_liPreviousClock > 0)
			{
				if(32 > m_iAverageCounter)
				{
					m_iAverageCounter++;
				}

				m_dTimeSpan = static_cast<double>(m_liCurrentClock - m_liPreviousClock) / m_dClocksPerSec;
				m_dFrameRate = static_cast<double>(lFrames) / m_dTimeSpan;
				m_dAverageFrameRate = (m_dAverageFrameRate * static_cast<double>(m_iAverageCounter - 1) + m_dFrameRate) / static_cast<double>(m_iAverageCounter);

				m_liPreviousClock = m_liCurrentClock;
				lFrames = 0;
				iRunnFlag ^= 1;
			}
			else
			{
				m_liPreviousClock = m_liCurrentClock;
				lFrames = 0;
				iRunnFlag = 0;
			}
		}

		return m_dTimeSpan;
	}

	void Stop()
	{
		m_iStop = 1;
	}

	void SaveLog(std::string sFilename, std::string sOutputFilePath)
	{
		int	iIndex;
		int	iLogIndex;
		int	iLast = (g_iLogCount + 2047) % MAX_ARRAY_SIZE;
		char	cFileName[256];
		double	dSpan;
		double	dPassed;
		LONGLONG	llPrevious = g_llStartTime;
		LONGLONG	llCurrent;

		if(!m_bSaved)
		{
			FILE	*pFile;

			if(g_iLogCount >= iLast)
			{
				g_iLogCount = iLast;
			}

			sprintf_s(cFileName, 255, "\\%s_Log%016I64x.txt", sFilename.c_str(), g_llTimeLog[g_iLogCount-1]);

			sOutputFilePath += cFileName;

			if(0 == fopen_s(&pFile, sOutputFilePath.c_str(), "w"))
			{
				for(iIndex = 0 ; iIndex < MAX_ARRAY_SIZE ; iIndex++)
				{
					iLogIndex = (g_iLogCount + iIndex) % MAX_ARRAY_SIZE;
					if(g_llTimeLog[iLogIndex] != 0)
					{
						llCurrent = g_llTimeLog[iLogIndex];
						dPassed = static_cast<double>(llCurrent - g_llStartTime) * 1000.0 / m_dClocksPerSec;
						dSpan = static_cast<double>(llCurrent - llPrevious) * 1000.0 / m_dClocksPerSec;
						llPrevious = llCurrent;
						fprintf_s(pFile, "%9.3f : %7.3f\n", dPassed, dSpan);
					}
				}

				fclose(pFile);
			}

		}

		m_bSaved = true;
	}
};
