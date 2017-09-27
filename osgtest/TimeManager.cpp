#include "TimeManager.h"
#include <iostream>
#include <boost/bind.hpp>

typedef boost::chrono::steady_clock steady_clock_t;
typedef boost::chrono::steady_clock::time_point timepoint_t;
typedef boost::chrono::steady_clock::duration duration_t;

TimeManager::TimeManager(int milliseconds):m_running(false),  m_epoch(milliseconds)
{
}

void TimeManager::loop(void)
{
	std::cout<<"time - starting "<<std::endl;
	timepoint_t t1 = steady_clock_t::now();
	while(m_running)
	{
		timepoint_t current_time = steady_clock_t::now();
		duration_t diff(current_time-t1);
		if(diff<m_epoch)
		{
			continue;
		}
		else
		{
			t1 = t1+m_epoch;
			//std::cout<<"time - "<<current_time.time_since_epoch().count()<<" "<<t1.time_since_epoch().count()<<" "<<current_time.time_since_epoch().count()-t1.time_since_epoch().count()<<std::endl;
		}
	}
	std::cout<<"time - stopping "<<std::endl;
}

void TimeManager::startTime(void)
{
	m_running=true;
	m_main.reset(new boost::thread(boost::bind( &TimeManager::loop, this )));
}

void TimeManager::stopTime(void)
{
	m_running=false;
}

void TimeManager::addEpoch(Epoch e)
{
	//time does not go backward!
	if(e.before(m_queue.front()) && !m_queue.empty())
	{
		//throw it away
		return;
	}
	m_queue.push(e);
}
TimeManager::~TimeManager(void)
{
}
