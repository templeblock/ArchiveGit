/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                calib.h

*****************************************************************************
*/

#ifndef _CALIB_H
#define _CALIB_H

#include "ppmodal.h"
#include "dlgsmap.h"

/* ****************************** Public Data ***************************** */
/* ****************************** Constants ******************************* */
/* ****************************** Typedefs ******************************** */
/* ****************************** Classes ********************************* */
class  CCalibrateApplyDlg;
class  CCalibratePrintDlg;
class  CCalibrateScanDlg;
class  CLoadDlg;
class  CPPCalibDlg;
class  CPrintVisualDlg;
class  CScanEditDlg;
class  CScanVisualDlg;

struct ImageToolProcData
{
    RECT  rGridRect;
    short iImageWidth;
    short iImageHeight;
};

class CPPCalibDlg : public CPPMapDlg
{
public:

    CPPCalibDlg(CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
    virtual BOOL PreDoModal(LPVOID lpVoid);
    virtual LPVOID PostDoModal();

    void HandleScanMapInit();
    void HandleScanMapLoad();
    void HandleScanChannels( int id, UINT codeNotify);
    void HandleScanCalAllSame( int id, UINT codeNotify);
    BOOL HandleScanSave( );
    BOOL LoadScanCalibrate( LPTSTR lpExtName );
    BOOL SaveScanCalibrate( LPTSTR lpExtName );
    void SetupScanMapControls();
    void UpdateScanMaps();

protected:

    BOOL	m_fScanCalVisual;
    BOOL	m_fUseRGBForGray;
    BOOL	m_fUseGrayForRGB;
    BOOL    m_fCalAll;

    EXTNAME m_Map_Name;
    FRMDATATYPE m_Scan_type;

private:

};

class  CCalibrateScanDlg : public CPPCalibDlg
{
public:
	CCalibrateScanDlg ( CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd = NULL );

    void CalibrateScan_InitMaps();

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

protected:

private:

    BOOL m_bModified;
    ImageToolProcData m_Data;
    BYTE m_Rstep[CALPOINTS];
    BYTE m_Gstep[CALPOINTS];
    BYTE m_Bstep[CALPOINTS];
    BYTE m_Xstep[CALPOINTS];
};

class  CCalibratePrintDlg : public CPPCalibDlg
{
public:
	CCalibratePrintDlg ( CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

protected:

private:
    BOOL m_bModified;
    ImageToolProcData m_Data;

    BYTE m_Rstep[CALPOINTS];
    BYTE m_Gstep[CALPOINTS];
    BYTE m_Bstep[CALPOINTS];
    BYTE m_Kstep[CALPOINTS];
};

class  CCalibrateApplyDlg : public CPPModalDlg
{
public:
	CCalibrateApplyDlg ( UINT nIDTemplate, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual BOOL PreDoModal (LPVOID lpData = NULL);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

protected:

private:

    STRING m_szScanMap;
    STRING m_szPrintMap;
    LPAPPLYCAL_PARMS m_CalibrateApply_lpParms;
    ITEMID m_idListToManage;
};

class  CScanVisualDlg : public CPPCalibDlg
{
public:
	CScanVisualDlg ( CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnQueryNewPalette();

protected:

private:

};

class  CPrintVisualDlg : public CPPCalibDlg
{
public:
	CPrintVisualDlg ( CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnQueryNewPalette();

protected:

private:
};

class  CLoadDlg : public CPPCalibDlg
{
public:
    // Maps Parameters can be Master,Cyan,Yellow,Magenta,Black also
    // in that order
	CLoadDlg (  ITEMID MapType, UINT nIDTemplate, CWnd* pParentWnd,
        MAP *pMasterMap, MAP *pRedMap,  MAP *pGreenMap,  MAP *pBlueMap,  MAP *pBlackMap);

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

protected:

private:

    ITEMID m_MapType;
    MAP *m_pMasterMap;  // Master
    MAP *m_pRedMap;     // can be Cyan
    MAP *m_pGreenMap;   // can be Yellow
    MAP *m_pBlueMap;    // can be Magenta
    MAP *m_pBlackMap;   // Black
};

class  CScanEditDlg : public CPPCalibDlg
{
public:
	CScanEditDlg (  UINT nIDTemplate, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

protected:

private:

};

/* *************************** Inline Methods ****************************** */
/* *************************** Global Data ********************************* */
/* *************************** Global Function ***************************** */
/* ************************************************************************* */

#endif // _CALIB_H


