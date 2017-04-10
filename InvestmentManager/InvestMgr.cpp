#include "pch.h"
#include "InvestMgr.h"
#include <algorithm>


CInvestMgr::CInvestMgr()
{
}

CInvestMgr::~CInvestMgr()
{
}

bool CInvestMgr::GetAgentIdList(std::vector<std::string>& idList) const
{
	if (m_investAgents.empty())
	{
		return false;
	}

	idList.clear();
	std::transform(m_investAgents.begin(), m_investAgents.end(), idList.begin(),
		[](const auto& item) { return item.first; });

	return true;
}
