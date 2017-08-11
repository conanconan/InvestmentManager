#include "pch.h"
#include "InvestDb.h"
#include "Utility.h"

const std::wstring dayTableName(L"DailyInfo");

CInvestDb::CInvestDb(const std::experimental::filesystem::path& dbFilePath,
    const std::map<std::wstring, std::vector<std::wstring>>& dbTable)
    : CDbWrapper(dbFilePath, dbTable), m_filePath(dbFilePath), m_table(dbTable)
{
}

CInvestDb::~CInvestDb()
{
}

bool CInvestDb::QueryData(std::wstring dataId, boost::gregorian::date date, CDataItem& data)
{
    CAutoLock lock(m_dbMutex);

    std::wstringstream condition;
    condition << L"id = '" << dataId.c_str() << "' and "
        << "date = '" << boost::gregorian::to_iso_wstring(date).c_str() << "'";

    std::vector<std::vector<std::wstring>> queryData;
    if (CDbWrapper::QueryData(dayTableName.c_str(), { L"*" }, condition.str(), queryData) &&
        queryData.size() == 1)
    {
        auto& item = queryData[0];
        data.date = item[0];
        data.id = item[1];
        data.name = item[2];
        data.totalVolume = item[3];
        data.openingPrice = item[4];
        data.closingPrice = item[5];
        data.dayHighPrice = item[6];
        data.dayLowPrice = item[7];

        return true;
    }

    return false;
}

bool CInvestDb::InsertData(CDataItem& data)
{
    CAutoLock lock(m_dbMutex);

    return CDbWrapper::InsertData(dayTableName.c_str(),
        { data.date, data.id, data.name, data.totalVolume,
            data.openingPrice, data.closingPrice, data.dayHighPrice, data.dayLowPrice });
}