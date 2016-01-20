/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                viewcmd2.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include "afxodlgs.h"
#include "id.h"
#include "svrdoc.h"
#include "dataitem.h"
#include "svritem.h"
#include "svrview.h"
#include "ipframe.h"
#include "cmdundo.h"
#include "paledit.h"
#include "macro.h"
#include "ppmodal.h"
#include "dlgsmap.h"
#include "dlgrotim.h"
#include "dlgexpim.h"
#include "dlgsize.h"
#include "calib.h"
#include "dlgtone.h"
#include "dlghisto.h"
#include "dlgchrma.h"
#include "dlgobjs.h"
#include "dlgmskld.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/******************************* EXPORTED Functions *****************************/
extern BOOL WINPROC EXPORT DlgChromaProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern void OnStatusPalette(int id);

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */
/* ********************************** Macros ******************************** */

/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
/* ******************************* Local Functions **************************** */
/* ******************************* Local Methods **************************** */

afx_msg void CServerView::OnUpdateRotate90(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_ROTATE90, this) );
}

afx_msg void CServerView::OnUpdateRotate180(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_ROTATE180, this) );
}

afx_msg void CServerView::OnUpdateRotate270(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_ROTATE270, this) );
}

afx_msg void CServerView::OnUpdateRotateAny(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_ROTATEANY, this) );
}

afx_msg void CServerView::OnUpdateObjLock(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_OBJLOCK, this) );
}

afx_msg void CServerView::OnUpdateObjUnLock(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_OBJUNLOCK, this) );
}

afx_msg void CServerView::OnUpdateObjGroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_GROUPOBJECTS, this) );
}

afx_msg void CServerView::OnUpdateObjUnGroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_UNGROUPOBJECTS, this) );
}

afx_msg void CServerView::OnUpdatePrintPreviewEnabled(CCmdUI* pCmdUI)
{
	CImage *pImage = GetDocument()->GetImage();
	pCmdUI->Enable( pImage && Control.CMSEnabled  );
}

afx_msg void CServerView::OnUpdateFeather(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_FEATHERMASK, this) );
}

afx_msg void CServerView::OnUpdateRemoveHoles(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_REMOVE_HOLES, this) );
}

afx_msg void CServerView::OnUpdateMaskSmooth(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MASK_SMOOTH, this) );
}

afx_msg void CServerView::OnUpdateObjAlign(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_OBJECTALIGN, this) );
}

afx_msg void CServerView::OnUpdateObjFeather(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_FEATHEROBJECT, this) );
}

afx_msg void CServerView::OnUpdatePosition(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_OBJECTPOS, this) );
}

afx_msg void CServerView::OnUpdateCombineObj(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_OBJCOMBOBJS, this) );
}

afx_msg void CServerView::OnUpdateCombine(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_COMBINE, this) );
}

afx_msg void CServerView::OnUpdateCombineSel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_COMBINESEL, this) );
}

afx_msg void CServerView::OnUpdateSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_SELECTOBJECTS, this) );
}

afx_msg void CServerView::OnUpdateSelectInvert(CCmdUI* pCmdUI)
{
	// !!! DANGER, DANGER, this command is not in the command list!!!
	CImage *pImage = GetDocument()->GetImage(); 
	pCmdUI->Enable(	pImage->MultipleObjects() );
}

afx_msg void CServerView::OnUpdateDownLevel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_ORDERDOWN, this) );
}

afx_msg void CServerView::OnUpdateUpLevel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_ORDERUP, this) );
}

afx_msg void CServerView::OnUpdateMoveToBottom(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_ORDERTOBOTTOM, this) );
}

afx_msg void CServerView::OnUpdateMoveToTop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_ORDERTOTOP, this) );
}

afx_msg void CServerView::OnUpdateApplyMask(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MERGEMASK, this) );
}

afx_msg void CServerView::OnUpdateObjDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_DELETEOBJECTS, this) );
}

afx_msg void CServerView::OnUpdateMaskFromObject(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MASKFROMOBJECT, this) );
}

afx_msg void CServerView::OnUpdateObjectFromMask(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_OBJECTFROMMASK, this) );
}

afx_msg void CServerView::OnUpdateChroma(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CHROMA, this) );
}

afx_msg void CServerView::OnUpdateSizeit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_SIZE, this) );
}

afx_msg void CServerView::OnUpdateExpand(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_EXPAND, this) );
}

afx_msg void CServerView::OnUpdateMirrorX(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MIRRORX, this) );
}

afx_msg void CServerView::OnUpdateMirrorY(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MIRRORY, this) );
}

afx_msg void CServerView::OnUpdateMirrorXY(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MIRRORXY, this) );
}

afx_msg void CServerView::OnUpdateSpecialFx(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_EFFECT, this) );
}

afx_msg void CServerView::OnUpdateInvert(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_INVERT, this) );
}

afx_msg void CServerView::OnUpdateSplitRgb(CCmdUI* pCmdUI)
{
	// !!! DANGER, DANGER, this command is not in the command list!!!
	BOOL state = IsCmdEnabled(0, this);

	if (state)
	{
		CImage *pImage = GetDocument()->GetImage();
	    BOOL fNotSplit = (pImage->DocumentType & 0xFF00) == IMG_NOSPLIT;
		FRMTYPEINFO TypeInfo;
		pImage->GetTypeInfo(&TypeInfo);
		state = (TypeInfo.DataType == FDT_RGBCOLOR && fNotSplit ? 
			TRUE : FALSE);

	}
	pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateSplitHsl(CCmdUI* pCmdUI)
{
	// !!! DANGER, DANGER, this command is not in the command list!!!
	BOOL state = IsCmdEnabled(0, this);

	if (state)
	{
		CImage *pImage = GetDocument()->GetImage(); 
	    BOOL fNotSplit = (pImage->DocumentType & 0xFF00) == IMG_NOSPLIT;
		FRMTYPEINFO TypeInfo;
		pImage->GetTypeInfo(&TypeInfo);
		state = (TypeInfo.DataType == FDT_RGBCOLOR && fNotSplit ? 
			TRUE : FALSE);
	}
	
	pCmdUI->Enable(	state);
}

afx_msg void CServerView::OnUpdateSplitCmyk(CCmdUI* pCmdUI)
{
	// !!! DANGER, DANGER, this command is not in the command list!!!
	BOOL state = IsCmdEnabled(0, this);

	if (state)
	{
		CImage *pImage = GetDocument()->GetImage(); 
	    BOOL fNotSplit = (pImage->DocumentType & 0xFF00) == IMG_NOSPLIT;
		FRMTYPEINFO TypeInfo;
		pImage->GetTypeInfo(&TypeInfo);
		state = (TypeInfo.DataType == FDT_CMYKCOLOR && fNotSplit ? 
			TRUE : FALSE);

	}
	pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateChannelCombine(CCmdUI* pCmdUI)
{
	// !!! DANGER, DANGER, this command is not in the command list!!!
	BOOL state = IsCmdEnabled(0, this);

	if (state)
	{
		CImage *pImage = GetDocument()->GetImage(); 
	    BOOL fNotSplit = (pImage->DocumentType & 0xFF00) == IMG_NOSPLIT;
		state = (fNotSplit ?  FALSE : TRUE);
	}
	pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateConvLa(CCmdUI* pCmdUI)
{
	CONVERT_PARMS parms;
	parms.cmsInfo.src.DataType = FDT_NONE;
	parms.cmsInfo.dst.DataType = FDT_LINEART;

	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CONVERT, this, (LPCMD_PARMS)&parms) );
}

afx_msg void CServerView::OnUpdateConvSp(CCmdUI* pCmdUI)
{
	CONVERT_PARMS parms;
	parms.cmsInfo.src.DataType = FDT_NONE;
	parms.cmsInfo.dst.DataType = FDT_LINEART;

	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CONVERT, this, (LPCMD_PARMS)&parms) );
}
 
afx_msg void CServerView::OnUpdateConvCt(CCmdUI* pCmdUI)
{
	CONVERT_PARMS parms;
	parms.cmsInfo.src.DataType = FDT_NONE;
	parms.cmsInfo.dst.DataType = FDT_GRAYSCALE;

	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CONVERT, this, (LPCMD_PARMS)&parms) );
}
 
afx_msg void CServerView::OnUpdateConvPalette(CCmdUI* pCmdUI)
{
	CONVERT_PARMS parms;
	parms.cmsInfo.src.DataType = FDT_NONE;
	parms.cmsInfo.dst.DataType = FDT_PALETTECOLOR;

	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CONVERT, this, (LPCMD_PARMS)&parms) );
}
 
afx_msg void CServerView::OnUpdateConvRgbColor(CCmdUI* pCmdUI)
{
	CONVERT_PARMS parms;
	parms.cmsInfo.src.DataType = FDT_NONE;
	parms.cmsInfo.dst.DataType = FDT_RGBCOLOR;

	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CONVERT, this, (LPCMD_PARMS)&parms) );
}
 
afx_msg void CServerView::OnUpdateConvCmykColor(CCmdUI* pCmdUI)
{
	CONVERT_PARMS parms;
	parms.cmsInfo.src.DataType = FDT_NONE;
	parms.cmsInfo.dst.DataType = FDT_CMYKCOLOR;

	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CONVERT, this, (LPCMD_PARMS)&parms) );
}
 
afx_msg void CServerView::OnUpdateConvColorManaged(CCmdUI* pCmdUI)
{
	CImage *pImage = GetDocument()->GetImage(); 
	BOOL state = IsCmdEnabled(0, this);

	if (state && !pImage->InMaskEditMode())
	{
		FRMTYPEINFO TypeInfo;
		pImage->GetTypeInfo(&TypeInfo);
		state = (TypeInfo.DataType == FDT_CMYKCOLOR ||
				 TypeInfo.DataType == FDT_RGBCOLOR) &&
				(Control.Retail && CmsGetMenuState(pImage))
				? TRUE : FALSE;
	}

    pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateCmdListUndo(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
		CImage *pImage = GetDocument()->GetImage(); 
	    int iUndoCount = ListGetCount(&pImage->lpCmdList->UndoPacketList);
	    if ((iUndoCount <= 1) || pImage->lpCmdList->fInsertMode)
    	    state = FALSE;
	    else
        	state = TRUE;
	}
    pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateCmdListRedo(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
		CImage *pImage = GetDocument()->GetImage(); 
	    int iRedoCount = ListGetCount(&pImage->lpCmdList->RedoPacketList);
	    if ((iRedoCount <= 0) || pImage->lpCmdList->fInsertMode)
    	    state = FALSE;
	    else
        	state = TRUE;
	}
    pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateCmdListEdit(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
		CImage *pImage = GetDocument()->GetImage(); 
	    int iUndoCount = ListGetCount(&pImage->lpCmdList->UndoPacketList);
    	if (pImage->lpCmdList->fInsertMode || (iUndoCount > 1))
        	state = TRUE;
		else
			state = FALSE;
	}
    pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateUndo(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(IDS_CMD_EDITUNDO, this);

	if (pCmdUI->m_pMenu)
	{
		LPSTR  lpString;
		STRING szString;

		szString[0] = '\0';
		if ( AstralStr( IDS_UNDO, &lpString ) )
		   	lstrcpy( szString, lpString );
		if (state)
		{
			CImage *pImage = GetDocument()->GetImage(); 
			if (!pImage->GetUndoState())
			{
  	    		if (AstralStr( IDS_REDO, &lpString ))
	        		lstrcpy( szString, lpString );
			}

			if (AstralStr((Control.UseApply ? 
				IDS_EDITS : pImage->GetDirtyState()), &lpString ) )
        		lstrcat( szString, lpString );
		}
		pCmdUI->SetText((LPCSTR)szString);
	}
    pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateApply(CCmdUI* pCmdUI)
{
	LPSTR lpString;

	if (pCmdUI->m_pMenu && AstralStr( (Control.UseApply ? IDS_MANUALAPPLY : IDS_AUTOAPPLY), &lpString ))
		pCmdUI->SetText(lpString);
    pCmdUI->Enable(	IsCmdEnabled(IDS_CMD_EDITUNDO, this) );
}

afx_msg void CServerView::OnUpdateDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_DELETE, this) );
}

afx_msg void CServerView::OnUpdateMap(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MAP, this) );
}

afx_msg void CServerView::OnUpdateContBrit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CONTBRIT, this) );
}

afx_msg void CServerView::OnUpdateColorBalance(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_BALANCE, this) );
}

afx_msg void CServerView::OnUpdatePosterize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_POSTERIZE, this) );
}

afx_msg void CServerView::OnUpdateThreshold(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_THRESHOLD, this) );
}

afx_msg void CServerView::OnUpdateHue(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_HUESAT, this) );
}

afx_msg void CServerView::OnUpdateHueShift(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_HUESHIFT, this) );
}

afx_msg void CServerView::OnUpdateToneBalance(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_TONEBALANCE, this) );
}

afx_msg void CServerView::OnUpdateHistogram(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
		CImage *pImage = GetDocument()->GetImage(); 
		FRMTYPEINFO TypeInfo;
		pImage->GetTypeInfo(&TypeInfo);
		state = TypeInfo.DataType != FDT_LINEART;
	}
	pCmdUI->Enable( state );
}

afx_msg void CServerView::OnUpdatePaletteEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_PALETTEEDIT, this) );
}

afx_msg void CServerView::OnUpdateMaskLoad(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(0, this) );
}

afx_msg void CServerView::OnUpdateMaskSave(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
		CImage *pImage = GetDocument()->GetImage(); 
		state = pImage->GetMask() != NULL;
	}
	pCmdUI->Enable(state);
}

afx_msg void CServerView::OnUpdateMaskUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MASKUNDO, this) );
}

afx_msg void CServerView::OnUpdateMaskRemove(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_REMOVEMASK, this) );
}

afx_msg void CServerView::OnUpdateMaskInvert(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_INVERTMASK, this) );
}

afx_msg void CServerView::OnUpdateMaskStroke(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_STROKEMASK, this) );
}

afx_msg void CServerView::OnUpdateCrop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CROP, this) );
}

afx_msg void CServerView::OnUpdateMoveObjects(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_MOVEOBJECTS, this) );
}

afx_msg void CServerView::OnUpdateCalibrateApply(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_APPLYCAL, this) );
}

afx_msg void CServerView::OnUpdateCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_CUT, this) );
}

afx_msg void CServerView::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_COPYTOCLIPBOARD, this) );
}

afx_msg void CServerView::OnUpdateCopyToFile(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(IDS_CMD_COPYTOFILE, this) );
}

afx_msg void CServerView::OnUpdateExportFile(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
    	CImage *pImage = GetDocument()->GetImage(); 

		state = GetPPFileType(pImage->CurFile) == IDN_PP ||
         		GetPPFileType(pImage->CurFile) == IDN_PPCOMPOUND;
	}
	pCmdUI->Enable(state);
}

afx_msg void CServerView::OnUpdateQuickZoom(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	pCmdUI->Enable(state);
}

afx_msg void CServerView::OnUpdateProbePoint(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	pCmdUI->Enable(state);
}

afx_msg void CServerView::OnUpdateProbeRect(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	pCmdUI->Enable(state);
}

afx_msg void CServerView::OnRotate90()
{
    DeactivateTool();
    CServerDoc* pDoc = GetDocument();
    if (!ConfirmMaskRemove(pDoc->GetImage()))
		return;
	ProcessCommand((pDoc->GetImage())->lpCmdList, IDS_CMD_ROTATE90, NULL);
}

afx_msg void CServerView::OnRotate180()
{
    DeactivateTool();
	CServerDoc* pDoc = GetDocument();
    if (!ConfirmMaskRemove(pDoc->GetImage()))
        return;
    ProcessCommand((pDoc->GetImage())->lpCmdList, IDS_CMD_ROTATE180, NULL);
}

afx_msg void CServerView::OnRotate270()
{
    DeactivateTool();
    CServerDoc* pDoc = GetDocument();
    if (!ConfirmMaskRemove(pDoc->GetImage()))
        return;
    ProcessCommand((pDoc->GetImage())->lpCmdList, IDS_CMD_ROTATE270, NULL);
}

afx_msg void CServerView::OnRotateAny()
{
    DeactivateTool();
    CServerDoc* pDoc = GetDocument();
	CRotateImageDlg dlg(this);
	if(dlg.DoModal()==IDOK)
    {
        ROTATE_PARMS parms;

        if (!ConfirmMaskRemove(pDoc->GetImage()))
            return;
		if (dlg.GetIsCCW())
	        parms.Angle = TOFIXED(360)-dlg.GetAngle();
		else
	        parms.Angle = dlg.GetAngle();
        parms.fUseWeightedAvg = dlg.GetSmartSize();
        ProcessCommand((pDoc->GetImage())->lpCmdList, IDS_CMD_ROTATEANY, &parms);
    }
}

void CServerView::do_split(UINT idCmd)
{
 	FRMTYPEINFO TypeInfo;

    CImage *pImage = GetDocument()->GetImage();

    DeactivateTool();
	pImage->GetTypeInfo(&TypeInfo);
    if ( (pImage->DocumentType & 0xFF00) == IMG_NOSPLIT &&
         (TypeInfo.DataType == FDT_RGBCOLOR ||
          TypeInfo.DataType == FDT_CMYKCOLOR))
    {
        if (pImage->MultipleObjects())
        {
            Message(IDS_OBJECTSNEEDCOMBINE);
            return;
        }

        if( ListGetCount( &pImage->lpCmdList->UndoPacketList ))
        {
            if( !( AstralAffirm( IDS_DESTROYCMDLIST )))
                return;
        }

        if (idCmd == IDM_SPLITRGB && TypeInfo.DataType == FDT_RGBCOLOR)
            SplitRGB(pImage);
        else 
        if (idCmd == IDM_SPLITHSL && TypeInfo.DataType == FDT_RGBCOLOR)
            SplitHSL(pImage);
        else 
        if (( idCmd == IDM_SPLITCMYK ) && ( TypeInfo.DataType == FDT_CMYKCOLOR ) )
            SplitCMYK(pImage);
    }
}

void CServerView::move_obj(UINT idCmd)
{
   	RECT     rSel;
	int      dx, dy;
	LPOBJECT lpObject, lpBase;
	int      x, y;
	RECT     rAll;
	MOVEOBJECTS_PARMS parms;

	CImage *pImage = GetDocument()->GetImage(); 
   	lpBase = pImage->GetBase();
   	if (pImage->GetSelObject(NULL) == lpBase)
      	return;
   	pImage->GetSelObjectRect(&rSel, YES);
   	dx = dy = 0;
	if (idCmd == IDM_MOVEUP)
    	--dy;
   	if (idCmd == IDM_MOVEDOWN)
    	++dy;
	if (idCmd == IDM_MOVELEFT)
     	--dx;
	if (idCmd == IDM_MOVERIGHT)
    	++dx;
   
	parms.iBaseRes = FrameResolution(pImage->GetBaseEditFrame());
	parms.x = rSel.left + dx;
	parms.y = rSel.top  + dy;
	ProcessCommand(pImage->lpCmdList, IDS_CMD_MOVEOBJECTS, &parms);

	AstralSetRectEmpty(&rAll);
	lpObject = NULL;
	while (lpObject = pImage->GetSelObject(lpObject))
   		AstralUnionRect(&rAll, &rAll, &lpObject->rObject);
	if (idCmd == IDM_MOVEUP)
   	{
   		x = (rAll.left + rAll.right)/2;
   		y = rAll.top;
   	}
	else if (idCmd == IDM_MOVEDOWN)
   	{
   		x = (rAll.left + rAll.right)/2;
   		y = rAll.bottom;
   	}
	else if (idCmd == IDM_MOVELEFT)
   	{
   		x = rAll.left;
   		y = (rAll.top + rAll.bottom)/2;
   	}
	else if (idCmd == IDM_MOVERIGHT)
   	{
   		x = rAll.right;
   		y = (rAll.top + rAll.bottom)/2;
   	}

	HWND hWnd = GetSafeHwnd();
	File2Display(hWnd, &x, &y);
	AutoScroll(x, y);
	AstralUpdateWindow(hWnd);
	DisplayInfo(-1, -1, &rAll);
	}

afx_msg void CServerView::OnPrintPreview()
{
	CImage *pImage = GetDocument()->GetImage(); 
    if (Control.Crippled)
    {
        Message(Control.Crippled);
        return;
    }
    DeactivateTool();
    CmsPrintPreview( pImage );
}

extern BOOL MaskFromObjectWithMask(LPIMAGE lpImage);
extern BOOL MaskFromObject(LPIMAGE lpImage, LPMASKFROMOBJECT_PARMS lpParms);

afx_msg void CServerView::OnMaskFromObject()
   {
	CImage *pImage = GetDocument()->GetImage(); 
   if (pImage->GetMask())
      MaskFromObjectWithMask(pImage);
   else
      {
      RECT     rSel;
      MASKFROMOBJECT_PARMS parms;

      // get rectangle for all floating objects
      if (!pImage->GetSelObjectRect(&rSel, YES))
         return;
   
		parms.fHiQuality = Mask.PasteQuality;
		parms.Mode = SHAPE_ADD;
		parms.iBaseRes = FrameResolution(pImage->GetBaseEditFrame());
		parms.DistortPoints[0].x = rSel.left;
		parms.DistortPoints[0].y = rSel.top;
		parms.DistortPoints[1].x = rSel.right;
		parms.DistortPoints[1].y = rSel.top;
		parms.DistortPoints[2].x = rSel.right;
		parms.DistortPoints[2].y = rSel.bottom;
		parms.DistortPoints[3].x = rSel.left;
		parms.DistortPoints[3].y = rSel.bottom;
		ProcessCommand(pImage->lpCmdList, IDS_CMD_MASKFROMOBJECT, &parms);
      }
   }

extern BOOL ObjectFromMask(LPIMAGE lpImage, LPOBJECTFROMMASK_PARMS lpParms);
afx_msg void CServerView::OnObjectFromMask()
   {
	CImage *pImage = GetDocument()->GetImage(); 

    OBJECTFROMMASK_PARMS parms;
   
	ProcessCommand(pImage->lpCmdList, IDS_CMD_OBJECTFROMMASK, &parms);

	#ifndef WIN32
	FORWARD_WM_COMMAND(PictPubApp.Get_hWndAstral(FALSE), IDC_SELECTOR, NULL, 0, SendMessage);
	#endif
   }

afx_msg void CServerView::OnChroma()
{
    CImage *pImage = GetDocument()->GetImage(); 
	CChromaDlg dlg(this, AfxGetMainWnd());

    DeactivateTool();
	dlg.DoModal();
}

afx_msg void CServerView::OnFeather()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    if (AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_FEATHER, DlgFeatherProc))
    {
        FEATHERALPHA_PARMS parms;
        int res;

        if (Mask.FeatherAmount < 1)
            Mask.FeatherAmount = 1;
        if (Mask.wFeatherEdge == 0)
            parms.edge = (Mask.FeatherEdgeHard*255L)/(Mask.FeatherAmount*100L);
        else if (Mask.wFeatherEdge == 2)  
            parms.edge = (Mask.FeatherEdgeSoft*255L)/(Mask.FeatherAmount*100L);
        else 
            parms.edge = (Mask.FeatherEdgeNormal*255L)/(Mask.FeatherAmount*100L);
        parms.inside = Mask.FeatherInside;
        res = FrameResolution(pImage->GetBaseEditFrame());
		parms.iBaseRes = res;
        parms.size = Mask.FeatherAmount;
        ProcessCommand(pImage->lpCmdList, IDS_CMD_FEATHERMASK, &parms);
    }
}

afx_msg void CServerView::OnRemoveHoles()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_REMOVE_HOLES, NULL);
}

afx_msg void CServerView::OnMaskSmooth()
{
    CImage *pImage = GetDocument()->GetImage(); 
    MASK_SMOOTH_PARMS parms;
	int res;

    DeactivateTool();
    if (AstralDlgParam( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_MASK_SMOOTH, DlgMaskSmoothProc, (LPARAM)&parms))
	{
		// make gaussian into proper units
        res = FrameResolution(pImage->GetBaseEditFrame());
		parms.iBaseRes = res;
        parms.gauss = parms.gauss;
        ProcessCommand(pImage->lpCmdList, IDS_CMD_MASK_SMOOTH, &parms);
	}
}

afx_msg void CServerView::OnObjFeather()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();

    if (AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_FEATHEROBJECT, DlgFeatherObjProc))
    {   
        FEATHERALPHA_PARMS parms;
        int res;

        if (Mask.FeatherAmount < 1)
            Mask.FeatherAmount = 1;
        if (Mask.wFeatherEdge == 0)
            parms.edge = (Mask.FeatherEdgeHard*255L)/(Mask.FeatherAmount*100L);
        else if (Mask.wFeatherEdge == 2)  
            parms.edge = (Mask.FeatherEdgeSoft*255L)/(Mask.FeatherAmount*100L);
        else 
            parms.edge = (Mask.FeatherEdgeNormal*255L)/(Mask.FeatherAmount*100L);
        parms.inside = 1;
        res = FrameResolution(pImage->GetBaseEditFrame());
		parms.iBaseRes = res;
        parms.size = Mask.FeatherAmount;
        ProcessCommand(pImage->lpCmdList, IDS_CMD_FEATHEROBJECT, &parms);
    }
}

afx_msg void CServerView::OnObjAlign()
{
    CImage *pImage = GetDocument()->GetImage(); 

    ALIGNOBJECTS_PARMS  AlignParms;
 	CObjAlgnDlg dlg(this, &AlignParms, AfxGetMainWnd());

    DeactivateTool();
	if (dlg.DoModal() == IDOK)
        ProcessCommand(pImage->lpCmdList, IDS_CMD_OBJECTALIGN, &AlignParms);
}

afx_msg void CServerView::OnObjPosition()
{
    CImage *pImage = GetDocument()->GetImage(); 

    POSOBJECTS_PARMS PosParms;
 	CObjPosDlg dlg(this, &PosParms, AfxGetMainWnd());

    DeactivateTool();
	if (dlg.DoModal() == IDOK)
        ProcessCommand(pImage->lpCmdList, IDS_CMD_OBJECTPOS, &PosParms);
}

afx_msg void CServerView::OnCombineObj()
{
    CImage *pImage = GetDocument()->GetImage(); 

    if (pImage)
    {
        CMD_PARMS parms;

        DeactivateTool();
        ProcessCommand(pImage->lpCmdList, IDS_CMD_OBJCOMBOBJS, &parms);
    }
}

afx_msg void CServerView::OnCombine()
{
    CImage *pImage = GetDocument()->GetImage(); 

    if (pImage)
    {
        CMD_PARMS parms;

        DeactivateTool();
        ProcessCommand(pImage->lpCmdList, IDS_CMD_COMBINE, &parms);
    }
}

afx_msg void CServerView::OnCombineSel()
{
    CImage *pImage = GetDocument()->GetImage(); 

    if (pImage)
    {
        CMD_PARMS parms;

        DeactivateTool();
        ProcessCommand(pImage->lpCmdList, IDS_CMD_COMBINESEL, &parms);
    }
}

afx_msg void CServerView::OnSelectAll()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    AstralCursor(IDC_WAIT);
    RemoveObjectMarquee(pImage);
    if (pImage->SelObject(NULL, NULL, NO, YES))
    {
        SELECTOBJECTS_PARMS parms;

        parms.fSelectAll = YES;
        PostCommand(pImage->lpCmdList, IDS_CMD_SELECTOBJECTS, &parms);
        SetupMiniViews(NULL, YES);
        if ( Tool.hRibbon )
            ::SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
    }
    AstralCursor(NULL);
}

afx_msg void CServerView::OnSelectInvert()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    AstralCursor(IDC_WAIT);
    RemoveObjectMarquee(pImage);
    pImage->InvertSelection();
    if ( Tool.hRibbon )
        ::SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
    AstralCursor(NULL);

}

afx_msg void CServerView::OnDownLevel()
{
    CImage *pImage = GetDocument()->GetImage(); 

    LAYEROBJECTS_PARMS parms;
	RECT rDisplay;

    DeactivateTool();
    pImage->AdjustLayerSelObj(FALSE, YES);
    pImage->GetSelObjectRect(&rDisplay, YES/*bExcludeBase*/);
    UpdateImage(pImage, &rDisplay, YES);
    SetupMiniViews(NULL, NO);

    PostCommand(pImage->lpCmdList,IDS_CMD_ORDERDOWN,&parms);
}

afx_msg void CServerView::OnUpLevel()
{
    CImage *pImage = GetDocument()->GetImage(); 

    LAYEROBJECTS_PARMS parms;
	RECT rDisplay;

    DeactivateTool();
    pImage->AdjustLayerSelObj(TRUE, YES);
    pImage->GetSelObjectRect(&rDisplay, YES/*bExcludeBase*/);
    UpdateImage(pImage, &rDisplay, YES);
    SetupMiniViews(NULL, NO);

    PostCommand(pImage->lpCmdList,IDS_CMD_ORDERUP,&parms);
}

afx_msg void CServerView::OnMoveToBottom()
{
    CImage *pImage = GetDocument()->GetImage(); 

    LAYEROBJECTS_PARMS parms;
	RECT rDisplay;

    DeactivateTool();
    pImage->AdjustLayerSelObj(FALSE, NO);
    pImage->GetSelObjectRect(&rDisplay, YES/*bExcludeBase*/);
    UpdateImage(pImage, &rDisplay, YES);
    SetupMiniViews(NULL, NO);

    PostCommand(pImage->lpCmdList,IDS_CMD_ORDERTOBOTTOM,&parms);
}

afx_msg void CServerView::OnMoveToTop()
{
    CImage *pImage = GetDocument()->GetImage(); 

    LAYEROBJECTS_PARMS parms;
	RECT rDisplay;

    DeactivateTool();
    pImage->AdjustLayerSelObj(TRUE, NO);
    pImage->GetSelObjectRect(&rDisplay, YES/*bExcludeBase*/);
    UpdateImage(pImage, &rDisplay, YES);
    SetupMiniViews(NULL, NO);

    PostCommand(pImage->lpCmdList,IDS_CMD_ORDERTOTOP,&parms);
}

afx_msg void CServerView::OnApplyMask()
{
    CImage *pImage = GetDocument()->GetImage(); 

    CMD_PARMS parms;
    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_MERGEMASK, &parms);
}

afx_msg void CServerView::OnObjDelete()
{
    CImage *pImage = GetDocument()->GetImage(); 
	CMD_PARMS parms;

    DeactivateTool();
	ProcessCommand(pImage->lpCmdList, IDS_CMD_DELETEOBJECTS, &parms);
}

afx_msg void CServerView::OnSizeit()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    CSizeImageDlg *pDlg = new CSizeImageDlg(this, CWnd::FromHandle(PictPubApp.Get_hWndAstral()));
    if (pDlg)
    {
        if (pDlg->DoModal()==IDOK)
        {
            SIZE_PARMS parms;

            if (!pDlg->GetMaintainSize())
                if (!ConfirmMaskRemove(pImage))
                    return;
    
            parms.Width = pDlg->GetWidth();
            parms.Height = pDlg->GetHeight();
            parms.Resolution = pDlg->GetResolution();
            parms.SmartSize = pDlg->GetSmartSize() && !pDlg->GetDistortable();
            parms.MaintainSize = pDlg->GetMaintainSize();
    		Edit.MaintainSize = NO;
            ProcessCommand(pImage->lpCmdList, IDS_CMD_SIZE, &parms);
        }

        delete pDlg;
    }
}

afx_msg void CServerView::OnExpand()
{
    CImage *pImage = GetDocument()->GetImage();
    CExpandImageDlg dlg(this); 

    DeactivateTool();
	if(dlg.DoModal()==IDOK)
    {
        EXPAND_PARMS parms;
		int pix, lin, res;
		LFIXED Width, Height;

        if (!ConfirmMaskRemove(pImage))
            return;
		pImage->GetInfo(&pix, &lin, NULL, NULL, &res);
		Width = FGET(pix, res);
		Height = FGET(lin, res);
        parms.Width = 0;
        parms.Height = 0;
		parms.fUseRightBottom = TRUE;
		parms.Top = dlg.GetTop();
		parms.Left = dlg.GetLeft();
		parms.Right = dlg.GetWidth() - Width - dlg.GetLeft();
		parms.Bottom = dlg.GetHeight() - Height - dlg.GetTop();
		parms.Color = dlg.GetFillColor();
        ProcessCommand(pImage->lpCmdList, IDS_CMD_EXPAND, &parms);
    }
}

afx_msg void CServerView::OnMirrorX()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_MIRRORX, NULL);
}

afx_msg void CServerView::OnMirrorY()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_MIRRORY, NULL);
}

afx_msg void CServerView::OnMirrorXY()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_MIRRORXY, NULL);
}

afx_msg void CServerView::OnSpecialFx()
{
    CImage *pImage = GetDocument()->GetImage(); 
	LPOBJECT lpObject;
	RECT rMask;

	if (pImage->CountSelObjects(NULL) != 1)
	{
	    Message(IDS_OBJECTSONLYONE);
	    return;
	}
	DeactivateTool();
	lpObject = pImage->GetSelObject(NULL);
	if (!lpObject)
	    return;
	
	DoEffectsDlg(pImage, ObjGetEditFrame(lpObject), pImage->GetMask(),
	                &lpObject->rObject, &rMask );
}

afx_msg void CServerView::OnInvert()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_INVERT, NULL);
}

afx_msg void CServerView::OnSplitRgb()
{
	do_split(IDM_SPLITRGB);
}

afx_msg void CServerView::OnSplitHsl()
{
	do_split(IDM_SPLITHSL);
}

afx_msg void CServerView::OnSplitCmyk()
{
	do_split(IDM_SPLITCMYK);
}

afx_msg void CServerView::OnChannelCombine()
{
    CImage *pImage = GetDocument()->GetImage(); 

	if (pImage->InMaskEditMode())
		return;

    DeactivateTool();
    if ((pImage->DocumentType & 0xFF00) == IMG_SPLITRGB)
        CombineRGB(pImage);
    else if ((pImage->DocumentType & 0xFF00) == IMG_SPLITCMYK)
        CombineCMYK(pImage);
    else if ((pImage->DocumentType & 0xFF00) == IMG_SPLITHSL)
        CombineHSL(pImage);
}

afx_msg void CServerView::OnConvLa()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ConvertTo(pImage, IDM_CONVLA);
}

afx_msg void CServerView::OnConvSp()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ConvertTo(pImage, IDM_CONVSP);
}

afx_msg void CServerView::OnConvCt()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ConvertTo(pImage, IDM_CONVCT);
}

afx_msg void CServerView::OnConvPalette()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ConvertTo(pImage, IDM_CONVPALETTE);
}

afx_msg void CServerView::OnConvRgbColor()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ConvertTo(pImage, IDM_CONVRGBCOLOR);
}

afx_msg void CServerView::OnConvCmykColor()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ConvertTo(pImage, IDM_CONVCMYKCOLOR);
}

afx_msg void CServerView::OnConvColorManaged()
{
    CImage *pImage = GetDocument()->GetImage(); 

    DeactivateTool();
    ConvertTo(pImage, IDM_COLORMANAGED);
}

afx_msg void CServerView::OnCmdListUndo()
{
    CImage *pImage = GetDocument()->GetImage(); 

    if (AstralDlgParam( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_CMDLIST_UNDO,
                    DlgCmdListUndoProc, (LPARAM)pImage ))
	    CommandListUndo(pImage->lpCmdList);
}

afx_msg void CServerView::OnCmdListRedo()
{
    CImage *pImage = GetDocument()->GetImage(); 

    if (AstralDlgParam( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_CMDLIST_REDO,
                    DlgCmdListRedoProc, (LPARAM)pImage ))
        CommandListRedo(pImage->lpCmdList);
}

afx_msg void CServerView::OnCmdListEdit()
{
    CImage *pImage = GetDocument()->GetImage(); 

	if (AstralDlgParam( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_CMDLIST_EDIT,
                    DlgCmdListEditProc, (LPARAM)pImage ))
	{
		if (ListGetCount(&pImage->lpCmdList->PacketList))
			CommandListRedo(pImage->lpCmdList);
		else
			CommandListUndo(pImage->lpCmdList);
	}
}

afx_msg void CServerView::OnUndo()
{
    CImage *pImage = GetDocument()->GetImage(); 

    CMD_PARMS parms;
    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_EDITUNDO, &parms);
}

afx_msg void CServerView::OnApply()
{
    CImage *pImage = GetDocument()->GetImage(); 
    CMD_PARMS parms;
    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_EDITAPPLY, &parms);
}

afx_msg void CServerView::OnDelete()
{
    CImage *pImage = GetDocument()->GetImage(); 
    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_DELETE, NULL);
}

afx_msg void CServerView::OnMap()
{
    CImage *pImage = GetDocument()->GetImage(); 
	CModifyColorMapsDlg dlg(this);


    DeactivateTool();

	if(dlg.DoModal()==IDOK)
    {
        ProcessCommand(pImage->lpCmdList, IDS_CMD_MAP, dlg.GetParmsPtr());
    }
}

afx_msg void CServerView::OnContBritStick()
{
    CImage *pImage = GetDocument()->GetImage(); 

	CContrastBrightnessJoystickDlg dlg(this);

    DeactivateTool();

	if(dlg.DoModal()==IDOK)
    {
        ProcessCommand(pImage->lpCmdList, IDS_CMD_CONTBRIT, dlg.GetParmsPtr());
    }
}

afx_msg void CServerView::OnColorBalStick()
{
    CImage *pImage = GetDocument()->GetImage(); 
	CColorBalanceJoystickDlg dlg(this);

    DeactivateTool();
	if(dlg.DoModal()==IDOK)
    {
        ProcessCommand(pImage->lpCmdList, IDS_CMD_BALANCE, dlg.GetParmsPtr());
    }
}

afx_msg void CServerView::OnContBritVisual()
{
    CImage *pImage = GetDocument()->GetImage(); 

	int id;
	int cxScreen,VisualMapSize;

    DeactivateTool();
    // use size form ini if any
    GetDefInt(VisualMapSize, 0);
    if (VisualMapSize == 3)
        id = IDD_CONTBRIT_VISUAL107;
    else if (VisualMapSize == 2)
        id = IDD_CONTBRIT_VISUAL86;
    else if (VisualMapSize == 1)
        id = IDD_CONTBRIT_VISUAL;
    else
    {
        // use correct size
        cxScreen = GetSystemMetrics(SM_CXSCREEN);
        if (cxScreen > 1000)
            id = IDD_CONTBRIT_VISUAL107;
        else
        if (cxScreen > 750)
            id = IDD_CONTBRIT_VISUAL86;
        else
            id = IDD_CONTBRIT_VISUAL;
    }
	CContrastBrightnessVisualDlg dlg(this, id);

	if(dlg.DoModal()==IDOK)
    	{
    	ProcessCommand(pImage->lpCmdList, IDS_CMD_CONTBRIT, dlg.GetParmsPtr());
		}
}

afx_msg void CServerView::OnColorBalVisual()
{
    CImage *pImage = GetDocument()->GetImage(); 

	int id,cxScreen,VisualMapSize;
	
    DeactivateTool();
    // use size form ini if any
    GetDefInt(VisualMapSize, 0);
    if (VisualMapSize == 3)
        id = IDD_COLORBAL_VISUAL107;
    else if (VisualMapSize == 2)
        id = IDD_COLORBAL_VISUAL86;
    else if (VisualMapSize == 1)
        id = IDD_COLORBAL_VISUAL;
    else
    {
        // use correct size
        cxScreen = GetSystemMetrics(SM_CXSCREEN);
        if (cxScreen > 1000)
            id = IDD_COLORBAL_VISUAL107;
        else
        if (cxScreen > 750)
            id = IDD_COLORBAL_VISUAL86;
        else
            id = IDD_COLORBAL_VISUAL;
    }
	
	CColorBalanceVisualDlg dlg(this, id);

	if(dlg.DoModal()==IDOK)
		{
    	ProcessCommand(pImage->lpCmdList, IDS_CMD_BALANCE, dlg.GetParmsPtr()/*&parms*/);
		}
}
extern BOOL  InPosterizeMode;
afx_msg void CServerView::OnPosterize()
{
    CImage *pImage = GetDocument()->GetImage(); 

	CPosterizeDlg dlg(this);


    DeactivateTool();
	if(dlg.DoModal()==IDOK)
    {
        ProcessCommand(pImage->lpCmdList, IDS_CMD_POSTERIZE, dlg.GetParmsPtr());
    }
}


afx_msg void CServerView::OnThreshold()
{
    CImage *pImage = GetDocument()->GetImage(); 

	CThresholdDlg dlg(this);


    DeactivateTool();
	if(dlg.DoModal()==IDOK)
    {
        ProcessCommand(pImage->lpCmdList, IDS_CMD_THRESHOLD, dlg.GetParmsPtr());
    }
}


afx_msg void CServerView::OnHue()
{
    CImage *pImage = GetDocument()->GetImage(); 

	CHueMapDlg dlg(this);


    DeactivateTool();

	if(dlg.DoModal()==IDOK)
    {
        ProcessCommand(pImage->lpCmdList, IDS_CMD_HUESAT, dlg.GetParmsPtr());
    }
}

afx_msg void CServerView::OnHueShift()
{
    CImage *pImage = GetDocument()->GetImage(); 

	CHueShiftDlg dlg(this);


    DeactivateTool();

	if(dlg.DoModal()==IDOK)
    {
        ProcessCommand(pImage->lpCmdList, IDS_CMD_HUESHIFT, dlg.GetParmsPtr());
    }
}

afx_msg void CServerView::OnToneBalance()
{
    CImage *pImage = GetDocument()->GetImage(); 
	CToneBalanceDlg dlg(this);

    DeactivateTool();
	if(dlg.DoModal()==IDOK)
        ProcessCommand(pImage->lpCmdList, IDS_CMD_TONEBALANCE, dlg.GetParmsPtr());
}

afx_msg void CServerView::OnHistogram()
{
	CHistogramDlg dlg(this);
	dlg.DoModal();
}

afx_msg void CServerView::OnPaletteEdit()
{
    PaletteEdit(GetDocument()->GetImage());
}

afx_msg void CServerView::OnMaskLoad()
{
	CLoadMaskDlg dlg(this);
	DeactivateTool();
	dlg.DoModal();
//    AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_MASKLOAD, DlgMaskLoadProc );

	UpdateStatusBar( YES, NO, YES, NO );
}

afx_msg void CServerView::OnMaskSave()
{
	AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_MASKSAVE, DlgMaskSaveProc );
}

afx_msg void CServerView::OnMaskUndo()
{
    CImage *pImage = GetDocument()->GetImage(); 
    CMD_PARMS parms;
    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_MASKUNDO, &parms);
}

afx_msg void CServerView::OnMaskRemove()
{
    CImage *pImage = GetDocument()->GetImage(); 
    CMD_PARMS parms;
    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_REMOVEMASK, &parms);
}

afx_msg void CServerView::OnMaskInvert()
{
    CImage *pImage = GetDocument()->GetImage(); 
    CMD_PARMS parms;
    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_INVERTMASK, &parms);
}

afx_msg void CServerView::OnMaskStroke()
{
    CImage *pImage = GetDocument()->GetImage(); 
	DeactivateTool();
	AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_MASKSTROKE, DlgMaskStrokeProc );
}

afx_msg void CServerView::OnCrop()
{
    CImage *pImage = GetDocument()->GetImage(); 
	DeactivateTool();
    pImage->Crop( NULL );
}

afx_msg void CServerView::OnMoveUp()
{
	move_obj(IDM_MOVEUP);
}

afx_msg void CServerView::OnMoveDown()
{
	move_obj(IDM_MOVEDOWN);
}

afx_msg void CServerView::OnMoveLeft()
{
	move_obj(IDM_MOVELEFT);
}

afx_msg void CServerView::OnMoveRight()
{
	move_obj(IDM_MOVERIGHT);
}

afx_msg void CServerView::OnCalibrateApply()
{
    CImage *pImage = GetDocument()->GetImage(); 
    LPAPPLYCAL_PARMS lpCalParms;

    if (!(lpCalParms = ( LPAPPLYCAL_PARMS )Alloc(sizeof(APPLYCAL_PARMS))))
    {
        Message(IDS_EMEMALLOC);
        return;
    }

    DeactivateTool();
    CCalibrateApplyDlg *pDlg = new CCalibrateApplyDlg(IDD_CALIBRATEAPPLY,
        CWnd::FromHandle(PictPubApp.Get_hWndAstral()));
    if (pDlg)
    {
        pDlg->PreDoModal((LPVOID)(LPARAM)lpCalParms);
        if (pDlg->DoModal())
            ProcessCommand(pImage->lpCmdList, IDS_CMD_APPLYCAL, lpCalParms);
        delete pDlg;
    }
    FreeUp(lpCalParms);
}

afx_msg void CServerView::OnCut()
{
    CImage *pImage = GetDocument()->GetImage(); 

	OnCopy();

	TINTFILL_PARMS parms;
    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_CUT, &parms);
}

afx_msg void CServerView::OnCopy()
{
	RECT rMask;
	FNAME   szFileName;
	szFileName[0] = '\0';
	LPCMDPKT lpCmdPkt;
	LPOBJECT lpObject = NULL;
	LPIMAGE lpNewImage = NULL;
	CDataItem* pDataItem = NULL;
	BOOL fRet = FALSE;

    CImage *pImage = GetDocument()->GetImage(); 

	DeactivateTool();
	AstralCursor(IDC_WAIT);
	
	TRY
	{

		// Note: the code below is almost identical to the code
		// in tobjsel.c funtion DoDragDrop()
		pImage->GetMaskRect(&rMask);
		BOOL bClipObject = ( pImage->CountSelObjects( NULL ) == 1 &&
			 pImage->GetSelObject(NULL) == pImage->GetBase( ) ) ? FALSE : TRUE;
        if (!(lpObject = pImage->CreateClipOutObject(bClipObject)))
			AfxThrowMemoryException();

		if (!(lpNewImage = CreateImage(NULL,NULL,lpObject,NULL,      
		   		IDN_PP,pImage->DataType,pImage->DocumentType,NULL))) 
			AfxThrowMemoryException();

		// Preserve Monitor xform on new image.
		// Ted - this seems bad to copy the pointer
		lpNewImage->m_cmsXform = pImage->m_cmsXform;

		lpNewImage->lpCmdList->dwImageID = pImage->lpCmdList->dwImageID;
		// only need to save command list if this image was loaded low-res
		// or PhotoCD
		lpCmdPkt = (LPCMDPKT)ListGetHead(&pImage->lpCmdList->UndoPacketList);
		if (lpCmdPkt && IsVariableResLoadCmd(lpCmdPkt))
		{
			CMD_PARMS parms;

			if (!CopyPacketList(&pImage->lpCmdList->UndoPacketList,
								&lpNewImage->lpCmdList->UndoPacketList))
				AfxThrowMemoryException();

			if (!(lpCmdPkt = CreatePacket(IDS_CMD_CREATECLIPOBJECT, &parms, YES)))
				AfxThrowMemoryException();
			ListAddTail(&lpNewImage->lpCmdList->UndoPacketList, lpCmdPkt);
		}

		if (lpNewImage)
		    pDataItem = new CDataItem (GetDocument(), lpNewImage, &rMask);

		if (pDataItem)
		{
			ITEMID FileType = pImage->FileType;
			BOOL bCopyLinkInfo = ((FileType == IDN_PP) || (FileType == IDN_PPCOMPOUND) ||
				(FileType == IDN_PP4) || (FileType == IDN_TIFF));
			bCopyLinkInfo = bCopyLinkInfo && (!pImage->fUntitled);

	 	    // NOTE!!the pDataItem will be internally destroyed once it isn't 
			// needed, therefore we do not need to delete the pDataItem here
    		pDataItem->CopyToClipboard (bCopyLinkInfo); 
			fRet = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		ASSERT(FALSE);

	    // if we are using an image, then it will free up the object
		if (lpNewImage)
			DestroyImage(lpNewImage);
		else
		if (lpObject)
			ObjFreeUp(lpObject);
	}
	END_CATCH_ALL

	AstralCursor(NULL);

	if (fRet)
	{
		CMD_PARMS parms;

		PostCommand(pImage->lpCmdList, IDS_CMD_COPYTOCLIPBOARD, &parms);
	}
}

afx_msg void CServerView::OnCopyToFile()
{
    DeactivateTool();
    AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_COPYTOFILE, DlgCopyToFileProc);
}

afx_msg void CServerView::OnExportFile()
{
    CImage *pImage = GetDocument()->GetImage(); 
	int nFileNum, nCopies;
	
    if ( !AstralDlgParam( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(),
			IDD_PACKAGEPP5, DlgPackageProc, (LPARAM)Names.szPackageDir ) )
        return;
   	ProgressBegin(1, PROGRESS_ID(IDS_UNDOEXPORTFILE));
	nCopies = nFileNum = 0;

	// wack the storage so ExportFile can do its job
	CServerDoc *pDoc = (CServerDoc *)GetDocument();
	pDoc->SetRootCompoundStorage(NULL, FALSE);
	// first find out how many copies are involved
	if (ExportFile(pImage->CurFile, Names.szPackageDir, NULL, &nCopies))
	{
		// now copy the files
		ExportFile(pImage->CurFile, Names.szPackageDir, &nFileNum, &nCopies);
	}
	pDoc->InitCompoundFile();
	ProgressEnd();
}

afx_msg void CServerView::OnObjLock()
{
   CImage *pImage = GetDocument()->GetImage(); 
   CMD_PARMS parms;

   DeactivateTool();
   ProcessCommand(pImage->lpCmdList, IDS_CMD_OBJLOCK, &parms);
}

afx_msg void CServerView::OnObjUnLock()
{
   CImage *pImage = GetDocument()->GetImage(); 
   CMD_PARMS parms;

   DeactivateTool();
   ProcessCommand(pImage->lpCmdList, IDS_CMD_OBJUNLOCK, &parms);
}

afx_msg void CServerView::OnObjGroup()
{
   CImage *pImage = GetDocument()->GetImage(); 
   CMD_PARMS parms;

   DeactivateTool();
   ProcessCommand(pImage->lpCmdList, IDS_CMD_GROUPOBJECTS, &parms);
}

afx_msg void CServerView::OnObjUnGroup()
{
	CImage *pImage = GetDocument()->GetImage(); 
	CMD_PARMS parms;

    DeactivateTool();
    ProcessCommand(pImage->lpCmdList, IDS_CMD_UNGROUPOBJECTS, &parms);
}

class CAreaZoom : public COneShotTool
{
public:
	CAreaZoom(CServerView *pView, int idCursor);
	~CAreaZoom(){}

	void OnLButtonDown( UINT keyFlags,CPoint point);
	void OnLButtonUp( UINT keyFlags,CPoint point);
	void OnMouseMove( UINT keyFlags,CPoint point);
protected:
	CRect	m_SelectRect;
	CPoint	m_StartPoint;
	BOOL	m_fSelection;
};

CAreaZoom::CAreaZoom(CServerView *pView, int idCursor) :
	COneShotTool(pView, idCursor)
{
	m_fSelection = NO;
}

void CAreaZoom::OnLButtonDown( UINT keyFlags,CPoint point)
{
	m_fToolActive = TRUE;
	m_StartPoint = point;
	m_fSelection = NO;
}						   

void CAreaZoom::OnLButtonUp( UINT keyFlags,CPoint point)
{
	BOOL fGotaRect;

	m_fToolActive = NO;
	if (m_fSelection)
	{
		EndSelection( m_pView->GetSafeHwnd(), NULL, &m_SelectRect, SL_BOX, YES );
		if (abs(m_SelectRect.Width()) <= SMALL_MOVEMENT &&
			abs(m_SelectRect.Height()) <= SMALL_MOVEMENT)
			fGotaRect = NO;
		else
			fGotaRect = YES;
	}
	else
	{
		fGotaRect = NO;
		Display2File(m_pView->GetSafeHwnd(), (LPINT)&m_StartPoint.x, (LPINT)&m_StartPoint.y );
		m_SelectRect.SetRect(m_StartPoint.x, m_StartPoint.y, m_StartPoint.x, m_StartPoint.y);
	}

	if ( fGotaRect )
		ViewRect(m_pView->GetSafeHwnd(), &m_SelectRect, FALSE);
	else
	{
		SaveLastView(m_pView->GetSafeHwnd());
		Zoom( m_pView->GetSafeHwnd(), m_SelectRect.left, m_SelectRect.top,
			SHIFT  ? -100 : +100, YES, CONTROL );
	}
	// kill ourselves
	if (!ALT)
		m_pView->SetupOneShotTool();
}

void CAreaZoom::OnMouseMove( UINT keyFlags,CPoint point)
{
	if (!m_fSelection)
	{
		if (m_fSelection = (abs(m_StartPoint.x-point.x) > SMALL_MOVEMENT ||
			abs(m_StartPoint.y-point.y) > SMALL_MOVEMENT))
		{
			AstralSetRectEmpty( &m_SelectRect );
			StartSelection( m_pView->GetSafeHwnd(), NULL, &m_SelectRect,
				SL_BOX | SL_SPECIAL, m_StartPoint, 0L );
		}
	}
	if (m_fSelection)
	{
		UpdateSelection( m_pView->GetSafeHwnd(), NULL, &m_SelectRect, SL_BOX,
			point, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER);
	}
}

afx_msg void CServerView::OnCustomView()
{
	TRY
	{	
		CAreaZoom *pAreaZoom = new CAreaZoom(this, ID_CUSTOMVIEW);
	}
	CATCH_ALL(e)
	{
		Message(IDS_EMEMALLOC);
	}
	END_CATCH_ALL
}

afx_msg void CServerView::OnQuickZoom()
{
	SEND_WM_COMMAND(PictPubApp.Get_hWndAstral(), IDM_SHOWZOOMBOX, NULL, 0);
}

class CProbe : public COneShotTool
{
public:
	CProbe(CServerView *pView, int idCursor, BOOL fDoArea);
	~CProbe(){}

	void OnLButtonDown( UINT keyFlags,CPoint point);
	void OnLButtonUp( UINT keyFlags,CPoint point);
	void OnMouseMove( UINT keyFlags,CPoint point);
protected:
	CRect	m_SelectRect;
	BOOL	m_fDoArea;
};

CProbe::CProbe(CServerView *pView, int idCursor, BOOL fDoArea) :
	COneShotTool(pView, idCursor)
{
	m_fDoArea = fDoArea;
}

void CProbe::OnLButtonDown( UINT keyFlags,CPoint point)
{
	HWND hWindow = m_pView->GetSafeHwnd();

	m_fToolActive = TRUE;
	if ( m_fDoArea )
	{
		AstralSetRectEmpty( &m_SelectRect );
		StartSelection( hWindow, NULL, &m_SelectRect, SL_BOX | SL_SPECIAL,
			point, 0L );
	}
	else
	{
		COLORINFO ProbeColor;
		int x = point.x;
		int y = point.y;
		LPIMAGE lpImage = m_pView->GetDocument()->GetImage();
		Display2File(hWindow,&x, &y);
		m_SelectRect.left = m_SelectRect.right = x;
		m_SelectRect.top = m_SelectRect.bottom = y;
		if ( Measure( lpImage, &m_SelectRect, &ProbeColor ) )
		{
			ColorStatus( &ProbeColor, 0L, NULL, YES );
			SetActiveColor( &ProbeColor, ProbeColor.ColorSpace, FALSE );
		}
	}
}						   

void CProbe::OnLButtonUp( UINT keyFlags,CPoint point)
{
	HWND hWindow = m_pView->GetSafeHwnd();
	m_fToolActive = NO;

	if ( m_fDoArea )
	{
		COLORINFO ProbeColor;
		LPIMAGE lpImage = m_pView->GetDocument()->GetImage();
		EndSelection( hWindow, NULL, &m_SelectRect, SL_BOX, YES );
		if ( Measure( lpImage, &m_SelectRect, &ProbeColor ) )
		{
			ColorStatus( &ProbeColor, 0L, NULL, YES );
			SetActiveColor( &ProbeColor, ProbeColor.ColorSpace, FALSE );
		}
	}
	// kill ourselves
	if (!ALT)
		m_pView->SetupOneShotTool();
}

void CProbe::OnMouseMove( UINT keyFlags,CPoint point)
{
	HWND hWindow = m_pView->GetSafeHwnd();

	// Bound the point to the window
	LPRECT lpRect = &m_pView->m_lpDisplay->DispRect;
	int x = bound( point.x, lpRect->left, lpRect->right );
	int y = bound( point.y, lpRect->top, lpRect->bottom );
	if ( m_fDoArea )
	{
		point.x = x; point.y = y;
		UpdateSelection( hWindow, NULL, &m_SelectRect, SL_BOX,
			point, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER);
	}
	else
	{
		COLORINFO ProbeColor;
		LPIMAGE lpImage = m_pView->GetDocument()->GetImage();
	  	int xx = x; int yy = y;
		Display2File(hWindow,&xx, &yy);
		m_SelectRect.left = m_SelectRect.right = xx;
		m_SelectRect.top = m_SelectRect.bottom = yy;
		if ( Measure( GetImagePtr(hWindow), &m_SelectRect, &ProbeColor ) )
		{
			ColorStatus( &ProbeColor, 0L, NULL, YES );
			SetActiveColor( &ProbeColor, ProbeColor.ColorSpace, FALSE );
		}
		DisplayInfo( x, y, NULL );
	}
}

afx_msg void CServerView::OnProbePoint()
{
	TRY
	{	
		CProbe *pProbe = new CProbe(this, ID_PROBE, FALSE);
	}
	CATCH_ALL(e)
	{
		Message(IDS_EMEMALLOC);
	}
	END_CATCH_ALL
}

afx_msg void CServerView::OnProbeRect()
{
	TRY
	{	
		CProbe *pProbe = new CProbe(this, ID_PROBE, TRUE);
	}
	CATCH_ALL(e)
	{
		Message(IDS_EMEMALLOC);
	}
	END_CATCH_ALL
}
