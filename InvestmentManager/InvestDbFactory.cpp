#include "pch.h"
#include "InvestDbFactory.h"
#include <vector>

using namespace std::experimental;

std::wstring GetDbFileName(const std::wstring& id)
{
    if (id == L"TwStock_StockExchangeMarket")
    {
        return L"tw_tse.db";
    }
    else if (id == L"TwStock_OverTheCounterMarket")
    {
        return L"tw_otc.db";
    }

    return L"unknown_name.db";
}

bool GetDbTables(const std::wstring& id, std::map<std::wstring, std::vector<std::wstring>>& table)
{
    table.clear();

    if (id == L"TwStock_StockExchangeMarket" || 
        id == L"TwStock_OverTheCounterMarket")
    {
        table[L"DailyInfo"] = {L"date text", L"id text", L"name text", L"volume text",
                               L"openingPrice text", L"closingPrice text", 
                               L"dayHighPrice text", L"dayLowPrice text",
                               L"primary key (date, id)"};

        return true;
    }

    return false;
}

std::shared_ptr<CInvestDb> CreateInvestDb(const std::wstring& id, const std::wstring& dbFolder)
{
    filesystem::path dbFilePath = dbFolder;
    std::map<std::wstring, std::vector<std::wstring>> dbTable;
    if (GetDbTables(id, dbTable))
    {
        dbFilePath = dbFilePath / GetDbFileName(id);
        return std::shared_ptr<CInvestDb>(new CInvestDb(dbFilePath, dbTable));
    }

    return std::shared_ptr<CInvestDb>(nullptr);
}