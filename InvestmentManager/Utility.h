#pragma once
#include <mutex>

class CAutoLock
{
public:
    CAutoLock(std::mutex& m);
    ~CAutoLock();

private:
    std::mutex& m_mutex;
};