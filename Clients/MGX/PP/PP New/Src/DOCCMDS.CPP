/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                doccmds.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "svrview.h"
#include "id.h"
#include "io.h"
#include "custools.h"

#ifdef _DEBUG
#undef THIS_FILE
static TCHAR BASED_CODE THIS_FILE[] = __FILE__;
#endif

/******************************* EXPORTED Functions *****************************/

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */
/* ********************************** Macros ******************************** */

/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
/* ******************************* Local Functions **************************** */
/* ******************************* Local Methods **************************** */

afx_msg void CServerDoc::OnUpdateCloseImage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( (GetImage() != NULL) && !GetImage()->IsBusy() );
}

afx_msg void CServerDoc::OnUpdateFileUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( (GetImage() != NULL) && !GetImage()->IsBusy() );
}

afx_msg void CServerDoc::OnUpdateFileSaveCopyAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( (GetImage() != NULL) && !GetImage()->IsBusy() );
}

afx_msg void CServerDoc::OnUpdateSave(CCmdUI* pCmdUI)
{
	ITEMID id;
	LPTSTR lpString;

	// Set the Save menu item based on whether a client owns the image
	id = ( ( GetImage() && GetImage()->fOwnedByClient ) ? IDS_UPDATESAVE : IDS_SAVE );
	if ( pCmdUI->m_pMenu && AstralStr( id, &lpString ) )
		pCmdUI->SetText(lpString);
	pCmdUI->Enable( (GetImage() != NULL) && !GetImage()->IsBusy() );
}

afx_msg void CServerDoc::OnUpdateSaveAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( (GetImage() != NULL) && !GetImage()->IsBusy() );
}

afx_msg void CServerDoc::OnUpdatePrint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( (GetImage() != NULL) && !GetImage()->IsBusy() );
}

afx_msg void CServerDoc::OnCloseImage()
{
    DeactivateTool();
    PictPubApp.CloseDocumentFile(GetImage());
}

afx_msg void CServerDoc::OnSave()
{
	BOOL fFastbits;
	FNAME szFileName;
	
	// fastbits are not real untitled
	fFastbits = (GetImage()->DocumentType == IMG_DOCPIECE);
	if (fFastbits)
		lstrcpy(szFileName, GetImage()->CurFile);
	else
		lstrcpy(szFileName, m_strPathName);
		
    AnsiToOem( szFileName, szFileName );		
	if ((_access(szFileName, 6) != 0) || (!fFastbits && GetImage()->fUntitled))
	{
	    // we do not have read-write access or the file does not (now) exist
		// or the image is UNTITLED
		OnSaveAs();
	}
	else
	{
	    DeactivateTool(); 
	    do_save(GetImage()->CurFile);
	}
}

afx_msg void CServerDoc::OnFileUpdate()
{
	FileUpdate();
}

afx_msg void CServerDoc::OnFileSaveCopyAs()
{
	FileSaveCopyAs();
}

afx_msg void CServerDoc::OnSaveAs()
{
    DeactivateTool();
    do_save(NULL);
}

afx_msg void CServerDoc::OnPrint()
{
	CImage *pImage = GetImage(); 
    if (Control.Crippled)
    {
        Message(Control.Crippled);
        return;
    }
    DeactivateTool();

    // Bring up the modal 'Print' box 
    if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_PRINT, DlgPrintProc ) )
        return;
    AstralCursor( IDC_WAIT );
    PrintFile( PictPubApp.Get_hWndAstral(), filename(pImage->CurFile), YES, pImage, NULL );
    AstralCursor( NULL );
}


