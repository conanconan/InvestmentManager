#pragma once
#include "IDataProvider.h"

class CTwStockTSEDataProvider : public IDataProvider
{
public:
	CTwStockTSEDataProvider();
	virtual ~CTwStockTSEDataProvider();

	virtual bool GetDataId(std::map<std::wstring, std::vector<std::wstring>>& dataId) const;
	virtual bool GetRealTimeData(std::wstring dataId, CDataItem& data) const;
	virtual bool GetData(std::wstring dataId,
		boost::gregorian::date date, CDataItem& data) const;
	virtual bool GetOneMonthData(std::wstring dataId,
		boost::gregorian::date date, std::vector<CDataItem>& data) const;
};

