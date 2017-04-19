#pragma once
#include <string>
#include <memory>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "IDataProvider.h"

class CInvestAgent
{
public:
	CInvestAgent(std::wstring agentId, std::shared_ptr<IDataProvider> dataProvider);
	virtual ~CInvestAgent();

	bool GetData(std::vector<std::wstring> dataId, boost::gregorian::date date, 
		std::vector<CDataItem>& data);
	bool GetData(std::wstring dataId, boost::gregorian::date date, CDataItem& data);
	bool GetData(std::wstring dataId, boost::gregorian::date fromDate, size_t dayCount, 
		std::vector<CDataItem>& data);
	bool GetSimpleData(std::vector<std::wstring> dataId, boost::gregorian::date date, 
		std::vector<std::wstring>& data);
	bool GetSimpleData(std::wstring dataId, boost::gregorian::date date, std::wstring& data);

private:
	std::wstring m_agentId;
	std::shared_ptr<IDataProvider> m_dataProvider;
};

