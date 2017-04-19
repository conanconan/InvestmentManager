#include "pch.h"
#include "TwStockDataProvider.h"
#include "cpprest/http_client.h"
#include <sstream>
#include <chrono>

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

bool ParseJsonToDataItem(const json::value jsonItem, CDataItem& item)
{
	item.date = jsonItem.as_object().find(L"d")->second.as_string();
	
	item.category = jsonItem.as_object().find(L"ex")->second.as_string();
	item.id = jsonItem.as_object().find(L"c")->second.as_string();
	item.fullName = jsonItem.as_object().find(L"nf")->second.as_string();
	item.abbrevName = jsonItem.as_object().find(L"n")->second.as_string();
	
	item.closingPrice = jsonItem.as_object().find(L"z")->second.as_string();
	item.totalVolume = jsonItem.as_object().find(L"v")->second.as_string();
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
			data.push_back(CDataItem());
			ParseJsonToDataItem(jsonData.at(i), data.back());
		}

		return true;
	}

	return false;
}

bool CTwStockDataProvider::GetData(std::vector<std::wstring> dataId, 
	boost::gregorian::date date, std::vector<CDataItem>& data) const
{
	http_client httpClient(U("http://mis.twse.com.tw"));
	
	std::wstringstream ex_chValue;
	std::wstring formatedDate = boost::gregorian::to_iso_wstring(date);
	for (auto id : dataId)
	{
		if (ex_chValue.tellp() != std::streampos(0))
		{
			ex_chValue << L"|";
		}

		ex_chValue << m_category.c_str() << L"_" << id.c_str() << L".tw";
	}

	http_response httpResponse = httpClient.request(methods::GET, U("/stock/index.jsp?lang=zh-tw")).get();
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

bool CTwStockDataProvider::GetData(std::wstring dataId,
	boost::gregorian::date date, CDataItem& data) const
{
	std::vector<std::wstring> dataIdList;
	dataIdList.push_back(dataId);
	std::vector<CDataItem> dataList;

	if (GetData(dataIdList, date, dataList))
	{
		data = dataList[0];
		return true;
	}

	return false;
}
