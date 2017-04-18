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
		int year, int month, int day, std::vector<CDataItem>& data);
	virtual bool GetData(std::wstring agentId, std::wstring dataId,
		int year, int month, int day, CDataItem& data);

private:
	std::map<std::wstring, std::shared_ptr<CInvestAgent>> m_investAgents;

	void Initialize();
};

