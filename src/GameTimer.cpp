#include "GameTimer.h"
#include "windows.h"

GameTimer::GameTimer()
	: mSecondsPerCount(0),
	mDeltaTime(0),
	mBaseTime(0),
	mPrevTime(0),
	mCurrTime(0),
	mPausedTime(0),
	mStopTime(0),
	mStopped(false)
{
	_int64 countsPesSec;

	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPesSec);
	mSecondsPerCount = 1.0 / countsPesSec;
}

float GameTimer::TotalTime() const
{
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}

	else return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
}

void GameTimer::Reset()
{
	_int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)& currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	_int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)& startTime);

	if (mStopped)
	{
		mPausedTime += startTime - mStopTime;
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		_int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)& currTime);
		mStopTime = currTime;
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0;
		return;
	}

	_int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)& currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;
	mPrevTime = mCurrTime;

	mDeltaTime = mDeltaTime < 0.0 ? 0.0 : mDeltaTime;
}