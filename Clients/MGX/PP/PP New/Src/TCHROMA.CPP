// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "svrview.h"
#include "dlgchrma.h"

BEGIN_MESSAGE_MAP(CChromaDlg, CPPViewModalDlg)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()                      


CChromaDlg*		pChromaDlg = NULL;

CChromaDlg::CChromaDlg( CServerView *pView,  CWnd *pParentWnd ):
    CPPViewModalDlg (pView, IDD_CHROMA_MASK, pParentWnd)
{
    ASSERT(pView);
    ASSERT(pView->GetDocument());
    ASSERT(pView->GetDocument()->m_lpImage);
}

BOOL CChromaDlg::OnInitDialog()
{
	HWND		hDlg = GetSafeHwnd();
   	int i;
   	FRMTYPEINFO TypeInfo;
   	BOOL        EnableFade;

    PreDoModal(NULL);

	bPreview = bProbePending = NO;
   	ImgGetTypeInfo(GetImage(), &TypeInfo);
   	EnableFade = TypeInfo.DataType != FDT_LINEART && TypeInfo.DataType != FDT_PALETTECOLOR;
    CenterPopup(hDlg);
	lpChromaMask = new CChromaMask(this);
    ControlEnable(hDlg, IDC_DELETEMASK, ImgGetMask(GetImage()) != NULL);
   	iCombo[0] = IDC_CHROMA_NORMAL;
	iCombo[1] = IDC_CHROMA_HSL;
	iCombo[2] = IDC_CHROMA_LUMIN;
   	nCombo    = 3;
	TableCheckComboItem(hDlg, IDC_CHROMASELECT, &iCombo[0], nCombo, lpChromaMask->ChromaColorMode, NO);
   	for (i=0; i < NUM_CHROMAMASKS; i++)
	{
    	InitDlgItemSpin(hDlg, IDC_MASKRANGE1+i, 10, NO, 0, 100);
	   	CheckDlgButton(IDC_MASKACTIVE1+i, FALSE);
	   	SetWindowLong(GetDlgItem(IDC_MASKCOLOR1+i)->GetSafeHwnd(),GWL_ACTIVE, 0);
	}
   	InitDlgItemSpin(hDlg, IDC_CHROMA_FADE, 10, NO, 0, 100);
   	CheckRadioButton(IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT, IDC_MASKMODE_ADD);
   	lpChromaMask->SetupChroma();
   	ControlEnable(hDlg, IDC_CHROMA, NO);
   	ControlEnable(hDlg, IDC_RESET, NO);
	ControlEnable( hDlg, IDC_CHROMA_FADE,EnableFade);
return(TRUE);
}

void CChromaDlg::OnLButtonUp(UINT nFlags, CPoint point) 
   {
   QuickProbeEnd( GetSafeHwnd(), -1, -1 );
   }


void CChromaDlg::OnMouseMove(UINT nFlags, CPoint Pt) 
{
	HWND	hDlg = GetSafeHwnd();
	COLORINFO ProbeColor;

   	if ( bProbePending )
	{
		bProbePending = NO;
	   	if (!QuickProbeStart(hDlg))
			return;
	}
   	if ( QuickProbeGet( hDlg, Pt.x, Pt.y, &ProbeColor ) )
		UpdateChromaMask(lpChromaMask->ActiveChromaMask, &ProbeColor);
}

BOOL CChromaDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg 		= GetSafeHwnd();
    BOOL bReturn 	= TRUE;
	UINT id 		= LOWORD(wParam);
	HWND hControl 	= (HWND)lParam;
	int codeNotify 	= HIWORD(wParam);
   	int  i;
   	long Color;
   	HWND hActiveWnd;
   	char Buff[10];
   	BYTE mask;

   	hActiveWnd = m_pView->GetSafeHwnd();

   	switch (id)
	{
    case IDC_CHROMASELECT:
	    i = TableHandleCombo(hDlg, &iCombo[0], nCombo, id, codeNotify);
	    if (i != IDC_CHROMA_NORMAL && i != IDC_CHROMA_HSL && i != IDC_CHROMA_LUMIN)
        	break;
        lpChromaMask->ChromaColorMode = i;
	    lpChromaMask->SetupChroma();
        break;

    case IDC_MAP_PROBE:
    	::GetWindowText(hControl, Buff, 10);
        id = atoi(Buff);
        if (id < 1 || id > 8)
        	break;
        id += IDC_MASKCOLOR1 - 1;
		GetDlgItem(id)->ShowWindow(SW_SHOW);
        GetDlgItem(id)->SetFocus();
		::ShowWindow(hControl, SW_HIDE);
	case IDC_MASKCOLOR1:
	case IDC_MASKCOLOR2:
	case IDC_MASKCOLOR3:
	case IDC_MASKCOLOR4:
	case IDC_MASKCOLOR5:
	case IDC_MASKCOLOR6:
	case IDC_MASKCOLOR7:
	case IDC_MASKCOLOR8:
		lpChromaMask->ActiveChromaMask = i = id - IDC_MASKCOLOR1;
	    if (codeNotify != 1 /*Not A DoubleClick*/)
		{
			if (hActiveWnd)
				bProbePending = YES;
		}
        else
        {
	    	bProbePending = NO;
	        if (!ColorPicker(&lpChromaMask->MaskColor[i], NULL))
		    	break;
	        CopyRGB(&lpChromaMask->MaskColor[i].rgb, &Color);
	        SetWindowLong(::GetDlgItem(hDlg, id), GWL_ACTIVE, Color);
	        AstralControlRepaint(hDlg, IDC_MASKCOLOR1+i);
        }
	    mask = 1 << (id - IDC_MASKCOLOR1);
        if (lpChromaMask->Mask & mask)
        	break;
        id = id - IDC_MASKCOLOR1 + IDC_MASKACTIVE1;

	case IDC_MASKACTIVE1:
	case IDC_MASKACTIVE2:
	case IDC_MASKACTIVE3:
	case IDC_MASKACTIVE4:
	case IDC_MASKACTIVE5:
	case IDC_MASKACTIVE6:
	case IDC_MASKACTIVE7:
	case IDC_MASKACTIVE8:
    	{
	    mask = 1 << (id - IDC_MASKACTIVE1);
	    lpChromaMask->Mask ^= mask;
	    CheckDlgButton(id, lpChromaMask->Mask & mask);
	    lpChromaMask->SetupChroma();
        }
	    break;
	case IDC_CHROMA_FADE:
		if (::GetFocus() != ::GetDlgItem(hDlg, id))
			break;
	   	if ( codeNotify == EN_CHANGE )
        {
        	BOOL Bool;
	      	lpChromaMask->Fade = GetDlgItemSpin(hDlg, id, &Bool, NO);
	      	lpChromaMask->SetupChroma();
        }
        break;
	case IDC_MASKRANGE1:
	case IDC_MASKRANGE2:
	case IDC_MASKRANGE3:
	case IDC_MASKRANGE4:
	case IDC_MASKRANGE5:
	case IDC_MASKRANGE6:
	case IDC_MASKRANGE7:
	case IDC_MASKRANGE8:
		if (::GetFocus() != ::GetDlgItem(hDlg, id))
			break;
	   	if ( codeNotify == EN_CHANGE )
        {
        	BOOL Bool;
	      	lpChromaMask->MaskRange[id-IDC_MASKRANGE1] = GetDlgItemSpin(hDlg, id, &Bool, NO);
	      	lpChromaMask->SetupChroma();
        }
		break;

    case IDC_MASKMODE_ADD:
    case IDC_MASKMODE_SUBTRACT:
    case IDC_MASKMODE_REVERSE:
	    lpChromaMask->ChromaCombineMode = SHAPE_REVERSE + (id - IDC_MASKMODE_REVERSE);
	    CheckRadioButton(IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,id);
	    break;

    case IDC_DELETEMASK:
	    lpChromaMask->Delete = !lpChromaMask->Delete;
	    CheckDlgButton(id, lpChromaMask->Delete);
        break;
	case IDC_CHROMA:
	    if (hActiveWnd)
        {
			idLastTool = ActivateTool(id);
	        CreateProc(hActiveWnd, 0, 0, this);
	        bPreview = YES;
        }
	    break;

	case IDC_RESET:
	    if ( bPreview )
	    	DeactivateTool();
	    bPreview = NO;
	    break;
    default:
	 	bReturn = FALSE;
	    break;
    }
    return bReturn ? bReturn : CPPViewModalDlg::OnCommand(wParam, lParam);
}


void CChromaDlg::UpdateChromaMask(int i, LPCOLORINFO lpColor)
{
    HWND hDlg 		= GetSafeHwnd();
	long        lColor;
	;

	lpChromaMask->UpdateChromaMask(i, lpColor);
	CopyRGB(&lpChromaMask->MaskColor[i].rgb, &lColor );
	SetWindowLong (::GetDlgItem(hDlg, IDC_MASKCOLOR1+i), GWL_ACTIVE, lColor);
	::InvalidateRect(::GetDlgItem(hDlg, IDC_MASKCOLOR1+i), NULL, FALSE);
}

BOOL CChromaDlg::PreDoModal(LPVOID lpVoid)
{
    return CPPViewModalDlg::PreDoModal(lpVoid);
}

LPVOID CChromaDlg::PostDoModal()
{
	HWND	hDlg = GetSafeHwnd();
	if ( bPreview )
		DeactivateTool();
	if (lpChromaMask)
	{
		delete lpChromaMask;
		lpChromaMask = NULL;
	}
   	QuickProbeEnd( hDlg, -1, -1 );
   	bPreview = NO;

    return CPPViewModalDlg::PostDoModal();
}

void CChromaDlg::OnOK()
{
    PostDoModal();
    if (m_pView->GetSafeHwnd())
    {
    	CHROMA_PARMS   parms;

	    if ( bPreview )
		  	DeactivateTool();
	    bPreview = NO;
        parms.ChromaCombineMode = lpChromaMask->ChromaCombineMode;
        parms.ChromaColorMode   = lpChromaMask->ChromaColorMode  ;
        parms.Fade              = lpChromaMask->Fade             ;
        parms.Mask              = lpChromaMask->Mask             ;
        parms.Delete            = lpChromaMask->Delete           ;
        for (int i=0; i < NUM_CHROMAMASKS; i++)
        {
          	parms.MaskRange[i]      = lpChromaMask->MaskRange[i];
           	parms.MaskColor[i]      = lpChromaMask->MaskColor[i];
        }
        ProcessCommand(GetImage()->lpCmdList, IDS_CMD_CHROMA, &parms);
    }
    CPPViewModalDlg::OnOK();
}

void CChromaDlg::OnCancel()
{
    PostDoModal();
    CPPViewModalDlg::OnCancel();
}

extern void SetMaskHook(HWND hWnd, LPMASKHOOK lpMaskHook);

void CChromaDlg::ShowChromaPreview()
{
    HWND hDlg 		= GetSafeHwnd();
	RECT rMask;
	HWND hActiveWnd;
	LPIMAGE lpImage = GetImage();


   	hActiveWnd = m_pView->GetSafeHwnd();
	if (!hActiveWnd)
		return;

	lpChromaMask->iMode = ImgGetViewMode(lpImage);
   	ImgSetViewMode(lpImage, VM_SLIME);
	if (ImgGetMask(lpImage))
		ImgGetMaskRect(lpImage, &rMask);
	else
		ImgGetSelObjectRect(lpImage, &rMask, NO);

	File2DispRect(hActiveWnd, &rMask, &rMask);

	if (Tool.id == IDC_CHROMA)
		AstralUnionRect(&rMask, &rMask, &rPreview);

	ImgGetMaskRect(lpImage, &rPreview);
	File2DispRect(hActiveWnd, &rPreview, &rPreview);
	SetMaskHook(hActiveWnd, (LPMASKHOOK)lpChromaMask);
	AstralCursor( IDC_WAIT );
	::InvalidateRect(hActiveWnd, &rMask, FALSE);
	AstralUpdateWindow(hActiveWnd);
	ControlEnable(hDlg, IDC_RESET, YES );
	AstralCursor(NULL);
}

void CChromaDlg::ShowChromaReset()
	{
    HWND hDlg 		= GetSafeHwnd();
	HWND hActiveWnd;
	LPIMAGE lpImage = GetImage();


   	hActiveWnd = m_pView->GetSafeHwnd();
	if (!hActiveWnd)
		return;

   	ImgSetViewMode(lpImage, lpChromaMask->iMode);
	SetMaskHook(hActiveWnd, NULL);
	AstralCursor( IDC_WAIT);
	::InvalidateRect(hActiveWnd, &rPreview, FALSE);
	AstralUpdateWindow(hActiveWnd);
	AstralCursor( NULL );
	ControlEnable(hDlg, IDC_RESET, NO);
   }
/**********************************************************************************************
/*			CChromaMask
/**********************************************************************************************/
CChromaMask::CChromaMask(CChromaDlg* pDlg)
{
	int			i;
   	FRMTYPEINFO TypeInfo;
   	BOOL        EnableFade;

	m_pDlg 		= pDlg;
   	ImgGetTypeInfo(pDlg->GetImage(), &TypeInfo);
   	EnableFade = TypeInfo.DataType != FDT_LINEART && TypeInfo.DataType != FDT_PALETTECOLOR;
	ChromaCombineMode = SHAPE_ADD;
   	Fade = EnableFade? 10 : 0;
   	Delete = FALSE;
   	ActiveChromaMask = Mask = 0;
   	ChromaColorMode = IDC_CHROMA_HSL;

   	for (i=0; i < NUM_CHROMAMASKS; i++)
	{
   		MaskRange[i] = 10;
    	MaskColor[i].rgb.red   = 255;
    	MaskColor[i].rgb.green = 255;
    	MaskColor[i].rgb.blue  = 255;
    	RGBtoHSL(0,	0,	0,	&MaskColor[i].hsl);
    	RGBtoCMYK(0,0,	0,	&MaskColor[i].cmyk);
    }
}

CChromaMask::CChromaMask(LPCHROMA_PARMS lpParms)
{
	m_pDlg = NULL;
    ChromaCombineMode = lpParms->ChromaCombineMode;
    ChromaColorMode   = lpParms->ChromaColorMode  ;
    Fade              = lpParms->Fade             ;
    Mask              = lpParms->Mask             ;
    Delete            = lpParms->Delete           ;
    for (int i=0; i < NUM_CHROMAMASKS; i++)
    {
    	MaskRange[i] = lpParms->MaskRange[i];
        MaskColor[i] = lpParms->MaskColor[i];
    }
    SetupChroma();
}

void  CChromaMask::InitMaskValue(BYTE Channel[NUM_CHROMAMASKS][256])
{
	int         i,j;
	for (j = 0; j < NUM_CHROMAMASKS; j++)
		for (i = 0; i < 256; ++i)
	   		Channel[j][i] = 0;
}

void CChromaMask::SetupChroma()
   {
   int   i;

   InitMaskValue(HMask);
   InitMaskValue(LMask);
   InitMaskValue(SMask);
   InitMaskValue(RMask);
   InitMaskValue(GMask);
   InitMaskValue(BMask);
   InitMaskValue(CMask);
   InitMaskValue(YMask);
   InitMaskValue(MMask);
   InitMaskValue(KMask);

 	for (i = 0; i < NUM_CHROMAMASKS; ++i)
      SetupChromaRange(i);

	if (m_pDlg)
		ControlEnable(m_pDlg->GetSafeHwnd(), IDC_CHROMA, Mask);
   }

/************************************************************************/
/*
                        255 max mask
              ****************************<----  100% masked
             *|             |            |*
            * |             |            | *
           *  |           color          |  *
          *   |             |            |   *<--- value of mask in fade area
         *    |             |            |    *
        *     |             |            |     *
|______*_fade_|_____________|____________|_fade_*_______|
0      |      |                          |      |      255
      min   min255                     max255  max         
       |                                        |
       |___________________range________________|

*/


void  CChromaMask::ComputeMaskValue(LPBYTE Channel)
   {
	int         i;
	int         min255, max255;
   int         value;
   int         fade;

   fade  = TORANGE(Fade,  ChromaRange) / 2;

   min255 = min + fade;
   max255 = max - fade;
	for (i = 0; i < 256; ++i)
      {
      if (i >= min && i <= max)
         {
         if (i >= min255 && i <= max255)
            value = 255;
         else
            value = (i < min255? i - min: max - i) * 255L / fade;
	      Channel[i] = max(Channel[i], value);
         }
      }
   }

void CChromaMask::ComputeChromaMask(int i)
{
	BOOL On = Mask & (1 << i);

   	if (!On)
    	return;

   	if (ChromaColorMode == IDC_CHROMA_HSL || ChromaColorMode == IDC_CHROMA_LUMIN)
    {
	   	min = min(MinMaxH[0], MinMaxH[1]);
	   	max = max(MinMaxH[0], MinMaxH[1]);
      	ComputeMaskValue(&HMask[i][0]);

	   	min = min(MinMaxS[0], MinMaxS[1]);
	   	max = max(MinMaxS[0], MinMaxS[1]);
      	ComputeMaskValue(&SMask[i][0]);

	   	min = min(MinMaxL[0], MinMaxL[1]);
	   	max = max(MinMaxL[0], MinMaxL[1]);
      	ComputeMaskValue(&LMask[i][0]);
    }
   	else
    {
	   	min = min(MinMaxR[0], MinMaxR[1]);
	   	max = max(MinMaxR[0], MinMaxR[1]);
      	ComputeMaskValue(&RMask[i][0]);

	   	min = min(MinMaxG[0], MinMaxG[1]);
	   	max = max(MinMaxG[0], MinMaxG[1]);
      	ComputeMaskValue(&GMask[i][0]);

	   	min = min(MinMaxB[0], MinMaxB[1]);
	   	max = max(MinMaxB[0], MinMaxB[1]);
      	ComputeMaskValue(&BMask[i][0]);

	   	min = min(MinMaxC[0], MinMaxC[1]);
	   	max = max(MinMaxC[0], MinMaxC[1]);
      	ComputeMaskValue(&CMask[i][0]);

	   	min = min(MinMaxM[0], MinMaxM[1]);
	   	max = max(MinMaxM[0], MinMaxM[1]);
      	ComputeMaskValue(&MMask[i][0]);

	   	min = min(MinMaxY[0], MinMaxY[1]);
	   	max = max(MinMaxY[0], MinMaxY[1]);
      	ComputeMaskValue(&YMask[i][0]);

	   	min = min(MinMaxK[0], MinMaxK[1]);
	   	max = max(MinMaxK[0], MinMaxK[1]);
    	ComputeMaskValue(&KMask[i][0]);
	}
}


void CChromaMask::SetupChromaRange(int i)
{
	int         range;
	COLORINFO   Color;
   	BOOL        On = Mask & (1 << i);

   	if (!On)
    	return;

	Color       = MaskColor[i];
	ChromaRange = TOGRAY(MaskRange[i]);
   	range       = ChromaRange / 2;

   	if (ChromaColorMode == IDC_CHROMA_HSL || ChromaColorMode == IDC_CHROMA_LUMIN)
    {
		MinMaxH[0] = (int)Color.hsl.hue - range;
	   	MinMaxS[0] = (int)Color.hsl.sat - range;
	   	MinMaxL[0] = (int)Color.hsl.lum - range;

	   	MinMaxH[1] = (int)Color.hsl.hue + range;
	   	MinMaxS[1] = (int)Color.hsl.sat + range;
	   	MinMaxL[1] = (int)Color.hsl.lum + range;
	}
   	else
    {
	   	MinMaxR[0] = (int)Color.rgb.red   - range;
	   	MinMaxG[0] = (int)Color.rgb.green - range;
	   	MinMaxB[0] = (int)Color.rgb.blue  - range;

	   	MinMaxR[1] = (int)Color.rgb.red   + range;
	   	MinMaxG[1] = (int)Color.rgb.green + range;
	   	MinMaxB[1] = (int)Color.rgb.blue  + range;

   		MinMaxC[0] = (int)Color.cmyk.c - range;
	   	MinMaxM[0] = (int)Color.cmyk.m - range;
	   	MinMaxY[0] = (int)Color.cmyk.y - range;
	   	MinMaxK[0] = (int)Color.cmyk.k - range;

	   	MinMaxC[1] = (int)Color.cmyk.c + range;
	   	MinMaxM[1] = (int)Color.cmyk.m + range;
	   	MinMaxY[1] = (int)Color.cmyk.y + range;
	   	MinMaxK[1] = (int)Color.cmyk.k + range;
    }
	ComputeChromaMask(i);
}


void CChromaMask::UpdateChromaMask(int i, LPCOLORINFO lpColor)
{
   if (lpColor)
      MaskColor[i] = *lpColor;

   SetupChroma();
}

int CChromaMask::GetRGBMask(LPTR lpLine)
{
	int   R,G,B;
   	int   Mask1;
   	int   v,i,Final;

   	R = *lpLine++;
   	G = *lpLine++;
   	B = *lpLine++;
	Final = 0;
   	if (ChromaColorMode == IDC_CHROMA_HSL || ChromaColorMode == IDC_CHROMA_LUMIN)
	{
    	HSLS  HSL;

      	RGBtoHSL(R,	G,	B,	&HSL);
		for (i = 0; i < NUM_CHROMAMASKS; i++)
		{
			Mask1 = 255;
			if (!(Mask & (1 << i)))
				continue;

      		if (ChromaColorMode != IDC_CHROMA_LUMIN)
         	{
         		v = HMask[i][HSL.hue];
         		if (v < Mask1)
            		Mask1 = v;
         		v = SMask[i][HSL.sat];
         		if (v < Mask1)
            		Mask1 = v;
         	}
      		v = LMask[i][HSL.lum];
      		if (v < Mask1)
         		Mask1 = v;
			if (Mask1 > Final)
				Final = Mask1;
		}
    }
   	else
   	{
		for (i = 0; i < NUM_CHROMAMASKS; i++)
		{
			Mask1 = 255;
			if (!(Mask & (1 << i)))
				continue;

      		v = RMask[i][R];
      		if (v < Mask1)
         		Mask1 = v;
      		v = GMask[i][G];
      		if (v < Mask1)
         		Mask1 = v;
      		v = BMask[i][B];
      		if (v < Mask1)
         		Mask1 = v;
			if (Mask1 > Final)
				Final = Mask1;
		}
    }
   	return(Final);
}

int CChromaMask::GetCMYKMask(LPTR lpLine)
{
	int   C, M, Y, K;
   	int   Mask1 = 255;
   	int   v,i,Final;

   	C = *lpLine++;
   	M = *lpLine++;
   	Y = *lpLine++;
   	K = *lpLine++;
	Final = 0;

	for (i = 0; i < NUM_CHROMAMASKS; i++)
	{
		Mask1 = 255;
		if (!(Mask & (1 << i)))
			continue;

   		v = CMask[i][C];
   		if (v < Mask1)
      		Mask1 = v;
   		v = MMask[i][M];
   		if (v < Mask1)
      		Mask1 = v;
   		v = YMask[i][Y];
   		if (v < Mask1)
      		Mask1 = v;
   		v = KMask[i][K];
   		if (v < Mask1)
      		Mask1 = v;
		if (Mask1 > Final)
			Final = Mask1;
	}
   	return(Final);
}

void CChromaMask::ChromaProc(int dx, FRMTYPEINFO TypeInfo,	LPTR lpLine, LPTR lpSlime, BOOL MaskExist)
   {
   int      i = 0;
   RGBS     RGB;
   int      Mask;
   int      iMode = ChromaCombineMode;
   BOOL     b = FALSE;

   if (!MaskExist && iMode == SHAPE_ADD)
      iMode = SHAPE_SUBTRACT;

	switch (TypeInfo.DataType)
      {
		case FDT_PALETTECOLOR:
         while (i++ < dx)
            {
            RGB = TypeInfo.ColorMap->RGBData[*lpLine++];
            Mask = GetRGBMask((LPTR)&RGB);
            if (ChromaCombineMode == SHAPE_SUBTRACT)
               Mask ^= 0xFF;
            b = iMode == SHAPE_ADD?Mask > *lpSlime : Mask < *lpSlime;
            if (b)
               {
               Dirty = TRUE;
               *lpSlime = Mask;
               if (i < xExtL)
                  xExtL = i;
               if (i > xExtR)
                  xExtR = i;
               }
            lpSlime++;
            }
         break;

		case FDT_LINEART:
		case FDT_GRAYSCALE:
         while (i++ < dx)
            {
            RGB.red = RGB.green = RGB.blue = *lpLine++;
            Mask = GetRGBMask((LPTR)&RGB);
            if (ChromaCombineMode == SHAPE_SUBTRACT)
               Mask ^= 0xFF;
            b = iMode == SHAPE_ADD?Mask > *lpSlime : Mask < *lpSlime;
            if (b)
               {
               Dirty = TRUE;
               *lpSlime = Mask;
               if (i < xExtL)
                  xExtL = i;
               if (i > xExtR)
                  xExtR = i;
               }
            lpSlime++;
            }
         break;

		case FDT_RGBCOLOR:
         while (i++ < dx)
            {
            Mask = GetRGBMask(lpLine);
            if (ChromaCombineMode == SHAPE_SUBTRACT)
               Mask ^= 0xFF;
            b = iMode == SHAPE_ADD?Mask > *lpSlime : Mask < *lpSlime;
            if (b)
               {
               Dirty = TRUE;
               *lpSlime = Mask;
               if (i < xExtL)
                  xExtL = i;
               if (i > xExtR)
                  xExtR = i;
               }
            lpSlime++;
            lpLine += 3;     
            }
         break;

		case FDT_CMYKCOLOR:
         while (i++ < dx)
            {
            if (ChromaColorMode == IDC_CHROMA_HSL || ChromaColorMode == IDC_CHROMA_LUMIN)
               {
               CMYKtoRGB(lpLine[0], lpLine[1], lpLine[2], lpLine[3], &RGB);
               Mask = GetRGBMask((LPTR)&RGB);
               }
            else
               Mask = GetCMYKMask(lpLine);
            if (ChromaCombineMode == SHAPE_SUBTRACT)
               Mask ^= 0xFF;
            b = iMode == SHAPE_ADD?Mask > *lpSlime : Mask < *lpSlime;
            if (b)
               {
               Dirty = TRUE;
               *lpSlime = Mask;
               if (i < xExtL)
                  xExtL = i;
               if (i > xExtR)
                  xExtR = i;
               }
            lpSlime++;
            lpLine += 4;
            }
         break;
      default:
         break;
      }
   }

void CChromaMask::MaskAddChroma(LPIMAGE lpImage, LPMASK lpMask, LPRECT lpRect, BOOL MaskExist)
{
	int         dx, dy, i, depth;
	LPTR        lpAlphaLine;
	LPTR        lpLine;
	LPOBJECT    lpBase = ImgGetBase(lpImage);
   	int         top = -1;
   	int         bottom = 0;
   	FRMTYPEINFO TypeInfo;

	ProgressBegin(1,PROGRESS_ID(IDS_CHROMA_APPLY));
	PixmapGetInfo(&lpMask->Pixmap, PMT_EDIT, &dx, &dy, NULL, NULL);
	
   	ImgGetTypeInfo(lpImage, &TypeInfo);
   	if (!(depth = FrameDepth(ObjGetEditFrame(lpBase))))
    	depth = 1; // never worry about lineart
	
   	xExtL = 10000;
   	xExtR = 0;
   	lpLine = Alloc((long)dx*depth);
	for (i = 0; i < dy; ++i)
	{
    	AstralClockCursor(i, dy, NO);
      	Dirty = FALSE;
		lpAlphaLine = PixmapPtr( &lpMask->Pixmap, PMT_EDIT, 0, i, YES);
      	if (Delete)
			clr(lpAlphaLine, dx);
      	ImgGetLine(lpImage, lpBase, 0, i, dx, lpLine, NULL);
      	ChromaProc(dx, TypeInfo, lpLine, lpAlphaLine, MaskExist);
      	if (Dirty)
        {
        	if (top == -1)
            	top = i;
         	if (i > bottom)
            bottom = i;
        }
	}
	lpRect->left   = xExtL;
   	lpRect->right  = xExtR;
   	lpRect->top    = top;
   	lpRect->bottom = bottom;
   	FreeUp(lpLine);
	ProgressEnd();
}

BOOL CChromaMask::JoinChromaMask(LPIMAGE lpImage, LPRECT lpUpdateRect)
{
	LPOBJECT    lpBase;
	LPMASK      lpMask;
	RECT        rDummy;
   	RECT        rMask;
	BOOL        fNewMask = NO;
   	BOOL        fCreateMask = NO;
   	UNDO_TYPE   Type;

   	AstralSetRectEmpty(lpUpdateRect);
   	if (ImgGetMask(lpImage))
    	Type = UT_ALPHA;
   	else
    {
    	fNewMask = TRUE;
      	Type = UT_CREATEMASK;
    }
         
	// get mask, create one if one does not exist
	lpMask = ImgGetMaskEx(lpImage, iMode==SHAPE_SUBTRACT ? OFF : ON, 
            Control.NoUndo, &fNewMask, lpUpdateRect);
	if (!lpMask)
		return(FALSE);

	// if not new mask and we are allowed to create undo, initialize undo
	lpBase = ImgGetBase(lpImage);
 	if (!ImgEditInit(lpImage, ET_OBJECT,Type, lpBase))
		return(FALSE);

	EnableMarqueeEx( lpImage, NO );

	// add Chroma to mask
	MaskAddChroma(lpImage, lpMask, &rMask, !fNewMask);
	AstralUnionRect(lpUpdateRect, lpUpdateRect, &rMask);

	// if using undo, indicate in-place editing has occurred
	ImgEditedObject(lpImage, lpBase, IDS_CHROMA_APPLY,	fNewMask ? NULL : &rMask);

	// after subtractive masks we dont know area
	if (iMode!=SHAPE_ADD)
		MaskRectUpdate(lpMask, &rDummy);

	// get area that needs to be redisplayed, may be larger
	// than rUpdate because of slime
	if (ImgGetMaskUpdateRect(lpImage, fNewMask, NO, &rMask))
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rMask);

	EnableMarqueeEx(lpImage, YES);
	return(TRUE);
}

/***********************************************************************************************
/*										Global Functions
/***********************************************************************************************/
BOOL ChromaMaskImage(LPIMAGE lpImage, LPCHROMA_PARMS lpParms)
{
	RECT rUpdate;
   	CChromaMask*		lpData = new CChromaMask(lpParms);

   	if (lpImage)
    {
    	if (lpData->JoinChromaMask(lpImage, &rUpdate))
	  	{
			lpParms->Common.UpdateType = UT_ACTIVEAREA;
			lpParms->Common.rUpdateArea = rUpdate;
	  	}
    }
	delete lpData;
   	return(TRUE);
}

void ShowChromaProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
   	{
	switch (msg)
		{
		case WM_CREATE:
			pChromaDlg = (CChromaDlg*)Option;
			pChromaDlg->ShowChromaPreview();
			Tool.bActive = YES;
		   	break;

		case WM_LBUTTONDBLCLK:
		case WM_DESTROY:	// The cancel operation message
			pChromaDlg->ShowChromaReset();
			Tool.bActive = NO;
			ActivateTool(pChromaDlg->idLastTool);
		   	break;

		default:
		   	break;
	   }
	}







