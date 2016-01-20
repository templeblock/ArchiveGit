////////////////////////////////////////////////////////////////
// Class that paints the caption area of a window
//	Code from example in MSJ
//
//	Modified by LWu	Feb, 1999



#ifndef _PAINTCAP_H
#define _PAINTCAP_H

#include "imgctrl.h"
#include "dcdrawingsurface.h"
#include "bitmapimage.h"

#define	CAC_THICKFRAME		0x00000200L	// thick, non-sizing frame

#define	CAC_HTCLOSE			0x1000		// close button

class CPImageControl;
typedef CTypedPtrList<CObList, CWnd*> WndCtrlList;

////////////////
// Class used to get the caption rectangle of a window in window coords.
// This is the area of the title bar inside the window frame, including
// the icon and min/max/close buttons.
// 
class CCaptionRect : public CRect 
{
public:	// methods
	CCaptionRect();
	CCaptionRect(const CWnd& wnd, const int nBdrInc, const int nTitleInc); // use reference to deny NULL ptr
	
	CalcCaptionRect(const CWnd& wnd, const int nBdrInc, const int nTitleInc);

public:	// members
	CSize m_czFrameInset;
};


////////////////
//	CCaptionPainter

class CCaptionPainter : public CWnd 
{
	
public:

	enum ECapFillType
	{
		kDefFillCapType		= 0,
		kClrFillCapType		,
		kGradFillCapType		,
		kPictFillCapType		
	} ;

	enum ECapCustFlags
	{
		kUseDefSetting		= 0x0000,
		kUseCustClrs		= 0x0001,
		kUseCustFont		= 0x0002,
		kUseCustBtns		= 0x0004
	};

	// construction & destruction
	CCaptionPainter();
	virtual ~CCaptionPainter();

	DECLARE_DYNAMIC(CCaptionPainter);

	BOOL Install(CDialog* pDlg, int fontSize, CString fontName, int nBdrInc = 0, int nTitleInc = 0);
	void SetTitlebarImgCtrl(CPImageControl*	pImgCtrl);
	void SetTBarCtrlImgCtrl(CPImageControl*	pImgCtrl, int nType, CRect& rcBtn, int nCtrlID);

	BOOL RelayEvent(MSG* pMsg);
	
	void Redraw();
	void Invalidate() { m_szCaption=CSize(0,0); }
	
	void SetCustFont(int fontSize, CString fontName);
	void CreateDefFont();

	void SetCustSolidColor(COLORREF clrSolid);
	void SetCustGradColors(COLORREF clrStart, COLORREF clrEnd);

	// Messages from parent dialog
	BOOL OnNcPaint(HRGN hUpdateRgn);
	BOOL OnNcActivate(BOOL bActive);
	BOOL OnSetText(LPCTSTR lpText);
	BOOL OnNcLBtnDown(UINT nHitTest, CPoint point);
	BOOL OnNcLBtnDblClk(UINT nHitTest, CPoint point);
	UINT OnNcHitTest(CPoint point);
	BOOL OnMouseMoveMsg(POINT pt);
	BOOL OnLButtonUp(UINT  nFlags, CPoint point) ;


	// Implementation
private:
	void PaintCaption(CCaptionRect& rc);
	
	void PaintGradiantCaption(CRect& capRect);
	void PaintSolidCaption(CRect& capRect, COLORREF theColor);
	void PaintPictureCaption( CRect& capRect );
	
	int	DrawIcon(CDC* dc);
	int	DrawButtons(CDC* dc, CSize capSize);
	int	DrawCustomButtons(CDC* dc, CRect& capRect);
	
	void	DrawCustomNcBtn( UINT btnType, int state);
	void	DrawStandardNcBtn( UINT btnType, int state );
	
	void DrawWindowText(CDC* dc, CRect& theTextRect);

	// members
private:

	enum CtrlState 
	{
		UP = 0,
		DOWN
	};

	CDialog*	m_pParentDlg;

	UINT		m_uiCustFlags;		// what type of customization
	INT		m_nCapFillType;	// what is the caption filled with?
	COLORREF	m_clrCustom;		// the custom color
	
	CCaptionRect m_crcCaption;	// the caption rectangle
	CSize		m_szCaption;		// size of caption rectangle
	
	// title bar button rectangles
	CRect		m_rcIcon;
	CRect		m_rcMinBtn;
	CRect		m_rcMaxBtn;
	CRect		m_rcCloseBtn;

	// here are all the title bar buttons images
	CPImageControl*	m_pTitleBkImgCtrl;
	CPImageControl*	m_pCloseBxImgCtrl;
	CPImageControl*	m_pMinBxImgCtrl;
	CPImageControl*	m_pMaxBxImgCtrl;
	CPImageControl*	m_pRestoreBxImgCtrl;
	CPImageControl*	m_pHelpBxImgCtrl;

	BOOL		m_bActive;			// active/inactive state

	CFont		m_fontDefCaption;			// normal system font for active caption
	CFont		m_fontCustCaption;		// customize font
	
	int		m_nBdrInc;
	int		m_nTitleInc;

	UINT		m_nCurNcBtn;
	BOOL		m_fNcMouseTracking;
	BOOL		m_fInButton;
};

#endif
