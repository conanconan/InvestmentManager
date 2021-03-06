#pragma once
#include "IDataProvider.h"

class CTwStockOTCDataProvider : public IDataProvider
{
public:
    CTwStockOTCDataProvider();
    virtual ~CTwStockOTCDataProvider();

    virtual boost::gregorian::date DataBeginsOn() const;
    virtual bool GetDataId(std::map<std::wstring, std::vector<std::wstring>>& dataId) const;
    virtual bool GetRealTimeData(std::wstring dataId, CDataItem& data) const;
    virtual bool GetData(std::wstring dataId,
        boost::gregorian::date date, CDataItem& data) const;
    virtual bool GetAllData(boost::gregorian::date date, std::vector<CDataItem>& data) const;

private:
    const boost::gregorian::date m_beginDate;
};
