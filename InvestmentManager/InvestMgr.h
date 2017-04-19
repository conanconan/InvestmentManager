#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "IInvestMgr.h"
#include "InvestAgent.h"

class CInvestMgr: public IInvestMgr
{
public:
	CInvestMgr();
	virtual ~CInvestMgr();

	virtual bool GetAgentIdList(std::vector<std::wstring>& idList) const;
	virtual bool GetData(std::wstring agentId, std::vector<std::wstring> dataId, 
		int year, int month, int day, std::vector<std::shared_ptr<CDataItem>>& data);
	virtual bool GetData(std::wstring agentId, std::wstring dataId,
		int year, int month, int day, std::shared_ptr<CDataItem>& data);
	virtual bool GetData(std::wstring agentId, std::wstring dataId,
		int fromYear, int fromMonth, int fromDay, size_t dayCount, 
		std::vector<std::shared_ptr<CDataItem>>& data);
	virtual bool GetData(std::wstring agentId, std::wstring dataId,
		int fromYear, int fromMonth, int fromDay,
		int toYear, int toMonth, int toDay,
		std::vector<std::shared_ptr<CDataItem>>& data);
	virtual bool GetSimpleData(std::wstring agentId, std::vector<std::wstring> dataId,
		int year, int month, int day, std::vector<std::wstring>& data);
	virtual bool GetSimpleData(std::wstring agentId, std::wstring dataId,
		int year, int month, int day, std::wstring& data);

private:
	std::map<std::wstring, std::shared_ptr<CInvestAgent>> m_investAgents;

	void Initialize();
};

