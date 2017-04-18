#pragma once
#include "IDataProvider.h"
#include <string>

class CTwStockDataProvider : public IDataProvider
{
public:
	CTwStockDataProvider(std::string category);
	virtual ~CTwStockDataProvider();

	virtual bool GetData(std::vector<std::string> dataId, 
		boost::gregorian::date date, std::vector<CDataItem>& data) const;
	virtual bool GetData(std::string dataId,
		boost::gregorian::date date, CDataItem& data) const;

private:
	std::string m_category;//tse: ¤W¥«, otc: ¤WÂd
};

