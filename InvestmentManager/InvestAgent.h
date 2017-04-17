#pragma once
#include <string>
#include <memory>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "IDataProvider.h"

class CInvestAgent
{
public:
	CInvestAgent(std::string agentId, std::shared_ptr<IDataProvider> dataProvider);
	virtual ~CInvestAgent();

	bool GetData(std::vector<std::string> dataId, boost::gregorian::date date, std::vector<double>& data);
	bool GetData(std::vector<std::string> dataId, boost::gregorian::date date, std::vector<CDataItem>& data);

private:
	std::string m_agentId;
	std::shared_ptr<IDataProvider> m_dataProvider;
};

