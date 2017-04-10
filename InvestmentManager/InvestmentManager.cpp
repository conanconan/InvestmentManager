#include "pch.h"
#include "InvestmentManager.h"

INVESTMGR_DLLAPI bool CreateInvestMgr(CInvestMgr** ppInvestMgr)
{
	*ppInvestMgr = new CInvestMgr();
	return true;
}

INVESTMGR_DLLAPI bool ReleaseInvestMgr(CInvestMgr* pInvestMgr)
{
	delete pInvestMgr;
	return true;
}
