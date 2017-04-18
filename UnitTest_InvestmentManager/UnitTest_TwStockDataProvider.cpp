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
		bool Exist(const std::vector<std::string>& list,
			std::string target)
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

			std::vector<std::string> agentId;
			investMgr->GetAgentIdList(agentId);
			Assert::IsTrue(Exist(agentId, "TwStock_StockExchangeMarket"));
			Assert::IsTrue(Exist(agentId, "TwStock_OverTheCounterMarket"));

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetData)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			std::vector<std::string> agentId;
			investMgr->GetAgentIdList(agentId);
			Assert::IsTrue(Exist(agentId, "TwStock_StockExchangeMarket"));
			Assert::IsTrue(Exist(agentId, "TwStock_OverTheCounterMarket"));

			std::vector<std::string> dataId = { "0050", "2330" };
			std::vector<CDataItem> data;
			investMgr->GetData("TwStock_StockExchangeMarket",
				dataId, 2017, 4, 13, data);
			Assert::IsTrue(data.size() == 2);
			Assert::IsTrue(data[0].id == L"0050" || data[1].id == L"0050");
			Assert::IsTrue(data[0].id == L"2330" || data[1].id == L"2330");

			ReleaseInvestMgr(investMgr);
		}
    };
}