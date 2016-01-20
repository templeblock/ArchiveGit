// WndHub.h : header file
//
#ifndef  WNDHUB_H
#define  WNDHUB_H
/////////////////////////////////////////////////////////////////////////////

#include "pmbmbut.h"
#include "ddb.h"
#include "hubmgr.h"

#define	WM_HUB_QUERY_ACTION	WM_USER+2049

class CHubQueryInfo 
{
public:
	CHubQueryInfo(WPARAM wp)
	{
		mInput = wp;
		mAuthentic = 1234.0;
		mRetVal = -1;
	}

	BOOL
	IsAuthentic(void)const
	{
		return mAuthentic == 1234.0 ? TRUE : FALSE; 
	}

	int
	RetVal(void)const
	{
		return mRetVal;
	}
	
	void
	RetValSet(int rv)
	{
		mRetVal = rv;
	}

	WPARAM
	Input(void)const
	{
		return mInput; 
	}
private:
	double	mAuthentic;
	int		mRetVal;
	WPARAM	mInput;
};

class BitmapButtonWithPosition : public CPmwBitmapButton
{
public:   
   BitmapButtonWithPosition::BitmapButtonWithPosition(ButtonSet *p);

   POINT Origin;
};

/////////////////////////////////////////////////////////////////////////////
// CWndHub window

class CWndHub : public CWnd
{
	DECLARE_DYNAMIC(CWndHub)

// Construction
public:
   CWndHub(LPCSTR DLLName);

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CWndHub)
   public:
   virtual BOOL Create(CWnd *pParent);
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CWndHub();
/*
   void 
   DoActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   void
   DoPaletteChanged(CWnd* pFocusWnd);
*/

   BOOL DoQueryNewPalette();

   int 
   StartModalLoop();

	void 
	EndModalLoop(int result);
   // Return the smallest size I want to be
   // Currently, the smallest bitmap size, as I have no boarder, etc.
   void
   GetMinSize(CPoint *p)const
   {
      m_HubResourceManager.GetMinSize(p);
   }

   BOOL
   SetSkipButton(int id)
   {
      return m_HubResourceManager.SetSkipButton(id);
   }
   
	BOOL
	SubstituteBitmap (UINT BitmapIDOld, UINT BitmapIDNew);

   // Generated message map functions
protected:
   //{{AFX_MSG(CWndHub)
   afx_msg void OnPaint();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnDestroy();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	//}}AFX_MSG
   afx_msg void OnParentNotify(UINT message, LPARAM lParam);
   DECLARE_MESSAGE_MAP()
	// This is static so the hubtable is loaded only once
	// and SubstituteBitmap() only reacts if the bitmap(s) actually change
   static CHubResourceManager  m_HubResourceManager;

   afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);

   virtual BOOL
   OnCommand(WPARAM wParam, LPARAM lParam);

   BOOL
   IsSkippedButton(int id)const 
   {
      return m_HubResourceManager.IsSkippedButton(id);
   }
   
   int
   GetSmallestWindowSizeAndIndex(CPoint *p)const
   {
      return m_HubResourceManager.GetSmallestWindowSizeAndIndex(p);
   }

private:
   enum {IDD = ID_HUB_WINDOW};

	static BOOL			m_fDailyTipShown;

   int               m_CurBitmapIndex;
   CBitmap           *m_pBitmap;
   CObArray          BitmapButtons;
   HWND              m_pLastFocusButton;
   BOOL					m_fInModalLoop;

   HINSTANCE
   GetResourceHandle(void)
   {
      return m_HubResourceManager.GetDLLHandle();
   }

   void
   KillBitmap(void)
   {
      if (m_pBitmap != NULL)
      {
         delete m_pBitmap;
         m_pBitmap = NULL;
      }
      
      m_CurBitmapIndex = -1;
   }

   void
   GetBitmapOffset(int *x, int *y)const
   {
      BITMAP   bm;
      CRect    RectClient;

      *x = *y = 0;
      
      if (m_pBitmap != NULL)
      {
         GetClientRect(&RectClient);
         m_pBitmap->GetBitmap(&bm);
         *x = RectClient.Width() - bm.bmWidth;
         *y = RectClient.Height() - bm.bmHeight;
         *x = *x >> 1;
         *y = *y >> 1;
      }
   }

   BOOL
   GetBitmap(void);

   int
   GetBitmapIndexBySize(void)const;
   
   BOOL
   GotButton(void)const
   {
      for (int i = 0; i < BitmapButtons.GetSize(); i ++)
         if (BitmapButtons[i] != NULL)
            return TRUE;
      
      return FALSE;
   }

   BOOL
   MoveButton(BitmapButtonWithPosition  *pButton);

   void
   MoveButtons(void)
   {
      for (int i = 0; i < BitmapButtons.GetSize(); i ++)
         MoveButton((BitmapButtonWithPosition*)BitmapButtons[i]);
   }

   void
   DoButtons(void);

   void
   KillButton(BitmapButtonWithPosition  *pButton)
   {
      if (pButton != NULL)
         delete pButton;
   }

   void
   KillButtons(void)
   {
      for (int i = 0; i < BitmapButtons.GetSize(); i ++)
      {
         KillButton((BitmapButtonWithPosition*)BitmapButtons [i]);
      }
      
      BitmapButtons.RemoveAll();
   }
};

/////////////////////////////////////////////////////////////////////////////
#endif
