#pragma once
#include "IDataProvider.h"
#include <string>

class CTwStockDataProvider : public IDataProvider
{
public:
	CTwStockDataProvider(std::wstring category);
	virtual ~CTwStockDataProvider();

	virtual bool GetDataId(std::map<std::wstring, std::vector<std::wstring>>& dataId) const;
	virtual bool GetRealTimeData(std::wstring dataId, CDataItem& data) const;
	virtual bool GetData(std::wstring dataId,
		boost::gregorian::date date, CDataItem& data) const;
	virtual bool GetOneMonthData(std::wstring dataId,
		boost::gregorian::date date, std::vector<CDataItem>& data) const;

private:
	const std::wstring m_category;//tse: ¤W¥«, otc: ¤WÂd

	bool GetRealTimeData(std::vector<std::wstring> dataId,
		std::vector<CDataItem>& data) const;
	bool GetTSEData(std::wstring dataId,
		boost::gregorian::date date, CDataItem& data) const;
	bool GetTSEOneMonthData(std::wstring dataId,
		boost::gregorian::date date, std::vector<CDataItem>& data) const;
	bool GetOTCData(std::wstring dataId,
		boost::gregorian::date date, CDataItem& data) const;
	bool GetOTCOneMonthData(std::wstring dataId,
		boost::gregorian::date date, std::vector<CDataItem>& data) const;
};

