#include "pch.h"
#include "InvestmentManager.h"
#include "InvestMgr.h"

INVESTMGR_DLLAPI bool CreateInvestMgr(IInvestMgr** ppInvestMgr)
{
	*ppInvestMgr = new CInvestMgr();
	return true;
}

INVESTMGR_DLLAPI bool ReleaseInvestMgr(IInvestMgr* pInvestMgr)
{
	delete pInvestMgr;
	return true;
}
