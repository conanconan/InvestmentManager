#include "pch.h"
#include "DbWrapper.h"
#include <winsqlite/winsqlite3.h>
#include <sstream>


CDbWrapper::CDbWrapper(const std::experimental::filesystem::path& dbFilePath,
    const std::map<std::wstring, std::vector<std::wstring>>& dbTable)
    : m_pDb(NULL)
{
    CreateDb(dbFilePath, dbTable);
}

CDbWrapper::~CDbWrapper()
{
    ReleaseDb();
}

std::wstring CDbWrapper::CreateTableCmd(const std::pair<std::wstring, 
    const std::vector<std::wstring>>& dbTable)
{
    std::wstring tableCmd = dbTable.first + L"(";
    auto& params = dbTable.second;
    for (size_t i = 0; i < params.size(); ++i)
    {
        tableCmd += params[i] + ((i != params.size() - 1) ? L", " : L"");
    }
    tableCmd += L")";

    return tableCmd;
}

void CDbWrapper::CreateDb(const std::experimental::filesystem::path& dbFilePath,
    const std::map<std::wstring, std::vector<std::wstring>>& dbTable)
{
    std::experimental::filesystem::create_directories(dbFilePath.parent_path());
    if (SQLITE_OK != sqlite3_open16(dbFilePath.c_str(), &m_pDb))
    {
        ReleaseDb();
        return;
    }

    std::wstring sqlCreateTablePrefix(L"create table if not exists ");
    for (auto& table : dbTable)
    {
        std::wstring sqlCmd = sqlCreateTablePrefix + CreateTableCmd(table) + L";";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare16_v2(m_pDb, sqlCmd.c_str(), sizeof(wchar_t) * sqlCmd.length(), 
                &stmt, nullptr) != SQLITE_OK ||
            sqlite3_step(stmt) != SQLITE_DONE || 
            sqlite3_finalize(stmt) != SQLITE_OK)
        {
            ReleaseDb();
            return;
        }
    }
}

void CDbWrapper::ReleaseDb()
{
    if (m_pDb != nullptr)
    {
        sqlite3_close_v2(m_pDb);
        m_pDb = nullptr;
    }
}

bool CDbWrapper::InsertData(const std::wstring& table, const std::vector<std::wstring>& data)
{
    std::wstringstream sqlCmd;
    sqlCmd << L"insert into " << table.c_str() << L" values (";
    for (size_t i = 0; i < data.size(); ++i)
    {
        sqlCmd << L"'" << data[i].c_str() << ((i != data.size() - 1) ? L"', " : L"'");
    }
    sqlCmd << L");";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare16_v2(m_pDb, sqlCmd.str().c_str(), sizeof(wchar_t) * sqlCmd.str().length(), 
            &stmt, nullptr) != SQLITE_OK ||
        sqlite3_step(stmt) != SQLITE_DONE ||
        sqlite3_finalize(stmt) != SQLITE_OK)
    {
        return false;
    }

    return true;
}

bool CDbWrapper::QueryData(const std::wstring& table, const std::vector<std::wstring>& queryColumn,
    const std::wstring& condition, std::vector<std::vector<std::wstring>>& data)
{
    bool ret = false;
    data.clear();

    std::wstringstream sqlCmd;
    sqlCmd << L"select ";
    for (size_t i = 0; i < queryColumn.size(); ++i)
    {
        sqlCmd << queryColumn[i].c_str() << ((i != queryColumn.size() - 1) ? L", " : L" ");
    }
    sqlCmd << L"from " << table.c_str() << L" where " << condition.c_str() << L";";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare16_v2(m_pDb, sqlCmd.str().c_str(), sizeof(wchar_t) * sqlCmd.str().length(), 
            &stmt, nullptr) == SQLITE_OK)
    {
        int sqlRet = SQLITE_OK;
        while ((sqlRet = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            data.push_back(std::vector<std::wstring>());
            auto& rowData = data.back();
            int columnCount = sqlite3_column_count(stmt);
            if (columnCount > 0)
            {
                for (int i = 0; i < columnCount; ++i)
                {
                    rowData.push_back(static_cast<const wchar_t*>(sqlite3_column_text16(stmt, i)));
                }
            }
            else
            {
                data.erase(data.begin() + data.size() - 1);
            }
        }

        if (sqlRet == SQLITE_DONE)
        {
            ret = true;
        }
    }

    sqlite3_finalize(stmt);
    return ret;
}