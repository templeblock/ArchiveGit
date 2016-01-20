//{{AFX_INCLUDES()
#include "grid.h"
//}}AFX_INCLUDES
#if !defined(AFX_MEASUREDIALOG_H__AA6BD661_8D02_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_MEASUREDIALOG_H__AA6BD661_8D02_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MeasureDialog.h : header file
//

#include "resource.h"
#include "DeviceControl.h"
#include "grid.h"	// Added by ClassView
#include "DeviceSheet.h"
#include "profilerdoc.h"

//defined message returned by communication device
#define WM_DATA_READY	(WM_USER + 102)

#define MEASURE_COLOR_DATA		(1)
#define	MEASURE_CURVE_DATA		(2)

/////////////////////////////////////////////////////////////////////////////
// CMeasureDialog dialog

class CMeasureDialog : public CDialog
{
// Construction
public:
	CMeasureDialog(int measuretype, CProfileDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	~CMeasureDialog();

	DeviceSetup	m_deviceSetup;

// Dialog Data
	//{{AFX_DATA(CMeasureDialog)
	enum { IDD = IDD_MEASURE };
	CButton	m_bOk;
	CButton	m_bStart;
	CButton	m_bSetup;
	CString	m_strStatus;
	CGrid	m_ctlGrid;
	CString	m_strPatch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasureDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	CProfileDoc *m_pDoc;
	char	*m_refRgb;

	HWND m_phWnd;
	int m_currentCol;
	int m_currentRow;
	int m_numCols;

	//lab patches
	int m_labPatches;

	//linear patchee
	int m_linearPatches;

	//current patch number and total patch number
	int m_currentPatch;
	int	m_totalPatches;

	int m_dataStart;
	int m_cyanStartPatches;
	int m_magentaStartPatches;
	int m_yellowStartPatches;
	int m_blackStartPatches;

	//check to see the device is reading data
	int	m_runningstatus;

	//device object
	CDeviceControl *m_pdevice;

	//paper density
	double m_paper[4];
	BOOL   m_haspaper;

	// Generated message map functions
	//{{AFX_MSG(CMeasureDialog)
	afx_msg void OnStartBut();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClickGrid();
	afx_msg void OnKeyPressGrid(short FAR* KeyAscii);
	afx_msg void OnSetupButton();
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnRequestComplete(WPARAM wParam, LPARAM lParam);

	void SetPaperDensity(double* cmyk);
	BOOL GetRgb(double density, char* cmyk);
	void CleanupDevice();
	BOOL InitReading();
	void ReadNextPatch();
	void SetGridPosition();
	void PutonGrid(int row, int startcol, int endcol, double *data);
	void SetParamForGrid();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASUREDIALOG_H__AA6BD661_8D02_11D1_9EE4_006008947D80__INCLUDED_)
