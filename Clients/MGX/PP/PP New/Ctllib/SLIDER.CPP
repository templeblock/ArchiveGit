// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef NO_MFC
#include "ctlutil.h"
#include "slider.h"

//************************************************************************
BOOL InitSlide( HWND hDlg, ITEMID idItem, int iValue, int iMin, int iMax )
//************************************************************************
{
	HWND hControl = GetDlgItem(hDlg, idItem);
	CSliderCtrl *pSlider = (CSliderCtrl *)CWnd::FromHandle(hControl);
	if (pSlider)
	{
		STRING szString;
		pSlider->GetWindowText(szString, sizeof(szString));
		if (lstrlen(szString))
		{
			BOOL bTranslated;
			int iStep, iBigTick, iSmallTick, iTick;

			iStep = GetIntParm(szString, 3, &bTranslated);
			if (!bTranslated) iStep = 1;
			pSlider->SetLineSize(iStep);

			iBigTick = GetIntParm(szString, 4, &bTranslated);
			if (!bTranslated) iBigTick = 0;
			iSmallTick = GetIntParm(szString, 5, &bTranslated);
			if (!bTranslated) iSmallTick = 0;
			iTick = min(iBigTick, iSmallTick);
			if (!iTick)
				iTick = iBigTick;
			if (!iTick)
				iTick = iSmallTick;
			pSlider->SetTicFreq(iTick);
		}
		pSlider->SetRange(iMin, iMax);
		pSlider->SetPos(iValue);
		return(TRUE);
	}
	else
		return(FALSE);
}

//************************************************************************
void SliderOnScroll( HWND hWindow, UINT uCode, int iPos )
//************************************************************************
{
	if (uCode == SB_THUMBTRACK)
	{
 		FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
 							hWindow, SN_VALUECHANGING, SendMessage);
	}
	else
	{
	  	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
	  						hWindow, SN_VALUESELECTED, SendMessage);
	}
}

//************************************************************************
BOOL SetSlide( HWND hDlg, ITEMID idItem, int iValue )
//************************************************************************
{
 	HWND hControl = GetDlgItem(hDlg, idItem);
	CSliderCtrl *pSlider = (CSliderCtrl *)CWnd::FromHandle(hControl);
	if (pSlider)
	{
		pSlider->SetPos(iValue);
		return(TRUE);
	}
	else
		return(FALSE);
}


//************************************************************************
int HandleSlide( HWND hDlg, ITEMID id, UINT codeNotify, LPINT lpBoolCompleted )
//************************************************************************
{
	HWND hControl = GetDlgItem(hDlg, id);
	CSliderCtrl *pSlider = (CSliderCtrl *)CWnd::FromHandle(hControl);
	if (pSlider)
	{
		return(pSlider->GetPos());
	}
	else
		return(FALSE);
}

#endif
