#pragma once
#include "DbWrapper.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "DataItem.h"
#include <mutex>

class CInvestDb : private CDbWrapper
{
public:
    CInvestDb(const std::experimental::filesystem::path& dbFilePath,
        const std::map<std::wstring, std::vector<std::wstring>>& dbTable);
    virtual ~CInvestDb();

    bool QueryData(std::wstring dataId, boost::gregorian::date date, CDataItem& data);
    bool InsertData(CDataItem& data);

private:
    std::experimental::filesystem::path m_filePath;
    std::map<std::wstring, std::vector<std::wstring>> m_table;
    std::mutex m_dbMutex;
};

