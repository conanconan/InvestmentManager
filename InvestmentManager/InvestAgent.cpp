#include "pch.h"
#include "InvestAgent.h"


CInvestAgent::CInvestAgent(std::wstring agentId, std::shared_ptr<IDataProvider> dataProvider)
	: m_agentId(agentId), m_dataProvider(dataProvider)
{
}

CInvestAgent::~CInvestAgent()
{
}

bool CInvestAgent::GetData(std::vector<std::wstring> dataId, boost::gregorian::date date, 
	std::vector<CDataItem>& data)
{
	return m_dataProvider->GetData(dataId, date, data);
}

bool CInvestAgent::GetData(std::wstring dataId, boost::gregorian::date date, CDataItem& data)
{
	return m_dataProvider->GetData(dataId, date, data);
}

bool CInvestAgent::GetSimpleData(std::vector<std::wstring> dataId, boost::gregorian::date date,
	std::vector<std::wstring>& data)
{
	std::vector<CDataItem> fullData;
	if (GetData(dataId, date, fullData))
	{
		data.clear();
		std::transform(fullData.begin(), fullData.end(), std::back_inserter(data),
			[](const auto& item) { return item.closingPrice; });

		return true;
	}

	return false;
}

bool CInvestAgent::GetSimpleData(std::wstring dataId, boost::gregorian::date date, std::wstring& data)
{
	CDataItem fullData;
	if (GetData(dataId, date, fullData))
	{
		data = fullData.closingPrice;
		return true;
	}

	return false;
}