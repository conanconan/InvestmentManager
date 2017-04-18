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
	m_investAgents[L"TwStock_StockExchangeMarket"] = std::shared_ptr<CInvestAgent>
		(new CInvestAgent(L"TwStock_StockExchangeMarket", 
			std::shared_ptr<IDataProvider>(new CTwStockDataProvider(L"tse"))));
	// ¤WÂd
	m_investAgents[L"TwStock_OverTheCounterMarket"] = std::shared_ptr<CInvestAgent>
		(new CInvestAgent(L"TwStock_OverTheCounterMarket",
			std::shared_ptr<IDataProvider>(new CTwStockDataProvider(L"otc"))));
	// ¿³Âd TwStock_EmergingStockMarket
}

bool CInvestMgr::GetAgentIdList(std::vector<std::wstring>& idList) const
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

bool CInvestMgr::GetData(std::wstring agentId, std::vector<std::wstring> dataId, 
	int year, int month, int day, std::vector<CDataItem>& data)
{
	auto agentIter = m_investAgents.find(agentId);
	if (agentIter != m_investAgents.end())
	{
		return agentIter->second->GetData(dataId, boost::gregorian::date(year, month, day), data);
	}

	return false;
}

bool CInvestMgr::GetData(std::wstring agentId, std::wstring dataId,
	int year, int month, int day, CDataItem& data)
{
	auto agentIter = m_investAgents.find(agentId);
	if (agentIter != m_investAgents.end())
	{
		return agentIter->second->GetData(dataId, boost::gregorian::date(year, month, day), data);
	}

	return false;
}
