#pragma once
#include <vector>
#include <memory>
#include "DataItem.h"

class IInvestMgr
{
public:
	virtual bool GetAgentIdList(std::vector<std::wstring>& idList) const = 0;
	virtual bool GetData(std::wstring agentId, std::vector<std::wstring> dataId,
		int year, int month, int day, std::vector<std::shared_ptr<CDataItem>>& data) = 0;
	virtual bool GetData(std::wstring agentId, std::wstring dataId, 
		int year, int month, int day, std::shared_ptr<CDataItem>& data) = 0;
	virtual bool GetData(std::wstring agentId, std::wstring dataId,
		int fromYear, int fromMonth, int fromDay, size_t dayCount, 
		std::vector<std::shared_ptr<CDataItem>>& data) = 0;
	virtual bool GetData(std::wstring agentId, std::wstring dataId,
		int fromYear, int fromMonth, int fromDay, 
		int toYear, int toMonth, int toDay, 
		std::vector<std::shared_ptr<CDataItem>>& data) = 0;
	virtual bool GetSimpleData(std::wstring agentId, std::vector<std::wstring> dataId,
		int year, int month, int day, std::vector<std::wstring>& data) = 0;
	virtual bool GetSimpleData(std::wstring agentId, std::wstring dataId,
		int year, int month, int day, std::wstring& data) = 0;
};