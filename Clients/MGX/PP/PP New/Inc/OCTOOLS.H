#ifndef _OCTOOLS_H
#define _OCTOOLS_H

#include <afxtempl.h>

#undef  TBBS_BUTTON
#define TBBS_BUTTON    0x04		// Button with bitmap
#define TBBS_TEXTBTN   0x08     // Button with text in it
#define TBBS_NEWLINE   0x10     // start a new line
#define TBBS_CONTROL   0x20		// control window
#define TBBS_DIALOG    0x40		// a dialog, ribbon
#define TBBS_WINDOW	   (TBBS_CONTROL | TBBS_DIALOG)
#define SIZINGFUDGE		2
#define WINDOWSEPARATOR 4

#define BETWEEN(x,y) ((x) > ((y)-SIZINGFUDGE) && (x) < ((y)+SIZINGFUDGE))

#define DEFAULT_GAP_WIN95	8
#define DEFAULT_GAP_WINNT	6
////////////////////////////////////////////
// COCBitmap

class COCBitmap : public CBitmap
{
public:
	COCBitmap(CSize Size);
	~COCBitmap();
	void SafeAttach(HBITMAP hbm);
 	HBITMAP LoadBitmap(LPCTSTR lpszResourceName);
	HBITMAP LoadBitmap(UINT nIDResource);
	HBITMAP LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc);
	void StartDraw();
  	BOOL DrawOCButton(CDC* pDC, CPoint PtDst, CPoint PtSrc, CSize BSize);
	void EndDraw();

	// member variables
	CSize	m_ImageSize;	  // size of image in each button
	UINT	m_ID;
	HBITMAP	m_hbmOld;
};

////////////////////////////////////////////
// OCBUTTON

struct OCBUTTON
{
	UINT 	nID;        // Command ID that this button sends
	int		x;			// x index into Master Bitmap of icons
	int		y;			// y index into Master Bitmap of Icons
	UINT	IDMasterBM;	// ID of Master Bitmap of Icons
};

typedef struct _ocgroup_
{
	int		Start;
	int		Width;
	int     ActualWidth;
	int		End;
	struct _ocgroup_*	pNext;
}	OCGROUP;

////////////////////////////////////////////
// COCButton

class  COCButton : public CObject
{
public:
	COCButton();
	DECLARE_SERIAL(COCButton)
	virtual ~COCButton();
	COCButton(UINT id, int xx, int yy, UINT BM);
	COCButton(UINT id, CString Str);
	COCButton(UINT id, CString UName, CString FName);	// For macro buttons
	COCButton(HWND	w);
	void CreateMask(CPoint offset, BOOL bHiliteShadow, int Dem);
	void DrawText(HDC hDC, CString &Str, CRect &r, COLORREF color);
	BOOL DrawButton(CDC* pDC, int Dem);
	const COCButton& operator=(const COCButton& Src);
	const COCButton& operator=(const OCBUTTON &Src);
	virtual BOOL Create(CWnd *pParent){return(TRUE);};
	virtual void Move(CPoint Pt, BOOL Horz);
	virtual COCButton* Duplicate();
	virtual BOOL Notify(int nCode){return(FALSE);};
	virtual void Serialize(CArchive &ar);
	virtual void  CalcSize(BOOL Large);

	UINT 	nID;        // Command ID that this button sends
	UINT 	nStyle;    	// TBBS_ styles
	int		x;			// x index into Master Bitmap of icons
	int		y;			// y index into Master Bitmap of Icons
	SIZE	Size;		// size of button;
	CPoint	Origin;		// left,top location of Button

	UINT	IDMasterBM;	// ID of Master Bitmap of Icons, or	Style for creating the window
	int 	iImage;     // index into mono bitmap of this button's picture
	CString	Text;		// text for text buttons, or caption for window buttons
	CString ClassName;	// name of class for window buttons
	HWND	hWnd;
};

////////////////////////////////////////////
// COCRectTracker

enum SizingAction
	{
	SizeNone,
	SizeExpand,
	SizeShrink,
	} ;

class COCToolBar;
class COCRectTracker : public CRectTracker
{
public:
	COCRectTracker();
	virtual void AdjustRect(int nHandle, LPRECT lpRect);
	BOOL StartTracking(COCToolBar *pBar, UINT flags);
	SizingAction AdjustRight(LPRECT lpRect, int Rows, int Max);
	SizingAction AdjustLeft(LPRECT lpRect, int Rows, int Max);
	SizingAction AdjustTop(LPRECT lpRect, int Rows, int Max);
	SizingAction AdjustBottom(LPRECT lpRect, int Rows, int Max);

	COCToolBar*		m_pBar;
	CRect			m_InitRect;
	CRect			m_PrevRect;
	CRect			m_CurRect;
	CRect			m_BarRect;
	SizingAction	m_PrevAction;
	int				m_nNewRows;
};


////////////////////////////////////////////
// COCToolBar

#define		OCTOOLS_ISUP		0x01
#define		OCTOOLS_ISFLOATING	0x02
#define 	OCTOOLS_LARGEICONS	0x04
#define     OCTOOLS_DOCKTOP	 	0x08
#define     OCTOOLS_DOCKBOTTOM	0x10
#define     OCTOOLS_DOCKLEFT	0x20
#define     OCTOOLS_DOCKRIGHT	0x40
#define		OCTOOLS_DOCKHORZ	(OCTOOLS_DOCKTOP|OCTOOLS_DOCKBOTTOM)
#define		OCTOOLS_DOCKVERT	(OCTOOLS_DOCKLEFT|OCTOOLS_DOCKRIGHT)
#define 	OCTOOLS_DOCKALL		(OCTOOLS_DOCKTOP|OCTOOLS_DOCKBOTTOM|OCTOOLS_DOCKLEFT|OCTOOLS_DOCKRIGHT)

class COCToolBar : public CControlBar
{
	DECLARE_DYNAMIC(COCToolBar)

// Construction
public:
	COCToolBar();
	virtual ~COCToolBar();
	void CloseToolBar();
	virtual BOOL ShowToolBar(CFrameWnd *pFrame, CWnd *pOwner = NULL);
	virtual BOOL Create(CWnd* pParentWnd,
			DWORD dwStyle = WS_CHILD | CBRS_TOP,
			UINT nID = AFX_IDW_TOOLBAR);

	CSize GetMaxButtonSize();
	BOOL IsToolBarUp();
	void SetPos(CPoint Pt){m_BarRect = CRect(Pt, m_BarRect.Size());}
	void SetName(CString Str);
	void SetLargeIcon(BOOL f);
	BOOL GetLargeIcons(){return m_Style2 & OCTOOLS_LARGEICONS;}
	CString	GetName(){return(m_Name);}
	void SetHeight(int cyHeight);
				// call after SetSizes, height overrides bitmap size
	virtual void Serialize(CArchive &rc);
	virtual BOOL InsertButton(COCButton *pButton, int loc = -1, BOOL ReBuild = TRUE);
	virtual BOOL DeleteButton(int loc, BOOL ReBuild = TRUE);
	virtual BOOL SetButtons(const OCBUTTON* lpIDArray, int nIDCount);
	virtual void InsertSeparator(int loc);
	virtual BOOL DeleteSeparator(int loc);
	virtual CSize GetDockedSize(BOOL bHorz, BOOL Update = TRUE);
	virtual BOOL Load(){return(FALSE);};
	BOOL AllocElements(int nElements);
	BOOL BuildBitmap();
	void MoveButton(int From, int To);
	void ExecuteDrag(int SrcLoc);
	void RecalcLayout();
	void StartResizing();
	void EndResizing();
	CSize GetNonDockedSize(BOOL Update);
	BOOL Resize(int nRows = -1);
	int FitButtons(int AvailW, int MaxRows, OCGROUP*	pGroup);
	int FitGroup(OCGROUP*	pGroup, int AvailW, int &ActualW);
	int GetSplitingWidth(int Width, int MaxRows);
	int GetNonSplitingWidth(int	Width, int MaxRows);
	int GetNewWidth(int	Width, int MaxRows);
	int GetWidthOfGroup(int &index, int &ActualW);
	int GetInsertLoc(CPoint &point);
	int GetTotalWidth();
	int GetNumOfRows(int OldRows, SizingAction Action);
	void ClearLineBreaks();
	int GetWhatLine(int y);
	int GetNumOfButtons(){return(m_nButtons);};
	void SetCustomizing(BOOL f);
	void DoWinButtonResizing(CPoint Pt);
	void SetControlRects();
	void GetBarRect();
			// lpIDArray can be NULL to allocate empty buttons

// Attributes
public: // standard control bar things
	int CommandToIndex(UINT nIDFind);
	UINT GetItemID(int nIndex);
	virtual void GetItemRect(int nIndex, LPRECT lpRect);
	int	 GetNumRows(){return m_nRows;};
	void SetNumRows(int r){m_nRows = r;};
	UINT GetStyle2(){return(m_Style2);}

// Implementation
public:
	inline UINT GetButtonStyle(int nIndex);
	void SetButtonStyle(int nIndex, UINT nStyle);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	COCButton* GetButtonPtr(int nIndex);
	COCButton** GetButtonPtrPtr(int nIndex);
	void SetUpState(BOOL f){m_UpState = f;}
	BOOL GetUpState(){return(m_UpState);}

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void InvalidateButton(int nIndex);
	void UpdateButton(int nIndex);

protected:
	UINT  		m_Style2;
	CRect		m_BarRect;
	HBITMAP 	m_hbmImageWell;     // glyphs only
	int 		m_iButtonCapture;       // index of button with capture (-1 => none)

	int	  		m_nRows;				// number of rows
	int			m_TotalWidth;
	int			m_nGroups;
	OCGROUP*	m_pGroup;
	BOOL		m_Spliting;
	int			m_nButtons;				// num of buttons without seperators
	CString		m_Name;
	BOOL		m_UpState;				// used to the ToolBar Dialog.
	// layout related data
	int				m_WinButton;		// currently selected window button. used with m_WinRect
										// for resizing
	COCRectTracker	m_tracker;
	CRectTracker	m_WinRect;
	CRect			m_RightSize; // of m_WinRect
	BOOL  m_Horz;				// orientation, for detecting when orientation has changed
	CRect m_FrameRect;			// rect of frame before resizing

	virtual void DoPaint(CDC* pDC);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual int HitTest(CPoint point);
	virtual UINT OnCmdHitTest(CPoint point, CPoint* pCenter);

	UINT GetDockingSide();
	//{{AFX_MSG(COCToolBar)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnSysColorChange();
	afx_msg LRESULT OnResizeChild(WPARAM, LPARAM lParam) ;
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

typedef CTypedPtrList<CObList, COCToolBar*> CToolList;

////////////////////////////////////////////
// COCToolList control
#define CX_BORDER	1
#define CY_BORDER	1

class COCToolList : public CCmdTarget
{
public:
	// system metrics
	int cxBorder2, cyBorder2;
	CFrameWnd*		m_pFrame;
	CToolList		m_List;
	CString			m_FileName;
	// color values of system colors used for CToolBar
	COLORREF clrBtnFace, clrBtnShadow, clrBtnHilite, clrWindowFrame;
	COLORREF clrBtnText, clrBtnGrayText;

	// other system information
	BOOL    bWin4;          // TRUE if Windows 4.0

	// Mono DC and Bitmap for disabled image
	HDC 	hDCMono;
	HDC 	hDCGlyphs;
	HBRUSH 	hbrDither;
	CFont	hfontSys;

	// customize related data
	BOOL		m_Customize;
	BOOL		m_ColorIcons;
	BOOL		m_ToolTips;
	// drag related data
	BOOL		m_Draging;			// We are in drag mode
	COCToolBar *m_pDragOverWnd;		// OCToolBar Draged Over
	CWnd	   *m_pDragOwner;		// what window initiated drag
	CPoint		m_DragPoint;		// current point for draging
	CSize		m_DragSize;			// Drag Size of Cursor
	CSize		m_DragOffset;		// offset of click within Button, always negative
	CPoint		m_SrcPoint;			// Drag start point in client coord
	CPoint		m_DropPoint;		// Drop point in client coord of COCToolBar, or in screen coord 
									// if not on top of COCToolBar
 	int m_SharedBorder;         	// shared border between buttons

	// Implementation
	virtual ~COCToolList();
	COCToolList();
	COCToolBar* AddToolBar();
	void FillListBox(CListBox*	pList, int Sel);
	BOOL Delete(COCToolBar* pBar);
	BOOL Delete(POSITION Pos);
	POSITION FindToolBox(CString &Name);
	CSize GetButtonSize(BOOL Large = FALSE);
	virtual void Serialize(CArchive &rc);
	virtual BOOL Save(){return(FALSE);};
	virtual BOOL Load(CFrameWnd *pFrame, CWnd* pOwner = NULL){return(FALSE);};
	virtual COCToolBar* NewToolBar(){return new COCToolBar;};
	BOOL GetColorIcons(){return(m_ColorIcons);}
	void SetColorIcons(BOOL f);
	BOOL GetToolTips(){return(m_ToolTips);}
	void SetToolTips(BOOL f);
	void UnLoad();
	BOOL CreateToolBar(COCButton *pButton, CPoint Pt, CString &Str);
	void UpdateSysColors();
	BOOL EndDrag(CPoint Pt, BOOL Cancel = FALSE);
	void StartDrag(CWnd *pOwner, CPoint Pt, CRect Bt);
	void MouseMove(CPoint point);
	BOOL IsDraging();
	void SetCustomizing(BOOL f);
	int  GetDefaultGap();
};

extern COCToolList* OCToolList;
extern HBITMAP	BuildBitmap(COCButton **pData, int Count, CSize Size);

#endif
/////////////////////////////////////////////////////////////////////////////
