// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


class CLoadMaskDlg : public CPPViewModalDlg
{
	LOADMASK_PARMS m_Parms;


	void OnOK();
	void OnCancel();
	BOOL OnInitDialog();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnDeleteMask();
	void SetupParms();

public:
	enum {IDD = IDD_MASKLOAD};
	CLoadMaskDlg(CServerView* pView=NULL, CWnd* pParent = NULL);

DECLARE_DYNCREATE(CLoadMaskDlg)
DECLARE_MESSAGE_MAP()

};
