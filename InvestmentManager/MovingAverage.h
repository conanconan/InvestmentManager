#pragma once
#include <queue>

class CMovingAverage
{
public:
    CMovingAverage(size_t size);
    virtual ~CMovingAverage();

    bool UpdateData(double data);
    double GetResult();

private:
    const size_t m_queueSize;
    std::queue<double> m_data;
    double m_dataAmount;
};

