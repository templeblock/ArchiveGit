// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "ppmodal.h"
#include "dlgmskld.h"


IMPLEMENT_DYNCREATE(CLoadMaskDlg, CPPViewModalDlg)

BEGIN_MESSAGE_MAP(CLoadMaskDlg, CPPViewModalDlg)
	ON_BN_CLICKED(IDC_DELETEMASK, OnDeleteMask)
END_MESSAGE_MAP()

CLoadMaskDlg::CLoadMaskDlg(CServerView* pView, CWnd* pParent)
:CPPViewModalDlg(pView, CLoadMaskDlg::IDD, pParent)
{

}


BOOL CLoadMaskDlg::OnInitDialog()
{
	CenterWindow();
	InitExtName( GetSafeHwnd(), IDC_EXTNAMES, IDC_FILENAME, Names.Mask, IDN_MASK );
	lstrcpy( Names.Saved, Names.Mask );
	CheckDlgButton(IDC_DELETEMASK, Mask.DeleteMask);
	ControlEnable(GetSafeHwnd(), IDC_DELETEMASK, ImgGetMask(GetImage()) != NULL);
	return CPPViewModalDlg::OnInitDialog();
}

void CLoadMaskDlg::OnDeleteMask()
{
	Mask.DeleteMask = !Mask.DeleteMask;
	CheckDlgButton(IDC_DELETEMASK, Mask.DeleteMask);
}

void CLoadMaskDlg::OnCancel()
{
	lstrcpy( Names.Mask, Names.Saved );
	CPPViewModalDlg::OnCancel();
}	


void CLoadMaskDlg::OnOK()
{
	FNAME szFileName;
	LPALPHA lpAlpha;
	RECT rUpdate;
	BOOL fTransform;
	LPIMAGE lpImage = GetImage();
	HWND hActiveWnd = GetViewHwnd();
	


	if ( !LookupExtFile( Names.Mask, szFileName, IDN_MASK ) ) {EndDialog(FALSE); return;}
	if (!(lpAlpha = ReadMask(lpImage, szFileName, 0, 0))) {EndDialog(FALSE); return;}
	EndDialog(TRUE);										   //why here????
	AstralUpdateWindow(GetViewHwnd());
	AstralSetRectEmpty(&rUpdate);
	if (!LoadMaskStart(lpImage, lpAlpha, Mask.DeleteMask, NO, &rUpdate, &fTransform)) {EndDialog(FALSE); return;}
	if (fTransform) TransformStartLoadMaskProc(lpImage, Mask.DeleteMask);
	else
	{
		SetupParms();
		PostCommand(lpImage->lpCmdList, IDS_CMD_LOADMASK, &m_Parms);
		if (!AstralIsRectEmpty(&rUpdate))
		{
			File2DispRect(hActiveWnd, &rUpdate, &rUpdate);
			::InvalidateRect(hActiveWnd, AstralToWindowsRect(&rUpdate), FALSE);
		}
		if ( Tool.hRibbon )  ::SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
	}
	CPPViewModalDlg::OnOK();
}

BOOL CLoadMaskDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hDlg = GetSafeHwnd();
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;

	switch (id)
	{

		case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Mask, IDN_MASK, codeNotify );
		break;

		case IDC_EXTMANAGE:
		PopupMenu( hDlg, id, IDC_FILENAME );
		break;

		case IDC_ADDEXT:
		case IDC_DELETEEXT:
		case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask,
			IDN_MASK, id, NO );
		break;

		default:
		break;
	}
	return CWnd::OnCommand(wParam, lParam);
}
void CLoadMaskDlg::SetupParms()
{
	int res, dx, dy;

	ImgGetInfo(GetImage(), &dx, &dy, NULL, NULL);
	res = FrameResolution(ImgGetBaseEditFrame(GetImage()));
	m_Parms.fHiQuality = Mask.PasteQuality;
	m_Parms.Mode = SHAPE_ADD;
	m_Parms.iBaseRes = res;
	m_Parms.DistortPoints[0].x = 0;
	m_Parms.DistortPoints[0].y = 0;
	m_Parms.DistortPoints[1].x = dx-1;
	m_Parms.DistortPoints[1].y = 0;
	m_Parms.DistortPoints[2].x = dx-1;
	m_Parms.DistortPoints[2].y = dy-1;
	m_Parms.DistortPoints[3].x = 0;
	m_Parms.DistortPoints[3].y = dy-1;
	lstrcpy(m_Parms.szMask, Names.Mask);
	m_Parms.fDeleteMask = Mask.DeleteMask;
}
