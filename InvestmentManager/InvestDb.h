#pragma once
#include <string>
#include <vector>
#include <experimental/filesystem>

struct sqlite3;

class CInvestDb
{
public:
    CInvestDb(std::experimental::filesystem::path dbFilePath, 
        std::vector<std::wstring> dbTables);
    virtual ~CInvestDb();

    bool InsertData(const std::wstring& table, const std::vector<std::wstring>& data);
    bool QueryData(const std::wstring& table, const std::vector<std::wstring>& queryColumn, 
        const std::wstring& condition, std::vector<std::vector<std::wstring>>& data);

private:
    sqlite3* m_pDb;

    void CreateDb(std::experimental::filesystem::path dbFilePath,
        std::vector<std::wstring> tables);
    void ReleaseDb();
};

