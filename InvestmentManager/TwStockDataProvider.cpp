#include "pch.h"
#include "TwStockDataProvider.h"
#include "cpprest/http_client.h"
#include <sstream>
#include <chrono>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std::chrono;

CTwStockDataProvider::CTwStockDataProvider(std::string category)
	: m_category(category)
{
}

CTwStockDataProvider::~CTwStockDataProvider()
{
}

bool CTwStockDataProvider::GetData(std::vector<std::string> dataId, 
	boost::gregorian::date date, void* pAdvData)
{
	http_client httpClient(U("http://mis.twse.com.tw"));
	
	std::ostringstream ex_chValue;
	std::string formatedDate = boost::gregorian::to_iso_string(date);
	for (auto id : dataId)
	{
		if (ex_chValue.tellp() != std::streampos(0))
		{
			ex_chValue << "|";
		}

		ex_chValue << m_category.c_str() << "_" << id.c_str()
				   << ".tw_" << formatedDate.c_str();
	}

	uri_builder uriBuilder(U("/stock/api/getStockInfo.jsp?"));
	uriBuilder.append_query(U("ex_ch"), ex_chValue.str().c_str())
			  .append_query(U("_"), system_clock::now().time_since_epoch().count())
			  .append_query(U("json"), "1")
			  .append_query(U("delay"), "0");

	http_response httpResponse = httpClient.request(methods::GET, uriBuilder.to_string()).get();

	return true;
}
