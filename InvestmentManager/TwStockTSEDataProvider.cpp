#include "pch.h"
#include "TwStockTSEDataProvider.h"
#include "TwStockUtility.h"
#include "cpprest/http_client.h"
#include <sstream>
#include <chrono>
#include "boost/locale/encoding.hpp"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std::chrono;

CTwStockTSEDataProvider::CTwStockTSEDataProvider()
    : m_beginDate(1992, 1, 4)
{}

CTwStockTSEDataProvider::~CTwStockTSEDataProvider()
{}

boost::gregorian::date CTwStockTSEDataProvider::DataBeginsOn() const
{
    return m_beginDate;
}

bool CTwStockTSEDataProvider::GetDataId(std::map<std::wstring, std::vector<std::wstring>>& dataId) const
{
    return GetTwStockDataId(L"tse", dataId);
}

bool CTwStockTSEDataProvider::GetRealTimeData(std::wstring dataId, CDataItem& data) const
{
    return GetTwStockRealTimeData(L"tse", dataId, data);
}

bool ParseTSEJsonData(const json::value& jsonData, const boost::gregorian::date date,
    std::vector<CDataItem>& data)
{
    return false;
}

bool CTwStockTSEDataProvider::GetData(std::wstring dataId,
	boost::gregorian::date date, CDataItem& data) const
{
    if (date < m_beginDate)
    {
        return false;
    }

    http_client httpClient(U("http://www.twse.com.tw"));
    uri_builder uriBuilder(U("/exchangeReport/MI_INDEX"));
    uriBuilder.append_query(U("date"), boost::gregorian::to_iso_string(date).c_str())
        .append_query(U("response"), "json")
        .append_query(U("type"), "ALL")
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
        auto iter = dataInJson.as_object().find(L"data5");
        if (iter != dataInJson.as_object().cend())
        {
            return false;//ParseTSEJsonData();
        }
    }

    return false;
    //
    std::vector<CDataItem> oneMonthData;
    if (GetOneMonthData(dataId, date, oneMonthData))
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

bool CTwStockTSEDataProvider::GetOneMonthData(std::wstring dataId,
    boost::gregorian::date date, std::vector<CDataItem>& data) const
{
    if (boost::gregorian::date(date.year(), date.month(), 1)
        < boost::gregorian::date(m_beginDate.year(), m_beginDate.month(), 1))
    {
        return false;
    }

    // new format
    // http://www.twse.com.tw/exchangeReport/MI_INDEX?date=20170619&response=json&type=ALL&_={unix timestamp}

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