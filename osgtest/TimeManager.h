#pragma once
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <queue>
#include "Epoch.h"

class TimeManager
{
private:
	boost::scoped_ptr<boost::thread> m_main;
	bool m_running;
	/**
	* epoch length
	*/
	boost::chrono::milliseconds m_epoch;
	std::queue<Epoch> m_queue;
public:
	TimeManager(int milliseconds=20);
	~TimeManager(void);
	
	void startTime(void);
	void loop(void);
	
	void stopTime(void);
	void addEpoch(Epoch e);
};

