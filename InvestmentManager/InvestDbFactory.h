#pragma once
#include <memory>
#include <string>
#include "InvestDb.h"

std::wstring GetDbFileName(const std::wstring& id);
bool GetDbTables(const std::wstring& id, std::map<std::wstring, std::vector<std::wstring>>& table);
std::shared_ptr<CInvestDb> CreateInvestDb(const std::wstring& id, const std::wstring& dbFolder);