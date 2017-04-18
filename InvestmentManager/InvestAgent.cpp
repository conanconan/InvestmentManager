#include "pch.h"
#include "InvestAgent.h"


CInvestAgent::CInvestAgent(std::wstring agentId, std::shared_ptr<IDataProvider> dataProvider)
	: m_agentId(agentId), m_dataProvider(dataProvider)
{
}

CInvestAgent::~CInvestAgent()
{
}

bool CInvestAgent::GetData(std::vector<std::wstring> dataId, boost::gregorian::date date, std::vector<CDataItem>& data)
{
	return m_dataProvider->GetData(dataId, date, data);
}

bool CInvestAgent::GetData(std::wstring dataId, boost::gregorian::date date, CDataItem& data)
{
	return m_dataProvider->GetData(dataId, date, data);
}