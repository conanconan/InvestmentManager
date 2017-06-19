#pragma once
#include "IDataProvider.h"
#include <string>
#include <memory>

std::shared_ptr<IDataProvider> CreateDataProvider(std::wstring id);