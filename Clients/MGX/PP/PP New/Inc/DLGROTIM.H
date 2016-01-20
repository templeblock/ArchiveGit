// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

//dlgrotim.h  :  CRotateImageDlg declaration

#include "ppmodal.h"

class CRotateImageDlg: public CPPViewModalDlg
{
private:
	BOOL m_bPrefSmartSize;
	BOOL m_nPrefIsCCW;
	LFIXED m_Angle;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual	LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual void afx_msg OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);

public:

	enum {IDD = IDD_ROTATE};
	CRotateImageDlg(CServerView* pView, CWnd* pParent = NULL);
	BOOL GetSmartSize() const {return m_bPrefSmartSize;}
	int GetIsCCW() const {return m_nPrefIsCCW;}
	LFIXED GetAngle() const {return m_Angle;}
	virtual int DoModal() { PreDoModal();  return CPPViewModalDlg::DoModal(); }

DECLARE_MESSAGE_MAP()
}; 
