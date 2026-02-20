#pragma once

#include <chrono>

namespace uti
{
	class Timer
	{
	public:
		Timer();
		~Timer();

		long long getElapsedTime_milliseconds() const;
		long long getElapsedTime_seconds() const;
		long long getElapsedTime_nanoSeconds() const;

		void start();


	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> start_point;

	};

	inline long long Timer::getElapsedTime_nanoSeconds() const
	{
		auto end_point = std::chrono::high_resolution_clock::now();
		std::chrono::nanoseconds nanoSecs = std::chrono::duration_cast<std::chrono::nanoseconds>(end_point - start_point);
		return nanoSecs.count();
	}

	inline long long Timer::getElapsedTime_seconds() const
	{
		auto end_point = std::chrono::high_resolution_clock::now();
		std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(end_point - start_point);
		return secs.count();
	}

	inline long long Timer::getElapsedTime_milliseconds() const
	{
		auto end_point = std::chrono::high_resolution_clock::now();
		std::chrono::milliseconds milliSec = std::chrono::duration_cast<std::chrono::milliseconds>(end_point - start_point);
		return milliSec.count();
	}

	inline void Timer::start()
	{
		start_point = std::chrono::high_resolution_clock::now();
	}

	inline Timer::Timer()
	{
		start();
	}

	inline Timer::~Timer()
	{
	}
}