#pragma once
#include "IDataProvider.h"

class CTwStockDataProvider : public IDataProvider
{
public:
	CTwStockDataProvider();
	virtual ~CTwStockDataProvider();

	virtual bool GetData(IDataProvider::DataType dataType, std::vector<std::string> dataId, 
		boost::gregorian::date date, void* pAdvData);
};

