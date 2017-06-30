#include "pch.h"
#include "InvestAgent.h"
#include <thread>
#include "InvestDb.h"

const size_t fewDayBoundary = 50;
const size_t aroundWorkingDayPerMonth = 22;
const boost::gregorian::date_duration oneDay(1);

CInvestAgent::CInvestAgent(std::shared_ptr<IDataProvider> dataProvider, 
    std::shared_ptr<CInvestDb> db)
	: m_dataProvider(dataProvider), m_db(db)
{
}

CInvestAgent::~CInvestAgent()
{
}

bool CInvestAgent::GetData(std::vector<std::wstring> dataId, boost::gregorian::date date, 
	std::vector<CDataItem>& data)
{
	data.clear();
	for (const auto& id : dataId)
	{
		CDataItem item;
		if (GetData(id, date, item))
		{
			data.push_back(item);
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool CInvestAgent::GetData(std::wstring dataId, boost::gregorian::date date, 
	CDataItem& data)
{
    if (m_db->QueryData(dataId, date, data))
    {
        return true;
    }

    if (m_dataProvider->GetData(dataId, date, data))
    {
        m_db->InsertData(data);
        return true;
    }

    return false;
}

bool CInvestAgent::GetData(std::wstring dataId, boost::gregorian::date fromDate, size_t dayCount,
	std::vector<CDataItem>& data)
{
	if (dayCount <= fewDayBoundary)
	{
		return GetFewData(dataId, fromDate, dayCount, data);
	}

	boost::gregorian::date lastDate = 
		(fromDate + boost::gregorian::months(1 + dayCount / aroundWorkingDayPerMonth)).end_of_month();
	data.clear();
	GetData(dataId, fromDate, lastDate, data);

	if (data.size() > dayCount)
	{
		data.erase(data.begin() + dayCount, data.end());
	}
	else if (data.size() < dayCount)
	{
		std::vector<CDataItem> fewData;
		GetFewData(dataId, lastDate + oneDay, dayCount - data.size(), fewData);

		std::transform(fewData.begin(), fewData.end(), std::back_inserter(data),
			[](const auto& item) { return item; });
	}

	return (data.size() == dayCount);
}

bool CInvestAgent::GetFewData(std::wstring dataId, boost::gregorian::date fromDate, size_t dayCount,
	std::vector<CDataItem>& data)
{
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();

	data.clear();
	boost::gregorian::date date = fromDate;
	while (data.size() < dayCount && date <= today)
	{
		CDataItem dataItem;
		if (GetData(dataId, date, dataItem))
		{
			data.push_back(dataItem);
		}

		date += oneDay;
	}

	return (data.size() == dayCount);
}

bool CInvestAgent::GetFewDataForMultiThread(CInvestAgent* agent, std::wstring dataId, 
	boost::gregorian::date fromDate, boost::gregorian::date toDate, 
	std::shared_ptr<std::vector<CDataItem>> data)
{
	return agent->GetFewData(dataId, fromDate, toDate, *data);
}

bool CInvestAgent::GetData(std::wstring dataId, boost::gregorian::date fromDate,
	boost::gregorian::date toDate, std::vector<CDataItem>& data)
{
	std::vector<std::shared_ptr<std::vector<CDataItem>>> allData;
	std::vector<std::shared_ptr<std::thread>> threads;
	for (size_t i = 0, monthCount = 1 + (toDate.year() - fromDate.year()) * 12 + (toDate.month() - fromDate.month()); 
		 i < monthCount; ++i)
	{
		boost::gregorian::date first(fromDate.year() + (i + fromDate.month() - 1) / 12,
									 (i + fromDate.month() - 1) % 12 + 1,
									 (i != 0) ? 1 : fromDate.day());
		boost::gregorian::date last = (first.year() != toDate.year() || first.month() != toDate.month()) 
			? first.end_of_month() : toDate;
		allData.push_back(std::make_shared<std::vector<CDataItem>>());

		threads.push_back(std::make_shared<std::thread>
			(&CInvestAgent::GetFewDataForMultiThread, this, dataId, first, last, allData.back()));
	}

	for (auto& thread : threads)
	{
		thread->join();
	}

	data.clear();
	for (auto& fewData : allData)
	{
		std::transform(fewData->begin(), fewData->end(), std::back_inserter(data),
			[](const auto& item) { return item; });
	}
	
	return true;
}

bool CInvestAgent::GetFewData(std::wstring dataId, boost::gregorian::date fromDate,
	boost::gregorian::date toDate, std::vector<CDataItem>& data)
{
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	boost::gregorian::date_duration oneDay(1);
	data.clear();

	boost::gregorian::date date = fromDate;
	boost::gregorian::date endDate = (toDate < today) ? toDate : today;
	while (date <= endDate)
	{
		CDataItem dataItem;
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