#include "pch.h"
#include "Utility.h"

CAutoLock::CAutoLock(std::mutex& m)
    : m_mutex(m)
{
    m_mutex.lock();
}

CAutoLock::~CAutoLock()
{
    m_mutex.unlock();
}
