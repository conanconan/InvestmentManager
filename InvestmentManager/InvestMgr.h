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

	virtual bool GetAgentIdList(std::vector<std::string>& idList) const;
	virtual bool GetData(std::string agentId, std::vector<std::string> dataId, 
		int year, int month, int day, void* pAdvData);

private:
	std::map<std::string, std::shared_ptr<CInvestAgent>> m_investAgents;

	void Initialize();
};

