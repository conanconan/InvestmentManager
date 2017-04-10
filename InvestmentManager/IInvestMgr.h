#pragma once
#include<vector>

class IInvestMgr
{
public:
	virtual bool GetAgentIdList(std::vector<std::string>& idList) const = 0;
};