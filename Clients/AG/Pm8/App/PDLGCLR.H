//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/PDLGCLR.H 1     3/03/99 6:09p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/PDLGCLR.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 6     7/26/98 7:09p Hforman
// moved OnQueryNewPalette() and OnPaletteChanged() to baseclass
// 
// 5     6/16/98 5:34p Dennis
// Enhancements to support any number of character frames and AG Specific
// Resource Loading.
// 
// 4     6/16/98 4:05p Dennis
// Character bitmap is now loaded into static member using
// LoadResourceBitmap()
// 
// 3     5/27/98 7:01p Hforman
// remove welcome stuff
//
//////////////////////////////////////////////////////////////////////////////

#ifndef  PDLGCLR_H
#define  PDLGCLR_H

#include "pmwdlg.h"
#include "colordef.h"


#define  WIZBUT_BACK       0x0001
#define  WIZBUT_NEXT       0x0002   // Note: NEXT, FINISH, and OK are the same button
#define  WIZBUT_FINISH     0x0004   // Only the text changes.
#define  WIZBUT_OK         0x0008   // Using OK hides BACK
#define  WIZBUT_CANCEL     0x0010
#define  WIZBUT_HELP       0x0020
#define  WIZBUT_OKONLY     0x0040   // Only the OK button shows
#define  WIZBUT_ALL        (WIZBUT_BACK | WIZBUT_NEXT | WIZBUT_CANCEL | WIZBUT_HELP)
#define  WIZBUT_OKCAN      (WIZBUT_CANCEL | WIZBUT_OK)

/////////////////////////////////////////////////////////////////////////////
// CPmwDialogColor dialog

class CPmwDialogColor : public CPmwDialog
{
   INHERIT (CPmwDialogColor, CPmwDialog)
// Construction
public:
   CPmwDialogColor(UINT idd = 0, 
                  CWnd* pParent = NULL,
                  COLORREF cr = INVALID_DIALOG_COLOR, 
                  BOOL UseLittleGuy = FALSE);


   void
   SetMatteColor (COLORREF cr)
   {
      MatteColor = cr;

      if (IsWindow (m_hWnd) != FALSE)
         if (IsWindowVisible () != FALSE)  
            Invalidate ();
   }

   COLORREF
   GetMatteColor (void)
   {
      return MatteColor;
   }

// Dialog Data
   //{{AFX_DATA(CPmwDialogColor)
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CPmwDialogColor)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   virtual  BOOL OnInitDialog (void);
   virtual  int
   DoModalNCMoveable (void)
   {
      int   i;
      m_NonClientMoveable = TRUE;
      i = INHERITED::DoModal ();      
      m_NonClientMoveable = FALSE;
      return i;
   }
   
protected:

   // Generated message map functions
   //{{AFX_MSG(CPmwDialogColor)
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   
   COLORREF                                  MatteColor;
   CBrush                                    Brush;
   CRect                                     m_ControlBounds, m_RectClient;
   CButton                                   m_Back, m_Next, m_Cancel;
   static const char                         *TextFinish, *TextNext, *TextOK;
   
   BOOL              m_UseLittleGuy, m_NonClientMoveable;
   static int        GuyFrame;        
   int               m_GuyFrame;
   static CBitmap    sm_bmGuyFrames;
   static int        sm_nNumFrames;
   
   void
   CreateWizzardButtons (void);

   void
   EnableWizardButtons (DWORD Flags);

   void
   GetControlBounds (CRect *Rect, BOOL Equalize = TRUE);

   void
   DrawColorBox (CDC* pDC);

   void
   DrawLittleGuy (CDC* pDC);
   
   void
   ForceColorBoxRecalc (void)
   {
      m_ControlBounds.SetRectEmpty ();
   }
};
/////////////////////////////////////////////////////////////////////////////
#endif
