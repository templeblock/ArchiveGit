/*
// $Workfile: phoprjprint.h $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/phoprjprint.h $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     1/30/99 11:32p Psasse
// Updated and fairly stable EasyPrints code
// 
//  
*/

class CPhotoProjectPrintDialog;
class CPhotoProjectStartDialog;

#ifndef __PHOPRJPRINT_H__
#define __PHOPRJPRINT_H__

#include "pmwprint.h"
#include "pmwdlg.h"

class CPhotoProjectPreview;

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectPrintDialog dialog

class CPhotoProjectPrintDialog : public CPmwPrint
{
// Construction
public:
	CPhotoProjectPrintDialog(CPmwView* pView);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPhotoProjectPrintDialog)
	enum { IDD = IDD_LABEL_PRINT };
	BOOL	m_fCollate;
	BOOL	m_fPageBreak;
	int	m_nPhotoCopies;
	//}}AFX_DATA

	int m_nTotalPhotos;
	int m_nTotalPages;

	int m_nStartingPhoto;
	int m_nStartingSubSection;
	void ComputeTotals(void);
	void UpdateStartingPhoto(void);
	void UpdateCollate(void);

	virtual void update_names_chosen(void);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPhotoProjectPrintDialog)
	afx_msg void OnStartingPhoto();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePhotoCopies();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectStartDialog dialog

class CPhotoProjectStartDialog : public CPmwDialog
{
// Construction
public:
	CPhotoProjectStartDialog(CPhotoPrjData* pPhotoInfo, CWnd* pParent = NULL, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPhotoProjectStartDialog)
	enum { IDD = IDD_PHOTOPROJECT_START };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CPhotoPrjData* m_pPhotoPrjData;
	int m_nStartingPhoto;
	int m_nStartingSubSection;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CPhotoProjectPreview* m_pPreview;

	// Generated message map functions
	//{{AFX_MSG(CPhotoProjectStartDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDoubleclickedPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
