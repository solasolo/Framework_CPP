#pragma once

#include <stdlib.h>
#include <string>
#include <list>
#include <chrono>

#include "CommonDef.hpp"

namespace SoulFab::System
{
	enum class StopWatchType
	{
		TimeWatch,
		CountWatch
	};

	struct CounterStatistics
	{
		double Max;
		double Min;
		double Sum;
		double Avg;
	};

	class _declspec(dllexport) PerformanceTimer
	{
	private:
		std::chrono::time_point<std::chrono::steady_clock> StartTime;
		std::chrono::time_point<std::chrono::steady_clock> StopTime;

	public:
		void Start();
		double Stop();
	};

	class _declspec(dllexport) StopWatch
	{
	private:
		std::string ModuleName;

	protected:
		int Size;		// �����С(����)
		bool Enabled;
		double* Buffer;	// ��¼����
		int CurrentIndex;

		PerformanceTimer Counter;

		virtual void Analysis(CounterStatistics& cs);

	public:
		StopWatch(const std::string module, int len);
		~StopWatch();

		int GetSize();

		std::string FormatString();
	};

	class _declspec(dllexport) TimeStopWatch : public StopWatch
	{
	public:
		TimeStopWatch(const std::string module, int len);
		~TimeStopWatch();

		virtual void Start();
		virtual bool Stop();
	};

	/**
	*���ܷ���������
	*/
	class _declspec(dllexport) PerformanceAnalysis
	{
	private:
		static PerformanceAnalysis* instance;

		std::list<TimeStopWatch*> WatchList;

	public:
		static PerformanceAnalysis* GetInstance();
		static void Destroy();

		PerformanceAnalysis();
		virtual ~PerformanceAnalysis();

		TimeStopWatch* Register(const std::string module, int len = 1024);
	};
}