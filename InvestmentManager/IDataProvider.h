#pragma once
#include <string>
#include <vector>
#include "boost/date_time/gregorian/gregorian.hpp"

class IDataProvider
{
public:
	virtual bool GetData(std::vector<std::string> dataId, 
		boost::gregorian::date date, void* pAdvData) = 0;
};