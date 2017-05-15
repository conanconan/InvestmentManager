#pragma once
#include "DataItem.h"
#include <string>
#include <vector>
#include <map>

bool GetTwStockDataId(std::wstring type, std::map<std::wstring, std::vector<std::wstring>>& dataId);

bool GetTwStockRealTimeData(std::wstring type, std::vector<std::wstring> dataId,
    std::vector<CDataItem>& data);
bool GetTwStockRealTimeData(std::wstring type, std::wstring dataId, CDataItem& data);