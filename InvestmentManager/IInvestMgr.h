#pragma once
#include<vector>
#include "DataItem.h"

class IInvestMgr
{
public:
	virtual bool GetAgentIdList(std::vector<std::wstring>& idList) const = 0;
	virtual bool GetData(std::wstring agentId, std::vector<std::wstring> dataId,
		int year, int month, int day, std::vector<CDataItem>& data) = 0;
	virtual bool GetData(std::wstring agentId, std::wstring dataId, 
		int year, int month, int day, CDataItem& data) = 0;
};