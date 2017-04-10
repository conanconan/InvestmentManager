#pragma once
#include "InvestMgr.h"

#ifdef _INVESTMGRDLL
#define INVESTMGR_DLLAPI extern "C" __declspec(dllexport)
#else
#define INVESTMGR_API extern "C" __declspec(dllimport)
#endif

INVESTMGR_DLLAPI bool CreateInvestMgr(CInvestMgr** ppInvestMgr);
INVESTMGR_DLLAPI bool ReleaseInvestMgr(CInvestMgr* pInvestMgr);