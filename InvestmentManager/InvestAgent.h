#pragma once
#include <string>
#include <memory>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "IDataProvider.h"
#include "InvestDb.h"

class CInvestAgent
{
public:
	CInvestAgent(std::shared_ptr<IDataProvider> dataProvider, 
        std::shared_ptr<CInvestDb> db);
	virtual ~CInvestAgent();

	bool GetData(std::vector<std::wstring> dataId, boost::gregorian::date date, 
		std::vector<CDataItem>& data);
	bool GetData(std::wstring dataId, boost::gregorian::date date, CDataItem& data);
	bool GetData(std::wstring dataId, boost::gregorian::date fromDate, size_t dayCount, 
		std::vector<CDataItem>& data);
	bool GetData(std::wstring dataId, boost::gregorian::date fromDate, 
		boost::gregorian::date toDate, std::vector<CDataItem>& data);
	bool GetSimpleData(std::vector<std::wstring> dataId, boost::gregorian::date date, 
		std::vector<std::wstring>& data);
	bool GetSimpleData(std::wstring dataId, boost::gregorian::date date, std::wstring& data);

private:
	std::shared_ptr<IDataProvider> m_dataProvider;
    std::shared_ptr<CInvestDb> m_db;

	bool GetFewData(std::wstring dataId, boost::gregorian::date fromDate, size_t dayCount,
		std::vector<CDataItem>& data);
	bool GetFewData(std::wstring dataId, boost::gregorian::date fromDate,
		boost::gregorian::date toDate, std::vector<CDataItem>& data);
	static bool GetFewDataForMultiThread(CInvestAgent* agent, std::wstring dataId, 
		boost::gregorian::date fromDate, boost::gregorian::date toDate, 
		std::shared_ptr<std::vector<CDataItem>> data);
};

