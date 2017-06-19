#include "pch.h"
#include "InvestDbFactory.h"
#include <vector>

using namespace std::experimental;

std::shared_ptr<CInvestDb> CreateInvestDb(const std::wstring& id, const std::wstring& dbFolder)
{
    filesystem::path dbFilePath = dbFolder;
    std::map<std::wstring, std::vector<std::wstring>> dbTable;
    if (id == L"TwStock_StockExchangeMarket")
    {
        dbFilePath = dbFilePath / L"tw_tse.db";
        dbTable = { { L"DailyInfo", 
        { L"date text", L"id text", L"name text", L"volume text", 
            L"openingPrice text", L"closingPrice text", L"dayHighPrice text", L"dayLowPrice text", 
            L"primary key (date, id)" } } };
    }
    else if (id == L"TwStock_OverTheCounterMarket")
    {
        dbFilePath = dbFilePath / L"tw_otc.db";
        dbTable = { { L"DailyInfo",
        { L"date text", L"id text", L"name text", L"volume text",
            L"openingPrice text", L"closingPrice text", L"dayHighPrice text", L"dayLowPrice text",
            L"primary key (date, id)" } } };
    }
    else
    {
        return std::shared_ptr<CInvestDb>(nullptr);
    }
    
    return std::shared_ptr<CInvestDb>(new CInvestDb(dbFilePath, dbTable));
}