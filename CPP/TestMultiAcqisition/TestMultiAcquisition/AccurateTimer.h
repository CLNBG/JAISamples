#ifndef __ACCURATETIMER_H__
#define __ACCURATETIMER_H__
class CAccurateTimer
{
public:
	CAccurateTimer() : bStarted(false) {};
	~CAccurateTimer() {};

	static BOOL Init() { QueryPerformanceFrequency(&CounterFrequency); return (CounterFrequency.QuadPart != 0); };

	void Start() { QueryPerformanceCounter(&StartTime); bStarted = true;} ;

	void Stop() { QueryPerformanceCounter(&EndTime); bStarted = false; } ;

	bool Started() { return bStarted; }

	double GetElapsedTimeInSec()
	{
		QueryPerformanceCounter(&EndTime);

		double dResolutionInCounstPerSec = (double)CounterFrequency.QuadPart;

		TimeDiff.QuadPart = (EndTime.QuadPart - StartTime.QuadPart);

		double dDiffInSeconds = (double)TimeDiff.QuadPart / dResolutionInCounstPerSec;

		return dDiffInSeconds;
	}

	double GetElapsedTimeInMsec()
	{
		QueryPerformanceCounter(&EndTime);

		double dResolutionInCounstPerSec = (double)CounterFrequency.QuadPart;

		double dResolutionInCounstPerMsec = dResolutionInCounstPerSec / 1000.;

		TimeDiff.QuadPart = (EndTime.QuadPart - StartTime.QuadPart);

		double dDiffInMseconds = (double)TimeDiff.QuadPart / dResolutionInCounstPerMsec;

		return dDiffInMseconds;
	}

	//Initialize timer
	LARGE_INTEGER StartTime, EndTime, TimeDiff;

private:
	static LARGE_INTEGER CounterFrequency;
	bool bStarted;
};


#endif //__ACCURATETIMER_H__
