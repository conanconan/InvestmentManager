#pragma once
#include <string>

class CDataItem
{
public:
	std::wstring id;
	std::wstring category;
	std::wstring name;
	std::wstring closingPrice;
	std::wstring totalVolume;
	std::wstring openingPrice;
	std::wstring dayHighPrice;
	std::wstring dayLowPrice;
	std::wstring date;
};