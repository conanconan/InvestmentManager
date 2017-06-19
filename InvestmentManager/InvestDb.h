#pragma once
#include <string>
#include <vector>
#include <map>
#include <experimental/filesystem>

struct sqlite3;

class CInvestDb
{
public:
    CInvestDb(const std::experimental::filesystem::path& dbFilePath, 
        const std::map<std::wstring, std::vector<std::wstring>>& dbTable);
    virtual ~CInvestDb();

    bool InsertData(const std::wstring& table, const std::vector<std::wstring>& data);
    bool QueryData(const std::wstring& table, const std::vector<std::wstring>& queryColumn, 
        const std::wstring& condition, std::vector<std::vector<std::wstring>>& data);

private:
    sqlite3* m_pDb;

    std::wstring CreateTableCmd(const std::pair<std::wstring, 
        const std::vector<std::wstring>>& dbTable);
    void CreateDb(const std::experimental::filesystem::path& dbFilePath,
        const std::map<std::wstring, std::vector<std::wstring>>& dbTable);
    void ReleaseDb();
};

