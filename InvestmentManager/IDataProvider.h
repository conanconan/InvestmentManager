#pragma once
#include <string>
#include <vector>
#include <memory>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "DataItem.h"

class IDataProvider
{
public:
	virtual bool GetData(std::vector<std::wstring> dataId, 
		boost::gregorian::date date, std::vector<std::shared_ptr<CDataItem>>& data) const = 0;
	virtual bool GetData(std::wstring dataId,
		boost::gregorian::date date, std::shared_ptr<CDataItem>& data) const = 0;
};