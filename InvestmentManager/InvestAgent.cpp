#include "pch.h"
#include "InvestAgent.h"


CInvestAgent::CInvestAgent(std::string agentId, std::shared_ptr<IDataProvider> dataProvider)
	: m_agentId(agentId), m_dataProvider(dataProvider)
{
}

CInvestAgent::~CInvestAgent()
{
}

bool CInvestAgent::GetDataIdList(std::vector<std::string>& idList) const
{
	return m_dataProvider->GetDataIdList(idList);
}

double CInvestAgent::GetData(std::string dataId, boost::gregorian::date date)
{
	return m_dataProvider->GetData(dataId, date);
}

bool CInvestAgent::GetAdvData(std::string dataId, boost::gregorian::date date, void* pAdvData)
{
	return m_dataProvider->GetAdvData(dataId, date, pAdvData);
}