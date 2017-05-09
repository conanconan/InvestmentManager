#include "pch.h"
#include "TwStockDataProvider.h"
#include "cpprest/http_client.h"
#include <sstream>
#include <chrono>
#include "boost/locale/encoding.hpp"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std::chrono;

CTwStockDataProvider::CTwStockDataProvider(std::wstring category)
	: m_category(category)
{
}

CTwStockDataProvider::~CTwStockDataProvider()
{
}

std::wstring RetrieveCategory(const std::wstring& content, std::wstring::size_type start,
	std::wstring::size_type end)
{
	std::wstring::size_type startCategory = content.find_first_not_of(L' ', start);
	std::wstring::size_type endCategory = content.find_last_not_of(L' ', end - 1);
	if (startCategory == std::wstring::npos || endCategory == std::wstring::npos)
	{
		return L"";
	}

	return content.substr(startCategory, endCategory - startCategory + 1);
}

void RetrieveDataIdInCategory(const std::wstring& content,
	std::wstring::size_type startPos, std::wstring::size_type endPos,
	std::vector<std::wstring>& dataId)
{
	std::wstring::size_type startDataId = startPos;
	std::wstring::size_type endDataId = std::wstring::npos;
	const std::wstring::size_type dataIdSymbolPrefixSize =
		sizeof(L"<tr><td bgcolor=#FAFAD2>") / sizeof(wchar_t) - 1;
	while (startDataId != std::wstring::npos)
	{
		startDataId = content.find(L"<tr><td bgcolor=#FAFAD2>", startDataId);
		if (startDataId == std::wstring::npos)
		{
			break;
		}
		startDataId += dataIdSymbolPrefixSize;
		if (startDataId >= endPos)
		{
			break;
		}
		endDataId = content.find(static_cast<wchar_t>(12288), startDataId);
		if (endDataId == std::wstring::npos)
		{
			break;
		}

		dataId.push_back(content.substr(startDataId, endDataId - startDataId));
	}
}

bool RetrieveDataId(const std::wstring& content,
	std::map<std::wstring, std::vector<std::wstring>>& dataId)
{
	dataId.clear();

	std::wstring::size_type startCategory = 0;
	std::wstring::size_type endCategory = 0;
	std::wstring::size_type nextStart = 0;
	const std::wstring::size_type categorySymbolSize = sizeof(L"<B>") / sizeof(wchar_t) - 1;
	while (startCategory != std::wstring::npos)
	{
		startCategory = content.find(L"<B>", nextStart);
		if (startCategory == std::wstring::npos)
		{
			break;
		}
		startCategory += categorySymbolSize;
		endCategory = content.find(L"<B>", startCategory);
		if (endCategory == std::wstring::npos)
		{
			break;
		}
		nextStart = endCategory + categorySymbolSize;

		std::wstring category = RetrieveCategory(content, startCategory, endCategory);
		if (category.empty())
		{
			break;
		}
	
		RetrieveDataIdInCategory(content, nextStart, content.find(L"<B>", nextStart), 
			dataId[category]);
	}

	return !dataId.empty();
}

bool CTwStockDataProvider::GetDataId(std::map<std::wstring, std::vector<std::wstring>>& dataId) const
{
	http_client httpClient(U("http://isin.twse.com.tw"));
	std::wstringstream request;
	request << L"/isin/C_public.jsp?strMode=" << ((m_category == L"tse") ? 2 : 4);
	http_response httpResponse = httpClient.request(methods::GET, request.str()).get();
	if (httpResponse.status_code() == status_codes::OK)
	{
		std::vector<unsigned char> vRawContent = httpResponse.extract_vector().get();
		std::string sRawContent(vRawContent.begin(), vRawContent.end());
		std::wstring UTFContent = boost::locale::conv::to_utf<wchar_t> (sRawContent, "BIG5");
		RetrieveDataId(UTFContent, dataId);
		return true;
	}

	return false;
}

bool ParseJsonToDataItem(const json::value jsonItem, CDataItem& item)
{
	item.date = jsonItem.as_object().find(L"d")->second.as_string();
	
	item.category = jsonItem.as_object().find(L"ex")->second.as_string();
	item.id = jsonItem.as_object().find(L"c")->second.as_string();
	item.fullName = jsonItem.as_object().find(L"nf")->second.as_string();
	item.abbrevName = jsonItem.as_object().find(L"n")->second.as_string();
	
	item.closingPrice = jsonItem.as_object().find(L"z")->second.as_string();
	item.totalVolume = jsonItem.as_object().find(L"v")->second.as_string();
	item.totalVolume.erase(
		std::remove(item.totalVolume.begin(), item.totalVolume.end(), L','), 
		item.totalVolume.end());
	item.openingPrice = jsonItem.as_object().find(L"o")->second.as_string();
	item.dayHighPrice = jsonItem.as_object().find(L"h")->second.as_string();
	item.dayLowPrice = jsonItem.as_object().find(L"l")->second.as_string();

	return true;
}

bool ParseJsonToDataItem(const json::value& jsonData, std::vector<CDataItem>& data)
{
	if (jsonData.is_array() && jsonData.size() != 0)
	{
		data.clear();
		for (size_t i = 0; i < jsonData.size(); ++i)
		{
			CDataItem item;
			ParseJsonToDataItem(jsonData.at(i), item);
			data.push_back(item);
		}

		return true;
	}

	return false;
}

bool CTwStockDataProvider::GetRealTimeData(std::vector<std::wstring> dataId,
	std::vector<CDataItem>& data) const
{
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	http_client httpClient(U("http://mis.twse.com.tw"));
	
	std::wstringstream ex_chValue;
	std::wstring formatedDate = boost::gregorian::to_iso_wstring(today);
	for (auto id : dataId)
	{
		if (ex_chValue.tellp() != std::streampos(0))
		{
			ex_chValue << L"|";
		}

		ex_chValue << m_category.c_str() << L"_" << id.c_str() << L".tw";
	}

	http_response httpResponse = httpClient.request(methods::GET, U("/stock/fibest.jsp?lang=zh-tw")).get();
	utility::string_t cookies = httpResponse.headers()[L"Set-Cookie"];
	std::wstring sessionId = cookies.substr(0, cookies.find(';'));

	uri_builder uriBuilder(U("/stock/api/getStockInfo.jsp"));
	uriBuilder.append_query(U("json"), "1")
		.append_query(U("delay"), "0")
		.append_query(U("d"), formatedDate.c_str())
		.append_query(U("_"), duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count())
		.append_query(U("ex_ch"), ex_chValue.str().c_str());

	http_request request;
	request.set_method(methods::GET);
	request.headers().add(L"Cookie", sessionId.c_str());
	request.set_request_uri(uriBuilder.to_uri());

	httpResponse = httpClient.request(request).get();
	if (httpResponse.status_code() == status_codes::OK)
	{
		utility::string_t content = httpResponse.extract_string().get()
			.erase(0, 16);//erase firt 8 "\r\n";
		json::value dataInJson = json::value::parse(content);
		auto iter = dataInJson.as_object().find(L"msgArray");
		if (iter != dataInJson.as_object().cend())
		{
			return ParseJsonToDataItem(iter->second, data);
		}
	}

	return false;
}

bool CTwStockDataProvider::GetRealTimeData(std::wstring dataId, CDataItem& data) const
{
	std::vector<std::wstring> dataIdList;
	dataIdList.push_back(dataId);
	std::vector<CDataItem> dataList;

	if (GetRealTimeData(dataIdList, dataList))
	{
		data = dataList[0];
		return true;
	}

	return false;
}

bool CTwStockDataProvider::GetData(std::wstring dataId,
	boost::gregorian::date date, CDataItem& data) const
{
	if (m_category == L"tse")
	{
		return GetTSEData(dataId, date, data);
	}
	else if (m_category == L"otc")
	{
		return GetOTCData(dataId, date, data);
	}

	return false;
}

bool CTwStockDataProvider::GetOneMonthData(std::wstring dataId,
	boost::gregorian::date date, std::vector<CDataItem>& data) const
{
	if (m_category == L"tse")
	{
		return GetTSEOneMonthData(dataId, date, data);
	}
	else if (m_category == L"otc")
	{
		return GetOTCOneMonthData(dataId, date, data);
	}

	return false;
}

std::wstring NextFieldInTheRow(const std::wstring& table, std::wstring::size_type& currentPos)
{
	std::wstring::size_type startPos = table.find(L"<td", currentPos);
	startPos = table.find(L">", startPos);
	if (startPos != std::wstring::npos)
	{
		++startPos;
		std::wstring::size_type endPos = table.find(L"</td>", startPos);
		if (endPos != std::wstring::npos)
		{
			currentPos = endPos + std::wstring(L"</td>").size();
			return table.substr(startPos, endPos - startPos);
		}
	}

	return L"";
}

bool CTwStockDataProvider::GetTSEData(std::wstring dataId,
	boost::gregorian::date date, CDataItem& data) const
{
	std::vector<CDataItem> oneMonthData;
	if (GetTSEOneMonthData(dataId, date, oneMonthData))
	{
		std::wstring sDate = boost::gregorian::to_iso_wstring(date);
		for (auto item : oneMonthData)
		{
			if (item.date == sDate)
			{
				data = item;
				return true;
			}
		}
	}

	return false;
}

bool RetrieveTSEDataInTable(const std::wstring& table, const std::wstring dataId, 
	const std::wstring abbrevName, const boost::gregorian::date date, 
	std::vector<CDataItem>& data)
{
	data.clear();
	std::wstringstream itemPrefix;
	itemPrefix << date.year() - 1911 << L"/"
		<< std::setfill(L'0') << std::setw(2) << date.month().as_number() << L"/";
	std::wstring::size_type dateContentLength = itemPrefix.str().size() + 2;

	std::wstring::size_type startPos = table.find(itemPrefix.str());
	while (startPos != std::wstring::npos)
	{
		CDataItem item;
		item.category = L"tse";
		item.id = dataId;
		item.abbrevName = abbrevName;

		std::wstring fieldContent = table.substr(startPos, dateContentLength);
		fieldContent.erase(fieldContent.find_last_of(L'/'), 1);
		fieldContent.erase(fieldContent.find_last_of(L'/'), 1);
		item.date = std::to_wstring(std::stoi(fieldContent.substr(0, fieldContent.size() - 4)) + 1911)
			+ fieldContent.substr(fieldContent.size() - 4, 4);
		startPos += dateContentLength + std::wstring(L"</td>").size();
	
		fieldContent = NextFieldInTheRow(table, startPos);
		fieldContent.erase(
			std::remove(fieldContent.begin(), fieldContent.end(), L','), fieldContent.end());
		item.totalVolume = std::to_wstring(std::stoll(fieldContent) / 1000);

		fieldContent = NextFieldInTheRow(table, startPos);// total price
		
		item.openingPrice = NextFieldInTheRow(table, startPos);
		item.dayHighPrice = NextFieldInTheRow(table, startPos);
		item.dayLowPrice = NextFieldInTheRow(table, startPos);
		item.closingPrice = NextFieldInTheRow(table, startPos);

		data.push_back(item);

		startPos = table.find(itemPrefix.str(), startPos);
	}

	return !data.empty();
}

std::wstring RetrieveAbbrevNameInTable(const std::wstring& table, 
	const boost::gregorian::date date, std::wstring dataId)
{
	std::wstringstream prefix;
	prefix << date.year() - 1911 << L"¦~"
		<< std::setfill(L'0') << std::setw(2) << date.month().as_number() << L"¤ë"
		<< L"&nbsp;" << dataId.c_str() << L"&nbsp;";
	
	std::wstring::size_type startPos = table.find(prefix.str());
	if (startPos != std::wstring::npos)
	{
		startPos += prefix.str().size();
		std::wstring::size_type endPos = table.find(L' ', startPos);
		return table.substr(startPos, endPos - startPos);
	}

	return L"";
}

bool CTwStockDataProvider::GetTSEOneMonthData(std::wstring dataId,
		boost::gregorian::date date, std::vector<CDataItem>& data) const
{
	http_client httpClient(U("http://www.twse.com.tw/ch/trading/exchange/STOCK_DAY/STOCK_DAYMAIN.php"));

	std::wstringstream resuestBody;
	resuestBody << L"download=" 
		<< L"&query_year=" << date.year() 
		<< L"&query_month=" << date.month().as_number()
		<< L"&CO_ID=" << dataId 
		<< L"&query-button=" << web::uri::encode_data_string(L"¬d¸ß");

	http_request request;
	request.set_method(methods::POST);
	request.headers().add(L"Content-Type", L"application/x-www-form-urlencoded");
	request.headers().add(L"Content-Length", resuestBody.str().size());
	request.set_body(resuestBody.str());

	http_response httpResponse = httpClient.request(request).get();
	if (httpResponse.status_code() == status_codes::OK)
	{
		utility::string_t content = httpResponse.extract_string().get();
		std::wstring::size_type tableStart = content.find(L"<table");
		if (tableStart != std::wstring::npos)
		{
			std::wstring::size_type tableEnd = content.find(L"/table>");
			if (tableEnd != std::wstring::npos)
			{
				tableEnd += std::wstring(L"/table>").size();
			}
			std::wstring table = content.substr(tableStart, tableEnd - tableStart);
			std::wstring abbrevName = RetrieveAbbrevNameInTable(table, date, dataId);
			
			return RetrieveTSEDataInTable(table, dataId, abbrevName, date, data);
		}
	}

	return false;
}

bool ParseOTCJsonToDataItem(const json::value& jsonData, const boost::gregorian::date date, 
	std::vector<CDataItem>& data)
{
	data.clear();
	std::wstring sDate = boost::gregorian::to_iso_wstring(date);

	auto& jsonItemArray = jsonData.as_array();
	for (auto& jsonItem : jsonItemArray)
	{
		CDataItem item;

		item.id = jsonItem.as_array().at(0).as_string();
		item.abbrevName = jsonItem.as_array().at(1).as_string();
		item.category = L"otc";
		item.date = sDate;
		item.openingPrice = jsonItem.as_array().at(4).as_string();
		item.closingPrice = jsonItem.as_array().at(2).as_string();
		item.dayHighPrice = jsonItem.as_array().at(5).as_string();
		item.dayLowPrice = jsonItem.as_array().at(6).as_string();
		utility::string_t sVolume = jsonItem.as_array().at(8).as_string();
		sVolume.erase(std::remove(sVolume.begin(), sVolume.end(), L','), sVolume.end());
		item.totalVolume = std::to_wstring(std::stoll(sVolume) / 1000);
		
		data.push_back(item);
	}

	return !data.empty();
}

bool GetOTCAllData(std::wstring dataId,
	boost::gregorian::date date, std::vector<CDataItem>& data)
{
	http_client httpClient(U("http://www.tpex.org.tw"));

	std::wstringstream sDate;
	sDate << date.year() - 1911 << L"/"
		<< std::setfill(L'0') << std::setw(2) << date.month().as_number() << L"/"
		<< std::setfill(L'0') << std::setw(2) << date.day().as_number();
	uri_builder uriBuilder(U("/web/stock/aftertrading/daily_close_quotes/stk_quote_result.php"));
	uriBuilder.append_query(U("l"), "zh-tw")
		.append_query(U("d"), sDate.str().c_str())
		.append_query(U("_"),
			duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());

	http_request request;
	request.set_method(methods::GET);
	request.set_request_uri(uriBuilder.to_uri());

	http_response httpResponse = httpClient.request(request).get();
	if (httpResponse.status_code() == status_codes::OK)
	{
		utility::string_t content = httpResponse.extract_string().get();
		json::value dataInJson = json::value::parse(content);

		auto iter = dataInJson.as_object().find(L"reportDate");
		if (iter != dataInJson.as_object().cend() &&
			iter->second.as_string() == sDate.str())
		{
			iter = dataInJson.as_object().find(L"aaData");
			if (iter != dataInJson.as_object().cend())
			{
				return ParseOTCJsonToDataItem(iter->second, date, data);
			}
		}
	}

	return false;
}

bool CTwStockDataProvider::GetOTCData(std::wstring dataId,
	boost::gregorian::date date, CDataItem& data) const
{
	std::vector<CDataItem> allData;
	if (GetOTCAllData(dataId, date, allData))
	{
		for (auto& item : allData)
		{
			if (item.id == dataId)
			{
				data = item;
				return true;
			}
		}
	}

	return false;
}

bool CTwStockDataProvider::GetOTCOneMonthData(std::wstring dataId,
	boost::gregorian::date date, std::vector<CDataItem>& data) const
{
	data.clear();
	boost::gregorian::date dateInMonth(date.year(), date.month(), 1);
	while (dateInMonth <= date.end_of_month())
	{
		CDataItem item;
		if (GetOTCData(dataId, dateInMonth, item))
		{
			data.push_back(item);
		}

		dateInMonth += boost::gregorian::date_duration(1);
	}

	return !data.empty();
}