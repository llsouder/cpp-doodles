#pragma once
#include <boost/cstdint.hpp>

class Epoch
{
private:
	int m_epoch;
	boost::uint64_t m_arrval;

public:
	Epoch(int epoch, boost::uint64_t arrival);
	~Epoch(void);
	bool before(Epoch e)
	{
		return m_epoch<e.m_epoch;
	}
};

