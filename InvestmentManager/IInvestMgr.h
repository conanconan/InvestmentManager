#pragma once
#include<vector>
#include "DataItem.h"

class IInvestMgr
{
public:
	virtual bool GetAgentIdList(std::vector<std::string>& idList) const = 0;
	virtual bool GetData(std::string agentId, std::vector<std::string> dataId,
		int year, int month, int day, std::vector<CDataItem>& data) = 0;
};