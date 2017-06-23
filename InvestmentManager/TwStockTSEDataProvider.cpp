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
    data.clear();
    std::wstring sDate = boost::gregorian::to_iso_wstring(date);

    auto& jsonItemArray = jsonData.as_array();
    for (auto& jsonItem : jsonItemArray)
    {
        CDataItem item;

        item.id = jsonItem.as_array().at(0).as_string();
        item.name = jsonItem.as_array().at(1).as_string();
        item.category = L"tse";
        item.date = sDate;
        item.openingPrice = jsonItem.as_array().at(5).as_string();
        item.closingPrice = jsonItem.as_array().at(8).as_string();
        item.dayHighPrice = jsonItem.as_array().at(6).as_string();
        item.dayLowPrice = jsonItem.as_array().at(7).as_string();
        utility::string_t sVolume = jsonItem.as_array().at(2).as_string();
        sVolume.erase(std::remove(sVolume.begin(), sVolume.end(), L','), sVolume.end());
        item.totalVolume = std::to_wstring(std::stoll(sVolume));

        data.push_back(item);
    }

    return !data.empty();
}

bool CTwStockTSEDataProvider::GetAllData(boost::gregorian::date date, std::vector<CDataItem>& data) const
{
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
            return ParseTSEJsonData(iter->second, date, data);
        }
    }

    return false;
}

bool CTwStockTSEDataProvider::GetData(std::wstring dataId,
	boost::gregorian::date date, CDataItem& data) const
{
    if (date < m_beginDate)
    {
        return false;
    }

    std::vector<CDataItem> allData;
    if (GetAllData(date, allData))
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