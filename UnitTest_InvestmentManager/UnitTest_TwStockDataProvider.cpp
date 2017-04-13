#include "pch.h"
#include "CppUnitTest.h"
#include "InvestmentManager/IInvestMgr.h"
#include "InvestmentManager/InvestmentManager.h"
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
    };
}