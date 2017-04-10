#pragma once
#include "IInvestMgr.h"

#ifdef _INVESTMGRDLL
#define INVESTMGR_DLLAPI extern "C" __declspec(dllexport)
#else
#define INVESTMGR_API extern "C" __declspec(dllimport)
#endif

INVESTMGR_DLLAPI bool CreateInvestMgr(IInvestMgr** ppInvestMgr);
INVESTMGR_DLLAPI bool ReleaseInvestMgr(IInvestMgr* pInvestMgr);
