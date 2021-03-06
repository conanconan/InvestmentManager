#include "pch.h"
#include "DataProviderFactory.h"
#include "TwStockTSEDataProvider.h"
#include "TwStockOTCDataProvider.h"

std::shared_ptr<IDataProvider> CreateDataProvider(std::wstring id)
{
    if (id == L"TwStock_StockExchangeMarket")
        return std::make_shared<CTwStockTSEDataProvider>();
    if (id == L"TwStock_OverTheCounterMarket") 
        return std::make_shared<CTwStockOTCDataProvider>();

    return std::shared_ptr<IDataProvider>(nullptr);
}