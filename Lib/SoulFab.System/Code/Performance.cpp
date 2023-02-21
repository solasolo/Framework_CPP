#include "Performance.hpp"

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <limits>

using namespace std;
using namespace std::chrono;

namespace SoulFab::System
{
	inline void PerformanceTimer::Start()
	{
		this->StartTime = high_resolution_clock::now();
	}

	double PerformanceTimer::Stop()
	{
		double Result;

		this->StopTime = high_resolution_clock::now();

		uint64 diff = duration_cast<microseconds>(high_resolution_clock::now() - this->StartTime).count();

		Result = (double)(diff) / 1000;		//milliseconds 

		return Result;
	}

	StopWatch::StopWatch(const string module, int len)
	{
		this->Size = len;
		this->Buffer = new double[this->Size];
		this->ModuleName = module;

		this->CurrentIndex = 0;

		this->Enabled = true;
	}

	StopWatch::~StopWatch()
	{
		SafeDestroy(this->Buffer);
	}

	void StopWatch::Analysis(CounterStatistics& cs)
	{
		cs.Max = this->Buffer[0];
		cs.Min = this->Buffer[0];
		cs.Sum = this->Buffer[0];
		cs.Avg = 0;

		for (int i = 1; i < this->Size; i++)
		{
			if (this->Buffer[i] > cs.Max)
			{
				cs.Max = this->Buffer[i];
			}
			else if (this->Buffer[i] < cs.Min)
			{
				cs.Min = this->Buffer[i];
			}

			cs.Sum += this->Buffer[i];
		}

		cs.Avg = cs.Sum / this->Size;
	}

	int StopWatch::GetSize()
	{
		return this->Size;
	}

	string StopWatch::FormatString()
	{
		CounterStatistics cs;
		this->Analysis(cs);

		char buf[100];
		buf[99] = '\x0';

		sprintf_s(buf, 99, "[%s] Timer(ms): Max:%5.2f Min:%5.2f Avg:%5.2f", this->ModuleName.c_str(), cs.Max, cs.Min, cs.Avg);

		return buf;
	}

	TimeStopWatch::TimeStopWatch(const string module, int len)
		: StopWatch(module, len)
	{
	}

	void TimeStopWatch::Start()
	{
		if (this->Enabled)
		{
			this->Counter.Start();
		}
	}

	bool TimeStopWatch::Stop()
	{
		bool Result = false;

		if (this->Enabled)
		{
			double timeCount = this->Counter.Stop();

			this->Buffer[this->CurrentIndex++] = timeCount;

			if (this->CurrentIndex >= this->Size)
			{
				Result = true;

				this->CurrentIndex = 0;
			}
		}

		return Result;
	}

	//性能分析器管理//
	PerformanceAnalysis* PerformanceAnalysis::instance;

	PerformanceAnalysis::PerformanceAnalysis()
	{

	}

	PerformanceAnalysis::~PerformanceAnalysis()
	{
		while (!this->WatchList.empty())
		{
			TimeStopWatch* timer = this->WatchList.front();
			this->WatchList.pop_front();
		}
	}

	PerformanceAnalysis* PerformanceAnalysis::GetInstance()
	{
		if (!instance)
		{
			instance = new PerformanceAnalysis();
		}

		return instance;
	}

	void PerformanceAnalysis::Destroy()
	{
		SafeDestroy(instance);
	}

	TimeStopWatch* PerformanceAnalysis::Register(const string module, int len)
	{
		TimeStopWatch* watcher = new TimeStopWatch(module, len);

		this->WatchList.push_back(watcher);

		return watcher;
	}
}
//string DoubleToString(double value, int decplaces=3)
//{
//	ostringstream out;
//	int prec = numeric_limits<double>::digits10; // 18
//	out.precision(prec); //覆盖默认精度
//	out << value;
//	string str = out.str(); //从流中取出字符串
//	char DECIMAL_POINT = '.'; // 欧洲用法为','
//	size_t n = str.find(DECIMAL_POINT);
//	if ((n != string::npos) && (str.size() > n + decplaces)) //有小数点吗？//后面至少还有decplaces位吗？
//	{
//		str[n+decplaces] = '\0';//覆盖第一个多余的数
//	}
//	str.swap(string(str.c_str()));//删除null之后的多余字符
//	return str;
//}