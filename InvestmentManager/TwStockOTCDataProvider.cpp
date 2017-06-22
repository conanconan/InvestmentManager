#include "pch.h"
#include "TwStockOTCDataProvider.h"
#include "TwStockUtility.h"
#include "cpprest/http_client.h"
#include "boost/locale/encoding.hpp"


using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std::chrono;

CTwStockOTCDataProvider::CTwStockOTCDataProvider()
    : m_beginDate(2007, 4, 24)
{}

CTwStockOTCDataProvider::~CTwStockOTCDataProvider()
{}

boost::gregorian::date CTwStockOTCDataProvider::DataBeginsOn() const
{
    return m_beginDate;
}

bool CTwStockOTCDataProvider::GetDataId(std::map<std::wstring, std::vector<std::wstring>>& dataId) const
{
    return GetTwStockDataId(L"otc", dataId);
}

bool CTwStockOTCDataProvider::GetRealTimeData(std::wstring dataId, CDataItem & data) const
{
    return GetTwStockRealTimeData(L"otc", dataId, data);
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
        item.totalVolume = std::to_wstring(std::stoll(sVolume));

        data.push_back(item);
    }

    return !data.empty();
}

bool GetOTCAllData(boost::gregorian::date date, std::vector<CDataItem>& data)
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

bool CTwStockOTCDataProvider::GetData(std::wstring dataId, boost::gregorian::date date, CDataItem & data) const
{
    if (date < m_beginDate)
    {
        return false;
    }

    std::vector<CDataItem> allData;
    if (GetOTCAllData(date, allData))
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

bool CTwStockOTCDataProvider::GetOneMonthData(std::wstring dataId, boost::gregorian::date date, std::vector<CDataItem>& data) const
{
    if (boost::gregorian::date(date.year(), date.month(), 1)
        < boost::gregorian::date(m_beginDate.year(), m_beginDate.month(), 1))
    {
        return false;
    }

    data.clear();
    boost::gregorian::date dateInMonth(date.year(), date.month(), 1);
    while (dateInMonth <= date.end_of_month())
    {
        CDataItem item;
        if (GetData(dataId, dateInMonth, item))
        {
            data.push_back(item);
        }

        dateInMonth += boost::gregorian::date_duration(1);
    }

    return !data.empty();
}
