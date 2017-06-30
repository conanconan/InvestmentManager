#include "pch.h"
#include "CppUnitTest.h"
#include "InvestmentManager/IInvestMgr.h"
#include "InvestmentManager/InvestmentManager.h"
#include "InvestmentManager/DataItem.h"
#include "InvestmentManager/MovingAverage.h"
#include "InvestmentManager/DbWrapper.h"
#include <string>
#include <vector>
#include <experimental/filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::experimental;
using namespace Windows::Storage;

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
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);

			std::vector<std::wstring> agentId;
			investMgr->GetAgentIdList(agentId);
			Assert::IsTrue(Exist(agentId, L"TwStock_StockExchangeMarket"));
			Assert::IsTrue(Exist(agentId, L"TwStock_OverTheCounterMarket"));

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetTSEData)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);

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
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);

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
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);

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
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);

			std::vector<CDataItem> data;
			investMgr->GetData(L"TwStock_StockExchangeMarket",
				L"9904", 2017, 2, 15, 30, data);
			Assert::IsTrue(data.size() == 30);

			ReleaseInvestMgr(investMgr);
		}

		TEST_METHOD(GetDataWithLargeDayCount)
		{
			IInvestMgr* investMgr = nullptr;
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);
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
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);

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
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);

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
			CreateInvestMgr(ApplicationData::Current->LocalFolder->Path->Data(), 
                &investMgr);

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

    TEST_CLASS(UnitTest_DbWrapper)
    {
    public:
        TEST_METHOD(CreateDbWithOneTable)
        {
            std::wstring localPath = 
                ApplicationData::Current->LocalFolder->Path->Data();
            filesystem::path dbFilePath = 
                filesystem::path(localPath) / L"db" / L"oneTable.db";
            std::map<std::wstring, std::vector<std::wstring>> dbTable =
            { {L"table1", {L"id1 TEXT", L"id2 TEXT"}} };
            
            CDbWrapper db(dbFilePath, dbTable);
            Assert::IsTrue(db.InsertData(L"table1", { L"a1", L"b2" }));
            Assert::IsTrue(db.InsertData(L"table1", { L"a2", L"b3" }));
            Assert::IsTrue(db.InsertData(L"table1", { L"a1", L"b4" }));

            std::vector<std::vector<std::wstring>> datas;
            Assert::IsTrue(db.QueryData(
                L"table1", { L"id1", L"id2" }, L"id1 = 'a1'", datas));
            Assert::IsTrue(datas.size() == 2);
            for (auto& data : datas)
            {
                Assert::IsTrue(data[1] == L"b2" || data[1] == L"b4");
            }
        }

        TEST_METHOD(InsertTheSameData)
        {
            std::wstring localPath =
                ApplicationData::Current->LocalFolder->Path->Data();
            filesystem::path dbFilePath =
                filesystem::path(localPath) / L"db" / L"sameData.db";
            std::map<std::wstring, std::vector<std::wstring>> dbTable =
            { { L"tableSame",{ L"id1 TEXT", L"id2 TEXT" } } };

            CDbWrapper db(dbFilePath, dbTable);
            Assert::IsTrue(db.InsertData(L"tableSame", { L"a1", L"b2" }));
            Assert::IsTrue(db.InsertData(L"tableSame", { L"a1", L"b2" }));
            Assert::IsTrue(db.InsertData(L"tableSame", { L"a1", L"b2" }));
            Assert::IsTrue(db.InsertData(L"tableSame", { L"a1", L"b2" }));
            Assert::IsTrue(db.InsertData(L"tableSame", { L"a1", L"b2" }));

            std::vector<std::vector<std::wstring>> datas;
            Assert::IsTrue(db.QueryData(
                L"tableSame", { L"id1", L"id2" }, L"id1 = 'a1'", datas));
            Assert::IsTrue(datas.size() == 5);
            for (auto& data : datas)
            {
                Assert::IsTrue(data[1] == L"b2");
            }
        }

        TEST_METHOD(CreateDbWithTwoTable)
        {
            std::wstring localPath =
                ApplicationData::Current->LocalFolder->Path->Data();
            filesystem::path dbFilePath =
                filesystem::path(localPath) / L"db" / L"twoTable.db";
            std::map<std::wstring, std::vector<std::wstring>> dbTable =
            { { L"table1",{ L"id1 TEXT", L"id2 TEXT", L"id3 TEXT" } },
              { L"table2",{ L"id1 TEXT", L"id2 TEXT", L"id3 TEXT", L"id4 TEXT" } } };

            CDbWrapper db(dbFilePath, dbTable);
            Assert::IsTrue(db.InsertData(L"table1", { L"a1", L"b2", L"c3"}));
            Assert::IsTrue(db.InsertData(L"table1", { L"a2", L"b3", L"c4"}));
            Assert::IsTrue(db.InsertData(L"table1", { L"a1", L"b4", L"c3" }));
            Assert::IsTrue(db.InsertData(L"table1", { L"a1", L"b4", L"c3" }));

            Assert::IsTrue(db.InsertData(L"table2", { L"d1", L"e3", L"f15", L"g71" }));
            Assert::IsTrue(db.InsertData(L"table2", { L"d2", L"e5", L"f15", L"g72" }));
            Assert::IsTrue(db.InsertData(L"table2", { L"d3", L"e3", L"f15", L"g73" }));
            Assert::IsTrue(db.InsertData(L"table2", { L"d4", L"e7", L"f15", L"g74" }));

            std::vector<std::vector<std::wstring>> datas;
            Assert::IsTrue(db.QueryData(
                L"table1", { L"id1", L"id2", L"id3" }, L"id1 = 'a1' and id3 == 'c3'", datas));
            Assert::IsTrue(datas.size() == 3);
            for (auto& data : datas)
            {
                Assert::IsTrue(data[1] == L"b2" || data[1] == L"b4");
            }

            datas.clear();
            Assert::IsTrue(db.QueryData(
                L"table2", { L"id1", L"id2", L"id4" }, L"id2 = 'e3'", datas));
            Assert::IsTrue(datas.size() == 2);
            for (auto& data : datas)
            {
                Assert::IsTrue(data[2] == L"g71" || data[2] == L"g73");
            }
        }

        TEST_METHOD(CreateDbWithPrimaryKey)
        {
            std::wstring localPath =
                ApplicationData::Current->LocalFolder->Path->Data();
            filesystem::path dbFilePath =
                filesystem::path(localPath) / L"db" / L"oneTableWithPrimaryKey.db";
            std::map<std::wstring, std::vector<std::wstring>> dbTable =
            { { L"table1",{ L"id1 TEXT", L"id2 TEXT", L"id3 TEXT", 
                            L"primary key (id1, id2)" } } };

            CDbWrapper db(dbFilePath, dbTable);
            Assert::IsTrue(db.InsertData(L"table1", { L"a1", L"b2", L"c3" }));
            Assert::IsTrue(db.InsertData(L"table1", { L"a2", L"b3", L"c3" }));
            Assert::IsFalse(db.InsertData(L"table1", { L"a2", L"b3", L"c4" }));
        }
    };
}