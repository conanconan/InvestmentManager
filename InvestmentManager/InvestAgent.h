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

	bool GetDataIdList(std::vector<std::string>& idList) const;
	double GetData(std::string dataId, boost::gregorian::date date);
	bool GetAdvData(std::string dataId, boost::gregorian::date date, void* pAdvData);

private:
	std::string m_agentId;
	std::shared_ptr<IDataProvider> m_dataProvider;
};

