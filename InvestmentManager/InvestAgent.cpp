#include "pch.h"
#include "InvestAgent.h"

const boost::gregorian::date_duration oneDay(1);

CInvestAgent::CInvestAgent(std::wstring agentId, std::shared_ptr<IDataProvider> dataProvider)
	: m_agentId(agentId), m_dataProvider(dataProvider)
{
}

CInvestAgent::~CInvestAgent()
{
}

bool CInvestAgent::GetData(std::vector<std::wstring> dataId, boost::gregorian::date date, 
	std::vector<std::shared_ptr<CDataItem>>& data)
{
	return m_dataProvider->GetData(dataId, date, data);
}

bool CInvestAgent::GetData(std::wstring dataId, boost::gregorian::date date, 
	std::shared_ptr<CDataItem>& data)
{
	return m_dataProvider->GetData(dataId, date, data);
}

bool CInvestAgent::GetData(std::wstring dataId, boost::gregorian::date fromDate, size_t dayCount,
	std::vector<std::shared_ptr<CDataItem>>& data)
{
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	data.clear();

	boost::gregorian::date date = fromDate;
	while (data.size() < dayCount && date <= today)
	{
		std::shared_ptr<CDataItem> dataItem;
		if (GetData(dataId, date, dataItem))
		{
			data.push_back(dataItem);
		}

		date += oneDay;
	}

	return (data.size() == dayCount);
}

bool CInvestAgent::GetData(std::wstring dataId, boost::gregorian::date fromDate,
	boost::gregorian::date toDate, std::vector<std::shared_ptr<CDataItem>>& data)
{
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	boost::gregorian::date_duration oneDay(1);
	data.clear();

	boost::gregorian::date date = fromDate;
	boost::gregorian::date endDate = (toDate < today) ? toDate : today;
	while (date <= endDate)
	{
		std::shared_ptr<CDataItem> dataItem;
		if (GetData(dataId, date, dataItem))
		{
			data.push_back(dataItem);
		}

		date += oneDay;
	}

	return !data.empty();
}

bool CInvestAgent::GetSimpleData(std::vector<std::wstring> dataId, boost::gregorian::date date,
	std::vector<std::wstring>& data)
{
	std::vector<std::shared_ptr<CDataItem>> fullData;
	if (GetData(dataId, date, fullData))
	{
		data.clear();
		std::transform(fullData.begin(), fullData.end(), std::back_inserter(data),
			[](const auto& item) { return item->closingPrice; });

		return true;
	}

	return false;
}

bool CInvestAgent::GetSimpleData(std::wstring dataId, boost::gregorian::date date, std::wstring& data)
{
	std::shared_ptr<CDataItem> fullData;
	if (GetData(dataId, date, fullData))
	{
		data = fullData->closingPrice;
		return true;
	}

	return false;
}