#include <string.h>

#include "proto.h"
#include "Author/Authres.h"
#include "Author.p"
#include "commctrl.h"
#include "vtable.h"
#include "cllsnid.h"

// global video world
static int iLevel;

static HWND hWndDialog = NULL;
static HWND hSceneWindow = NULL;
static LPVIDEO lpVideo = NULL;
static LPSHOT lpCurShot = NULL;
static BOOL bDragging = FALSE;

static char *pszPrefix = "SPT"; // Shot Pool Tree atom prefix
static char szName[80]; // Name used in atoms

//BOOL ShotSelOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
//void ShotSelOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
BOOL WINPROC EXPORT ShotSelDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
//BOOL ShotSelOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );


/************************************************************************/
void ShotSelStart(HWND hWnd, LPVIDEO lpVid)
/************************************************************************/
{
	hSceneWindow = hWnd;
	lpVideo = lpVid;
	hWndDialog = DialogStart( YES, GetWindowInstance(hWnd), hWnd, IDD_ShotSelAGER, ShotSelDlgProc);
	DialogShow(IDD_ShotSelAGER);
}

/************************************************************************/
void ShotSelEnd(void)
/************************************************************************/
{
	if (hWndDialog)
		DialogEnd( hWndDialog, IDOK);
}


/************************************************************************/
BOOL ShotSelOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{

	MCIStop( lpVideo->hDevice, YES/*bWait*/ );

	return TRUE;
}

/************************************************************************/
void ShotSelOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	if (id == IDOK)
	{
	}
}

/***********************************************************************/
void ShotSelOnNotify(HWND hWnd, int iCtrl, NMHDR FAR* pHdr)
/***********************************************************************/
{
	if (iCtrl == IDC_SHOTPOOLS)
	{
		NM_TREEVIEW *pNMView = (NM_TREEVIEW *)pHdr;

		// If there was a selection change
		if (pNMView->hdr.code == TVN_SELCHANGED)
		{
			//SetCurrentShot(pNMView);
		}
		else if (pNMView->hdr.code == TVN_BEGINDRAG)
		{
			bDragging = TRUE;
		}
	}

	//else if (iCtrl == IDC_EVENTS)
	//{
	//	if (pHdr->code == CBN_SELCHANGE)
	//	{
	//		int i=0;
	//	}
	//}
}

/***********************************************************************/
BOOL ShotSelOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{			  
	return (FALSE);
}

/***********************************************************************/
BOOL WINPROC EXPORT ShotSelDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, WM_INITDIALOG, ShotSelOnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, ShotSelOnCommand);
		HANDLE_DLGMSG(hDlg, WM_NOTIFY, ShotSelOnNotify);
		default:
			return(ShotSelOnMessage(hDlg, msg, wParam, lParam));
		break;
	}
	return(FALSE);
}

/***********************************************************************/
void StartLoad(HWND hWnd)
/***********************************************************************/
{
}
