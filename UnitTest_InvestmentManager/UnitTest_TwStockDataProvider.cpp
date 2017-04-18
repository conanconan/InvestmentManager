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

			std::vector<std::wstring> agentId;
			investMgr->GetAgentIdList(agentId);

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

			std::vector<std::wstring> agentId;
			investMgr->GetAgentIdList(agentId);

			CDataItem data;
			investMgr->GetData(L"TwStock_OverTheCounterMarket",
				L"6419", 2017, 4, 13, data);
			Assert::IsTrue(data.id == L"6419");
			
			ReleaseInvestMgr(investMgr);
		}
    };
}