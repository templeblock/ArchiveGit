//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


#ifndef CUSTOOLS_H_
#define CUSTOOLS_H_

/**************************************************************************/
/*                            CCustTool                                   */
/**************************************************************************/

typedef struct _custCmds 
   {
   UINT  Cmd;
   UINT  Kind; // 0(menu), 1(tool)
   UINT  LongName;
	} CUSTCMD;

typedef enum
   {
      CUSTOOL_CMD = 0,
      CUSTOOL_MACRO,
      CUSTOOL_ICON
   } CUSTOOL_TYPE;

class CCustTool : public CObject
   {
   public:
      CCustTool();
      CCustTool(const CCustTool &src);
      CCustTool(CUSTOOL_TYPE Type,UINT Cmd, CString &SName);
      CCustTool(CString &SName, CString &MName);
     ~CCustTool();
      CString GetShortName(){return(m_ShortName);}
      CString GetMacroName(){return(m_MacroName);}
      void    SetShortName(CString &s){m_ShortName = s;}
      void    SetHWnd(){m_Wnd = NULL;}
      HWND    GetHWnd(){return m_Wnd;}
      UINT   RePosition(UINT Order, UINT NumNonIconTools, UINT Width);
      BOOL   Create (CWnd *pParent, UINT Width);
      CUSTOOL_TYPE GetType(){return m_Type;}
      BOOL Save(UINT Order, LPTSTR SectionName);
      BOOL Load(UINT Order, LPTSTR SectionName);
      BOOL PlayMacro();
      static INT  ButtonHeight;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   protected:   
	   DECLARE_DYNAMIC(CCustTool)
      CString        m_MacroName;
      CString        m_ShortName;
      UINT           m_Cmd;
      CUSTOOL_TYPE   m_Type;
      HWND           m_Wnd;
   };

/**************************************************************************/
/*                            CCustToolBox                                */
/**************************************************************************/

class CCustToolBox : public CDialog
   {
   public:
      CCustToolBox();
      CCustToolBox(CString &);
      CCustToolBox(const CCustToolBox & src);
     ~CCustToolBox();
      CString GetName(){return m_Name;}
      BOOL    GetUp(){return m_WasUp;}
      UINT    GetNumNonIconTools(){return m_NumNonIconTools;}
      UINT    GetWidth(){return m_Width;}
      void    SetWidth(UINT w){m_Width = w;}
      CObList *GetList(){return &m_List;}
      void     Delete(UINT, BOOL);
      void     DeleteAll();
      void     Add(CCustTool *pct, POSITION Pos = NULL);
      void     UpdateActiveTool(int id);
      void     ReName(CString &Name);
      void     ReLoad();
      BOOL     Load(BOOL LoadPos = TRUE);
      BOOL     LoadMain();
      BOOL     IsPatchUp(){return(m_Patch);}
      void     SetPatchUp(BOOL b){m_Patch = b;}
      HWND     GetPatchWnd(){return(m_ColorPatch);}
      BOOL     Save();
	   virtual BOOL OnInitDialog();
	   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
      virtual BOOL PreTranslateMessage( MSG* pMsg );
      BOOL Create(CWnd *pParent = NULL);
      BOOL ReSize();
      void FindTool(HWND hDlg, int idCmd, HWND hControl, UINT codeNotify);
      void OnActiveColor(HWND hDlg, int idCmd, HWND hControl, UINT codeNotify);
	  void OnUpdateCmdUI(CFrameWnd *pTarget, BOOL bDisableIfNoHndler);
#ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
#endif

      CObList    m_List;
      BOOL       m_IsMain;
      BOOL       m_IsInsitue;
   protected:
      void CalcSysMenuRect(BOOL bClient, CRect &rMenu);
      void CalcTitleRect(BOOL bClient, CRect &rTitle);
      void InvertSysMenu();

	   DECLARE_DYNAMIC(CCustToolBox)
	   //{{AFX_MSG(CCustToolBox)
	   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	   afx_msg void OnDestroy();
	   afx_msg UINT OnNcHitTest(CPoint point);
	   afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	   afx_msg void OnClose();
	   afx_msg BOOL OnNcActivate(BOOL f);
	   afx_msg void OnNcPaint();
      afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
      afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
      afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	   afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	   afx_msg BOOL OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT msg);
		afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	   //}}AFX_MSG
	   DECLARE_MESSAGE_MAP()
      CString     m_Name;
      BOOL        m_WasUp;
      UINT        m_NumNonIconTools;
      CPoint      m_Pos;
      int         m_Width;
      BOOL        m_Patch;
      HWND        m_ColorPatch;
   }; 


/**************************************************************************/
/*                            CCustToolList                               */
/**************************************************************************/

class CCustToolList : public CCmdTarget
   {
   public:
      CCustToolList();
     ~CCustToolList();
      void SetDlgCtrl(HWND hDlg, HWND hControl, UINT codeNotify);

	   POSITION GetHeadPosition()
         {return m_List.GetHeadPosition();};
	   CObject* GetNext(POSITION& Pos)
         {return m_List.GetNext(Pos);};
	   CObject* GetTail()
         {return m_List.GetTail();};
	   CObject* GetHead()
         {return m_List.GetHead();};
	   void RemoveAt(POSITION Pos)
         {m_List.RemoveAt(Pos);};
	   CObject* GetAt(POSITION Pos)
         {return m_List.GetAt(Pos);};
	   POSITION FindIndex(int nIndex)
         {return m_List.FindIndex(nIndex);};

      CCustToolBox *CreateInsitueBox(CWnd *pParent);

      void ShowMain();
      BOOL IsMainUp();
      POSITION FindToolBox(CString &Name);
      CCustToolBox *FindMain();
      POSITION Add(CCustToolBox *pBox);
      BOOL Remove(CCustToolBox *pBox);
      BOOL Save();
      BOOL Load(CWnd *pParent = NULL, BOOL MainOnly = FALSE);
      BOOL Add(CString &Name, BOOL Open = FALSE, CWnd *pParent = NULL);
      BOOL Delete(POSITION Pos);
      BOOL Delete(UINT i);
      BOOL ReName(UINT i, CString &Name);
      void NotifyColorPatches(int what);
      void InvalidateColorPatches(BOOL Erase);
      void NotifyOfActiveTool(int cat, int id);
      void UpdateStatusButton(int id, BOOL b);
      CCustToolBox  *m_pMain;

    DECLARE_MESSAGE_MAP ();
      afx_msg void OnUpdateRMCheck(CCmdUI* pCmdUI);
      afx_msg void OnUpdateRMEnable(CCmdUI* pCmdUI);
      afx_msg void OnUpdateRMEnableAndCheck(CCmdUI* pCmdUI);
      afx_msg void OnFreeHand();
      afx_msg void OnAutoMask();
      afx_msg void OnDrawLine();
      afx_msg void OnDrawBezier();
      afx_msg void OnIsBezier();
      afx_msg void OnIsPoint();
      afx_msg void OnMovePoints();
      afx_msg void OnAddPoints();
      afx_msg void OnDelPoint();
      afx_msg void OnPointMode();
	  afx_msg void OnCustomLoad();
	  afx_msg void OnUpdateCustomLoad(CCmdUI* pCmdUI);
	  afx_msg void OnCustomCreate();
	  afx_msg void OnUpdateCustomCreate(CCmdUI* pCmdUI);
      afx_msg void OnMaskClick();
      afx_msg void OnMaskDblClk();
      afx_msg void OnRetouchClick();
      afx_msg void OnRetouchDblClk();
      afx_msg void OnFilterClick();
      afx_msg void OnFilterDblClk();
      afx_msg void OnFillClick();
      afx_msg void OnFillDblClk();
      afx_msg void OnDrawClick();
      afx_msg void OnDrawDblClk();

      afx_msg void OnSelViewTextProbe(int ID);
      afx_msg void OnSelector();
      afx_msg void OnCustomView();
      afx_msg void OnText();
      afx_msg void OnProbe();
      afx_msg void OnLastTool();

      afx_msg void OnMaskTool(int ID);
      afx_msg void OnMaskFree();
      afx_msg void OnMaskSquare();
      afx_msg void OnMaskCircle();
      afx_msg void OnMaskPainton();
      afx_msg void OnMaskMagic();
      afx_msg void OnMaskShield();
      afx_msg void OnMaskTransformer();
      afx_msg void OnMaskPointer();
      afx_msg void OnMaskCrop();

      afx_msg void OnRetouchTool(int ID);
      afx_msg void OnRetouchPaint();
      afx_msg void OnRetouchSpray();
      afx_msg void OnRetouchClone();
      afx_msg void OnRetouchTexture();
      afx_msg void OnRetouchSmear();
      afx_msg void OnRetouchEraser();

      afx_msg void OnFilterTool(int ID);
      afx_msg void OnFilterSharp();
      afx_msg void OnFilterSmooth();
      afx_msg void OnFilterLighten();
      afx_msg void OnFilterDarken();

      afx_msg void OnFillTool(int ID);
      afx_msg void OnFillVignette();
      afx_msg void OnFillTexture();
      afx_msg void OnFillTintFill();
      afx_msg void OnFillFlood();

      afx_msg void OnDrawTool(int ID);
      afx_msg void OnDrawPencil();
      afx_msg void OnDrawSquare();
      afx_msg void OnDrawFree();

   protected:
      CObList     m_List;
      HWND        m_hDlg;
      HWND        m_hControl;
      UINT        m_codeNotify;
      DECLARE_DYNAMIC(CCustToolList)
   };

#endif


