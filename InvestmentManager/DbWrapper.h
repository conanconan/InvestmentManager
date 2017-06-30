#pragma once
#include <string>
#include <vector>
#include <map>
#include <experimental/filesystem>

struct sqlite3;

class CDbWrapper
{
public:
    CDbWrapper(const std::experimental::filesystem::path& dbFilePath, 
        const std::map<std::wstring, std::vector<std::wstring>>& dbTable);
    virtual ~CDbWrapper();

    virtual bool InsertData(const std::wstring& table, const std::vector<std::wstring>& data);
    virtual bool QueryData(const std::wstring& table, const std::vector<std::wstring>& queryColumn,
        const std::wstring& condition, std::vector<std::vector<std::wstring>>& data);

private:
    sqlite3* m_pDb;

    std::wstring CreateTableCmd(const std::pair<std::wstring, 
        const std::vector<std::wstring>>& dbTable);
    void CreateDb(const std::experimental::filesystem::path& dbFilePath,
        const std::map<std::wstring, std::vector<std::wstring>>& dbTable);
    void ReleaseDb();
};

