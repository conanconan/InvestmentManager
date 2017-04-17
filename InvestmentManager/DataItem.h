#pragma once
#include <string>

class CDataItem
{
public:
	std::string id;
	std::string category;
	std::string fullName;
	std::string abbrevName;
	float closingPrice;
	float totalVolume;
	float openingPrice;
	float dayHighPrice;
	float dayLowPrice;
};