// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


//dlghisto.cpp

#include "pp.h"
#include "id.h"
#include "ppafx.h"
#include "ctlutil.h"
#include "histoctl.h"
#include "tagctl.h"
#include "gradctl.h"
#include "ppmodal.h"
#include "dlgsmap.h"
#include "dlghisto.h"

IMPLEMENT_DYNCREATE(CHistogramDlg, CPPMapExDlg)

BEGIN_MESSAGE_MAP(CHistogramDlg, CPPMapExDlg)	  //for future
END_MESSAGE_MAP()

CHistogramDlg::CHistogramDlg(CServerView* pView, CWnd* pParent)
:CPPMapExDlg(pView, CHistogramDlg::IDD, pParent)
{

}


BOOL CHistogramDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int id = LOWORD(wParam);					//unpack the message
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;

	int i, val;
	

	switch (id)
	{
		case IDC_CHANNELS:
			i=ChannelsCombo_Handle(id, codeNotify);
			if (!i)
				break;
			m_wChannel = i;
			set_histo_channel();
		break;
	
		case IDC_HISTOGRAM:
			// value changed
			if (WITHIN(codeNotify, HTN_CHANGED_FIRST, HTN_CHANGED_LAST))
				i = codeNotify-HTN_CHANGED_FIRST;
			else if (WITHIN(codeNotify, HTN_CHANGING_FIRST, HTN_CHANGING_LAST))
				i = codeNotify-HTN_CHANGING_FIRST;
			else if (codeNotify = HTN_BROWSE)
			{
				val = Histo_GetBrowse(hControl);
				histo_browse(val);
				break;
			}	
			else
				break;
			val = Histo_GetMark(hControl, i);
			histo_browse(val);
			histo_change_mark(i, val);
		break;

		case IDC_TAGS:
			// value changed
			if (WITHIN(codeNotify, TN_CHANGED_FIRST, TN_CHANGED_LAST))
				i = codeNotify-TN_CHANGED_FIRST;
			else if (WITHIN(codeNotify, TN_CHANGING_FIRST, TN_CHANGING_LAST))
				i = codeNotify-TN_CHANGING_FIRST;
			else
				break;
			val = Tag_GetMark(hControl, i);
			histo_browse(val);
			histo_change_mark(i, val);
		break;

   		default:
		break;
   }
return CWnd::OnCommand(wParam, lParam);
}





BOOL CHistogramDlg::OnInitDialog()
{
	HWND hActiveWnd;
	HWND hTags;
	FRMDATATYPE type;
	LPIMAGE lpImage = NULL;
	int icons[HISTOMARKS];
	HWND hDlg = GetSafeHwnd();
	

	lpImage = GetImage();
	RightPopup( hDlg );
	
	// set channels combo
	type = FrameType(ImgGetBaseEditFrame(lpImage));
	m_wChannel = IDC_DO_ALL;
	m_MapValues.wChannel = m_wChannel;
	ChannelsCombo_Setup(IDC_CHANNELS, type);	 //This NEEDS to be fixed!!!

	
	// setup tags
	hTags = ::GetDlgItem(hDlg, IDC_TAGS);
	icons[0] = IDC_HISTOTAG1;
	icons[1] = IDC_HISTOTAG2;
	Tag_Init(hTags, HISTOMARKS, 255, &icons[0]);
	Tag_SetMark(hTags, 0, 85, NO);
	Tag_SetMark(hTags, 1, 170, NO);
	
	m_lpHistos[0] = NULL;
	
	// setup the actual histogram and controls 
	set_histo_channel();
	return CPPMapExDlg::OnInitDialog();
}


//		Updates the display.
void CHistogramDlg::set_histo_channel()
{
	HWND hHisto, hTags;
	BOOL bInv, bMakeTotals;
	LPDWORD lpHisto;
	int i, j, iHisto;
	LONG sum;
	LONG lMaxEntry;
	LPDWORD lpdw;
	char szNum[40];
	HWND hDlg = GetSafeHwnd();
	
	bMakeTotals = (m_lpHistos[0] == NULL);
	lpHisto = get_histo(m_lpHistos, m_wChannel, &bInv, &iHisto);
	if (!lpHisto)
		return;
	
	// need to compute totals?
	if (bMakeTotals)
	{	
    	for (j=0;j<5;j++)
		{
			lpdw = m_lpHistos[j];
			sum = 0L;
			for (i=0;i<256;i++)
				sum += *lpdw++;
			m_lTotals[j] = sum;
		}
	}
	
	// setup the histo control
	lMaxEntry = compute_yscale(lpHisto);
	hHisto = ::GetDlgItem(hDlg, IDC_HISTOGRAM);
	Histo_Init(hHisto, lpHisto, lMaxEntry, HISTOMARKS, bInv ? HTS_FLIPX:0);
	// reset marks based on tag
	hTags = ::GetDlgItem(hDlg, IDC_TAGS);
	i = Tag_GetMark(hTags,0);
	Histo_SetMark(hHisto, 0, i, NO);
	i = Tag_GetMark(hTags,1);
	Histo_SetMark(hHisto, 1, i, NO);
	
	// setup the gradient
	set_gradient(IDC_HISTO_GRAD, m_wChannel);
	
	// setup readouts
	::SetDlgItemText(hDlg, IDC_HISTO_TOTAL, ltoa(m_lTotals[iHisto],szNum,10));
	::SetDlgItemText(hDlg, IDC_HISTO_POS, (LPSTR)"");
	::SetDlgItemText(hDlg, IDC_HISTO_VALUE, (LPSTR)"");
	histo_set_HMSvalues();
}

//		Coordinates the tag control and the histo control for the 
//		histogram dialog.  
//		Updates readouts for H,M&S
void CHistogramDlg::histo_change_mark(int nMark, int val)
{

	HWND hHisto, hTag;
	int i;
	int Marks[HISTOMARKS];
	HWND hDlg = GetSafeHwnd();
	
	hHisto = ::GetDlgItem(hDlg, IDC_HISTOGRAM);
	
	// get the marks
	for (i=0;i<HISTOMARKS;i++)
		Marks[i] = Histo_GetMark(hHisto, i);
	
	// set value and adjust others
	if (nMark == 0)
	{
		Marks[0] = val;
		if (Marks[1] < Marks[0])
			Marks[1] = Marks[0];
	}
	else if (nMark == 1)
	{
		Marks[1] = val;
		if (Marks[1] < Marks[0])
			Marks[0] = Marks[1];
	}
	
	// update anything that needs it
	hTag = ::GetDlgItem(hDlg, IDC_TAGS);
	for (i=0;i<HISTOMARKS;i++)
	{
		if (Marks[i] != Histo_GetMark(hHisto,i))
			Histo_SetMark(hHisto,i, Marks[i], NO);
		if (Marks[i] != Tag_GetMark(hTag,i))
			Tag_SetMark(hTag,i, Marks[i], NO);
	}
	
	// set HMS readouts
	histo_set_HMSvalues();
}

//		Sets the HMS readouts for the histo dialog based on the histogram control!
void CHistogramDlg::histo_set_HMSvalues()
{
	HWND hHisto;
	LPDWORD lpHisto;
	BOOL bInv;
	LONG lTotal;
	int iHisto;
	int i,j, start, end;
	int marks[HISTOMARKS];
	LONG sums[HISTOMARKS+1];
	double percents[HISTOMARKS+1];
	LONG sum;
	int add;
	char szDouble[30];
	HWND hDlg = GetSafeHwnd();

	lpHisto = get_histo(m_lpHistos, m_wChannel, &bInv, &iHisto);
	if (!lpHisto)
		return;
	lTotal = m_lTotals[iHisto];
	
	hHisto = ::GetDlgItem(hDlg, IDC_HISTOGRAM);
	
	// get the marks
	for (i=0;i<HISTOMARKS;i++)
	{
		marks[i] = Histo_GetMark(hHisto, i);
	}
	
	// get the three sums
	end = bInv ? 255:0;
	add = bInv ? -1:1;
	for (i=0;i<=HISTOMARKS;i++)
	{
		start = end;
		if (i >= HISTOMARKS)
			end = bInv ? 0:255;
		else
			end = bInv ? (255-marks[i]):marks[i];
		sum = 0L;
		for (j=start;j*add<end*add;j += add)
			sum += lpHisto[j];
		sums[i] = sum;
	}
	// scale if needed
	if (lTotal > 0x100000)
	{
		lTotal >>= 8L;
		for (i=0;i<=HISTOMARKS;i++)
			sums[i] >>= 8L;
	}
	// get percentages
	for (i=0;i<=HISTOMARKS;i++)
		percents[i] = (sums[i]*100.0 + (double)lTotal/2000)/lTotal;
	// write out percentages
	sprintf(szDouble, "%.1f", percents[2]);
	lstrcat(szDouble, (LPSTR)"%");
	::SetDlgItemText(hDlg,IDC_HISTO_HI  ,szDouble);
	sprintf(szDouble, "%.1f", percents[1]);
	lstrcat(szDouble, (LPSTR)"%");
	::SetDlgItemText(hDlg,IDC_HISTO_MID ,szDouble);
	sprintf(szDouble, "%.1f", percents[0]);
	lstrcat(szDouble, (LPSTR)"%");
	::SetDlgItemText(hDlg,IDC_HISTO_SHAD,szDouble);
}
//		Called when point is browsed in histo dialog
void CHistogramDlg::histo_browse(int x)
{
	BOOL bInv;
	LPDWORD lpHisto;
	HWND hDlg = GetSafeHwnd();

	if (!WITHIN(x,0,255))
	{
		::SetDlgItemText(hDlg, IDC_HISTO_POS, (LPSTR)"");
		::SetDlgItemText(hDlg, IDC_HISTO_VALUE, (LPSTR)"");
		return;
	}

	lpHisto = get_histo(m_lpHistos, m_wChannel, &bInv);
	if (!lpHisto)
		return;
		
	if (Control.DisplayPixelValue)
		SetDlgItemInt(IDC_HISTO_POS, x, FALSE);
	else
		SetDlgItemIntExt(hDlg, IDC_HISTO_POS, x*100L/255, "%");
		
	if (bInv)
		x = 255-x;
	SetDlgItemInt(IDC_HISTO_VALUE, lpHisto[x], FALSE);
}

