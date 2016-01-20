//*****************************************************************************
//
// File Name:		SaveAsDialog.cpp
//
// Project:			Renaissance Text Component
//
// Author:			Mike Heydlauf
//
// Description:	Front end for modified generic CFileDialog.
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WindowsSaveAsDialog.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//*****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"SaveAsDialog.h"
#include <direct.h>

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIDs.h"
#include "ApplicationGlobals.h"
#include "DialogUtilities.h"
#include "ResourceManager.h"
#include "FileType.h"
#include "StandaloneApplication.h"

#ifndef	_WINDOWS
	#error	This file must be compilied only on Windows
#endif	//	_WINDOWS

// Static variables
BOOLEAN						RSaveAsDialog::m_fLoadProjectPath	= TRUE;
BOOLEAN						RSaveAsDialog::m_fLoadHTMLProjectPath	= TRUE;

//*****************************************************************************
//
// Function Name:	RSaveAsData::ChangeToDefaultProjectDirectory
//
// Description:	Change to the directory where we store projects by default.
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
void RSaveAsData::ChangeToDefaultProjectDirectory( )
{
	//	Use the change method in the dialog utilities.
	::ChangeToDefaultProjectDirectory( );

	//	Remember that we have already shown the user where the projects directory is.
	RSaveAsDialog::m_fLoadProjectPath = FALSE;
}

//*****************************************************************************
//
// Function Name:	RSaveAsData::ChangeToDefaultProjectDirectory
//
// Description:	Return TRUE if we should change to the directory where we store 
//						projects by default.
//
// Returns:			^
//
// Exceptions:		None
//
//*****************************************************************************
//
BOOLEAN RSaveAsData::ShouldChangeToDefaultProjectDirectory( )
{
	return RSaveAsDialog::m_fLoadProjectPath;
}

//*****************************************************************************
//
// Function Name:	RSaveAsHTMLData::ChangeToDefaultProjectDirectory
//
// Description:	Change to the directory where we store projects by default.
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
void RSaveAsHTMLData::ChangeToDefaultProjectDirectory( )
{
	//	Change to the projects directory so our projects will at least go there
	//	if for some reason we can't find the html projects directory.
	RSaveAsData::ChangeToDefaultProjectDirectory( );

	//	Start in the default project directory.
	RMBCString rFilePath = ::GetApplication( ).GetApplicationPath( );
	rFilePath += ::GetResourceManager().GetResourceString(STRING_OPEN_DEFAULT_PATH);//"Projects\";

	// Change directory to the HTML projects directory.
	rFilePath += ::GetResourceManager().GetResourceString(STRING_HTML_DIRECTORY_NAME);//"HTML Projects\";
	_chdir( (LPCSZ)rFilePath ) ;

	//	Remember that we have already shown the user where the html directory is.
	RSaveAsDialog::m_fLoadHTMLProjectPath = FALSE;
}

//*****************************************************************************
//
// Function Name:	RSaveAsHTMLData::ChangeToDefaultProjectDirectory
//
// Description:	Return TRUE if we should change to the directory where we store 
//						html projects by default.
//
// Returns:			^
//
// Exceptions:		None
//
//*****************************************************************************
//
BOOLEAN RSaveAsHTMLData::ShouldChangeToDefaultProjectDirectory( )
{
	return RSaveAsDialog::m_fLoadHTMLProjectPath;
}

//*****************************************************************************
//
// Function Name:	RSaveAsDialog::RSaveAsDialog
//
// Description:	Constructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
RSaveAsDialog::RSaveAsDialog( CWnd* pParent /*=NULL*/ ) : m_pParent( pParent )
{
	;
}

//*****************************************************************************
//
// Function Name:	RSaveAsDialog::DoModal
//
// Description:	Sets up the OPENFILENAME struct for the modified generic
//						SaveAS dialog and calls GetofnSaveData to draw the dialog.
//
// Returns:			The return value from the call to GetSaveFileName
//
// Exceptions:		None
//
//*****************************************************************************
//
int RSaveAsDialog::DoModal( RSaveAsData* pSaveAsData ) 
{
	// Get the save file types
	RFileTypeCollection saveFileTypes;
	pSaveAsData->m_FileFormatCollection.GetSaveFileTypes( saveFileTypes );

	// From the types, get the filter string
	RMBCString filterString;
	saveFileTypes.GetFileDialogFilterString( filterString, TRUE );
	// 

	//
	// By default we want to load the save as dialog with the project path.  We only
	// want to do it once though because if the user changes the path we want
	// to restore with that one..
	if( pSaveAsData->ShouldChangeToDefaultProjectDirectory( ) )
		pSaveAsData->ChangeToDefaultProjectDirectory( );

	CFileDialog cFileDlg( FALSE, filterString, (LPCSZ)pSaveAsData->sPathName, 
								 OFN_HIDEREADONLY | 
								 OFN_OVERWRITEPROMPT | 
								 OFN_SHOWHELP |
								 OFN_PATHMUSTEXIST |
								 OFN_EXPLORER, 
								 filterString, NULL );
	cFileDlg.SetHelpID( DIALOG_SAVEAS_HELPID );
	int nRetVal = cFileDlg.DoModal();
	if( nRetVal == IDOK )
	{
		pSaveAsData->m_pSelectedFileFormat	= pSaveAsData->m_FileFormatCollection[ cFileDlg.m_ofn.nFilterIndex - 1 ]; 
		pSaveAsData->sPathName					= RMBCString( cFileDlg.GetPathName() );
	}
	else
	{
		//
		// User canceled, but still need to set this parameter...
		//pSaveAsData->m_pSelectedFileFormat	= pSaveAsData->m_FileFormatCollection[ 0 ]; 
		//pSaveAsData->sPathName					= "";
	}
	return nRetVal;
}
