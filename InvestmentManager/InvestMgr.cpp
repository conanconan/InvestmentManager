#include "pch.h"
#include "InvestMgr.h"
#include <algorithm>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "InvestAgent.h"
#include "TwStockDataProvider.h"


CInvestMgr::CInvestMgr()
{
	Initialize();
}

CInvestMgr::~CInvestMgr()
{
}

void CInvestMgr::Initialize()
{
	// ¤W¥«
	m_investAgents["TwStock_StockExchangeMarket"] = std::shared_ptr<CInvestAgent>
		(new CInvestAgent("TwStock_StockExchangeMarket", 
			std::shared_ptr<IDataProvider>(new CTwStockDataProvider("tse"))));
	// ¤WÂd
	m_investAgents["TwStock_OverTheCounterMarket"] = std::shared_ptr<CInvestAgent>
		(new CInvestAgent("TwStock_OverTheCounterMarket",
			std::shared_ptr<IDataProvider>(new CTwStockDataProvider("otc"))));
	// ¿³Âd TwStock_EmergingStockMarket
}

bool CInvestMgr::GetAgentIdList(std::vector<std::string>& idList) const
{
	if (!m_investAgents.empty())
	{
		idList.clear();
		std::transform(m_investAgents.begin(), m_investAgents.end(), std::back_inserter(idList),
			[](const auto& item) { return item.first; });

		return true;
	}

	return false;
}

bool CInvestMgr::GetData(std::string agentId, std::vector<std::string> dataId, 
	int year, int month, int day, void* pAdvData)
{
	auto agentIter = m_investAgents.find(agentId);
	if (agentIter != m_investAgents.end())
	{
		return agentIter->second->GetData(dataId, boost::gregorian::date(year, month, day), pAdvData);
	}

	return false;
}
