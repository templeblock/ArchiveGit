// ****************************************************************************
//
//  File Name:			WindowsStandaloneDocument.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RStandaloneDocument class
//
//  Portability:		Windows Specific
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WindowsStandaloneDocument.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"StandaloneDocument.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"StandaloneView.h"
#include "SaveAsDialog.h"
#include "UndoManager.h"
#include "ApplicationGlobals.h"
#include "FileCloseAction.h"
#include "StandaloneApplication.h"
#include "Mainframe.h"
#include "Menu.h"
#include "FrameworkResourceIds.h"
#include "FolderDialog.h"

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::Initialize( )
//
//  Description:		Initializer. We must use an initializer rather than a
//							constructor, as this document will be created by MFC's
//							document template mechanism.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::Initialize( RApplication* pApp, YDocumentType documentType )
	{
	// Iterate the MFC view list, and add the views to our view list
	POSITION pos = GetFirstViewPosition( );

	while( pos )
		{
		CView* pCView = GetNextView( pos );
		RStandaloneView*	pStandaloneView = dynamic_cast<RStandaloneView*>( pCView );
		TpsAssert( pStandaloneView, "The view retrieved from a StandaloneDocument was NOT a standalone view");
		AddRView( pStandaloneView );
		}

	// Call the cross platform initializer to continue intialization
	XInitialize( pApp, documentType );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetDocumentFilename( )
//
//  Description:		Retrieves the full path name of the file associated with
//							this document. Returns an empty string if the document 
//							does not have a file yet.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString RStandaloneDocument::GetDocumentFilename( ) const
	{
	return GetPathName( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::SetDocumentFilename( )
//
//  Description:		Sets the file associated with this document
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::SetDocumentFilename( const RMBCString& filename )
	{
	TpsAssert( filename.IsEmpty( ) == FALSE, "Empty filename" );
	SetPathName( filename );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::DoFileSaveDialog( )
//
//  Description:		Displays the file save dialog
//
//  Returns:			The file chosen from the dialog. The string is empty if
//							cancel was chosen
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::DoFileSaveDialog( RSaveAsData* pSaveAsData ) const
	{
	RSaveAsDialog SaveAsDlg;
	//
	//Draw generic Save As dialog.
	if ( SaveAsDlg.DoModal(pSaveAsData) == IDOK )
	{
		;
	}
	else
	{
		//	the user canceled; notify our caller with an empty path
		pSaveAsData->sPathName.Empty();
	}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::DoSelectDirectoryDialog( )
//
//  Description:		Displays the file save dialog allowing the selection of
//							directories only.
//
//  Returns:			Nothing.
//
//							The path chosen is placed in the pSaveAsData
//							structure supplied within the sPathName member.
//
//							Whether to use the full HTML extension is stored in the
//							pbUseFullHTMLExtension pointer.  If set to FALSE .htm is desired.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::DoSelectDirectoryDialog( RSaveAsData* pSaveAsData, BOOLEAN *pbUseFullHTMLExtension, YFloatType *pfPageSizePercentage ) const
	{
	CString strDefaultDir = pSaveAsData->sPathName.operator CString();
	int nUseFullExt = (*pbUseFullHTMLExtension) ? 0 : 1;
	RFolderDialog SelectDirectoryDlg( strDefaultDir, nUseFullExt );
	//
	//Draw generic Save As dialog.
	SelectDirectoryDlg.DoModal();
	if ( SelectDirectoryDlg.OKSelected() )
		{
		pSaveAsData->sPathName = SelectDirectoryDlg.GetDirPath();
		*pbUseFullHTMLExtension = (SelectDirectoryDlg.GetUseFullHTMLExt() == 0);	// If 0, then .html was selected.
		*pfPageSizePercentage = SelectDirectoryDlg.GetPageSizePercentage();
		}
	else
		{
		//	the user canceled; notify our caller with an empty path
		pSaveAsData->sPathName.Empty();
		}
	}


// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnOpenDocument( )
//
//  Description:		Called by MFC to open a document. Stubbed out, as we do
//							the load ourself.
//
//  Returns:			See CDocument::OnOpenDocument
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneDocument::OnOpenDocument( LPCTSTR )
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::SaveModified( )
//
//  Description:		Called by MFC to open see if a doc needs to be saved
//							This is how Renaissance is notified that the user clicked
//							the close box on the docs window so we save here and tell
//							the mfc not to.
//
//  Returns:			TRUE: the mfc can continue doing whatever it was doing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneDocument::SaveModified( )
	{
	//	if the doc has changed then save it before closing it
	YAlertValues yResponse = kAlertYes;

	if ( m_pUndoManager->DocDirty() )
		{
		RAlert AskUser;
		
//		WinCode( RMBCString rAppTitle( AfxGetApp()->m_pszAppName ) );
//		WinCode( AskUser.SetTitle( rAppTitle ) );

#ifdef _WINDOWS
		// Get the document title
		CString szTitle( GetCDocument()->GetTitle( ) );
		RMBCString String;
		String = AskUser.FormatAlertString( STRING_PROMPT_DOC_CHANGED, szTitle );
		yResponse = AskUser.ConfirmUser( String );

		if ( kAlertYes == yResponse )
			yResponse = OnFileSave( FALSE );
		}

	if ( kAlertCancel != yResponse )
		{
		// Make sure close is scripted
		RFileCloseAction* pAction = new RFileCloseAction( GetDocumentFilename() );
		::GetApplication().SendAction( pAction );
		}
#endif //_WINDOWS

	return ( kAlertCancel != yResponse );	//	the mfc can continue doing whatever it was doing
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetDefaultMenu( )
//
//  Description:		Called by MFC to get the default menu for this document.
//
//  Returns:			The menu that we modified to add the non standard components
//
//  Exceptions:		None
//
// ****************************************************************************
//
HMENU RStandaloneDocument::GetDefaultMenu( )
	{
	return ::GetApplication( ).GetApplicationMenu( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::SetPathName( )
//
//  Description:		Called by MFC to set the path name for this document. We
//							override this and force bAddToMRU to FALSE. This is because
//							we are manipulating the MRU list ourselves.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::SetPathName( LPCTSTR lpszPathName, BOOL )
	{
	FrameworkDocument::SetPathName( lpszPathName, FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RRenaissanceApplication::OnClose( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnClose( )
	{
	CDocument::OnFileClose( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnCloseDocument( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnCloseDocument( )
	{
	OnXCloseDocument( );
	CDocument::OnCloseDocument( );

	// Kick idle to force an update on the toolbars.
	TpsAssertIsObject( RMainframe, ::AfxGetMainWnd( ) );
	RMainframe* pMainframe = static_cast<RMainframe*>( ::AfxGetMainWnd( ) );
	pMainframe->ForceIdle( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::Activate( )
//
//  Description:		Activates this document
//
//							NOTE: This code copied from MFC 4.1 DOCMGR.CPP, line 783
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::Activate( )
	{
	// Get the first view position
	POSITION pos = GetFirstViewPosition( );
	if( pos != NULL )
		{
		// Get the first view
		CView* pView = GetNextView( pos );
		ASSERT_VALID( pView );

		// Get the views frame
		CFrameWnd* pFrame = pView->GetParentFrame( );

		// Activate the frame
		TpsAssert( pFrame != NULL, "Can not find a frame for document to activate." );
		pFrame->ActivateFrame( );

		// If the frame is not the app frame, activate the app frame
		CFrameWnd* pAppFrame;
		if( pFrame != ( pAppFrame = (CFrameWnd*)AfxGetApp( )->m_pMainWnd ) )
			{
			ASSERT_KINDOF(CFrameWnd, pAppFrame);
			pAppFrame->ActivateFrame();
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::DeleteContents( )
//
//  Description:		MFC Notification to free up the documents contents.
//							We need to call the FreeDocumentContents function.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::DeleteContents( )
	{
	// Call our cross platform method
	NULL;	//	the data has already been freed in the OnXCloseDocument
	}
