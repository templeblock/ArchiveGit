//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "visual.h"
#include "framelib.h"

//*****************************Static Prototypes***********************************
static void SetupFrames(HWND hDlg, LPVISUAL lpVisual);
static BOOL Move2DDirectional(HWND hDlg, int x, int y, LPVISUAL lpVisual);
static BOOL MoveCenterOnly(HWND hDlg, int x, int y, LPVISUAL lpVisual);
static void UpdateVisual(HWND hDlg,int id);
static HWND GetExtraWindow(HWND hParent, int id, int n);

//***********************************************************************
//***************************FUNCTIONS*************************************
//***********************************************************************

//***********************************************************************
//	Pass all messages that the dlg dosen't handle to this routine.
//	Also pass WM_INITDIALOG, WM_CLOSE etc.

/************************************************************************/
BOOL VisualMap_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam,
							LPVISUAL lpVisual)
/************************************************************************/
{
lpVisual->lpBasisFrame=NULL;
SetupFrames(hDlg, lpVisual);
SetDlgItemIntExt(hDlg,IDC_VSTEP_TEXT,lpVisual->step," %");
InitSlide(hDlg, IDC_VSTEP, lpVisual->step, 1, 25);
SetVisual(hDlg, 0, 0, lpVisual);
return(TRUE);
}

/************************************************************************/
void VisualMap_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify,
							LPVISUAL lpVisual)
/************************************************************************/
{
int x, y;

switch (id)
	{
	case IDC_VSTEP:
   	lpVisual->step = HandleSlide (hDlg,id,codeNotify,NULL);
   	SetDlgItemIntExt(hDlg,IDC_VSTEP_TEXT,lpVisual->step," %");
   	SetSlide (hDlg,id,lpVisual->step);
		if (codeNotify == SN_VALUESELECTED)
			SetVisual(hDlg, 0, 0, lpVisual);
	break;
	case IDC_RESET:
		id = IDC_VISUAL_BEFORE;
	// Fall through.
	case IDC_VISUAL_BEFORE:
	case IDC_VISUAL_00:	
	case IDC_VISUAL_01:	
	case IDC_VISUAL_02:
	case IDC_VISUAL_10:	
	case IDC_VISUAL_11:	
	case IDC_VISUAL_12:
	case IDC_VISUAL_20:	
	case IDC_VISUAL_21:	
	case IDC_VISUAL_22:
		if (GetPosFromID(id, &x,&y))
			MoveVisual(hDlg, x,y,lpVisual);
	break;
   default:
	break;
   }
}

/************************************************************************/
void VisualMap_OnDestroy(HWND hDlg, LPVISUAL lpVisual)
/************************************************************************/
{
if (lpVisual->lpBasisFrame)
	{
	FrameClose(lpVisual->lpBasisFrame);
	lpVisual->lpBasisFrame = NULL;
	}
if (lpVisual->lpWorkFrame)
	{
	FrameClose(lpVisual->lpWorkFrame);
	lpVisual->lpWorkFrame = NULL;
	}
}

//***********************************************************************
//	Call when the calling dlg wants to change the posion of the visual
//		map control. Only those image which need to be re-calculated 
//		will be.
//***********************************************************************
BOOL MoveVisual(HWND hDlg, int x, int y, LPVISUAL lpVisual)
//***********************************************************************
{
	int i;
	
	if (x == VISUAL_BEFORE || !WITHIN(x,-1,1) || !WITHIN(y, -1, 1))
		return(SetVisual(hDlg, x, y, lpVisual));
	switch(lpVisual->moveType)
	{
		case 1:
			return(Move2DDirectional(hDlg, x, y, lpVisual));
		case 2:
			return(MoveCenterOnly(hDlg, x, y, lpVisual));
	}
	// assume update all 
	AstralCursor(IDC_WAIT);
	if (!(lpVisual->pMoveVisual)(x,y,lpVisual))
	{
		AstralCursor(NULL);
		return(FALSE);
	}
	for (i=IDC_VISUAL_00; i<=IDC_VISUAL_22; i++)
		UpdateVisual(hDlg,i);
	AstralCursor(NULL);
	return(TRUE);
	
}


//***********************************************************************
//	Call when the calling dlg wants to set/reset the visual
//		map control. Will redraw all image previews.
//***********************************************************************
BOOL SetVisual(HWND hDlg, int x, int y, LPVISUAL lpVisual)
//***********************************************************************
{
	int i;
	
	AstralCursor(IDC_WAIT);
	if (!(lpVisual->pMoveVisual)(x,y,lpVisual))
	{
		AstralCursor(NULL);
		return(FALSE);
	}
	for (i=IDC_VISUAL_BEFORE; i<=IDC_VISUAL_22; i++)
		UpdateVisual(hDlg,i);
	AstralCursor(NULL);
	return(TRUE);
}




//***********************************************************************
//	Returns true if sucessful
//***********************************************************************
BOOL GetPosFromID(ITEMID id, LPINT lpx, LPINT lpy)   
//***********************************************************************
{
	switch (id)
	{
		case IDC_VISUAL_BEFORE:
			*lpx = *lpy = VISUAL_BEFORE;
		break;
		case IDC_VISUAL_00:	
		case IDC_VISUAL_01:	
		case IDC_VISUAL_02:
			*lpx = id-IDC_VISUAL_01;
			*lpy = -1;
		break;
		case IDC_VISUAL_10:	
		case IDC_VISUAL_11:	
		case IDC_VISUAL_12:
			*lpx = id-IDC_VISUAL_11;
			*lpy = 0;
		break;
		case IDC_VISUAL_20:	
		case IDC_VISUAL_21:	
		case IDC_VISUAL_22:
			*lpx = id-IDC_VISUAL_21;
			*lpy = 1;
		break;
		default:
			return(FALSE);
	}
	return(TRUE);
}		


//***********************************************************************
//	Returns 0 if failure
//***********************************************************************
int GetIDFromPos(int x, int y)
//***********************************************************************
{
	if (x==VISUAL_BEFORE)
		return(IDC_VISUAL_BEFORE);
	if (x<-1 || x>1)
		return(0);
	switch (y)
	{
		break;
		case -1:	
			return(IDC_VISUAL_01+x);
		case 0:	
			return(IDC_VISUAL_11+x);
		case 1:	
			return(IDC_VISUAL_21+x);
		default:
			return(0);
	}
	return(0);
}		



//***********************************************************************
//	Sets up the extra frames 
//***********************************************************************
static void SetupFrames(HWND hDlg, LPVISUAL lpVisual)
//***********************************************************************
{
	LFIXED 	rate, line;
	LPTR	lpSrc, lpDst;
	int 	yread;
	int 	x,y,depth, lin, pix, id, width;
	HWND 	hControl;
	LONG 	size;
	BOOL 	fUseImage;
	FRMTYPEINFO TypeInfo;
	LPTR lpLineBuf;
	LPIMAGE lpImage;

	lpImage = GetActiveImage();

	// set lpVisual for all windows
	for (id = IDC_VISUAL_BEFORE; id <= IDC_VISUAL_22; id++)
		if (hControl=GetDlgItem(hDlg, id))
			SetWindowLong( hControl, GWL_LPVISUAL,(LONG)lpVisual);
	// there may be 2 center panes 
	if (hControl = GetExtraWindow(hDlg,IDC_VISUAL_11,2))
		SetWindowLong( hControl, GWL_LPVISUAL,(LONG)lpVisual);
		
	// Check if frames setup
	fUseImage = (lpVisual->lpImage != NULL);
	if (!lpVisual->lpOrgFrame && !fUseImage)
	{
		lpVisual->lpBasisFrame=NULL;
		return;
	}
	if (fUseImage)
		lpVisual->lpOrgFrame = ImgGetBaseEditFrame(lpVisual->lpImage);
	size = 0;
	for (id = IDC_VISUAL_BEFORE; id <= IDC_VISUAL_22; id++)
		if (hControl=GetDlgItem(hDlg, id))
			if (size = SendMessage(hControl, WM_IMAGESPACE, 0, 0L))
				break;
	if (!size)
		return;
	pix = Max(LOWORD(size) - 10, 1);
	lin = Max(HIWORD(size) - 10, 1);
	
	TypeInfo = FrameTypeInfo(lpVisual->lpOrgFrame);
	depth = FrameDepth(lpVisual->lpOrgFrame);
	width = FrameXSize(lpVisual->lpOrgFrame);
	if (!lpVisual->lpBasisFrame)
	{
		// Create basis frame
		x = FrameXSize(lpVisual->lpOrgFrame);
		y = FrameYSize(lpVisual->lpOrgFrame);
		ScaleToFit(&pix, &lin, x,y);
		lin = Max(1, lin);
		pix = Max(1, pix);	
		rate = FGET(x, pix);
		// get new frame 
		lpVisual->lpBasisFrame = FrameOpen(TypeInfo, 
				   		pix, 
				   		lin,
				   		FrameResolution(lpVisual->lpOrgFrame));
		if (!lpVisual->lpBasisFrame)
			return;
		// setup buffers for frame sampling
		lpLineBuf=NULL;
		if (fUseImage)
			if (!(lpSrc=lpLineBuf=Alloc(depth*width)))
				fUseImage = FALSE;
		// sample frame
		line = 0L;
		yread = 0;
		for (y=0;y<lin;y++)
		{
			yread = WHOLE(line);
			lpDst = FramePointer(lpVisual->lpBasisFrame, 0,y,YES);
			if (fUseImage)
				ImgGetLine(lpImage,NULL,0,yread,width,lpSrc);
			else
				lpSrc = FramePointer(lpVisual->lpOrgFrame, 0,yread,NO);
			if (!lpSrc || !lpDst)
			{
				FrameClose(lpVisual->lpBasisFrame);
				lpVisual->lpBasisFrame=NULL;
				return;
			}
			FrameSample(lpVisual->lpBasisFrame, lpSrc, 0, lpDst, 0, pix, rate);
			line += rate; 
		} 	// for y
		// free up buffers 
		if (lpLineBuf) FreeUp(lpLineBuf);
	} // if !lpBasisFrame
	// make copy for lpWorkFrame
	lpVisual->lpWorkFrame = FrameOpen(
					TypeInfo, 
					pix, 
					lin,
				   	FrameResolution(lpVisual->lpOrgFrame));
}


//***********************************************************************
//	Call when the calling dlg wants to change the posion of the visual
//		map control. Only those image which need to be re-calculated 
//		will be. Moves images like a 2-d joystick.
//***********************************************************************
static BOOL Move2DDirectional(HWND hDlg, int x, int y, LPVISUAL lpVisual)
//***********************************************************************
{
	int id,dx, dy,xpos, ypos, xstart;
//	LPFRAME lpDstFrame, lpSrcFrame; 
	
	if (x == VISUAL_BEFORE || !WITHIN(x,-1,1) || !WITHIN(y, -1, 1))
		return(SetVisual(hDlg, x, y, lpVisual));
	AstralCursor(IDC_WAIT);
	if (!(lpVisual->pMoveVisual)(x,y,lpVisual))
	{
		AstralCursor(NULL);
		return(FALSE);
	}
	if (!x && !y)
	{
		AstralCursor(NULL);
		return(TRUE);
	}
	xstart = xpos = x ? -x:-1;
	ypos = y ? -y:-1;
	dx = -xpos;
	dy = -ypos;
	for (;WITHIN(ypos,-1,1); ypos+=dy)
		for (xpos = xstart;WITHIN(xpos,-1,1); xpos+=dx)
		{
			if (!(id = GetIDFromPos(xpos,ypos)))
				continue;
//			if (!(hWndDst = GetDlgItem(hDlg,id)))
//				continue;
//			if (!(lpDstFrame=(LPFRAME)GetWindowLong(hWndDst, GWL_IMAGE)))
//				continue;
//			hWndSrc = NULL;
//			if (id = GetIDFromPos(xpos+x,ypos+y))
//				hWndSrc = GetDlgItem(hDlg,id);
//			if (hWndSrc && (lpSrcFrame=(LPFRAME)GetWindowLong(hWndSrc, GWL_IMAGE)))
//			{
//				// swap frames
//				SetWindowLong(hWndDst, GWL_IMAGE, (LONG)lpSrcFrame);
//				SetWindowLong(hWndSrc, GWL_IMAGE, (LONG)lpDstFrame);
//			}
//			else // can not swap
			// draw the new window
			UpdateVisual(hDlg,id);
		}  // for x pos	
	AstralCursor(NULL);
	return(TRUE);
}

//***********************************************************************
//	Call when the calling dlg wants to change the posion of the visual
//		map control. The selected image will become the center image.
//		All other images will be re-calculated.
//***********************************************************************
static BOOL MoveCenterOnly(HWND hDlg, int x, int y, LPVISUAL lpVisual)
//***********************************************************************
{
	int id, idDone;
	
	if (x == VISUAL_BEFORE || !WITHIN(x,-1,1) || !WITHIN(y, -1, 1))
		return(SetVisual(hDlg, x, y, lpVisual));
	AstralCursor(IDC_WAIT);
	if (!(lpVisual->pMoveVisual)(x,y,lpVisual))
	{
		AstralCursor(NULL);
		return(FALSE);
	}
	// swap selected with center
	idDone = 0;
//	if (id = GetIDFromPos(x,y))
//		if (hWndSrc = GetDlgItem(hDlg,id))
//			if (lpSrcFrame=(LPFRAME)GetWindowLong(hWndSrc, GWL_IMAGE))
//				if (hWndDst = GetDlgItem(hDlg,IDC_VISUAL_11))
//				 	if (lpDstFrame=(LPFRAME)GetWindowLong(hWndDst, GWL_IMAGE))
//					{
//						// swap frames
//						SetWindowLong(hWndDst, GWL_IMAGE, (LONG)lpSrcFrame);
//						SetWindowLong(hWndSrc, GWL_IMAGE, (LONG)lpDstFrame);
//						idDone = IDC_VISUAL_11;
//					}
//	// assume update all except idDone
	for (id=IDC_VISUAL_00; id<=IDC_VISUAL_22; id++)
		UpdateVisual(hDlg,id);
	AstralCursor(NULL);
	return(TRUE);
	
}

//***********************************************************************
//	Gets the n'th (if any) child window with the given id.
//	Returns NULL for failure.
//***********************************************************************
static HWND GetExtraWindow(HWND hParent, int id, int n)
//***********************************************************************
{
	HWND hWnd;
	
	hWnd = GetDlgItem(hParent, id);
	n--;
	while (n)
	{
		hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
		if (!hWnd)
			break;
		if (GetDlgCtrlID(hWnd) == id)
			n--;
	}
	return(hWnd);
}

//***********************************************************************
//	Updates the visual element with given id  in the dlg.
//***********************************************************************
static void UpdateVisual(HWND hDlg,int id)
//***********************************************************************
{
	HWND hWnd;
	int n;
	
	n = 1;
	if (id == IDC_VISUAL_11)
		n =2;
	while (n)
	{
		hWnd = GetExtraWindow(hDlg,id,n--);
		if (hWnd)
		{
			InvalidateRect(hWnd, NULL, YES);
			UpdateWindow(hWnd);
		}
	}
}


