#include "pch.h"
#include "CppUnitTest.h"
#include "InvestmentManager/IInvestMgr.h"
#include "InvestmentManager/InvestmentManager.h"
#include "InvestmentManager/DataItem.h"
#include "InvestmentManager/MovingAverage.h"
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

		TEST_METHOD(GetTSEData)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			CDataItem data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				L"2412", 2017, 4, 13, data);
			Assert::IsTrue(data.id == L"2412");
			Assert::IsTrue(data.date == L"20170413");

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetOTCData)
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

		TEST_METHOD(GetSimpleData)
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

		TEST_METHOD(GetDataWithLargeDayCount)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);
			// data only from 2015/03/23 orz
			std::vector<CDataItem> data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				L"2412", 2015, 3, 23, 200, data);
			Assert::IsTrue(data.size() == 200);

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetDataWithDayDurationInTheSameMonth)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			std::vector<CDataItem> data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				L"2105", 2017, 2, 15,
				2017, 2, 22, data);
			Assert::IsTrue(data[0].date == L"20170215");
			Assert::IsTrue(data[data.size() - 1].date == L"20170222");

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

		TEST_METHOD(GetDataWithLargeDayDuration)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(&investMgr);

			std::vector<CDataItem> data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				L"2884", 2016, 8, 15,
				2017, 2, 15, data);
			Assert::IsTrue(data[0].date == L"20160815");
			Assert::IsTrue(data[data.size() - 1].date == L"20170215");

			ReleaseInvestMgr(investMgr);
		}
    };

    TEST_CLASS(UnitTest_MovingAverage)
    {
    public:
        TEST_METHOD(InsufficientSample)
        {
            CMovingAverage ma(5);
            ma.UpdateData(1.0);
            ma.UpdateData(2.0);
            ma.UpdateData(3.0);
            ma.UpdateData(4.0);
            Assert::IsTrue(ma.GetResult() == 0.0);
        }

        TEST_METHOD(SimpleTest)
        {
            CMovingAverage ma(5);
            ma.UpdateData(1.0);
            ma.UpdateData(2.0);
            ma.UpdateData(3.0);
            ma.UpdateData(4.0);
            ma.UpdateData(5.0);
            Assert::IsTrue(ma.GetResult() == 3.0);
            ma.UpdateData(6.0);
            Assert::IsTrue(ma.GetResult() == 4.0);
            ma.UpdateData(7.0);
            Assert::IsTrue(ma.GetResult() == 5.0);
        }
    };
}