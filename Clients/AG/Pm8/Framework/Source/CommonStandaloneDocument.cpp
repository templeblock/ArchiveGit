// ****************************************************************************
//
//  File Name:			CommonStandaloneDocument.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Definition of the RStandaloneDocument class
//
//  Portability:		Platform independent
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
//  $Logfile:: /PM8/Framework/Source/CommonStandaloneDocument.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"StandaloneDocument.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"StandaloneApplication.h"
#include	"MultiUndoManager.h"
#include	"MultiUndoableAction.h"
#include	"FrameworkResourceIDs.h"
#include "Script.h"
#include "FileSaveAction.h"
#include "ChunkyStorage.h"
#include "SaveAsDialog.h"
#include "ApplicationGlobals.h"
#include "DataTransfer.h"
#include "MergeData.h"
#include "View.h"
#include "FileOpenAction.h"
#include "BufferStream.h"
#include "Printer.h"
#include "PrintManager.h"
#include "VersionInfo.h"
#include "StandaloneView.h"
#include "FileStream.h"

//	chunk tag for version info
const uLONG kVersionInfoTag = 'VERS';

// Command Map
RCommandMap<RStandaloneDocument, RDocument> RStandaloneDocument::m_CommandMap;

RCommandMap<RStandaloneDocument, RDocument>::RCommandMap( )
	{
	RouteCommandToObject( (RCommandTarget* RStandaloneDocument::*) &RStandaloneDocument::m_pUndoManager );
	HandleCommand( COMMAND_MENU_FILE_CLOSE, RStandaloneDocument::OnClose );
	HandleAndUpdateCommand( COMMAND_MENU_FILE_SAVE, RStandaloneDocument::OnSave, RStandaloneDocument::OnUpdateSave );
	HandleCommand( COMMAND_MENU_FILE_SAVEAS, RStandaloneDocument::OnSaveAs );
	HandleCommand( COMMAND_MENU_EDIT_UNDO, RStandaloneDocument::OnUndo );
	HandleCommand( COMMAND_MENU_EDIT_REDO, RStandaloneDocument::OnRedo );
	HandleAndUpdateCommand( COMMAND_MENU_FILE_REVERT, RStandaloneDocument::OnRevertToSaved, RStandaloneDocument::OnUpdateRevertToSaved );
	HandleCommand( COMMAND_MENU_FILE_PRINT, RStandaloneDocument::OnPrint );
	HandleAndUpdateCommand( COMMAND_MENU_FILE_FASTPRINT, RStandaloneDocument::OnFastPrint, RStandaloneDocument::OnUpdateFastPrint );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::RStandaloneDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument::RStandaloneDocument( )
	: m_pUndoManager( NULL ),
	  m_pDocumentStorage( NULL ),
	  m_pMergeData( NULL ),
	  m_pFileFormat( NULL ),
	  m_pPrinter( NULL )
#ifdef	MAC
		, LSingleDoc( LCommander::GetTopCommander( ) )
#endif
	{
	m_pUndoManager = new RUndoManager();
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::~RStandaloneDocument( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument::~RStandaloneDocument( )
	{
	delete m_pUndoManager;
	delete m_pFileFormat;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::CreateCopy( )
//
//  Description:		Virtual constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument* RStandaloneDocument::CreateCopy( ) const
	{
	UnimplementedCode( );
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::XInitialize( )
//
//  Description:		Cross platform initializer. Called from the platform
//							specific implementation of Initialize
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::XInitialize( RApplication* pApp, YDocumentType documentType )
	{
	m_DocumentType = documentType;

	// Call the base initializer
	RDocument::Initialize( pApp );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::FreeDocumentContents( )
//
//  Description:		Free all of the document contents.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RStandaloneDocument::FreeDocumentContents( )
	{
	// Clear out the undo manager
	m_pUndoManager->FreeAllActions( );

	// Delete the merge data
	delete m_pMergeData;
	m_pMergeData = NULL;

	// Delete the document storage
	delete m_pDocumentStorage;
	m_pDocumentStorage = NULL;

	// Call the base method
	RDocument::FreeDocumentContents( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::SendAction( )
//
//  Description:		Component API function to send an action to a Component
//
//  Returns:			TODO: What does this return?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RStandaloneDocument::SendAction( RAction* pAction )
	{
	BOOLEAN					fReturnValue	= FALSE;
	RAutoCleanupAction	cleanup( pAction );

	//	First Always, try to write the script as it is in its state before the 
	//	Do occurs. This will allow us to Create the Action and then call Do
	//	in the script processing.
	try
		{
		if ( GetRecordingScript( ) )
			{
			RScript&	script	= *GetRecordingScript( );
			script << pAction->GetActionId( );
			pAction->WriteScript( script );
			}
		}
	catch( YException exception )
		{
		::ReportException( exception );///xxx	TODO: Script Dumping Failed... Alert user and continue
		}

	if( pAction->Do( ) )
		{
		fReturnValue = TRUE;
			//	Action was Done and the script was written, we succeded.
			//	If action is undoable, give it to the undo mananger...
			//	If it is not undoable, we are done with it so delete it...
		if( pAction->IsUndoable( ) )
			{
			RUndoableAction*	pUndoAction = dynamic_cast<RUndoableAction*>( pAction );
			TpsAssert( pUndoAction, "The given undoable action is not derived from RUndoableAction");
			TpsAssertValid( m_pUndoManager );	
			m_pUndoManager->AddAction( pUndoAction );
			//
			//	Tell cleanup class to release control of the object, the action object
			//	ownership has passed to the undo manager
			cleanup.Release( );

			//	Error should be presented by the UndoManager for not
			//		being able to add the action to its list...
			}

		}
	else
		{
		;///xxx	TODO: Do processing Failed... Alert user and throw an error...
		}

	return fReturnValue;
	}


// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::BeginMultiUndo( )
//
//  Description:		Start a MultiUndo chain of actions
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::BeginMultiUndo( )
	{
	//	Create and set the Multi Undo manager
	RMultiUndoManager*	pNewManager = new RMultiUndoManager( m_pUndoManager );
	m_pUndoManager = pNewManager;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::CancelMultiUndo( )
//
//  Description:		Cancel a MultiUndo chain of actions
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::CancelMultiUndo( )
	{
	TpsAssertIsObject( RMultiUndoManager, m_pUndoManager );
	RMultiUndoManager*		pMultiManager = (RMultiUndoManager*)m_pUndoManager;
	//	First, restore the Previous undo manager
	m_pUndoManager = pMultiManager->GetPreviousUndoManager( );
	delete pMultiManager;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::EndMultiUndo( )
//
//  Description:		End a MultiUndo chain of actions
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::EndMultiUndo( uWORD uwUndoStringId, uWORD uwRedoStringId )
	{
	TpsAssertIsObject( RMultiUndoManager, m_pUndoManager );
	RMultiUndoManager*		pMultiManager = (RMultiUndoManager*)m_pUndoManager;
	//	First, restore the Previous undo manager
	m_pUndoManager = pMultiManager->GetPreviousUndoManager( );

	//	Create a multi-undo action iff the pMultiManager has undo actions
	//	If an error occurs somewhere, we may have a multi undo with no actions
	//		in which case, we don't want to do anything...
	if (pMultiManager->CountUndoableActions() > 0)
		{
		//	Create the MultiUndo action and restore the undo manager to the previous manager
		RMultiUndoableAction*	pAction		= new RMultiUndoableAction( pMultiManager, uwUndoStringId, uwRedoStringId );
		//	Now Send the action to the document
		SendAction( pAction );
		}
	else
		delete pMultiManager;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetNextActionId( )
//
//  Description:		Get the id of the action that will be undone next
//
//  Returns:			The id
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YActionId& RStandaloneDocument::GetNextActionId( ) const
	{
	return m_pUndoManager->GetNextActionId( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetTopLevelDocument( )
//
//  Description:		Return the Top Level Standalone document
//
//  Returns:			this
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument* RStandaloneDocument::GetTopLevelDocument( ) const
	{
	return const_cast<RStandaloneDocument*>( this );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::CreateScriptAction( )
//
//  Description:		Try to create an action from the script
//
//  Returns:			The pointer to the action, or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction* RStandaloneDocument::CreateScriptAction( const YActionId& actionId, RScript& script )
	{
	return RDocument::CreateScriptAction( actionId, script );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::Write( )
//
//  Description:		Writes this documents data to the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStandaloneDocument::Write( RChunkStorage& storage, EWriteReason reason ) const
	{
	// Write version info
	storage.AddChunk( kVersionInfoTag );
	RVersionInfo version;
	version.GetVersionInfo( );
	version.Write( storage );
	storage.SelectParentChunk( );

	// Call the base method
	RDocument::Write( storage, reason );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::Read( )
//
//  Description:		Reads from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStandaloneDocument::Read( RChunkStorage& storage, EReadReason reason )
	{
	// Read version info
	if( !storage.Start( kVersionInfoTag, FALSE ).End( ) )
		{
		YStreamLength yStartPos = storage.GetPosition( );
		
		m_Version.Read( storage );
		storage.SelectParentChunk( );
		
		// set the version info of the file being read in 
		// this should be cleared after the file is completely read
		(RApplication::GetApplicationGlobals())->SetCurrentDocumentVersion( m_Version );

		//	Start reading again where we left off before reading the version info
		storage.SeekAbsolute( yStartPos );
		}

	// Reset the warned flag so that we will warn about any missing components.
	m_fWarnedAboutMissingComponent = FALSE;

	// Call the base method
	RDocument::Read( storage, reason );

	// create a blank version info and set it in the global after the file is read
	RVersionInfo info;
	(RApplication::GetApplicationGlobals())->SetCurrentDocumentVersion(info);

	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OpenPlaceholder( )
//
//  Description:		Opens a placeholder file to prevent the document from being
//							deleted
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OpenPlaceholder( const RMBCString& filename )
	{
	if( !filename.IsEmpty( ) )
		{
	try
			{
			//	delete the existing placeholder and open the new doc
			delete m_pDocumentStorage;
			m_pDocumentStorage = NULL;
			//	Changed from kReadWrite to kRead so that revert to saved
			//	would work under Windows 95. REVIEW BDR - this change
			//	may allow the user to delete an open document from the NT
			//	Explorer, however only Win95 is important now. 6/9/97
			m_pDocumentStorage = new RChunkStorage( filename, kRead, kPlaceholder );
			m_pDocumentStorage->SetSaveOnClose( FALSE );
			}
		catch( ... )
			{
			//	it is ok to fail while opening a placeholder;
			//	this can happen if the doc is already open
			NULL;
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnFileSave( )
//
//  Description:		Called when File::Save is chosen from the menu.
//
//  Returns:			user's response if any to filename request
//
//  Exceptions:		File
//
// ****************************************************************************
//
YAlertValues RStandaloneDocument::OnFileSave( BOOLEAN fSaveAs, RSaveAsData* pGivenSaveData /*= NULL*/ )
	{
	BOOLEAN fContinue = TRUE;
	BOOLEAN fSaveCanceled = FALSE;
	RMBCString filename;
	RFileFormat* pFileFormat = NULL;

	while( fContinue )
		{
		// If we dont have a file format, or we are doing a save as, display the save as dialog
		if( !m_pFileFormat || fSaveAs )
			{
			// Initialize a SaveAsStruct with this documents information
			RSaveAsData saveAsData;
			RSaveAsData* pSaveData = pGivenSaveData;

			//	If we weren't given save as data then get it.
			if ( NULL == pGivenSaveData )
			{
				saveAsData.sPathName = GetDocumentFilename( );
				GetSaveFileFormats( saveAsData.m_FileFormatCollection );
				pSaveData = &saveAsData;
			}
	
			//	Remove the file path from the file name so the save dialog
			//	will install the right one for the user's choice of format.
#ifdef _WINDOWS
			int nExtensionStartsAt = pSaveData->sPathName.operator CString( ).ReverseFind( '.' );
			if ( -1 != nExtensionStartsAt )
				pSaveData->sPathName = pSaveData->sPathName.Head( nExtensionStartsAt );
#endif

			// Do the filesave dialog
			DoFileSaveDialog( pSaveData );
			if( pSaveData->sPathName.IsEmpty() ) fSaveCanceled = TRUE;

			// Retrieve the required information
			filename = pSaveData->sPathName;
			pFileFormat = pSaveData->m_pSelectedFileFormat;
			pSaveData->m_FileFormatCollection.Remove( pFileFormat );
			}

		// Otherwise, use the current type and name
		else
			{
			pFileFormat = m_pFileFormat;
			filename = GetDocumentFilename( );
			}

		// If we have a file type, try to save
		if( !fSaveCanceled )
			{
			try
				{
				TpsAssert( pFileFormat, "Save being called on NULL FileFormat" );
				pFileFormat->Save( filename );

				// If this is a symmetric format, update all the document info
				if( pFileFormat->IsSymmetric( ) )
					{
					// Set the format
					if( m_pFileFormat != pFileFormat )
						delete m_pFileFormat;

					m_pFileFormat = pFileFormat;

					// Set the new document filename
					SetDocumentFilename( filename );

					// Update the MRU list
					WinCode( AfxGetApp( )->AddToRecentFileList( filename ) );
					}

//				else
//					delete pFileFormat;

				return kAlertOk;
				}

			catch( YException exception )
				{
				RAlert askUser;

				if( kShareViolation == exception || kAccessDenied == exception )
					fContinue = static_cast<BOOLEAN>( askUser.WarnUser( STRING_ERROR_ACCESS_DENIED ) == kAlertOk );
				else if( kFileNotFound == exception )
					fContinue = static_cast<BOOLEAN>( askUser.WarnUser( STRING_FILE_MOVED_ERROR ) == kAlertOk );
				else
					{
					::ReportException( exception );
					fContinue = FALSE;
					}

				//	Clean up any allocated file formats so they will not be leaked above
				//	when we try to save again.
				if( !pGivenSaveData && (m_pFileFormat==pFileFormat) )
					{
					delete m_pFileFormat;
					m_pFileFormat = NULL;
					pFileFormat = NULL;
					}
				else if ( pGivenSaveData )
					{
					//	insert the format back into the list of formats
					pGivenSaveData->m_FileFormatCollection.InsertAtEnd( pFileFormat );
					pFileFormat = NULL;
					}
				}
			}

		else
			fContinue = FALSE;
		}

	return kAlertCancel;
	}


// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetVersion( )
//
//  Description:		Accessor
//
//  Returns:			m_rVersion
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RVersionInfo& RStandaloneDocument::GetVersion( ) const
	{
	return m_Version;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetStorage( )
//
//  Description:		return our storage
//
//  Returns:			m_pDocumentStorage
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChunkStorage* RStandaloneDocument::GetStorage( ) const
	{
	return m_pDocumentStorage;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::CanIncorporateData( )
//
//  Description:		Determines if this document can incorporate the data
//							available from the specified data source.
//
//  Returns:			TRUE if data can be incorporated
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RStandaloneDocument::CanIncorporateData( const RDataTransferSource& ) const
	{
	// We'll try anything
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::SelectMergeField( )
//
//  Description:		Select a field to be merged into a component data.
//
//  Returns:			TRUE if mergeId is valid, FALSE otherwise;
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RStandaloneDocument::SelectMergeField( RMergeFieldContainer& container )
	{
	BOOLEAN	fValid = FALSE;
	if ( (m_pMergeData != NULL) || ((m_pMergeData = GetMergeData( TRUE )) != NULL ) )
		fValid = m_pMergeData->SelectField( container );
	return fValid;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetMergeData( )
//
//  Description:		Select and return a MergeData class (orphaned).
//
//  Returns:			NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeData*	RStandaloneDocument::GetMergeData( BOOLEAN /* fQuery */ )
	{
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetDocumentType( )
//
//  Description:		Accessor
//
//  Returns:			The type of this document
//
//  Exceptions:		None
//
// ****************************************************************************
//
YDocumentType RStandaloneDocument::GetDocumentType( ) const
	{
	return m_DocumentType;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetPreviewBitmap( )
//
//  Description:		Gets a preview bitmap for this document.
//
//  Returns:			A pointer to the preview bitmap. The caller adopts this
//							pointer.
//
//							NULL is returned if this document does not support a
//							preview bitmap
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBitmapImage* RStandaloneDocument::GetPreviewBitmap( const RIntSize&, const RSolidColor&, int ) const
	{
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnXCloseDocument( )
//
//  Description:		Called when this document is closing, before the views are
//							deleted
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnXCloseDocument( )
	{	
	// Tell the application we are closing
	::GetApplication( ).CloseDocument( this );

	//	Free out internal data
	FreeDocumentContents( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::PreparePrinting( )
//
//  Description:		Displays a print dialog and prepares this document for
//							printing.
//
//  Returns:			TRUE if printing should continue
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RStandaloneDocument::PreparePrinting( )
	{
	TpsAssertAlways( "No default printing code." );

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::Print( )
//
//  Description:		Prints the document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::Print( RCancelPrintingSignal* )
	{
	TpsAssertAlways( "No default printing code." );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::CanPrint( )
//
//  Description:		Determines if this document is currently in a state where
//							it can be printed.
//
//  Returns:			TRUE if we can print this document
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RStandaloneDocument::CanPrint( ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::InitialUpdate( )
//
//  Description:		Does the initial update for this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::InitialUpdate( )
	{
	// Initial update the views
	YViewIterator iterator = GetViewCollectionStart( );
	for( ; iterator != GetViewCollectionEnd( ); ++iterator )
		static_cast<RStandaloneView*>( *iterator )->InitialUpdate( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnSave( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnSave( )
	{
	//	Save if the doc is dirty or hasn't been saved yet.
	//	Broderbund wants save to be available always so we test here
	//	instead of in the OnUpdateSave.
	if ( m_pUndoManager->DocDirty() || (NULL == m_pDocumentStorage) )
		OnFileSave( FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnUpdateSave( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnUpdateSave( RCommandUI& commandUI ) const
	{
	//	Broderbund wants save to be available always.
	commandUI.Enable( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnSaveAs( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnSaveAs( )
	{
	OnFileSave( TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnUndo( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnUndo( )
	{
	TpsAssertValid( m_pUndoManager );
	m_pUndoManager->UndoAction( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnRedo( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnRedo( )
	{
	TpsAssertValid( m_pUndoManager );
	m_pUndoManager->RedoAction( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnRevertToSaved( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnRevertToSaved( )
	{
	try
		{
		RAlert alert;

		// If we have a filename, but no storage, the file was already open when we opened it
		if( m_pDocumentStorage == NULL )
			alert.InformUser( STRING_ERROR_FILE_ALREADY_OPEN_REVERT );

		else
			{
			// Ask the user if they really want to revert
			if( alert.QueryUser( STRING_ALERT_REVERT ) == kAlertYes )
				{
				RWaitCursor waitCursor;

				// Get the file formats supported for loading by our application
				RFileFormatCollection fileFormatCollection;
				::GetApplication( ).GetLoadFileFormats( fileFormatCollection );

				// Get the file format which matches our file
				RFileFormat* pFileFormat = fileFormatCollection.GetMatchingFileFormat( GetDocumentFilename( ) );

				TpsAssert( pFileFormat, "No file format to revert to." );

				// Delete the documents contents. We are going to reuse it
				FreeDocumentContents( );

				// Revert the file
				pFileFormat->Revert( GetDocumentFilename( ), this );

				// We are back to the state just after we saved; tell the undo manager
				m_pUndoManager->DocSaved( );

				// Update the scroll bars
				GetActiveView( )->UpdateScrollBars( kResize );

				// Invalidate the views
				InvalidateAllViews( );
				}
			}
		}

	catch( YException e )
		{
		::ReportException( e );
		}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnUpdateRevertToSaved( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnUpdateRevertToSaved( RCommandUI& commandUI ) const
	{
	//	enable if the doc is dirty and has been saved previously
	if( m_pUndoManager->DocDirty( ) && !GetDocumentFilename( ).IsEmpty( ) )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnPrint( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnPrint( )
	{
	// Prepare for printing, then tell our application to print us
	if( PreparePrinting( ) )
		::GetApplication( ).PrintDocument( this );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnFastPrint( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnFastPrint( )
	{
	// Tell our application to print us
	::GetApplication( ).PrintDocument( this );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::OnUpdateFastPrint( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::OnUpdateFastPrint( RCommandUI& commandUI ) const
	{
	if( CanPrint( ) )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::PrinterChanged( )
//
//  Description:		update the settins for all views if our printer has changed.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::PrinterChanged( const RPrinter* /*pGivenPrinter*/, const uLONG /*ulReason*/ )
	{
	//	This is implemented elsewhere.
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::SetPrinter( )
//
//  Description:		Save the given printer in this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::SetPrinter( RPrinter* pPrinter)
	{
	TpsAssert( NULL != pPrinter, "NULL input printer" );

	m_pPrinter = pPrinter;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetPrinter( )
//
//  Description:		Return the printer used by this document.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter* RStandaloneDocument::GetPrinter( ) const
	{
	// If we don't have a printer yet, just use the default printer
	if( !m_pPrinter )
		const_cast<RStandaloneDocument*>( this )->m_pPrinter = RPrintManager::GetPrintManager( ).GetDefaultPrinter( );

	return m_pPrinter;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::CanInsertComponent()
//
//  Description:		Determines if a component of the specified type may be
//							inserted into this document.
//
//  Returns:			TRUE if the component can be inserted
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RStandaloneDocument::CanInsertComponent( const YComponentType& ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::SetDefaultTitle( )
//
//  Description:		Sets the default title for this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::SetDefaultTitle( )
	{
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RAutoCleanupAction::RAutoCleanupAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAutoCleanupAction::RAutoCleanupAction( RAction* pAction )
	: m_pAction( pAction )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RAutoCleanupAction::~RAutoCleanupAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAutoCleanupAction::~RAutoCleanupAction( )
	{
	delete m_pAction;
	}

// ****************************************************************************
//
//  Function Name:	RAutoCleanupAction::Release( )
//
//  Description:		Releases this action pointer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAutoCleanupAction::Release( )
	{
	m_pAction = NULL;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::Validate( )
//
//  Description:		Validate the Document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::Validate( ) const
	{
	RDocument::Validate();
	TpsAssertIsObject( RUndoManager, m_pUndoManager );
	}

#endif	//	TPSDEBUG

