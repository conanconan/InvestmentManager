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
	virtual bool GetData(std::wstring agentId, std::wstring dataId,
		int fromYear, int fromMonth, int fromDay, size_t dayCount, std::vector<CDataItem>& data) = 0;
	virtual bool GetSimpleData(std::wstring agentId, std::vector<std::wstring> dataId,
		int year, int month, int day, std::vector<std::wstring>& data) = 0;
	virtual bool GetSimpleData(std::wstring agentId, std::wstring dataId,
		int year, int month, int day, std::wstring& data) = 0;
};