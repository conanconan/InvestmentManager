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
	std::wstring::size_type endCategory = content.find_last_not_of(L' ', end);
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
		endDataId = content.find_first_of(static_cast<wchar_t>(12288), startDataId);
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
	const std::wstring::size_type categorySymbolSize = sizeof(L"<B>") / sizeof(wchar_t) - 1;
	std::wstring::size_type startCategory = content.find(L"<B>");
	if (startCategory == std::wstring::npos)
	{
		return false;
	}
	startCategory += categorySymbolSize;
	std::wstring::size_type endCategory = content.find(L"<B>", startCategory);
	while (startCategory != std::wstring::npos && endCategory != std::wstring::npos)
	{
		std::wstring category = RetrieveCategory(content, startCategory, endCategory - 1);
		if (category.empty())
		{
			return false;
		}
	
		std::wstring::size_type nextCategory = content.find(L"<B>", endCategory + categorySymbolSize);
		RetrieveDataIdInCategory(content, endCategory + categorySymbolSize,
			nextCategory, dataId[category]);

		if (nextCategory == std::wstring::npos)
		{
			break;
		}
		startCategory = nextCategory + categorySymbolSize;
		if (startCategory == std::wstring::npos)
		{
			break;
		}
		endCategory = content.find(L"<B>", startCategory);
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

bool ParseJsonToDataItem(const json::value jsonItem, std::shared_ptr<CDataItem>& item)
{
	item->date = jsonItem.as_object().find(L"d")->second.as_string();
	
	item->category = jsonItem.as_object().find(L"ex")->second.as_string();
	item->id = jsonItem.as_object().find(L"c")->second.as_string();
	item->fullName = jsonItem.as_object().find(L"nf")->second.as_string();
	item->abbrevName = jsonItem.as_object().find(L"n")->second.as_string();
	
	item->closingPrice = jsonItem.as_object().find(L"z")->second.as_string();
	item->totalVolume = jsonItem.as_object().find(L"v")->second.as_string();
	item->openingPrice = jsonItem.as_object().find(L"o")->second.as_string();
	item->dayHighPrice = jsonItem.as_object().find(L"h")->second.as_string();
	item->dayLowPrice = jsonItem.as_object().find(L"l")->second.as_string();

	return true;
}

bool ParseJsonToDataItem(const json::value& jsonData, std::vector<std::shared_ptr<CDataItem>>& data)
{
	if (jsonData.is_array() && jsonData.size() != 0)
	{
		data.clear();
		for (size_t i = 0; i < jsonData.size(); ++i)
		{
			data.push_back(std::make_shared<CDataItem>());
			ParseJsonToDataItem(jsonData.at(i), data.back());
		}

		return true;
	}

	return false;
}

bool CTwStockDataProvider::GetData(std::vector<std::wstring> dataId, 
	boost::gregorian::date date, std::vector<std::shared_ptr<CDataItem>>& data) const
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

bool CTwStockDataProvider::GetData(std::wstring dataId,
	boost::gregorian::date date, std::shared_ptr<CDataItem>& data) const
{
	std::vector<std::wstring> dataIdList;
	dataIdList.push_back(dataId);
	std::vector<std::shared_ptr<CDataItem>> dataList;

	if (GetData(dataIdList, date, dataList))
	{
		data = dataList[0];
		return true;
	}

	return false;
}
