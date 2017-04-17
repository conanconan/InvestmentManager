#include "pch.h"
#include "InvestAgent.h"


CInvestAgent::CInvestAgent(std::string agentId, std::shared_ptr<IDataProvider> dataProvider)
	: m_agentId(agentId), m_dataProvider(dataProvider)
{
}

CInvestAgent::~CInvestAgent()
{
}

bool CInvestAgent::GetData(std::vector<std::string> dataId, boost::gregorian::date date, std::vector<double>& data)
{
	return false;
}

bool CInvestAgent::GetData(std::vector<std::string> dataId, boost::gregorian::date date, std::vector<CDataItem>& data)
{
	return m_dataProvider->GetData(dataId, date, data);
}