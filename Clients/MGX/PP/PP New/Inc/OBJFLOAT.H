//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


#ifndef OBJFLOAT_H_
#define OBJFLOAT_H_

class CObjFloat : public CDialog
   {
   public:
      CObjFloat();
     ~CObjFloat();
	   virtual BOOL OnInitDialog();
	   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
      virtual BOOL PreTranslateMessage( MSG* pMsg );
      BOOL Create();

   protected:
      void CalcSysMenuRect(BOOL bClient, CRect &rMenu);
      void CalcTitleRect(BOOL bClient, CRect &rTitle);

	   DECLARE_DYNAMIC(CObjFloat)
	   //{{AFX_MSG(CObjFloat)
      afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	   afx_msg UINT OnNcHitTest(CPoint point);
	   afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	   afx_msg void OnClose();
	   afx_msg BOOL OnNcActivate(BOOL f);
	   afx_msg void OnNcPaint();
      afx_msg void OnShowWindow(BOOL bShow, UINT Status);
      afx_msg void OnPaint();
	   afx_msg void OnSetFocus(CWnd* pOldWnd);
	   //}}AFX_MSG
	   DECLARE_MESSAGE_MAP()
   }; 


extern CObjFloat   ObjFloat;
#endif

