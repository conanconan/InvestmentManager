#include "pch.h"
#include "CppUnitTest.h"
#include "InvestmentManager/IInvestMgr.h"
#include "InvestmentManager/InvestmentManager.h"
#include "InvestmentManager/DataItem.h"
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_InvestmentManager
{
	TEST_CLASS(UnitTest_TwStockDataProvider)
	{
	public:
		bool Exist(const std::vector<std::wstring>& list,
			std::wstring target)
		{
			for (auto item : list)
			{
				if (item == target)
				{
					return true;
				}
			}
			return false;
		}

		TEST_METHOD(GetAgentIdList)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			std::vector<std::wstring> agentId;
			investMgr->GetAgentIdList(agentId);
			Assert::IsTrue(Exist(agentId, L"TwStock_StockExchangeMarket"));
			Assert::IsTrue(Exist(agentId, L"TwStock_OverTheCounterMarket"));

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetDataMulti)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			std::vector<std::wstring> dataId = { L"0050", L"2330" };
			std::vector<CDataItem> data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				dataId, 2017, 4, 13, data);
			Assert::IsTrue(data.size() == 2);
			Assert::IsTrue(data[0].id == L"0050" || data[1].id == L"0050");
			Assert::IsTrue(data[0].id == L"2330" || data[1].id == L"2330");

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetDataSingle)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			CDataItem data;
			investMgr->GetData(L"TwStock_OverTheCounterMarket",
				L"6419", 2017, 4, 13, data);
			Assert::IsTrue(data.id == L"6419");
			Assert::IsTrue(data.date == L"20170413");
			
			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetSimpleDataMulti)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			std::vector<std::wstring> dataId = { L"2618", L"2317" };

			std::vector<CDataItem> data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				dataId, 2017, 4, 13, data);

			std::vector<std::wstring> closingPrice;
			investMgr->GetSimpleData(L"TwStock_StockExchangeMarket",
				dataId, 2017, 4, 13, closingPrice);

			Assert::IsTrue(closingPrice.size() == data.size());
			Assert::IsTrue(closingPrice[0] == data[0].closingPrice);
			Assert::IsTrue(closingPrice[1] == data[1].closingPrice);

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetSimpleDataSingle)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			CDataItem data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				L"2610", 2017, 4, 13, data);
			
			std::wstring closingPrice;
			investMgr->GetSimpleData(L"TwStock_StockExchangeMarket", 
				L"2610", 2017, 4, 13, closingPrice);

			Assert::IsTrue(closingPrice == data.closingPrice);

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetDataWithDayCount)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			std::vector<CDataItem> data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				L"9904", 2017, 2, 15, 30, data);
			Assert::IsTrue(data.size() == 30);

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetDataWithDayDuration)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			std::vector<CDataItem> data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				L"2105", 2017, 2, 15, 
				2017, 4, 18, data);
			Assert::IsTrue(data[0].date == L"20170215");
			Assert::IsTrue(data[data.size() - 1].date == L"20170418");

			ReleaseInvestMgr(investMgr);
		}
    };
}