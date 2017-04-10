#pragma once
#include <string>
#include <vector>
#include "boost/date_time/gregorian/gregorian.hpp"

class IDataProvider
{
public:
	virtual bool GetDataIdList(std::vector<std::string>& idList) const = 0;
	virtual double GetData(std::string dataId, boost::gregorian::date date) = 0;
	virtual bool GetAdvData(std::string dataId, boost::gregorian::date date, void* pAdvData) = 0;
};