#pragma once
#include "IDataProvider.h"
#include <string>

class CTwStockDataProvider : public IDataProvider
{
public:
	CTwStockDataProvider(std::wstring category);
	virtual ~CTwStockDataProvider();

	virtual bool GetDataId(std::map<std::wstring, std::vector<std::wstring>>& dataId) const;
	virtual bool GetData(std::vector<std::wstring> dataId, 
		boost::gregorian::date date, std::vector<std::shared_ptr<CDataItem>>& data) const;
	virtual bool GetData(std::wstring dataId,
		boost::gregorian::date date, std::shared_ptr<CDataItem>& data) const;

private:
	const std::wstring m_category;//tse: ¤W¥«, otc: ¤WÂd
};

