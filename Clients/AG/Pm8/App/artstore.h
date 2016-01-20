/*
// $Workfile: artstore.h $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/artstore.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 18    11/18/98 10:30p Psasse
// New download button support
// 
// 17    10/30/98 5:25p Jayn
// 
// 16    10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
// 15    10/23/98 5:45p Johno
// Moved code in OnArtStorePreview() to mainfrm.cpp
// 
// 14    10/21/98 4:14p Jayn
// More improvements to the order process.
// 
// 13    10/19/98 5:41p Jayn
// Beginnings of the online order form.
// 
// 12    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 11    7/26/98 7:05p Hforman
// changed baseclass from CDialog to CPmwDialog for palette handling
// 
// 10    7/09/98 10:45a Jayn
// New GetOrderForms & FindORDFiles
// 
// 9     7/08/98 1:04p Jayn
// Simplified filtering. Added Array variant to FillxxxFromList.
// 
// 8     7/01/98 5:21p Psasse
// better error checking and more robust filtering
// 
// 7     6/29/98 9:45a Psasse
// New dynamic addition/deletion and filtering of artstore components
// 
// 6     6/29/98 9:43a Psasse
// A new dynamic way to add/delete categories and filtering for artstore
// components
// 
// 5     6/02/98 4:24p Rlovejoy
// Check for mouse down events so we can remove Hints.
// 
// 4     5/19/98 3:02p Dennis
// Added History
// 
*/

#ifndef __ARTSTORE_H__
#define __ARTSTORE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "pictprvw.h"
#include "error.h"
#include "PmwDlg.h"

// CODImageList class currently provides a means to retrieve the original
// image while satisfying the needs of common controls that rely on it
// for sizing information such as CTabCtrl
class CODImageList : public CImageList
   {
   public:
      int               Add(CBitmap *pbmImage, CBitmap *pbmMask);
      int               Add(CBitmap *pbmImage, COLORREF crMask);
      BOOL              Draw(CDC *pDC, int nImageIndex, POINT pt, UINT nStyle);
      CBitmap *         GetBitmap(int nImage);

   protected:
   // Helpers
       static HBITMAP   DuplicateDDB ( HBITMAP hbmOriginal, HPALETTE hPalette=NULL);

   protected:
      CBitmap  m_cbmImage;
   };

/////////////////////////////////////////////////////////////////////////////
// CODTabCtrl window

// CODTabCtrl is an owner draw tab control that uses a CODImageList control
// for sourcing the bitmap used for tabs
class CODTabCtrl : public CTabCtrl
{
// Construction
public:
	CODTabCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CODTabCtrl)
	//}}AFX_VIRTUAL

   void DrawItem(LPDRAWITEMSTRUCT lpDrawItem);
   CODImageList * GetImageList();

// Implementation
public:
	virtual ~CODTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CODTabCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CUnlockPackInfo
   {
   public:
   // Construction
      CUnlockPackInfo();
      ~CUnlockPackInfo();

   // Get Operations
      const CString &   GetName() const;
      const CString &   GetCollectionBaseName() const;
      const CString &   GetImageFileName() const;
      const CString &   GetProductCode() const;
      const CString &   GetFilters() const;

   // Set Operations
      void              SetName(LPCSTR szName);
      void              SetCollectionBaseName(LPCSTR szName);
      void              SetImageFileName(LPCSTR szFileName);
      void              SetProductCode(LPCSTR szNewCode);
      void              SetFilters(LPCSTR szNewCode);

   protected:
      CString     m_csName;
      CString     m_csCollectionBaseName;
      CString     m_csImageFileName;
      CString     m_csProductCode;
      CString     m_csFilterString;
   };

class COrderFormInfo
   {
   public:
   // Construction
      COrderFormInfo()  {};
      ~COrderFormInfo();

   // Get Operations
      const CString &   GetFileName() const;
      const CString &   GetProductImageFileName() const;
      const CString &   GetTabImageFileName() const;
      const CString &   GetProductName() const;
      const CString &   GetFilters() const;
		int					GetPackCount() const;
		int					GetTabSortOrder() const;
      const CUnlockPackInfo * GetPack(int nPack) const;
		BOOL					GetIsInternetContent(void) const;

   // General Operations
      void              Empty();
      // Read() reads the specified order form file 
      ERRORCODE         Read(LPCSTR szOrderFormFileName);

   // Set Operations
      void              SetFileName(LPCSTR szNewFileName);
      void              SetProductName(LPCSTR szNewProductName);
      void              SetProductImageFileName(LPCSTR szNewFileName);
      void              SetTabImageFileName(LPCSTR szNewFileName);
      void              SetFilters(LPCSTR szNewFilters);
      void              SetImageIndex(int nNewIndex);
      void              SetTabSortOrder(int nNewIndex);
		void					SetIsInternetContent(BOOL fIsInternetContent);

   protected:
   // Helpers
      CString           GetFirstWord(LPCSTR szSourceString);
      CString           GetNextWord();

   protected:
      CString     m_csFileName;
      CString     m_csProductName;
      CString     m_csProductImageFileName;
      CString     m_csTabImageFileName;
		CString		m_csFilterString;
      int         m_nImageIndex;       // Index to image in image list
      CTypedPtrArray <CPtrArray, CUnlockPackInfo *>  m_cpaPackInfo;

      CString     m_csSourceString;    // String used by GetFirstWord(), GetNextWord()
      int         m_nCurCharIndex;     // State variable used by GetNextWord()
		int			m_nTabSortOrder;
		BOOL			m_fIsInternetContent;
   };

/////////////////////////////////////////////////////////////////////////////
// CArtStoreDialog dialog

class CArtStoreDialog : public CPmwDialog
{
	INHERIT(CArtStoreDialog, CPmwDialog)
// Construction
public:
	CArtStoreDialog(CWnd* pParent = NULL);   // standard constructor
   virtual ~CArtStoreDialog();

   const CUnlockPackInfo* GetCurrentPack();
	COrderFormInfo* GetCurrentOrderForm();

	CPMWCollection* GetCollectionToPreview(void)
		{ return m_pCollectionToPreview;	}

// Dialog Data
	//{{AFX_DATA(CArtStoreDialog)
	enum { IDD = IDD_ART_STORE };
	CButton	m_btnPreview;
	CButton	m_btnOrder;
	CButton	m_btnDownload;
	CListBox	m_clbPack;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArtStoreDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArtStoreDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeArtStorePackList();
	afx_msg void OnDblclkArtStorePackList();
	afx_msg void OnArtStoreOrder();
	afx_msg void OnArtStorePreview();
	afx_msg void OnArtStoreDownload();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateOnline();
	//}}AFX_MSG
	afx_msg void OnArtStoreTabChange(NMHDR* pNMHDR, LRESULT* pResult); 
	DECLARE_MESSAGE_MAP()

// Helpers
   BOOL GetOrderForms(void);
   ERRORCODE DisplayPacks(void);
   void GetSortedArray();
	void FindORDFiles(CStringArray& csaORDFiles);
	void FindORDFiles(LPCSTR pszDirectory, CStringArray& csaORDFiles);
	void UpdatePackButtons(void);
	void PositionButton(UINT uID, int nNewButtonBottom);
	void UpdateOnlineControls(BOOL fIsOnline);

	BOOL InitImageList(void);
	BOOL BuildImageListBitmap(CBitmap& cbmImages, CSize& czBitmap);

protected:
// Data Members
   int               m_nCurTab;
   BOOL              m_bHasTabControl;
   CODTabCtrl        m_tabctrlProduct;
   CPicturePreview   m_cppPackPreview;
   CODImageList      m_ilLogos;
   CTypedPtrArray	<CPtrArray, COrderFormInfo *> m_cpaOrderInfo;
   CTypedPtrArray <CPtrArray, COrderFormInfo *> m_cpaSortedOrderInfo;
	CSize					m_cszTabImage;
	IPathManager*		m_pPathManager;
	ICollectionManager* m_pCollectionManager;
	IContentManager*	m_pContentManager;
	CPMWCollection* m_pCollectionToPreview;
};

// Inline Section

// COrderFormInfo inline methods
inline const CString & COrderFormInfo::GetFileName() const
   { return m_csFileName; }

inline const CString & COrderFormInfo::GetProductImageFileName() const
   { return m_csProductImageFileName; }

inline const CString & COrderFormInfo::GetTabImageFileName() const
   { return m_csTabImageFileName; }

inline const CString & COrderFormInfo::GetProductName() const
   { return m_csProductName; }

inline const CString & COrderFormInfo::GetFilters() const
   { return m_csFilterString; }

inline const CUnlockPackInfo * COrderFormInfo::GetPack(int nPack) const
   { return m_cpaPackInfo[nPack]; }

inline int COrderFormInfo::GetPackCount() const
   { return m_cpaPackInfo.GetSize(); }

inline void COrderFormInfo::SetFileName(LPCSTR szNewFileName)
   { m_csFileName = szNewFileName; }

inline void COrderFormInfo::SetProductName(LPCSTR szNewProductName)
   { m_csProductName = szNewProductName; }

inline void COrderFormInfo::SetProductImageFileName(LPCSTR szNewFileName)
   { m_csProductImageFileName = szNewFileName; }

inline void COrderFormInfo::SetTabImageFileName(LPCSTR szNewFileName)
   { m_csTabImageFileName = szNewFileName; }

inline void COrderFormInfo::SetFilters(LPCSTR szNewFileName)
   { m_csFilterString = szNewFileName; }

inline int COrderFormInfo::GetTabSortOrder() const
   { return m_nTabSortOrder; }

inline void COrderFormInfo::SetTabSortOrder(int nNewIndex)
   { m_nTabSortOrder = nNewIndex; }

inline BOOL COrderFormInfo::GetIsInternetContent() const
   { return m_fIsInternetContent; }

inline void COrderFormInfo::SetIsInternetContent(BOOL fIsInternetContent)
   { m_fIsInternetContent = fIsInternetContent; }

// CUnlockPackInfo inline methods
inline const CString & CUnlockPackInfo::GetName() const
   { return m_csName; }

inline const CString & CUnlockPackInfo::GetCollectionBaseName() const
   { return m_csCollectionBaseName; }

inline const CString & CUnlockPackInfo::GetImageFileName() const
   { return m_csImageFileName; }

inline const CString & CUnlockPackInfo::GetProductCode() const
   { return m_csProductCode; }

inline const CString & CUnlockPackInfo::GetFilters() const
   { return m_csFilterString; }

inline void CUnlockPackInfo::SetName(LPCSTR szNewName)
   { m_csName = szNewName; }

inline void CUnlockPackInfo::SetCollectionBaseName(LPCSTR szNewName)
   { m_csCollectionBaseName = szNewName; }

inline void CUnlockPackInfo::SetImageFileName(LPCSTR szNewFileName)
   { m_csImageFileName = szNewFileName; }

inline void CUnlockPackInfo::SetProductCode(LPCSTR szNewProductCode)
   { m_csProductCode = szNewProductCode; }

inline void CUnlockPackInfo::SetFilters(LPCSTR szNewFilterString)
   { m_csFilterString = szNewFilterString; }

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __ARTSTORE_H__
