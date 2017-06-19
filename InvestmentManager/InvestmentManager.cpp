#include "pch.h"
#include "InvestmentManager.h"
#include "InvestMgr.h"

INVESTMGR_DLLAPI bool CreateInvestMgr(const std::wstring& storagePath, IInvestMgr** ppInvestMgr)
{
	*ppInvestMgr = new CInvestMgr(storagePath);
	return true;
}

INVESTMGR_DLLAPI bool ReleaseInvestMgr(IInvestMgr* pInvestMgr)
{
	delete pInvestMgr;
	return true;
}
