#pragma once
#include <string>
#include <vector>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "DataItem.h"

class IDataProvider
{
public:
	virtual bool GetData(std::vector<std::string> dataId, 
		boost::gregorian::date date, std::vector<CDataItem>& data) const = 0;
};