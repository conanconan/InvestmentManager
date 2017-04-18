#pragma once
#include "IDataProvider.h"
#include <string>

class CTwStockDataProvider : public IDataProvider
{
public:
	CTwStockDataProvider(std::wstring category);
	virtual ~CTwStockDataProvider();

	virtual bool GetData(std::vector<std::wstring> dataId, 
		boost::gregorian::date date, std::vector<CDataItem>& data) const;
	virtual bool GetData(std::wstring dataId,
		boost::gregorian::date date, CDataItem& data) const;

private:
	std::wstring m_category;//tse: ¤W¥«, otc: ¤WÂd
};

