/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                svrview.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "pptbars.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "dataitem.h"
#include "svritem.h"
#include "svrview.h"
#include "id.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */
/* ********************************** Macros ******************************** */

#if 0
    #define IFDEBUG(s) {s;}
    #define IFCOPY(s) s
#else
    #define IFDEBUG(s)
    #define IFCOPY(s)
#endif

#define STOP ((m_lpDisplay==NULL) || (GetDocument()==NULL) || ((GetDocument()->GetImage())==NULL))

/* ********************************** Types ********************************* */

struct DROPMSG
{
	CPoint point;
	LPOBJECT lpObject;
	BOOL bExtName;
	FNAME szClipName;
	FRMTYPEINFO SrcTypeInfo;
};

COneShotTool::COneShotTool(CServerView *pView, int idCursor)
{
	m_pView = pView;
	m_pView->SetupOneShotTool(this);
	m_hOldCursor = Window.hCursor;
	m_fToolActive = FALSE;
	SetWindowCursor(idCursor);
}

COneShotTool::~COneShotTool()
{
	SetWindowCursor(0, m_hOldCursor);
}

/* ******************************* Local Data ******************************* */

// these need to be merged into the class itself
static	BOOL LastActivated;
extern BOOL fAppActive;

IMPLEMENT_DYNAMIC(CUpdateArea, CObject) // used for OnUpdate message processing

IMPLEMENT_DYNCREATE(CPPMDIChildWnd,CMDIChildWnd)
BEGIN_MESSAGE_MAP(CPPMDIChildWnd, CMDIChildWnd)
    ON_WM_CLOSE() 
    ON_WM_INITMENU()
    ON_WM_MOUSEACTIVATE()    
    ON_WM_PAINT()
    ON_WM_PALETTECHANGED()
    ON_WM_SYSCOMMAND()
    ON_WM_WINDOWPOSCHANGING()    
    ON_WM_MDIACTIVATE()
	ON_WM_NCLBUTTONDOWN()    
END_MESSAGE_MAP()                      

IMPLEMENT_DYNCREATE(CServerView, CView)
BEGIN_MESSAGE_MAP(CServerView, CView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()                    
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEMOVE()
    ON_WM_MOVE()
	ON_WM_NCLBUTTONDOWN()    
	ON_WM_NCPAINT()
    ON_WM_PALETTECHANGED()
    ON_WM_SETCURSOR()
    ON_WM_SIZE()
    ON_WM_TIMER()

	ON_MESSAGE(WM_DROPACTIVATE, OnDropActivate)

	ON_COMMAND(				IDC_STITCH, 		OnStitch)
	ON_UPDATE_COMMAND_UI(	IDC_STITCH, 		OnUpdateStitch)
	ON_COMMAND(				IDM_OBJ_HIDEMARQUEE,OnHideObjMarquee)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_HIDEMARQUEE,OnUpdateHideObjMarquee)
	ON_COMMAND(				IDM_PASTE, 			OnPaste)
	ON_UPDATE_COMMAND_UI(	IDM_PASTE, 			OnUpdatePaste)
	ON_COMMAND(				IDM_PASTESPECIAL, 	OnPasteSpecial)
	ON_UPDATE_COMMAND_UI(	IDM_PASTESPECIAL, 	OnUpdatePasteSpecial)
	ON_COMMAND(				IDM_HIDEMARQUEE, 	OnHideMarquee)
	ON_UPDATE_COMMAND_UI(	IDM_HIDEMARQUEE, 	OnUpdateHideMarquee)
	ON_COMMAND(				IDM_RULERSHOW, 		OnRulerShow)
	ON_UPDATE_COMMAND_UI(	IDM_RULERSHOW, 		OnUpdateRulerShow)
	ON_COMMAND(				IDM_ESCAPE, 		OnEscape)
	ON_UPDATE_COMMAND_UI(	IDM_ESCAPE, 		OnUpdateEscape)
	ON_COMMAND(				IDC_VIEWFULL, 		OnViewFull)
	ON_UPDATE_COMMAND_UI(	IDC_VIEWFULL, 		OnUpdateViewFull)
	ON_COMMAND(				IDC_VIEWLAST, 		OnViewLast)
	ON_UPDATE_COMMAND_UI(	IDC_VIEWLAST, 		OnUpdateViewLast)
	ON_COMMAND(				IDC_VIEW1TO1, 		OnView1To1)
	ON_UPDATE_COMMAND_UI(	IDC_VIEW1TO1, 		OnUpdateView1To1)
	ON_COMMAND(				IDC_VIEWALL, 		OnViewAll)
	ON_UPDATE_COMMAND_UI(	IDC_VIEWALL, 		OnUpdateViewAll)
	ON_COMMAND(				IDC_ZOOMIN, 		OnZoomIn)
	ON_UPDATE_COMMAND_UI(	IDC_ZOOMIN, 		OnUpdateZoomIn)
	ON_COMMAND(				IDC_ZOOMOUT, 		OnZoomOut)
	ON_UPDATE_COMMAND_UI(	IDC_ZOOMOUT, 		OnUpdateZoomOut)
	ON_COMMAND(				IDM_LOADSHAPE, 		OnLoadShape)
	ON_UPDATE_COMMAND_UI(	IDM_LOADSHAPE, 		OnUpdateLoadShape)
	ON_COMMAND(				IDM_SAVESHAPE,		OnSaveShape)
	ON_UPDATE_COMMAND_UI(	IDM_SAVESHAPE, 		OnUpdateSaveShape)
	
	// Commands
	ON_COMMAND(				IDM_ROTATE90,		OnRotate90)
	ON_UPDATE_COMMAND_UI(	IDM_ROTATE90,		OnUpdateRotate90)
	ON_COMMAND(				IDM_ROTATE180, 		OnRotate180)
	ON_UPDATE_COMMAND_UI(	IDM_ROTATE180, 		OnUpdateRotate180)
	ON_COMMAND(				IDM_ROTATE270, 		OnRotate270)
	ON_UPDATE_COMMAND_UI(	IDM_ROTATE270, 		OnUpdateRotate270)
	ON_COMMAND(				IDM_ROTATEANY, 		OnRotateAny)
	ON_UPDATE_COMMAND_UI(	IDM_ROTATEANY, 		OnUpdateRotateAny)
	ON_COMMAND(				IDM_OBJ_LOCK, 		OnObjLock)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_LOCK, 		OnUpdateObjLock)
	ON_COMMAND(				IDM_OBJ_UNLOCK, 	OnObjUnLock)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_UNLOCK, 	OnUpdateObjUnLock)
	ON_COMMAND(				IDM_OBJ_GROUP, 		OnObjGroup)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_GROUP, 		OnUpdateObjGroup)
	ON_COMMAND(				IDM_OBJ_UNGROUP, 	OnObjUnGroup)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_UNGROUP, 	OnUpdateObjUnGroup)
	ON_COMMAND(				IDM_PRINTPREVIEW, 	OnPrintPreview)
	ON_UPDATE_COMMAND_UI(	IDM_PRINTPREVIEW, 	OnUpdatePrintPreviewEnabled)
	ON_COMMAND(				IDM_FEATHER, 		OnFeather)
	ON_UPDATE_COMMAND_UI(	IDM_FEATHER, 		OnUpdateFeather)
	ON_COMMAND(				IDM_REMOVE_HOLES, 	OnRemoveHoles)
	ON_UPDATE_COMMAND_UI(	IDM_REMOVE_HOLES, 	OnUpdateRemoveHoles)
	ON_COMMAND(				IDM_MASK_SMOOTH, 	OnMaskSmooth)
	ON_UPDATE_COMMAND_UI(	IDM_MASK_SMOOTH, 	OnUpdateMaskSmooth)
	ON_COMMAND(				IDM_OBJ_ALIGN, 		OnObjAlign)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_ALIGN, 		OnUpdateObjAlign)
	ON_COMMAND(				IDM_OBJ_FEATHER, 	OnObjFeather)					 
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_FEATHER, 	OnUpdateObjFeather)
	ON_COMMAND(				IDM_OBJ_POSITION, 	OnObjPosition)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_POSITION, 	OnUpdatePosition)
	ON_COMMAND(				IDM_OBJ_COMBOBJS, 	OnCombineObj)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_COMBOBJS, 	OnUpdateCombineObj)
	ON_COMMAND(				IDM_COMBINE, 		OnCombine)
	ON_UPDATE_COMMAND_UI(	IDM_COMBINE, 		OnUpdateCombine)
	ON_COMMAND(				IDM_COMBINE_SEL, 	OnCombineSel)
	ON_UPDATE_COMMAND_UI(	IDM_COMBINE_SEL, 	OnUpdateCombineSel)
	ON_COMMAND(				IDM_OBJ_SELECTALL, 	OnSelectAll)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_SELECTALL, 	OnUpdateSelectAll)
	ON_COMMAND(				IDM_OBJ_SELECTINVERT,OnSelectInvert)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_SELECTINVERT,OnUpdateSelectInvert)
	ON_COMMAND(				IDM_OBJ_DOWNLEVEL, 	OnDownLevel)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_DOWNLEVEL, 	OnUpdateDownLevel)
	ON_COMMAND(				IDM_OBJ_UPLEVEL, 	OnUpLevel)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_UPLEVEL, 	OnUpdateUpLevel)
	ON_COMMAND(				IDM_OBJ_MOVETOBOTTOM,OnMoveToBottom)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_MOVETOBOTTOM,OnUpdateMoveToBottom)
	ON_COMMAND(				IDM_OBJ_MOVETOTOP, 	OnMoveToTop)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_MOVETOTOP, 	OnUpdateMoveToTop)
	ON_COMMAND(				IDM_OBJ_APPLYMASK, 	OnApplyMask)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_APPLYMASK, 	OnUpdateApplyMask)
	ON_COMMAND(				IDM_OBJ_DELETE,		OnObjDelete)
	ON_UPDATE_COMMAND_UI(	IDM_OBJ_DELETE,		OnUpdateObjDelete)
	ON_COMMAND(				IDM_MASKFROMOBJECT, OnMaskFromObject)
	ON_UPDATE_COMMAND_UI(	IDM_MASKFROMOBJECT, OnUpdateMaskFromObject)
	ON_COMMAND(				IDM_OBJECTFROMMASK, OnObjectFromMask)
	ON_UPDATE_COMMAND_UI(	IDM_OBJECTFROMMASK, OnUpdateObjectFromMask)
	ON_COMMAND(				IDM_CHROMA_MASK, 	OnChroma)
	ON_UPDATE_COMMAND_UI(	IDM_CHROMA_MASK, 	OnUpdateChroma)
	ON_COMMAND(				IDM_SIZE, 			OnSizeit)
	ON_UPDATE_COMMAND_UI(	IDM_SIZE, 			OnUpdateSizeit)
	ON_COMMAND(				IDM_EXPAND, 		OnExpand)
	ON_UPDATE_COMMAND_UI(	IDM_EXPAND, 		OnUpdateExpand)
	ON_COMMAND(				IDM_MIRRORX, 		OnMirrorX)
	ON_UPDATE_COMMAND_UI(	IDM_MIRRORX, 		OnUpdateMirrorX)
	ON_COMMAND(				IDM_MIRRORY, 		OnMirrorY)
	ON_UPDATE_COMMAND_UI(	IDM_MIRRORY, 		OnUpdateMirrorY)
	ON_COMMAND(				IDM_MIRRORXY, 		OnMirrorXY)
	ON_UPDATE_COMMAND_UI(	IDM_MIRRORXY, 		OnUpdateMirrorXY)
	ON_COMMAND(				IDM_SPECIALFX, 		OnSpecialFx)
	ON_UPDATE_COMMAND_UI(	IDM_SPECIALFX, 		OnUpdateSpecialFx)
	ON_COMMAND(				IDM_INVERT, 		OnInvert)
	ON_UPDATE_COMMAND_UI(	IDM_INVERT, 		OnUpdateInvert)
	ON_COMMAND(				IDM_SPLITRGB, 		OnSplitRgb)
	ON_UPDATE_COMMAND_UI(	IDM_SPLITRGB, 		OnUpdateSplitRgb)
	ON_COMMAND(				IDM_SPLITHSL, 		OnSplitHsl)
	ON_UPDATE_COMMAND_UI(	IDM_SPLITHSL, 		OnUpdateSplitHsl)
	ON_COMMAND(				IDM_SPLITCMYK, 		OnSplitCmyk)
	ON_UPDATE_COMMAND_UI(	IDM_SPLITCMYK, 		OnUpdateSplitCmyk)
	ON_COMMAND(				IDM_CHANNELCOMBINE, OnChannelCombine)
	ON_UPDATE_COMMAND_UI(	IDM_CHANNELCOMBINE, OnUpdateChannelCombine)
	ON_COMMAND(				IDM_CONVLA, 		OnConvLa)
	ON_UPDATE_COMMAND_UI(	IDM_CONVLA, 		OnUpdateConvLa)
	ON_COMMAND(				IDM_CONVSP, 		OnConvSp)
	ON_UPDATE_COMMAND_UI(	IDM_CONVSP, 		OnUpdateConvSp)
	ON_COMMAND(				IDM_CONVCT, 		OnConvCt)
	ON_UPDATE_COMMAND_UI(	IDM_CONVCT, 		OnUpdateConvCt)
	ON_COMMAND(				IDM_CONVPALETTE, 	OnConvPalette)
	ON_UPDATE_COMMAND_UI(	IDM_CONVPALETTE, 	OnUpdateConvPalette)
	ON_COMMAND(				IDM_CONVRGBCOLOR, 	OnConvRgbColor)
	ON_UPDATE_COMMAND_UI(	IDM_CONVRGBCOLOR, 	OnUpdateConvRgbColor)
	ON_COMMAND(				IDM_CONVCMYKCOLOR, 	OnConvCmykColor)
	ON_UPDATE_COMMAND_UI(	IDM_CONVCMYKCOLOR, 	OnUpdateConvCmykColor)
	ON_COMMAND(				IDM_COLORMANAGED, 	OnConvColorManaged)
	ON_UPDATE_COMMAND_UI(	IDM_COLORMANAGED, 	OnUpdateConvColorManaged)
	ON_COMMAND(				IDM_CMDLIST_UNDO, 	OnCmdListUndo)
	ON_UPDATE_COMMAND_UI(	IDM_CMDLIST_UNDO, 	OnUpdateCmdListUndo)
	ON_COMMAND(				IDM_CMDLIST_REDO, 	OnCmdListRedo)
	ON_UPDATE_COMMAND_UI(	IDM_CMDLIST_REDO, 	OnUpdateCmdListRedo)
	ON_COMMAND(				IDM_CMDLIST_EDIT, 	OnCmdListEdit)
	ON_UPDATE_COMMAND_UI(	IDM_CMDLIST_EDIT, 	OnUpdateCmdListEdit)
	ON_COMMAND(				IDM_UNDO, 			OnUndo)
	ON_UPDATE_COMMAND_UI(	IDM_UNDO, 			OnUpdateUndo)
	ON_COMMAND(				IDM_APPLY, 			OnApply)
	ON_UPDATE_COMMAND_UI(	IDM_APPLY, 			OnUpdateApply)
	ON_COMMAND(				IDM_DELETE, 		OnDelete)
	ON_UPDATE_COMMAND_UI(	IDM_DELETE, 		OnUpdateDelete)
	ON_COMMAND(				IDM_MAP, 			OnMap)
	ON_UPDATE_COMMAND_UI(	IDM_MAP, 			OnUpdateMap)
	ON_COMMAND(				IDM_CONTBRIT_STICK, OnContBritStick)
	ON_UPDATE_COMMAND_UI(	IDM_CONTBRIT_STICK, OnUpdateContBrit)
	ON_COMMAND(				IDM_COLORBAL_STICK, OnColorBalStick)
	ON_UPDATE_COMMAND_UI(	IDM_COLORBAL_STICK, OnUpdateColorBalance)
	ON_COMMAND(				IDM_CONTBRIT_VISUAL,OnContBritVisual)
	ON_UPDATE_COMMAND_UI(	IDM_CONTBRIT_VISUAL,OnUpdateContBrit)
	ON_COMMAND(				IDM_COLORBAL_VISUAL,OnColorBalVisual)
	ON_UPDATE_COMMAND_UI(	IDM_COLORBAL_VISUAL,OnUpdateColorBalance)
	ON_COMMAND(				IDM_POSTERIZE, 		OnPosterize)
	ON_UPDATE_COMMAND_UI(	IDM_POSTERIZE, 		OnUpdatePosterize)
	ON_COMMAND(				IDM_THRESHOLD, 		OnThreshold)
	ON_UPDATE_COMMAND_UI(	IDM_THRESHOLD, 		OnUpdateThreshold)
	ON_COMMAND(				IDM_HUE, 			OnHue)
	ON_UPDATE_COMMAND_UI(	IDM_HUE, 			OnUpdateHue)
	ON_COMMAND(				IDM_HUE_SHIFT, 		OnHueShift)
	ON_UPDATE_COMMAND_UI(	IDM_HUE_SHIFT, 		OnUpdateHueShift)
	ON_COMMAND(				IDM_QUARTERTONE,	OnToneBalance)
	ON_UPDATE_COMMAND_UI(	IDM_QUARTERTONE,	OnUpdateToneBalance)
	ON_COMMAND(				IDM_HISTOGRAM, 		OnHistogram)
	ON_UPDATE_COMMAND_UI(	IDM_HISTOGRAM, 		OnUpdateHistogram)
	ON_COMMAND(				IDM_PALETTEEDIT,	OnPaletteEdit)
	ON_UPDATE_COMMAND_UI(	IDM_PALETTEEDIT,	OnUpdatePaletteEdit)
	ON_COMMAND(				IDM_MASKLOAD, 		OnMaskLoad)
	ON_UPDATE_COMMAND_UI(	IDM_MASKLOAD, 		OnUpdateMaskLoad)
	ON_COMMAND(				IDM_MASKSAVE, 		OnMaskSave)
	ON_UPDATE_COMMAND_UI(	IDM_MASKSAVE, 		OnUpdateMaskSave)
	ON_COMMAND(				IDM_MASKUNDO, 		OnMaskUndo)
	ON_UPDATE_COMMAND_UI(	IDM_MASKUNDO, 		OnUpdateMaskUndo)
	ON_COMMAND(				IDM_MASKREMOVE, 	OnMaskRemove)
	ON_UPDATE_COMMAND_UI(	IDM_MASKREMOVE, 	OnUpdateMaskRemove)
	ON_COMMAND(				IDM_MASKINVERT, 	OnMaskInvert)
	ON_UPDATE_COMMAND_UI(	IDM_MASKINVERT, 	OnUpdateMaskInvert)
	ON_COMMAND(				IDM_MASKSTROKE, 	OnMaskStroke)
	ON_UPDATE_COMMAND_UI(	IDM_MASKSTROKE, 	OnUpdateMaskStroke)
	ON_COMMAND(				IDM_CROP, 			OnCrop)
	ON_UPDATE_COMMAND_UI(	IDM_CROP, 			OnUpdateCrop)
	ON_COMMAND(				IDM_MOVEUP, 		OnMoveUp)
	ON_UPDATE_COMMAND_UI(	IDM_MOVEUP, 		OnUpdateMoveObjects)
	ON_COMMAND(				IDM_MOVEDOWN, 		OnMoveDown)
	ON_UPDATE_COMMAND_UI(	IDM_MOVEDOWN, 		OnUpdateMoveObjects)
	ON_COMMAND(				IDM_MOVELEFT, 		OnMoveLeft)
	ON_UPDATE_COMMAND_UI(	IDM_MOVELEFT, 		OnUpdateMoveObjects)
	ON_COMMAND(				IDM_MOVERIGHT, 		OnMoveRight)
	ON_UPDATE_COMMAND_UI(	IDM_MOVERIGHT, 		OnUpdateMoveObjects)
	ON_COMMAND(				IDM_CALIBRATEAPPLY, OnCalibrateApply)
	ON_UPDATE_COMMAND_UI(	IDM_CALIBRATEAPPLY, OnUpdateCalibrateApply)
	ON_COMMAND(				IDM_CUT, 			OnCut)
	ON_UPDATE_COMMAND_UI(	IDM_CUT, 			OnUpdateCut)
	ON_COMMAND(				IDM_COPY, 			OnCopy)
	ON_UPDATE_COMMAND_UI(	IDM_COPY, 			OnUpdateCopy)
	ON_COMMAND(				IDM_COPYTOFILE, 	OnCopyToFile)
	ON_UPDATE_COMMAND_UI(	IDM_COPYTOFILE, 	OnUpdateCopyToFile)
	ON_COMMAND(				IDM_EXPORTFILE, 	OnExportFile)
	ON_UPDATE_COMMAND_UI(	IDM_EXPORTFILE, 	OnUpdateExportFile)

	ON_COMMAND(				IDC_CUSTOMVIEW, 	OnCustomView)
	ON_UPDATE_COMMAND_UI(	IDC_CUSTOMVIEW, 	OnUpdateZoomOut)
	ON_COMMAND(				IDC_QUICKZOOM, 		OnQuickZoom)
	ON_UPDATE_COMMAND_UI(	IDC_QUICKZOOM, 		OnUpdateQuickZoom)
	ON_COMMAND(				IDC_PROBEPOINT, 			OnProbePoint)
	ON_UPDATE_COMMAND_UI(	IDC_PROBEPOINT, 			OnUpdateProbePoint)
	ON_COMMAND(				IDC_PROBERECT, 			OnProbeRect)
	ON_UPDATE_COMMAND_UI(	IDC_PROBERECT, 			OnUpdateProbeRect)

END_MESSAGE_MAP()            

/* ******************************* Public Data ****************************** */

/* *************************** Private Functions **************************** */

/* *************************** Private Methods **************************** */

/////////////////////////////////////////////////////////////////////////
// CPPMDIChildWnd Methods

CImage* CPPMDIChildWnd::get_view_imageptr()
{
    CView *pView = GetActiveView (); 
    ASSERT(pView);    
    ASSERT(((CServerView *)pView)->GetDocument()); 
    return(((CServerView *)pView)->GetDocument()->GetImage());    
}
                                      
afx_msg void CPPMDIChildWnd::OnPaletteChanged(
CWnd *pFocusWnd)
{   
	if (pFocusWnd && IsIconic())
		InvalidateRect(NULL, TRUE);
}

BOOL CPPMDIChildWnd::handle_syscmd(
// Returns TRUE if the system message handled
UINT cmd,
LPARAM lParam) 
{
    BOOL bResult = FALSE;
    ITEMID idFileType;
    FNAME szFileName;
    LPFRAME lpFastbitsPreview;
    RECT rFastbitsGrid;
    CImage* pImage = get_view_imageptr();    
    
    switch (cmd)
        {
        case IDM_FASTBITS:
			if (pImage)
			{
            	idFileType = pImage->FileType;
            	lstrcpy( szFileName, pImage->CurFile );
            	lpFastbitsPreview = pImage->lpFastbitsPreview;
            	rFastbitsGrid = pImage->rFastbitsGrid;
            	DoFastbitsLoad( idFileType, szFileName, NULL, lpFastbitsPreview, 
            			&rFastbitsGrid, YES);
            	bResult = TRUE;         
			}
            break;
    
        case IDM_AVI_PREVIOUS:
        case IDM_AVI_NEXT:
//            if (pImage->DocumentType != IMG_AVIDOC)
//                break;
//            i = pImage->Element;
//            i += cmd == IDM_AVI_PREVIOUS ? -1:1;
//            if ( !ConfirmClose(NO, NULL) )
//                break;
//            lstrcpy(szFileName, pImage->CurFile);
//            lpFrame = avirdr(szFileName, -1, &idDataType, i);
//            if (!lpFrame)
//                break;
//            CloseImage(NO, pImage, NULL);
//            // Setup the new image and bring up the new image window
//            hNewWnd = NewImageWindow(
//                    szFileName, // Name
//                    lpFrame,    // lpNewFrame
//                    NULL,       // lpBase
//                    NULL,       // lpCmdList
//                    IDN_AVI,    // pImage->FileType
//                    idDataType,     // pImage->DataType
//                    IMG_AVIDOC, // pImage->DocumentType
//                    NULL,       // pImage->ImageName
//                    MAYBE       // Maximized?
//                    );
//            if (hNewWnd)
//                {
//			      pImage = ??GetDocument()->GetImage();
//                pImage->Element = AVIOptions.AVIPosition;
//                }
//            break;
    
        default:
//            FORWARD_WM_SYSCOMMAND(GetSafeHwnd(), cmd, x, y, ImageDoc_DefProc);
//            bResult = TRUE;
            bResult = FALSE;
            break;
    }       
    return (bResult);
} 


///////////////////////////////////////////////////////////////////////////
// CServerView Methods
void CServerView::draw_it (
CDC *pDC,
LPRECT lpRepairRect,
LPRECT lpInvalidRect)
{
    if (STOP) return;    
    CServerDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    ASSERT(pDoc->GetImage());
    ASSERT(m_lpDisplay);

    if (IsRectEmpty(lpRepairRect))
	    return;

    // Printing in progress... (SuperBlt not reentrant)
    if ( !pDC->IsPrinting () && !SuperPaint( GetSafeHwnd() ))
    {
        CImage* pImage;

        // Begin AstralImagePaint - ruttsa ruck bub!!!
        pImage = pDoc->GetImage();
        
        // Update rates from possible new view
		this->setup_rates( pImage );    // don't you just loveit!
        
        // Turn off our brush display for painting, make sure it
        // happens before erase background
        // *** Moved to WM_ERASEBKGND message processing ***
        if (GetSafeHwnd() == PictPubApp.Get_hActiveWindow())
            DisplayBrush(0, 0, 0, OFF);

        // Need to paint the image into the image window
        pDC->SetMapMode( MM_TEXT ); // work in pixel mode
		WindowsToAstralRect(lpRepairRect);
#ifdef WIN32
	    if (ImageIsBusy(pImage))
	    {
	        // save area needing repair
	        AstralUnionRect(&m_lpDisplay->PaintRect, &m_lpDisplay->PaintRect, lpRepairRect);
	        // Indicate that we're finished painting the entire client area
    	    return;
	    }
#endif
	    pImage->Paint( GetSafeHwnd(), pDC->GetSafeHdc(), lpRepairRect, lpInvalidRect);
	}
}
    
void CServerView::setup_rates( 
CImage* pImage)
{
    int DispHeight, DispWidth;

    DispHeight = RectHeight(&m_lpDisplay->DispRect);
    DispWidth = RectWidth(&m_lpDisplay->DispRect);
    // Always set new rates to be as accurate as possible
    if (DispHeight > 0 && DispWidth > 0)
    {
        if (DispWidth > DispHeight)
        {
            m_lpDisplay->FileRate = 
                FGET(RectWidth( &m_lpDisplay->FileRect ), DispWidth );
            m_lpDisplay->DispRate = 
                FGET(DispWidth, RectWidth( &m_lpDisplay->FileRect ) );
        }
        else
        {
            m_lpDisplay->FileRate = 
                FGET(RectHeight( &m_lpDisplay->FileRect),DispHeight);
            m_lpDisplay->DispRate = 
                FGET(DispHeight, RectHeight(&m_lpDisplay->FileRect ) );
        }
    }

    if (!m_lpDisplay->ViewPercentage)
        m_lpDisplay->ViewPercentage = FMUL(100,m_lpDisplay->DispRate);

	if (ZoomForum.GetViewPercentage() != m_lpDisplay->ViewPercentage)
		ZoomForum.SetViewPercentage(m_lpDisplay->ViewPercentage);
    // Draw the title bar with the new view percentage: lpDisplay->DispRate
    CFrameWnd* pFrame = GetParentFrame();        
	if (pFrame)
		pFrame->OnUpdateFrameTitle(TRUE);

}

/* ***************************** Public Methods ***************************** */
 
/////////////////////////////////////////////////////////////////////////
// CPPMDIChildWnd Methods

CPPMDIChildWnd::CPPMDIChildWnd () : CMDIChildWnd ()
{
}

BOOL CPPMDIChildWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
        CWnd* pParentWnd, CCreateContext* pContext)
/* This method is identical to CMDIChildWnd's LoadFrame EXCEPT for the call
   to "Create" where we pass in the preregistered CPPMDIChildWnd class name.
   This allows us to NOT have to use an ICON and thereby allow us to be able 
   to draw the current image into the iconized window.
*/
{
    // only do this once
//    ASSERT_VALID_IDR(nIDResource);
    ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);
    ASSERT(m_hMenuShared == NULL);      // only do once

    m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

    // parent must be MDI Frame (or NULL for default)
    ASSERT(pParentWnd == NULL || pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
    // will be a child of MDIClient
    ASSERT(!(dwDefaultStyle & WS_POPUP));
    dwDefaultStyle |= WS_CHILD;

    // if available - get MDI child menus from doc template
    ASSERT(m_hMenuShared == NULL);      // only do once
    CMultiDocTemplate* pTemplate;
    if (pContext != NULL &&
        (pTemplate = (CMultiDocTemplate*)pContext->m_pNewDocTemplate) != NULL)
    {
        ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CMultiDocTemplate)));
        // get shared menu from doc template
        m_hMenuShared = pTemplate->m_hMenuShared;
        m_hAccelTable = pTemplate->m_hAccelTable;
    }
    else
    {            
        TRACE0("Warning: no shared menu/acceltable for MDI Child window\n");
            // if this happens, programmer must load these manually
    }

    CString strFullString, strTitle;
    if (strFullString.LoadString(nIDResource))
        AfxExtractSubString(strTitle, strFullString, 0);    // first sub-string

    ASSERT(m_hWnd == NULL);

	//---------------------------------------------------------------------
	//==> :) lpszMDIChildClass passed in here!!! (Registered in AstralInit)
	//---------------------------------------------------------------------
    if (!CMDIChildWnd::Create(lpszMDIChildClass,
      strTitle, dwDefaultStyle, rectDefault,
      (CMDIFrameWnd*)pParentWnd, pContext))
        return FALSE;   // will self destruct on failure normally

    // it worked !
    return TRUE;
}


BOOL CPPMDIChildWnd::PreCreateWindow(CREATESTRUCT& cs)
{
    CMDIChildWnd::PreCreateWindow(cs);

	if (cs.style & FWS_ADDTOTITLE)
        cs.style ^= FWS_ADDTOTITLE;

    return (TRUE);
}

void CPPMDIChildWnd::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIChildWnd::OnUpdateFrameTitle(bAddToTitle);

    /////////////////////////////////////////////////////////////
    // In CPPMDIChildWnd::PreCreateWindow we remove the FWS_ADDTOFILE 
    // window style to allow us to handle all the required within 
    // the document title bar. With this in mind, build a name of 
    // the following form:  "viewpercent filename" 
    /////////////////////////////////////////////////////////////

	CServerView *pView = (CServerView *)GetActiveView (); 
	if (!pView || !pView->m_lpDisplay || !pView->GetDocument()) 
		return;
	
	CServerDoc *pDoc = ((CServerView*)pView)->GetDocument();
	CString TitleName = pDoc->GetTitle();
    int  Percentage;
    char szFormat[MAX_STR_LEN];
    char szTitle[MAX_STR_LEN];
    char szLastTitle[MAX_STR_LEN];

	// Redraw the title bar with the proper view percentage
    AstralStrEx( IDS_PREVIEW, szFormat, sizeof(szFormat) );
    if ( !(Percentage = pView->m_lpDisplay->ViewPercentage) )
        Percentage = FMUL( 100, pView->m_lpDisplay->DispRate );
    pView->m_lpDisplay->ViewPercentage = Percentage;

    if ( IsIconic() )
		lstrcpy( szTitle, TitleName );
    else    
		wsprintf( szTitle, szFormat, Percentage, TitleName );
        
    GetWindowText(szLastTitle, MAX_STR_LEN);
    if (Control.WinVer == WV_WINDOWS95)
	    stripext(szTitle);
    if (!StringsEqual(szTitle, szLastTitle))
        SetWindowText(szTitle);
}

afx_msg void CPPMDIChildWnd::OnClose ()
{
    CImage* pImage = get_view_imageptr();    
    if (pImage && ImageIsBusy(pImage))
        return;          
	DeactivateTool();

    CMDIChildWnd::OnClose();
}        
                                     
afx_msg void CPPMDIChildWnd::OnInitMenu(
CMenu *pMenu)
{
    ASSERT(pMenu);    
    CImage* pImage = get_view_imageptr();
    LPSTR lpString;
    CMenu *pSysMenu;
    
    if (!pImage || (ImageIsBusy(pImage) && (NumImageWindows(pImage) <= 1)))
        pMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
	else
        pMenu->EnableMenuItem(SC_CLOSE, MF_ENABLED);

    if (!pImage)
        return;

    if (pImage->DocumentType == IMG_DOCPIECE)
    {
        int state = pMenu->GetMenuState(IDM_FASTBITS, MF_BYCOMMAND);
        if (state < 0)
        {
            if (!AstralStr(IDS_FASTBITS, &lpString))
                return;
            pSysMenu = GetSystemMenu(FALSE);
            if (!pSysMenu)
                return;
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_BYCOMMAND|MF_STRING, IDM_FASTBITS, 
                lpString);
        }
    }
    else if (pImage->DocumentType == IMG_AVIDOC)
    {
        if ((int)pMenu->GetMenuState(IDM_AVI_PREVIOUS, MF_BYCOMMAND) < 0)
        {
            if (!AstralStr(IDS_AVI_PREVIOUS, &lpString))
                return;
            pSysMenu = GetSystemMenu(FALSE);
            if (!pSysMenu)
                return;
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_BYCOMMAND|MF_STRING, IDM_AVI_PREVIOUS, 
                lpString);
        }
        if ((int)pMenu->GetMenuState(IDM_AVI_NEXT, MF_BYCOMMAND) < 0)
        {
            if (!AstralStr(IDS_AVI_NEXT, &lpString))
                return;
            pSysMenu = GetSystemMenu(FALSE);
            if (!pSysMenu)
                return;
            pSysMenu->AppendMenu(MF_BYCOMMAND|MF_STRING, IDM_AVI_NEXT, lpString);
        }
    }
}   

afx_msg void CPPMDIChildWnd::OnMDIActivate(
BOOL bActivate,
CWnd* pActivateWnd, 
CWnd* pDeactivateWnd)
{
	CMDIChildWnd::OnMDIActivate(bActivate,pActivateWnd,pDeactivateWnd);
}

afx_msg void CPPMDIChildWnd::OnPaint()
{
	// if Iconic then draw the Image in the Icon Area
	CServerView *pView = (CServerView*)GetActiveView();
	CImage *pImage = pView->GetDocument()->GetImage();
	if (IsIconic() && pImage && !pImage->IsBusy())
	{
	    CPaintDC dc(this);
		CRect rBounds;
		ASSERT(pView->GetDocument()->GetImage());
		CBrush brush; brush.CreateStockObject(LTGRAY_BRUSH);
		dc.FillRect(&dc.m_ps.rcPaint,&brush);

		ImgGetImageRect(pView->GetDocument()->GetImage(), &rBounds);
		CRect DstRect(dc.m_ps.rcPaint.left,dc.m_ps.rcPaint.top,
			dc.m_ps.rcPaint.right,dc.m_ps.rcPaint.bottom);
		int w1 = rBounds.Width();
		int h1 = rBounds.Height();
		int w2 = DstRect.Width();
		int h2 = DstRect.Height();
		if (w1 != h1)
		{
			if (w1 > h1)
				h2 = (long)w2*(long)h1/(long)w1;
			else 
				w2 = (long)w1*(long)h2/(long)h1;
		
			POINT ptCent;
			ptCent.x = DstRect.Width()/2;
			ptCent.y = DstRect.Height()/2;
			DstRect.left = ptCent.x - w2 / 2;
			DstRect.top = ptCent.y - h2 / 2;
			DstRect.right = DstRect.left+w2;
			DstRect.bottom = DstRect.top+h2;
		}

		DrawImage(dc.GetSafeHdc(),NULL,&rBounds,&DstRect,
			pView->GetDocument()->GetImage());
		return;
	}

	CMDIChildWnd::OnPaint();
}

afx_msg int  CPPMDIChildWnd::OnMouseActivate( 
CWnd *pwndTopLevel, 
UINT nHitTest, 
UINT msg)
{
    if (!fAppActive)
        LastActivated = TRUE;
        
    return (CMDIChildWnd::OnMouseActivate(pwndTopLevel,nHitTest,msg));
}

afx_msg void CPPMDIChildWnd::OnSysCommand(
UINT cmd,                                               
LPARAM lParam)
{                        
    if (!handle_syscmd(cmd,lParam))
        CMDIChildWnd::OnSysCommand(cmd,lParam);
}

afx_msg void CPPMDIChildWnd::OnWindowPosChanging( 
LPWINDOWPOS lpWindowPos)
{
    BOOL bMaximized;    
	LPDISPLAY lpDisplay = NULL;

    CServerView *pServerView = (CServerView*)GetActiveView(); 
        // on this MDIChild
    //ASSERT(pServerView && pServerView->m_lpDisplay);

	if (pServerView)
	    lpDisplay = pServerView->m_lpDisplay;    

    CServerDoc *pServerDoc = (CServerDoc*)GetActiveDocument();  
        //current Doc on current active view
    //ASSERT(pServerDoc);

    // looking for a z-order change with an activate
    if (!lpDisplay || !pServerDoc ||
		(lpWindowPos->flags & (SWP_NOZORDER|SWP_NOACTIVATE)) )
    {
        CMDIChildWnd::OnWindowPosChanging(lpWindowPos);
        return;
    }
    
        
    CMDIChildWnd* pMDIChild = 
        ((CMDIFrameWnd*)PictPubApp.m_pMainWnd)->MDIGetActive(&bMaximized);                        
    HWND hWndActive = pMDIChild ? pMDIChild->GetSafeHwnd() : NULL;

  	// The WindowPosChanged flag is used to tell us whether this is 
	// a new window or not.  We want an activate for new windows
	// because it's possible when playing a macro back that the 
	// windows get opened in a different order than when the macro
	// was recorded.  The activates makes sure the windows are
	// in the correct Z-order so that functions and activations
	// later do the right thing
    if ((hWndActive && (GetSafeHwnd() != hWndActive)) || !lpDisplay->WindowPosChanged)
    {
        ACTIVATEWINDOW_PARMS parms;
        
        lpDisplay->WindowPosChanged = YES;
        lstrcpy(parms.szFileName, pServerDoc->GetPathName());

        int iOffset = 0;
        HWND hNextWnd = hWndActive;
        // look in forward direction for offset between active
        // window and the window being activated
        while (hNextWnd)
        {
            if (hNextWnd == GetSafeHwnd())
                break;
            ++iOffset;
            hNextWnd = ::GetWindow(hNextWnd, GW_HWNDNEXT);
        }
        if (iOffset || lstrlen(parms.szFileName))
        {
            parms.iOffset = iOffset;
            PostProgCommand(IDS_CMD_ACTIVATEWINDOW, &parms);
        }
    }                                                  
    CMDIChildWnd::OnWindowPosChanging(lpWindowPos);    
}

afx_msg void CPPMDIChildWnd::OnNcLButtonDown(
UINT nHitTest,
CPoint point)
{                   
    if ( this != GetFocus() )
        SetFocus();
    CMDIChildWnd::OnNcLButtonDown(nHitTest,point);                                
}


/////////////////////////////////////////////////////////////////////////////
// CServerView Methods

CServerView::CServerView()
{
    // TODO: add construction code here
    m_lpDisplay = NULL;
	m_bAllowInterrupt = TRUE;
	m_iSizeCallDepth = 0;
	m_fCapture = FALSE;

	m_prevDropEffect = DROPEFFECT_NONE;

	m_fActivated = FALSE;
	m_fPositioned = FALSE;

	m_pOneShotTool = NULL;
}

CServerView::~CServerView()
{
    if (m_lpDisplay)
	{
        FreeUp((LPTR)m_lpDisplay);
		m_lpDisplay = NULL;
	}
}

void CServerView::SetupOneShotTool(COneShotTool *pTool)
{
	if (m_pOneShotTool)
		delete m_pOneShotTool;
	m_pOneShotTool = pTool;
}

void CServerView::UpdateWindowNow()
{
	RECT rUpdate;
	BOOL bSaveAllowInterrupt;

	if (m_lpDisplay)
	{
    	rUpdate = m_lpDisplay->UpdateRect;
    	AstralSetRectEmpty(&m_lpDisplay->UpdateRect);
    	bSaveAllowInterrupt = m_bAllowInterrupt;
		m_bAllowInterrupt = FALSE;
    	if (!AstralIsRectEmpty(&rUpdate))
        	InvalidateRect(AstralToWindowsRect(&rUpdate), FALSE);
    	UpdateWindow();
		m_bAllowInterrupt = bSaveAllowInterrupt;
	}
}

void CServerView::DoActivate(
BOOL bActivate,
CWnd* pActivateWnd, 
CWnd* pDeactiveWnd )  
{
    if (STOP) return;    

	CImage* pImage = GetDocument()->GetImage();

	// Handle a deactivation
	if ( !bActivate )
    {
		if (m_fActivated) // only do deactivate code if we are currently active
		{
			ZoomForum.SetViewPercentage(m_lpDisplay->ViewPercentage);
			// always kill the one shot tool on activates
			SetupOneShotTool();

	    	if (pDeactiveWnd)   // any current window active?
        	{
    	    	DeactivateTool();               // deactivate current tool
	        	EnableMaskDisplay(pImage, NO);  // undraw marquee or slime
	        	SetupMiniViews(NULL, NO);
        	}
	    	// when a document is deactivated, make sure it gets
    		// unplugged from superblt
	    	SetNewPalette(lpBltScreen, lpBltScreen->ColorPaletteType,
                      	lpBltScreen->hColorPal, lpBltScreen->lpColorPaletteLUT,
                      	lpBltScreen->ColorPalette, lpBltScreen->nColorEntries,
					  	FALSE /* fApplyGamma */);
	    	SetUnitResolution( 0 );     // reset unit resolution - why here?
			// if we have an HDC, always release it in here for deactivate
			if ( ::Window.hDC )
    		{
    			::ReleaseDC( GetSafeHwnd(), ::Window.hDC );
		    	::Window.hDC = NULL;
    		}
			m_fActivated = FALSE;
		}
    }
	// Handle a Activation
	else
	{
		if (!m_fActivated) // only do activate code if we are currently deactive
		{
			ZoomForum.SetViewPercentage(0);
			// always kill the one shot tool on activates
			SetupOneShotTool();

			// if we have an HDC, always release it in here for activate 
			if ( ::Window.hDC )
    		{
    			::ReleaseDC( GetSafeHwnd(), ::Window.hDC );
		    	::Window.hDC = NULL;
    		}
			// Handle an activation
			::Window.hDC = ::GetDC(GetSafeHwnd());
			// Handle an activation
			SetupImageDisplayPalette();
			EnableMaskDisplay(pImage, YES); // turn on marquee or redraw slime
			// setup title bar, resolution and zoom box
			HookZoom();
			SetUnitResolution( 0 );
			SetupMiniViews(NULL, NO);
			DisplayInfo(0, 0, NULL);    // redisplay info for this tyhpe
	
			// make sure status bar is up to data
			UpdateStatusBar( YES, YES, YES, NO );
			m_fActivated = TRUE;
		}
	}
}

void CServerView::SetupImageDisplayPalette()
{
	if (STOP) return;
		 
	HDC hDC;
	CServerDoc* pDoc = GetDocument();
	CImage* pImage = pDoc->GetImage();

    if (pImage->hPal) // any new palette?
    {   // select and realize palette for this image
        SetNewPalette(lpBltScreen, pImage->PaletteType,
                    pImage->hPal, pImage->lpPaletteLUT,
                     pImage->Palette, pImage->nPaletteEntries,
					 TRUE /* fApplyGamma */);
        hDC = ::GetDC(GetSafeHwnd());
		SuperBltSetupPalette(lpBltScreen, hDC, lpBltScreen->hPal, TRUE);
        ::ReleaseDC(GetSafeHwnd(), hDC);
     }
    // Always update the active colors
    PictPubApp.GetCustomBoxes()->InvalidateColorPatches(FALSE);
}

BOOL CServerView::AutoScroll(int x, int y)
{
	HWND hWnd;
	RECT ClientRect;
	BOOL fScrolled = NO, fScrollStarted = NO;

	if (!STOP)
	{
		hWnd = GetSafeHwnd();
		GetClientRect(&ClientRect);
		--ClientRect.right;
		--ClientRect.bottom;

		if (x < ClientRect.left)
		{
			fScrollStarted = YES;
			fScrolled = ScrollImage(hWnd, NO, SB_LINEUP, 0);
		}
		else
		if (x > ClientRect.right)
		{
			fScrollStarted = YES;
			fScrolled = ScrollImage(hWnd, NO, SB_LINEDOWN, 0);
		}
		if (y < ClientRect.top)
		{
			fScrollStarted = YES;
			fScrolled = ScrollImage(hWnd, YES, SB_LINEUP, 0);
		}
		else
		if (y > ClientRect.bottom)
		{
			fScrollStarted = YES;
			fScrolled = ScrollImage(hWnd, YES, SB_LINEDOWN, 0);
		}

		if (fScrollStarted)
			ScrollImage(hWnd, YES, SB_ENDSCROLL, 0);

		if ( fScrolled )
			UpdateWindowNow();
	}
	return( fScrolled );
}


BOOL CServerView::PreCreateWindow(
CREATESTRUCT& cs)
{          
    BOOL bResult ;
    
    if (bResult = CView::PreCreateWindow(cs))    
    {
        // OR in Horizontal and Vertical Scrolling bars
        cs.style = cs.style | WS_HSCROLL | WS_VSCROLL;    
    }        
            
    return (bResult);        
}


void CServerView::OnActivateView( 
// Called by Framework when View is activated or deactivated
BOOL bActivate, 
CView* pActivateView, 
CView* pDeactiveView )  
{
   // Only Interested in adding ourselves to the Task/view list if
   // we are the currently active view.  
   if (bActivate)
	   PictPubApp.SetActiveView(this);

   /* If we're activating this child, remember it */
   if (bActivate)
   {
      SetFocus();

      //BOOL bMaximized;
      //CMDIChildWnd *pMDIChild = 
      //	   ((CMDIFrameWnd*)PictPubApp.m_pMainWnd)->MDIGetActive(&bMaximized);
      //if (pMDIChild)
      //{
      //   if ( Tool.hRibbon )
      //      ::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 0 );
      //   DoActivate(FALSE,pActivateView,pMDIChild->GetActiveView());
      //}

      DoActivate(bActivate,pActivateView,pDeactiveView);
      if ( Tool.hRibbon )
         ::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 0 );
   }
   else
   {
      LastActivated = TRUE;
   	//BOOL bMaximized;
   	//CMDIChildWnd *pMDIChild = 
   	//	((CMDIFrameWnd*)PictPubApp.m_pMainWnd)->MDIGetActive(&bMaximized);
   	//if (pMDIChild)
   	//{
   	//   if ( Tool.hRibbon )
   	//      ::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 0 );
   	//	DoActivate(FALSE,pActivateView,pMDIChild->GetActiveView());
   	//}
   
      if ( Tool.hRibbon )
   	     ::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 0 );
   	DoActivate(bActivate,pActivateView,pDeactiveView);
   }
}              

afx_msg void CServerView::OnDestroy()   
{    
    DeactivateTool();                                           
	UnhookZoom(GetSafeHwnd());

	CServerDoc* pDoc = GetDocument();
	if (pDoc && !pDoc->IsInPlaceActive())
	{
		CMDIFrame* pMDIFrame = (CMDIFrame*)PictPubApp.m_pMainWnd;
		int nDocs = pMDIFrame->GetNumMDIChildWindows() - 1;
		if (nDocs <= 0)
    	{
        	::Window.fHasZoom = FALSE;
        	if ( Tool.hRibbon )
            	::SendMessage( Tool.hRibbon, WM_DOCACTIVATED, 0, 1L );
    	}
	}

    if ( Tool.hRibbon )
        POST_WM_DOCCHANGED( Tool.hRibbon );

	// We are Dying... If we are the currently active View,then set it to NO-ACTIVE View
	if (PictPubApp.GetActiveView() == this)
		PictPubApp.SetActiveView(NULL);
    CView::OnDestroy();
}

BOOL CServerView::GetObjectInfo(COleDataObject* pDataObject,
    CSize* pSize, CSize* pOffset)
{
    ASSERT(pSize != NULL);

    // get object descriptor data
    HGLOBAL hObjDesc = pDataObject->GetGlobalData(_oleData.cfObjectDescriptor);
    if (hObjDesc == NULL)
    {
        if (pOffset != NULL)
            *pOffset = CSize(0, 0); // fill in defaults instead
        *pSize = CSize(0, 0);
        return FALSE;
    }
    ASSERT(hObjDesc != NULL);

    // otherwise, got CF_OBJECTDESCRIPTOR ok.  Lock it down and extract size.
    LPOBJECTDESCRIPTOR pObjDesc = (LPOBJECTDESCRIPTOR)GlobalLock(hObjDesc);
    ASSERT(pObjDesc != NULL);
    pSize->cx = (int)pObjDesc->sizel.cx;
    pSize->cy = (int)pObjDesc->sizel.cy;
    if (pOffset != NULL)
    {
        pOffset->cx = (int)pObjDesc->pointl.x;
        pOffset->cy = (int)pObjDesc->pointl.y;
    }
    GlobalUnlock(hObjDesc);
    GlobalFree(hObjDesc);

    // successfully retrieved pSize & pOffset info
    return TRUE;
}

DROPEFFECT CServerView::OnDragEnter(
COleDataObject* pDataObject,
DWORD dwKeyState, 
CPoint point)
{
	ASSERT(m_prevDropEffect == DROPEFFECT_NONE);
	if (STOP)
		return(DROPEFFECT_NONE);

	GetObjectInfo(pDataObject, &m_dragSize, &m_dragOffset);
	CClientDC dc(NULL);
	dc.HIMETRICtoDP(&m_dragSize);
	dc.HIMETRICtoDP(&m_dragOffset);
	return OnDragOver(pDataObject, dwKeyState, point);
}

extern void CheckifBrowserDlg(CPoint pt, DROPEFFECT &de);
DROPEFFECT CServerView::OnDragOver(
COleDataObject* pDataObject,
DWORD dwKeyState, 
CPoint point)
{
    DROPEFFECT de = DROPEFFECT_NONE;
	if (!m_lpDisplay)
		return de;

	CRect rImageRect = m_lpDisplay->DispRect;
    CPoint   pt;

	if (pDataObject->IsDataAvailable(_oleData.cfPPFileName) ||
		pDataObject->IsDataAvailable(Control.cfImage) ||
	    pDataObject->IsDataAvailable(CF_DIB) ||
	    pDataObject->IsDataAvailable(CF_BITMAP))
    {
        pt = point;
    	ClientToScreen(&pt);
    	if (point.x <= rImageRect.right && point.x >= rImageRect.left &&
    		point.y <= rImageRect.bottom && point.y >= rImageRect.top)
    	{
    		point -= m_dragOffset;  // adjust target rect by original cursor offset
    	
    	    if ((dwKeyState & MK_ALT) == MK_ALT)
    	        de = DROPEFFECT_MOVE;
    	    else
    	        de = DROPEFFECT_COPY;
    	
    		if (point == m_dragPoint)
    	    	return de;
    	}
    }    

    CheckifBrowserDlg(pt, m_prevDropEffect);
    // otherwise, cursor has moved -- need to update the drag feedback
    CClientDC dc(this);
    if (m_prevDropEffect != DROPEFFECT_NONE)
    {
        // erase previous focus rect
        dc.DrawFocusRect(CRect(m_dragPoint, m_dragSize));
    }
    
    m_prevDropEffect = de;

    if (m_prevDropEffect != DROPEFFECT_NONE)
    {
        m_dragPoint = point;
        dc.DrawFocusRect(CRect(point, m_dragSize));
    }

   	return de;
}

LONG CServerView::OnDropActivate(UINT wParam, LONG lParam)
// user defined message sent by OnDrop method to handle the actual drop
{
	ASSERT(wParam);
	if (!wParam)
		return 0L;
	HGLOBAL hMem = (HGLOBAL)wParam;

    CServerDoc* pDoc = GetDocument();
	if (!pDoc)
		return 0L;

	DROPMSG FAR *lpDropmsg = (DROPMSG FAR *)GlobalLock(hMem);
	if (lpDropmsg && lpDropmsg->lpObject)
	{
		if (GetParentFrame()->IsKindOf(RUNTIME_CLASS(CPPMDIChildWnd)))
			((CMDIFrame*)::AfxGetMainWnd())->MDIActivate(GetParentFrame());
		init_paste(lpDropmsg->lpObject,&lpDropmsg->point);
		TransformObjectsStart(pDoc->GetImage(), PasteFromEndProc, 
			IDC_OBJECTTRANSFORM, YES,
			IDS_UNDOPASTE, YES, lpDropmsg->szClipName, 0, lpDropmsg->bExtName,
			&lpDropmsg->SrcTypeInfo);
		GlobalUnlock(hMem);
	 	GlobalFree(hMem);
		return 1L;
	}
	return 0L;
}

BOOL CServerView::OnDrop(
COleDataObject* pDataObject,
DROPEFFECT dropEffect, 
CPoint point)
{
	int cfAvailable = -1;
	STGMEDIUM StgMedium;
    FORMATETC FormatEtc;

    // clean up focus rect
	OnDragLeave();

	if (pDataObject->IsDataAvailable(_oleData.cfPPFileName))
    {
 		cfAvailable = _oleData.cfPPFileName;
        SETDefFormatEtc(FormatEtc,cfAvailable,TYMED_HGLOBAL);
   	}
	else
	if (pDataObject->IsDataAvailable(Control.cfImage5Ptr))
	{
 		cfAvailable = Control.cfImage5Ptr;
        SETDefFormatEtc(FormatEtc,cfAvailable,TYMED_HGLOBAL);
	}
	else if (pDataObject->IsDataAvailable(Control.cfImage))
	{
		cfAvailable = Control.cfImage;
	    SETDefFormatEtc(FormatEtc,Control.cfImage,TYMED_HGLOBAL);
	}
	else
	if (pDataObject->IsDataAvailable(CF_DIB))
	{
		cfAvailable = CF_DIB;
	    SETDefFormatEtc(FormatEtc,CF_DIB,TYMED_HGLOBAL);
	}
	else
	if (pDataObject->IsDataAvailable(CF_BITMAP))  
	{
		cfAvailable = CF_BITMAP;
	    SETDefFormatEtc(FormatEtc,CF_BITMAP,TYMED_GDI);
	}

	if (cfAvailable == -1)
		return FALSE;

	if (pDataObject->GetData(cfAvailable,&StgMedium,&FormatEtc))
	{
		LPOBJECT lpObject;
		BOOL bExtName = FALSE;
		FNAME szClipName;
		FRMTYPEINFO SrcTypeInfo;
	    CServerDoc* pDoc = GetDocument();
		ASSERT(pDoc);
		ASSERT(pDoc->GetImage());

		if (!pDoc->GetImage())
			return FALSE;

		FrameSetTypeInfo(&SrcTypeInfo, FDT_NONE);
    	DeactivateTool();
        ProgressBegin(1, PROGRESS_ID(IDS_UNDOPASTECLIP));

        if (cfAvailable == _oleData.cfPPFileName)
        {
        	PPFILENAME *ppFileName = (PPFILENAME *)GlobalLock(StgMedium.hGlobal);
			FNAME 		szDataFile;
			FNAME 		szMaskFile;

			szMaskFile[0] ='\0';
			if (bExtName = ppFileName->bExtName)
			{
				lstrcpy(szClipName, ppFileName->szFileName);
				if (LookupExtFileN(szClipName, szDataFile, IDN_CLIPBOARD, NO))
				{
					if (GetPPFileType(szDataFile) != IDN_PP)
						LookupExtFileN(szClipName, szMaskFile, IDN_MASK, NO);
				}
				else
				{
					ProgressEnd();
			        GlobalUnlock(StgMedium.hGlobal);
					ReleaseStgMedium (&StgMedium);
					return (FALSE);
				}
			}
			else
			{
				lstrcpy(szDataFile, ppFileName->szFileName);
				lstrcpy(szClipName, szDataFile);
			}

		    lpObject = ImgCreateClipInObject(pDoc->GetImage(), szDataFile, szMaskFile, Control.CMSEnabled, &SrcTypeInfo);
	        GlobalUnlock(StgMedium.hGlobal);
        }
		else
        if (cfAvailable == Control.cfImage5Ptr)
        {
        	LPDWORD pImagePtr = (LPDWORD)GlobalLock(StgMedium.hGlobal);
			CImage* lpSrcImage = (CImage*)pImagePtr;
	        GlobalUnlock(StgMedium.hGlobal);

			lpObject = GetObjectFromInternalData(pDoc->GetImage(),
											 	lpSrcImage,
											 	FALSE,
											 	szClipName,
												&SrcTypeInfo);
        }
		else
		{
			FormFileName (Names.PasteImageFile, IDN_CLIPBOARD,"WINCLIP.BMP");
	    	Names.PasteMaskFile[0] = '\0';

			if (!PasteDropData(cfAvailable,StgMedium.hGlobal,Names.PasteImageFile,
				Names.PasteMaskFile))
			{
	            ProgressEnd();
				return(FALSE);
			}

			lpObject = GetObjectFromExternalData(pDoc->GetImage(),
											 	 Names.PasteImageFile,
										 	 	 Names.PasteMaskFile,
										 	 	 FALSE,
										 	 	 szClipName,
									  		 	 &SrcTypeInfo);
		}
		ReleaseStgMedium (&StgMedium);
		ProgressEnd();
		if (lpObject)
		{
			DROPMSG FAR *lpDropMsg = (DROPMSG*)GlobalAllocPtr(GMEM_MOVEABLE, 
				sizeof(DROPMSG));

			if (lpDropMsg)
			{
				lpDropMsg->point = point;
				lpDropMsg->lpObject = lpObject;
				lpDropMsg->bExtName = bExtName;
				lpDropMsg->SrcTypeInfo = SrcTypeInfo;
				lstrcpy(lpDropMsg->szClipName,szClipName);
			 	HGLOBAL hMem = GlobalPtrHandle(lpDropMsg);
			 	GlobalUnlockPtr(lpDropMsg);
				PostMessage(WM_DROPACTIVATE,(WPARAM)hMem,0L);

				return (TRUE);
			}
		}
	}
    return (FALSE);
}

void CServerView::OnDragLeave()
{
	CClientDC dc(this);
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
	    dc.DrawFocusRect(CRect(m_dragPoint,m_dragSize)); // erase previous focus rect
	    m_prevDropEffect = DROPEFFECT_NONE;
	}
}

void CServerView::OnInitialUpdate( )
{   
    ASSERT(GetDocument() != NULL);
    CImage* pImage = GetDocument()->GetImage();

    CFrameWnd *pParentFrame = GetParentFrame();

	// set some window class stuff that doesn't matter whether
	// we have an image or not
	::SetClassBackground(GetSafeHwnd(), (HBRUSH)(::Window.hBGBrush ? 
        ::Window.hBGBrush : GetStockObject(LTGRAY_BRUSH)));

    ::SetClassCursor( GetSafeHwnd(), ::Window.hCursor );

	// if we don't have an image yet, we can't do this
	// let CView handle it for now
	if (!pImage)
	{
		int x, y;
		if (!m_fPositioned)
		{
			GetDocumentPosition(&x, &y);
		    pParentFrame->SetWindowPos(NULL, x, y, 0, 0,
				SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
			m_fPositioned = TRUE;
		}
		CView::OnInitialUpdate();
		return;
	}

    ImgInitDisplay(pImage);

	if (!m_lpDisplay) // This allows us to reenter this init routine  w/out realloc
	{
    	RECT WindowRect;
    	int dx, dy;
		int x, y;

	    if (!(m_lpDisplay = (LPDISPLAY)Alloc( (long)sizeof(DISPLAY) )) )
	    {
	        ASSERT(m_lpDisplay);
	        return;
	    }
	    clr ((LPTR)m_lpDisplay, sizeof(DISPLAY));

    	ImgGetInfo(pImage, &dx, &dy, NULL, NULL);

    	m_lpDisplay->FileRate = TINYRATE;
    	m_lpDisplay->ViewPercentage = 0;
    	SetRect( &m_lpDisplay->FileRect, 0,0,dx-1, dy-1);
    	AstralSetRectEmpty( &m_lpDisplay->UpdateRect );
    	AstralSetRectEmpty( &m_lpDisplay->PaintRect );
                         
    	m_lpDisplay->iScrollMin[0]      = m_lpDisplay->iScrollMin[1] = 0;
    	m_lpDisplay->iScrollMax[0]      = m_lpDisplay->iScrollMax[1] = 100;
    	m_lpDisplay->iScrollOutMin[0]   = m_lpDisplay->iScrollOutMin[1] = 0;
    	m_lpDisplay->iScrollOutMax[0]   = m_lpDisplay->iScrollOutMax[1] = 100;
    	m_lpDisplay->ScrollRate[0]      = m_lpDisplay->ScrollRate[1] = FUNITY;

		if (m_fPositioned)
		{
			pParentFrame->GetWindowRect(&WindowRect);
			CWnd *pParent = pParentFrame->GetParent();
			pParent->ScreenToClient((LPPOINT)&WindowRect.left);
			x = WindowRect.left;
			y = WindowRect.top;
		}
		else
			x = y = -1;

    	CalcFullViewWindowSize(&WindowRect, &m_lpDisplay->FileRect,
        	m_lpDisplay->FileRate, pImage->npix, pImage->nlin,
        	x, y, View.UseRulers, YES);

    	if ( View.UseRulers )
        	CreateRulers(GetSafeHwnd());

		if (!pParentFrame->IsZoomed())
	    	pParentFrame->MoveWindow(	WindowRect.left,
										WindowRect.top,
										RectWidth(&WindowRect),
    						    		RectHeight(&WindowRect));
		m_fPositioned = TRUE;

    	SetupScrollBars(GetSafeHwnd());
    	this->GetWindowRect(&m_lpDisplay->WindowRect);
    	GetParent()->GetParent()->ScreenToClient(&m_lpDisplay->WindowRect);

    	SaveLastView(GetSafeHwnd());
	}
	else
	{
		RECT rClient;
		GetClientRect(&rClient);
		OnSize(SIZE_RESTORED, rClient.right-rClient.left, rClient.bottom-rClient.top);
	}
	if (this == PictPubApp.GetActiveView())
	   	DoActivate(TRUE,this,NULL);
}

void CServerView::OnDraw(CDC* pDC)
{                 
	RECT rClient;

    if (STOP) return;

    // Update our rulers, scroll bars, and rates from possible new view
    UpdateRulers( GetSafeHwnd() );
    SetupScrollBars( GetSafeHwnd() );
        
	GetClientRect(&rClient);
	draw_it( pDC , &rClient );
}

void CServerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    ASSERT(pSender != this);
	RECT rUpdate, rDisp;

	if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CUpdateArea)))
	{
		rUpdate = *((CUpdateArea*)pHint)->GetUpdateArea();
		File2DispRect(GetSafeHwnd(), &rUpdate, &rDisp);
		InvalidateRect(&rDisp, FALSE);
		return;	// handled therefore return
	}

	CView::OnUpdate(pSender,lHint,pHint);
}

/////////////////////////////////////////////////////////////////////////////
// CServerView diagnostics

#ifdef _DEBUG
void CServerView::AssertValid() const
{
    CView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CServerDoc* CServerView::GetDocument() // non-debug version is inline
{
    ASSERT( (m_pDocument == NULL) ||
		m_pDocument->IsKindOf(RUNTIME_CLASS(CServerDoc)));
    return (CServerDoc*)m_pDocument;
}
#endif //_DEBUG


afx_msg int CServerView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	m_dropTarget.Register(this);
    return(CView::OnCreate(lpCreateStruct));
}

afx_msg BOOL CServerView::OnEraseBkgnd(CDC *cdc)
{
    if (!STOP)
	{
    	// Turn off our brush display for painting, make sure it
    	// happens before erase background
    	//  if (GetSafeHwnd() == PictPubApp.Get_hActiveWindow())
    	DisplayBrush(0, 0, 0, OFF);
	}

    return (CView::OnEraseBkgnd(cdc));
}
    
afx_msg void CServerView::OnSetFocus(CWnd *pOldWnd)
{
	if (STOP) return;

    if (Tool.bActive)
        MsgProc(GetSafeHwnd(), WM_SETFOCUS, 0, 0, (UINT32)(LPTR)pOldWnd->GetSafeHwnd());
}

afx_msg void CServerView::OnKillFocus(CWnd *pNewWnd)
{
	if (STOP) return;

    FocusObjectEnd(GetSafeHwnd());
    if (Tool.bActive)
        MsgProc(GetSafeHwnd(), WM_KILLFOCUS, 0, 0, (UINT32)(LPTR)pNewWnd->GetSafeHwnd());
}

/***********************************************************************/
afx_msg void CServerView::OnChar( UINT ch, UINT cRepeat, UINT nFlags)
/***********************************************************************/
{
	if (STOP) return;

    HWND hWnd = GetSafeHwnd();

    if (Tool.bActive)
        MsgProc(hWnd, WM_CHAR, 0, 0, (UINT32)ch);
}

afx_msg void CServerView::OnKeyDown( UINT nChar, UINT cRepeat, UINT flags)
{
	if (STOP) return;

    if ( Tool.bActive )
        {
        if (nChar == VK_RETURN && Tool.id != IDC_TEXT)
                DeactivateTool();
        else
            MsgProc(GetSafeHwnd(), WM_KEYDOWN, 0, 0, (UINT32)nChar);
        return;
        }
    /* Translate keyboard messages to scroll commands */
    switch (nChar)
    {
    case VK_TAB:
        FocusObjectStart( GetSafeHwnd() );
        break;
    case VK_SPACE:
        FocusObjectSelect( GetSafeHwnd() );
        break;
    case VK_DELETE:
        if (!FocusObjectDelete(GetSafeHwnd()))
            MsgProc(GetSafeHwnd(), WM_KEYDOWN, 0, 0, (UINT32)nChar);
        break;
    case VK_UP:     PostMessage (WM_VSCROLL, SB_LINEUP,   0L);
        break;
    case VK_DOWN:   PostMessage (WM_VSCROLL, SB_LINEDOWN, 0L);
        break;
    case VK_PRIOR:  PostMessage (WM_VSCROLL, SB_PAGEUP,   0L);
        break;
    case VK_NEXT:   PostMessage (WM_VSCROLL, SB_PAGEDOWN, 0L);
        break;
    case VK_HOME:   PostMessage (WM_HSCROLL, SB_PAGEUP,   0L);
        break;
    case VK_END:    PostMessage (WM_HSCROLL, SB_PAGEDOWN, 0L);
        break;
    case VK_LEFT:   PostMessage (WM_HSCROLL, SB_LINEUP,   0L);
        break;
    case VK_RIGHT:  PostMessage (WM_HSCROLL, SB_LINEDOWN, 0L);
        break;
   }
}

afx_msg void CServerView::OnKeyUp( UINT nChar, UINT cRepeat, UINT flags)
{
	if (STOP) return;

    if ( Tool.bActive )
        {
        if (nChar == VK_ESCAPE)
            DestroyProc( GetSafeHwnd(), 1 );
        else
            MsgProc(GetSafeHwnd(), WM_KEYUP, 0, 0, (UINT32)nChar);
        return;
        }
    switch (nChar)
    {
    case VK_UP:
    case VK_DOWN:
    case VK_PRIOR:
    case VK_NEXT:
        PostMessage (WM_VSCROLL, SB_ENDSCROLL, 0L);
        break;
    case VK_HOME:
    case VK_END:
    case VK_LEFT:
    case VK_RIGHT:
        PostMessage (WM_HSCROLL, SB_ENDSCROLL, 0L);
        break;
    case VK_ESCAPE:
        break;
    }
}

afx_msg void CServerView::OnHScroll(UINT code, UINT pos,CScrollBar *pScrollBar)
{                  
    if (STOP) return;
        
    CImage* pImage;

    pImage = GetDocument()->GetImage();
    if (ImageIsBusy(pImage))
        return;
    ScrollImage( GetSafeHwnd(), NO, code, pos );
}

afx_msg void CServerView::OnVScroll(UINT code, UINT pos,CScrollBar *pScrollBar)
{            
    if (STOP) return;
        
    CImage* pImage;
    
    pImage = GetDocument()->GetImage();
    if (ImageIsBusy(pImage))
        return;
    ScrollImage( GetSafeHwnd(), YES, code, pos );
}

afx_msg void CServerView::OnLButtonDown( UINT keyFlags,CPoint point)
{                 
    if (STOP) return;
        
    BOOL bOnImage;
    CImage* pImage;
    int x = point.x;
    int y = point.y;

    pImage = GetDocument()->GetImage();
    if (ImageIsBusy(pImage))
        return;
    if ( m_fCapture )
        return;
	if (this != GetFocus())
		SetFocus();
    FocusObjectEnd(GetSafeHwnd());
    bOnImage = IsOnImage( GetSafeHwnd(), x, y );
    if (LastActivated)
    {
        LastActivated = FALSE;
        if (Tool.id != IDC_STITCH)
        {
            if (Window.hCursor == Window.hNullCursor)
            {
                DisplayBrush(0, 0, 0, OFF);
                if ( bOnImage )
                    DisplayBrush(GetSafeHwnd(), x, y, ON);
            }
            return;
        }
    }
    if ( !bOnImage )
        return;
    ConstrainXY(&x, &y, TRUE, TRUE, CONSTRAINXY && Tool.fConstrain);
	if (m_pOneShotTool)
	{
		point.x = x; point.y = y;
        m_pOneShotTool->OnLButtonDown( keyFlags, point );
		if (m_pOneShotTool->IsToolActive())
		{
        	// Never display the marquee when we have capture
        	EnableMarquee(NO);
        	SetCapture( );
        	m_fCapture = TRUE;
		}
	}
	else
	{
    	if ( !Tool.bActive )
        	CreateProc( GetSafeHwnd(), x, y, keyFlags );
    	if (Tool.bActive)
        	ButtonDownProc( GetSafeHwnd(), x, y, keyFlags );
    	if (Tool.bActive )
        {
        	// Never display the marquee when we have capture
        	EnableMarquee(NO);
        	SetCapture( );
        	m_fCapture = TRUE;
        }
	}
}

afx_msg void CServerView::OnLButtonUp( UINT keyFlags,CPoint point)
{
    if (STOP) return;

    CImage* pImage;
    int x = point.x;
    int y = point.y;

    pImage = GetDocument()->GetImage();
    if (ImageIsBusy(pImage))
        return;
    if ( this != GetFocus() )
        SetFocus();
    if ( !m_fCapture )
        return;
    LastActivated = FALSE;
    ConstrainXY(&x, &y, m_fCapture, NO, FALSE);
    ReleaseCapture();
    // go back to displaying marquee cause we don't have capture any more
    EnableMarquee(YES);
    m_fCapture = FALSE;
	if (m_pOneShotTool)
	{
		point.x = x; point.y = y;
        m_pOneShotTool->OnLButtonUp( keyFlags, point );
	}
	else
    if (Tool.bActive)
        ButtonUpProc( GetSafeHwnd(), x, y, keyFlags );
    ConstrainXY(&x, &y, m_fCapture, NO, FALSE);
}




static LPSTR BASED_CODE PowerMenus[16] = {"SELECTORMENU","MASKMENU","RETOUCHMENU",
       "FILTERMENU","FILLMENU","POINTEDITMENU","DRAWMENU","TEXTMENU","PROBEMENU",
       "VIEWMENU","STITCHMENU","POINTEDITFREEHAND","POINTEDITMODE","POINTEDITDRAW",
       "DRAWSHAPEMENU","FREEHANDMASKMENU"};

afx_msg void CServerView::OnRButtonDown( UINT keyFlags,CPoint pt)
{
    if (STOP) return;

    CMenu      Menu;
    CImage*    pImage;
	BOOL		fNoAltTool;
    int        x;
    int        y;
    int        MenuId;

    if( Control.Retail )
        MenuId=0;
    else
        MenuId=1;

    x = pt.x;
    y = pt.y;

    pImage = GetDocument()->GetImage();
    if (ImageIsBusy(pImage))
        return;
    FocusObjectEnd(GetSafeHwnd());
	if (m_fCapture)
        return;

    switch (Tool.id)
        {
        case IDC_SELECTOR:
            if( Control.Retail )
                MenuId = 0;
            break;
        case IDC_FREE:
            if (Tool.bActive)
                MenuId = 11;
            else
                MenuId = 15;
            break;
               
        case IDC_SQUARE:
        case IDC_MAGICWAND:
        case IDC_PAINTON:
        case IDC_TRANSFORMER:
        case IDC_CROP:
            MenuId = 1;
            break;
        case IDC_PAINT:
        case IDC_CLONE:
        case IDC_SMEAR:
        case IDC_SPRAY:
        case IDC_TEXTURE:
        case IDC_ERASER:
            MenuId = 2;
            break;
        case IDC_SHARP:
        case IDC_LIGHTEN:
        case IDC_SMOOTH:
        case IDC_DARKEN:
            MenuId = 3;
            break;
        case IDC_VIGNETTE:
        case IDC_TINTFILL:
        case IDC_TEXTUREFILL:
        case IDC_FLOOD:
            MenuId = 4;
            break;

        case IDC_POINTER:
            if (Tool.bActive)
                MenuId = 12;
            else
                MenuId = 5;
            break;
        case IDC_PENCIL:
        case IDC_DRAWFREE:
            if (Tool.bActive)
                MenuId = 13;
            else
                MenuId = 6;
            break;
        case IDC_DRAWSQUARE:
            MenuId = 14;
            break;
        case IDC_TEXT:
            MenuId = 7;
            break;
        case IDC_PROBE:
            MenuId = 8;
            break;
        case IDC_CUSTOMVIEW:
            MenuId = 9;
            break;
        case IDC_STITCH:
            MenuId = 10;
            break;
        }

    BOOL Loaded = FALSE;
	fNoAltTool = (Tool.bActive && !(Tool.id == IDC_STITCH && !Tool.bAltTool)) ||
				 !Tool.Alt.lpToolProc;
	if (!fNoAltTool && (keyFlags & MK_SHIFT))
	{
       	if (!IsOnImage(GetSafeHwnd(), x, y))
        	return;
    	if (Tool.bActive)
        	DeactivateTool();
    	InstallTool( !Tool.bAltTool );
    	if ( Tool.bPassRButton )
        	OnLButtonDown(keyFlags,pt);
	}
    else
    if( Control.Retail )    
        Loaded = Menu.LoadMenu( PowerMenus[MenuId] );

    if (Loaded)
	{
	    ClientToScreen(&pt);
	    Menu.GetSubMenu(0)->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y-2, AfxGetMainWnd());
	}
}

afx_msg void CServerView::OnRButtonUp( UINT keyFlags,CPoint point)
{
    if (STOP) return;

    CImage* pImage;

    pImage = GetDocument()->GetImage();
    if (ImageIsBusy(pImage))
        return;
    if (!Tool.bActive || !Tool.bAltTool || !Tool.bPassRButton)
        return;
    OnLButtonUp(keyFlags,point);
}

afx_msg void CServerView::OnLButtonDblClk( UINT keyFlags,CPoint point)
{
    if (STOP) return;

    CImage* pImage;
    int x = point.x;
    int y = point.y;

    pImage = GetDocument()->GetImage();
    if (ImageIsBusy(pImage))
        return;
    DoubleClickProc( GetSafeHwnd(), x, y, keyFlags );
}

afx_msg void CServerView::OnMouseMove( UINT keyFlags,CPoint point)
{
    if (STOP) return;

    POINT fpt;
    BOOL bOnImage;
    CImage* pImage;
    int x = point.x;
    int y = point.y;
	HWND hWnd;
  
	if (!fAppActive)
	{
		if (!GetDocument()->IsInPlaceActive())	
			return;
	}

	hWnd = GetSafeHwnd();

	// don't process this message if the window is not active
	if (hWnd != GetActiveDoc())
		return;

    pImage = GetDocument()->GetImage();
    if (ImageIsBusy(pImage))
        return;
    LastActivated = FALSE;
    if ( !(bOnImage = IsOnImage( hWnd, x, y )) && Tool.bActive)
    {
		if (!Control.DisableAutoScroll)
		{
        	if (Tool.fAutoScrollSetPos)
           	{
            	fpt.x = x;
            	fpt.y = y;
            	Display2File(hWnd, (LPINT)&fpt.x, (LPINT)&fpt.y);
           	}
        	if (AutoScroll( x, y ) && Tool.fAutoScrollSetPos)
           	{                              
            	File2Display(hWnd, (LPINT)&fpt.x, (LPINT)&fpt.y);
            	x = fpt.x; y = fpt.y;
            	bOnImage = IsOnImage(hWnd, fpt.x, fpt.y);      
            	ClientToScreen(&fpt);
            	SetCursorPos(fpt.x, fpt.y);
           	}
		}
    }
    if (!ConstrainXY(&x, &y, m_fCapture, NO, FALSE))
        return; /* not really a mouse move with constrain */
    Window.fLButtonDown = (keyFlags & MK_LBUTTON);
    Window.fMButtonDown = (keyFlags & MK_MBUTTON);
    Window.fRButtonDown = (keyFlags & MK_RBUTTON);
    SetRulerTicks(hWnd, x, y);
    if ((!Tool.bActive || !Tool.fDisplaysInfo) && (m_pOneShotTool == NULL) )
        DisplayInfo(x, y, NULL);

	if (m_pOneShotTool)
	{
        if ( m_pOneShotTool->IsToolActive() && (bOnImage || m_fCapture) )
		{
			point.x = x; point.y = y;
        	m_pOneShotTool->OnMouseMove( keyFlags, point );
		}
	}
	else
    if ( Tool.bActive )
    {
        if ( bOnImage || m_fCapture )
            MoveProc( hWnd, x, y, keyFlags );
    }
    else
    if (Window.hCursor == Window.hNullCursor)
    {
        DisplayBrush(0, 0, 0, OFF);
        if ( bOnImage )
            DisplayBrush(hWnd, x, y, ON);
    }
}

afx_msg void CServerView::OnMove( int x, int y)
{
    if (STOP) return;

    BOOL bIconic, bZoomed;
    RECT rWin;
    
    // the 2 lines below we necessary because I believe there
    // is a bug in the compiler.  The line '!IsIconic(GetSafeHwnd()) &&
    // !IsZoomed(GetSafeHwnd())' would not work correctly, it always
    // evaluated to FALSE.
    bIconic = GetParentFrame()->IsIconic();
    bZoomed = GetParentFrame()->IsZoomed();
    if ( bIconic || bZoomed )
        {
        CView::OnMove(x,y);        
        return;
        }
    GetWindowRect(&rWin);
    GetParent()->GetParent()->ScreenToClient(&rWin);
    OffsetRect(&m_lpDisplay->WindowRect,
        -m_lpDisplay->WindowRect.left,
        -m_lpDisplay->WindowRect.top);
    OffsetRect(&m_lpDisplay->WindowRect, rWin.left, rWin.top);
    OffsetRect(&m_lpDisplay->LastWindowRect,
        -m_lpDisplay->LastWindowRect.left,
        -m_lpDisplay->LastWindowRect.top);
    OffsetRect(&m_lpDisplay->LastWindowRect, rWin.left, rWin.top);
    
    CView::OnMove(x,y);
}

afx_msg void CServerView::OnNcLButtonDown(
UINT nHitTest,
CPoint point)
{                   
    if ( this != GetFocus() )
        SetFocus();
    CView::OnNcLButtonDown(nHitTest,point);                                
}
    
afx_msg void CServerView::OnPaint()
{
	RECT rInvalid, rUpdate;

	if (STOP)
	{
		CView::OnPaint();
		return;
	}

    // Update our rulers, scroll bars, and rates from possible new view
    UpdateRulers( GetSafeHwnd() );
    SetupScrollBars( GetSafeHwnd() );
        
    // standard paint routine
	{
		// !!! beware this code !!!
		// this code is scoped in so that the CPaintDC object
		// will get destructed before the interruptable redraw
		// code is executed
    	CPaintDC dc(this);
    	OnPrepareDC(&dc);
		AstralSetRectEmpty(&rInvalid);
		draw_it( &dc , &dc.m_ps.rcPaint,
			m_bAllowInterrupt ? (LPRECT)&rInvalid : (LPRECT)NULL);
	}

	// See if we aborted and have image left to paint
	// If so, we need to save the area still in need
	// of painting and process the message that aborted us
	if (m_bAllowInterrupt && !AstralIsRectEmpty(&rInvalid))
	{
	    // save area needing repair
	    AstralUnionRect(&rUpdate, &m_lpDisplay->UpdateRect, &rInvalid);
	
	    // set the windows update area to NULL
	    AstralSetRectEmpty(&m_lpDisplay->UpdateRect);
	
	    // set back pImage and m_lpDisplay and invalidate our area
	    // needing repair
	    if (!AstralIsRectEmpty(&rUpdate))
	        InvalidateRect( AstralToWindowsRect(&rUpdate), FALSE);
	}
	else
	{
	    // set the windows area to NULL
	    AstralSetRectEmpty(&m_lpDisplay->UpdateRect);
	}
}

afx_msg void CServerView::OnSize( UINT state, int cx, int cy)
{
    if (STOP) return;

    int x, y;
    
    ++m_iSizeCallDepth;
    // going from iconic to normal or maximized
    if ( state != SIZEICONIC &&
         m_lpDisplay->ResizeType == SIZEICONIC )
        { // Restoring an iconic image
        RevertLastView(GetSafeHwnd());
        GetWindowRect(&m_lpDisplay->WindowRect);
        GetParent()->GetParent()->ScreenToClient(&m_lpDisplay->WindowRect);
        SaveLastView(GetSafeHwnd());
        }
    else
        { // sizing either normal, maximized, or iconic
        x = -1;
        y = -1;
        if ( state == SIZEICONIC ) // Minimizing an image
            { // Force a full view below with a small FileRate
            SaveLastView(GetSafeHwnd());
            m_lpDisplay->FileRate = TINYRATE;
            }
        // Compute new DispRect and FileRect
        ComputeDispRect(GetSafeHwnd());
        ComputeFileRect(GetSafeHwnd(), x, y, m_lpDisplay->FileRate );
        }
    
    // save current save state and window size
    m_lpDisplay->ResizeType = state;
    GetWindowRect(&m_lpDisplay->WindowRect);
    GetParent()->GetParent()->ScreenToClient(&m_lpDisplay->WindowRect);

    // only setup scroll bars once, because when they are added or
    // taken away they can cause WM_SIZE events
    if (m_iSizeCallDepth == 1)
	{
        SetupScrollBars(GetSafeHwnd());
	    ZoomMainViewChanged(GetSafeHwnd());
    	// setup rulers for new window size
    	SizeRulers(GetSafeHwnd());
    	SetupRulers(GetSafeHwnd());
    	// area is always invalid for a resize
    	InvalidateRect( NULL, TRUE );
	}
    --m_iSizeCallDepth;     
    
    CView::OnSize(state,cx,cy);
}

afx_msg void CServerView::OnTimer( UINT nIDEvent )
{
    if (STOP) return;

	if ( FocusObjectEnd(GetSafeHwnd()) )
		return;

	if (Tool.bActive)
	{
		const MSG *pMsg = GetCurrentMessage();
		if (pMsg)
			TimerProc(GetSafeHwnd(), pMsg->lParam);
	}
}

afx_msg BOOL CServerView::OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT msg)
{
    if (!STOP)
	{
		if (m_pOneShotTool == NULL)
	    	CursorProc(GetSafeHwnd(), (LPTR)pWnd->GetSafeHwnd());
	}
    return (CView::OnSetCursor(pWnd,nHitTest,msg));
}

afx_msg void CServerView::OnPaletteChanged(
CWnd *pFocusWnd)
{   
	InvalidateRect(NULL, FALSE);
}


