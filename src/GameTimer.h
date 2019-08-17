#pragma once

class GameTimer
{
public:
	GameTimer();

	float TotalTime() const;
	inline float DeltaTime() const { return float(mDeltaTime); } 

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double mSecondsPerCount;
	double mDeltaTime;

	_int64 mBaseTime;
	_int64 mPausedTime;
	_int64 mStopTime;
	_int64 mPrevTime;
	_int64 mCurrTime;

	bool mStopped;
};