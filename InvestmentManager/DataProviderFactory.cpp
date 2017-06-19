#include "pch.h"
#include "DataProviderFactory.h"
#include "TwStockTSEDataProvider.h"
#include "TwStockOTCDataProvider.h"

std::shared_ptr<IDataProvider> CreateDataProvider(std::wstring id)
{
    IDataProvider* pInstance = nullptr;
    if (id == L"TwStock_StockExchangeMarket")
        return std::shared_ptr<IDataProvider>(new CTwStockTSEDataProvider());
    if (id == L"TwStock_OverTheCounterMarket") 
        return std::shared_ptr<IDataProvider>(new CTwStockOTCDataProvider());

    return std::shared_ptr<IDataProvider>(nullptr);
}