#include "pch.h"
#include "MovingAverage.h"


CMovingAverage::CMovingAverage(size_t size)
    : m_queueSize(size)
{}

CMovingAverage::~CMovingAverage()
{}

bool CMovingAverage::UpdateData(double data)
{
    if (m_queueSize == m_data.size())
    {
        m_dataAmount -= m_data.front();
        m_data.pop();
    }

    m_data.push(data);
    m_dataAmount += data;

    return true;
}

double CMovingAverage::GetResult()
{
    if (m_queueSize == m_data.size())
    {
        return (m_dataAmount / m_queueSize);
    }

    return 0.0;
}
