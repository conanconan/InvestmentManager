#include "pch.h"
#include "InvestAgent.h"
#include "InvestDb.h"
#include <thread>
#include <future>
#include <chrono>
#include <random>

const size_t fewDayBoundary = 50;
const size_t aroundWorkingDayPerMonth = 22;
const boost::gregorian::date_duration oneDay(1);
const size_t maxUpdateThreadCount = 32;

CInvestAgent::CInvestAgent(std::shared_ptr<IDataProvider> dataProvider, 
    std::shared_ptr<CInvestDb> db, bool updateDbSilently)
	: m_dataProvider(dataProvider), m_db(db), 
    m_updateDbThread(nullptr), m_startUpdateDbThread(false)
{
    m_dataProvider->GetDataId(m_dataId);

    if (updateDbSilently)
    {
        CreateUpdateDbThread();
    }
}

CInvestAgent::~CInvestAgent()
{
    if (m_startUpdateDbThread)
    {
        ReleaseUpdateDbThread();
    }
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
	std::vector<std::pair<boost::gregorian::date, boost::gregorian::date>> datePeriodInMonths;
    DevideDatePeriodIntoMonths(fromDate, toDate, datePeriodInMonths);

    std::vector<std::shared_ptr<std::vector<CDataItem>>> allData;
	std::vector<std::future<bool>> futures;
    for (auto& datePeriod : datePeriodInMonths)
    {
        allData.push_back(std::make_shared<std::vector<CDataItem>>());
        if (futures.size() >= maxUpdateThreadCount)
        {
            bool hasResource = false;
            while (!hasResource)
            {
                for (int i = (int)futures.size() - 1; i >= 0; --i)
                {
                    if (futures[i].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                    {
                        hasResource = true;
                        futures.erase(futures.begin() + i);
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }

        futures.push_back(std::async(std::launch::async, CInvestAgent::GetFewDataForMultiThread,
            this, dataId, datePeriod.first, datePeriod.second, allData.back()));
    }

    for (auto& future : futures)
	{
        while (future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
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

bool CInvestAgent::IsDataInDb(boost::gregorian::date date)
{
    size_t testCount = m_dataId.size();
    for (auto& categoryDataId : m_dataId)
    {
        if (!categoryDataId.second.empty())
        {
            std::default_random_engine generator;
            std::uniform_int_distribution<int> distribution(0, categoryDataId.second.size() - 1);
            CDataItem item;
            if (m_db->QueryData(categoryDataId.second[distribution(generator)], date, item))
            {
                testCount--;
            }
        }
    }

    return (testCount == 0);
}

void CInvestAgent::DevideDatePeriodIntoMonths(boost::gregorian::date fromDate, boost::gregorian::date toDate,
    std::vector<std::pair<boost::gregorian::date, boost::gregorian::date>>& datePeriodInMonths)
{
    datePeriodInMonths.clear();
    for (size_t i = 0, monthCount = 1 + (toDate.year() - fromDate.year()) * 12 + (toDate.month() - fromDate.month());
        i < monthCount; ++i)
    {
        boost::gregorian::date first(fromDate.year() + (i + fromDate.month() - 1) / 12,
            (i + fromDate.month() - 1) % 12 + 1,
            (i != 0) ? 1 : fromDate.day());
        boost::gregorian::date last = (first.year() != toDate.year() || first.month() != toDate.month())
            ? first.end_of_month() : toDate;

        datePeriodInMonths.push_back(std::pair<boost::gregorian::date, boost::gregorian::date>(first, last));
    }
}

void CInvestAgent::UpdateDb(CInvestAgent* agent, 
    boost::gregorian::date fromDate, boost::gregorian::date toDate)
{
    if (fromDate > toDate)
    {
        return;
    }

    if (fromDate.year() == toDate.year() && fromDate.month() == toDate.month())
    {

        return;
    }

    std::vector<std::pair<boost::gregorian::date, boost::gregorian::date>> datePeriodInMonths;
    agent->DevideDatePeriodIntoMonths(fromDate, toDate, datePeriodInMonths);

    std::vector<std::future<void>> futures;
    for (auto& datePeriod : datePeriodInMonths)
    {
        if (futures.size() >= maxUpdateThreadCount)
        {
            bool hasResource = false;
            while (!hasResource)
            {
                for (int i = (int)futures.size() - 1; i >= 0; --i)
                {
                    if (futures[i].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                    {
                        hasResource = true;
                        futures.erase(futures.begin() + i);
                    }
                }

                if (!agent->m_startUpdateDbThread)
                {
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }

        if (!agent->m_startUpdateDbThread)
        {
            break;
        }

        futures.push_back(std::async(std::launch::async, CInvestAgent::UpdateDb,
            agent, datePeriod.first, datePeriod.second));
    }

    for (auto& future : futures)
    {
        while (future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
}

void CInvestAgent::CreateUpdateDbThread()
{
    m_startUpdateDbThread = true;

    boost::gregorian::date firstDay = m_dataProvider->DataBeginsOn();
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    m_updateDbThread = std::make_shared<std::thread>(CInvestAgent::UpdateDb, this, firstDay, today);
}

void CInvestAgent::ReleaseUpdateDbThread()
{
    m_startUpdateDbThread = false;
    m_updateDbThread->join();
}