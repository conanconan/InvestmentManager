#pragma once
#include <string>
#include <vector>
#include <memory>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "DataItem.h"

class IDataProvider
{
public:
    virtual boost::gregorian::date DataBeginsOn() const = 0;
	virtual bool GetDataId(std::map<std::wstring, std::vector<std::wstring>>& dataId) const = 0;// key: category, value: id list
	virtual bool GetRealTimeData(std::wstring dataId, CDataItem& data) const = 0;
	virtual bool GetData(std::wstring dataId,
		boost::gregorian::date date, CDataItem& data) const = 0;
	virtual bool GetOneMonthData(std::wstring dataId,
		boost::gregorian::date date, std::vector<CDataItem>& data) const = 0;
};