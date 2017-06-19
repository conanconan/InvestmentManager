#pragma once
#include <memory>
#include <string>
#include "InvestDb.h"

std::shared_ptr<CInvestDb> CreateInvestDb(const std::wstring& id, const std::wstring& dbFolder);