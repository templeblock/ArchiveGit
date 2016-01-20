//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef NO_MFC
#ifndef _SPIN_H_
#define _SPIN_H_

#include "afxcmn.h"

class CSpinFixedCtrl : public CSpinButtonCtrl
{
	DECLARE_DYNAMIC(CSpinFixedCtrl)
public:
	CSpinFixedCtrl() {}
	~CSpinFixedCtrl() {}

public:
	// implementation
	CSpinFixedCtrl *Init(LFIXED fValue, LFIXED fMin, LFIXED fMax, BOOL bConvert);
	LFIXED 	SetPos(LFIXED fValue);
	LFIXED 	GetPos();
	LFIXED 	GetEditValue(LPINT lpTranslated);
	void	SetRange(LFIXED fMin, LFIXED fMax);
	void 	OnNotifyDeltaPos(LPNM_UPDOWN lpUpDown);

private:
	void 	BumpValue(int iDelta);
	LFIXED	BumpFixedSpinnerValue(LFIXED Fixed, BOOL bUp, long step);
	void 	DoInit(LFIXED fValue, LFIXED fMin, LFIXED fMax, BOOL bConvert);
			
protected:
	LFIXED	m_fMin;
	LFIXED	m_fMax;
	LFIXED	m_fValue;
	BOOL	m_bConvert;
	BOOL	m_bGetting;
};

class CSpinIntCtrl : public CSpinButtonCtrl
{
	DECLARE_DYNAMIC(CSpinIntCtrl)
public:
	CSpinIntCtrl() {}
	~CSpinIntCtrl() {}

public:
	// implementation
	void	Init(int iValue, int iMin, int iMax);
	int 	GetEditValue(LPINT lpTranslated);
	int		SetPos(int iValue);
	int		GetPos();
};

void InitDlgItemSpin( HWND hWindow, int idControl, int iValue, BOOL bSigned,
                        int iMin, int iMax );
void SetDlgItemSpinLimits( HWND hWindow, int idControl, int iMin, int iMax );
void SetDlgItemSpinStep( HWND hWindow, int idControl, long lStep );
int SetDlgItemSpin( HWND hWindow, int idControl, int iValue, BOOL bSigned );
int GetDlgItemSpin( HWND hWindow, int idControl, LPINT lpTranslated,
                        BOOL bSigned );
void InitDlgItemSpinFixed( HWND hWindow, int idControl, LFIXED fValue,
                              BOOL bConvert, LFIXED fMin, LFIXED fMax );
void SetDlgItemSpinFixedLimits( HWND hWindow, int idControl, LFIXED fMin,
                                 LFIXED fMax );
LFIXED SetDlgItemSpinFixed( HWND hWindow, int idControl, LFIXED fValue,
                              BOOL bConvert );
LFIXED GetDlgItemSpinFixed( HWND hWindow, int idControl, LPINT lpTranslated,
                              BOOL bConvert );

#endif // _SPIN_H_
#endif // NO_MFC

