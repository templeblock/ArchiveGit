// ****************************************************************************
//
//  File Name:			Alert.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RAlert class
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
//  $Logfile:: /PM8/Framework/Source/Alert.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "StandaloneApplication.h"
#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "FrameworkResourceIds.h"
#include "dbt.h"

// ****************************************************************************
//
//  Function Name:	RAlert::RAlert( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAlert::RAlert( )
	{
	// Default the alert title to the application name
	SetTitle( STRING_APPLICATION_NAME );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::~RAlert( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAlert::~RAlert( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RAlert::SetTitle( )
//
//  Description:		Set the alert's title to the given string
//							Retrieve the title from a resource file.
//							NOTE: this title works for the next alert only
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RAlert::SetTitle( LPCSZ lpszTitle )
	{
	m_rAlertTitle = lpszTitle;
	}

// ****************************************************************************
//
//  Function Name:	RAlert::SetTitle( )
//
//  Description:		Set the alert's title to the given string
//							Retrieve the title from a resource file.
//							NOTE: this title works for the next alert only
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RAlert::SetTitle( YResourceId alertId )
	{
	m_rAlertTitle = GetResourceManager().GetResourceString( alertId );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::AlertUser( )
//
//  Description:		Alert the user of a problem with an Alert and an OK button
//							Retrieve the message from a resource file.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RAlert::AlertUser( YResourceId alertId )
	{
	RMBCString	string = GetResourceManager().GetResourceString( alertId );
#ifdef	TPSDEBUG
	char	buffer[128];
	buffer[0] = '\0';
	WinCode( wsprintf( buffer, " - %d", alertId ) );
	string += buffer;
#endif
	AlertUser( (LPCSZ)string );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::WarnUser( )
//
//  Description:		Warn the user of a problem with an Alert and an OK
//							and Cancel button.
//							Retrieve the message from a resource file.
//
//  Returns:			kAlertOk or kAlertCancel
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::WarnUser( YResourceId warningId )
	{
	RMBCString	string = GetResourceManager().GetResourceString( warningId );
#ifdef	TPSDEBUG
	char	buffer[128];
	buffer[0] = '\0';
	WinCode( wsprintf( buffer, " - %d", warningId ) );
	string += buffer;
#endif
	return WarnUser( (LPCSZ)string );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::QueryUser( )
//
//  Description:		Ask the user a question with an Alert and Yes/No buttons.
//							Retrieve the message from a resource file.
//
//  Returns:			kAlertYes or kAlertNo
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::QueryUser( YResourceId queryId )
	{
	RMBCString	string = GetResourceManager().GetResourceString( queryId );
#ifdef	TPSDEBUG
	char	buffer[128];
	buffer[0] = '\0';
	WinCode( wsprintf( buffer, " - %d", queryId ) );
	string += buffer;
#endif
	return QueryUser( (LPCSZ)string );
	}


// ****************************************************************************
//
//  Function Name:	RAlert::ConfirmUser( )
//
//  Description:		Ask the user a question with an Alert and
//							Yes/No/Cancel buttons.
//							Retrieve the message from a resource file.
//
//  Returns:			kAlertYes, kAlertNo, or kAlertCancel
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::ConfirmUser( YResourceId confirmId )
	{
	RMBCString	string = GetResourceManager().GetResourceString( confirmId );
#ifdef	TPSDEBUG
	char	buffer[128];
	buffer[0] = '\0';
	WinCode( wsprintf( buffer, " - %d", confirmId ) );
	string += buffer;
#endif
	return ConfirmUser( (LPCSZ)string );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::InformUser( )
//
//  Description:		Ask the user a question with an inform icon and OK
//							button.
//							Retrieve the message from a resource file.
//
//  Returns:			kAlertYes
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::InformUser( YResourceId informId )
	{
	RMBCString	string = GetResourceManager().GetResourceString( informId );
#ifdef	TPSDEBUG
	char	buffer[128];
	buffer[0] = '\0';
	WinCode( wsprintf( buffer, " - %d", informId ) );
	string += buffer;
#endif
	return InformUser( (LPCSZ)string );
	}

// ****************************************************************************
//
//  Function Name:	AskUser::InformUser( )
//
//  Description:		Ask the user a question with an inform icon and OK
//							button.
//							Retrieve the message from a resource file.
//
//  Returns:			kAlertYes
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::AskUser( YResourceId informId )
	{
	RMBCString	string = GetResourceManager().GetResourceString( informId );
#ifdef	TPSDEBUG
	char	buffer[128];
	buffer[0] = '\0';
	WinCode( wsprintf( buffer, " - %d", informId ) );
	string += buffer;
#endif
	return AskUser( (LPCSZ)string );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::AlertUser( )
//
//  Description:		Alert the user of a problem with an Alert and an OK button
//							Retrieve the message from a resource file.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RAlert::AlertUser( LPCSZ lpszAlert )
	{
	YAlertFlags	flags = MacWinDos( 0, MB_ICONSTOP|MB_OK, xxx );
	
	// Note (DCS) any error messages come through after the resource
	// manager has been shutdown will contain empty strings, don't
	// display these.
	if ( strlen(lpszAlert) > 0 )
		DisplayMessage( flags, lpszAlert );
	else
		TRACE( "RAlert::AlertUser called with an empty string. Resource manager already gone??\n" );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::WarnUser( )
//
//  Description:		Warn the user of a problem with an Alert and an OK
//							and Cancel button.
//							Retrieve the message from a resource file.
//
//  Returns:			kAlertOk or kAlertCancel
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::WarnUser( LPCSZ lpszWarning )
	{
	YAlertFlags	flags = MacWinDos( 0, MB_ICONSTOP|MB_OKCANCEL, xxx );
	return DisplayMessage( flags, lpszWarning );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::QueryUser( )
//
//  Description:		Ask the user a question with an Alert and Yes/No buttons.
//							Use the given message.
//
//  Returns:			kAlertYes or kAlertNo
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::QueryUser( LPCSZ lpszQuery )
	{
	YAlertFlags	flags = MacWinDos( 0, MB_ICONQUESTION|MB_YESNO, xxx );
	return DisplayMessage( flags, lpszQuery );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::ConfirmUser( )
//
//  Description:		Ask the user a question with an Alert and Yes/No/Cancel
//							buttons.
//							Use the given message.
//
//  Returns:			kAlertYes, kAlertNo, or kAlertCancel
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::ConfirmUser( LPCSZ lpszConfirm )
	{
	YAlertFlags	flags = MacWinDos( 0, MB_ICONQUESTION|MB_YESNOCANCEL, xxx );
	return DisplayMessage( flags, lpszConfirm );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::InformUser( )
//
//  Description:		Ask the user a question with an inform icon and OK
//							button.
//							Use the given message.
//
//  Returns:			kAlertYes, kAlertNo, or kAlertCancel
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::InformUser( LPCSZ lpszInform )
	{
	YAlertFlags	flags = MacWinDos( 0, MB_ICONINFORMATION|MB_OK, xxx );
	return DisplayMessage( flags, lpszInform );
	}

// ****************************************************************************
//
//  Function Name:	RAlert::AskUser( )
//
//  Description:		Ask the user a question with an inform icon and OK
//							button.
//							Use the given message.
//
//  Returns:			kAlertYes, kAlertNo, or kAlertCancel
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::AskUser( LPCSZ lpszInform )
	{
	YAlertFlags	flags = MacWinDos( 0, MB_ICONINFORMATION|MB_OKCANCEL, xxx );
	return DisplayMessage( flags, lpszInform );
	}


// ****************************************************************************
//
//  Class Name:	RCdSwapDialog
//
//  Description:	Helper class for detecting when a CD is inserted.
//
// ****************************************************************************
//
class RCdSwapDialog : public CDialog
{
public:
							RCdSwapDialog()
							:	CDialog(),
								pActiveWindow( NULL ),
								pOSWndProc( NULL )
							{
								TpsAssert( pThisDialog==NULL, "pThisDialog is not NULL!" );

								pActiveWindow = ::GetActiveWindow();
								TpsAssert( pActiveWindow, "pActiveWindow is NULL - can't detect CD insertion." );

								if (pActiveWindow)
								{
									pOSWndProc = (WNDPROC)::SetWindowLong( pActiveWindow, GWL_WNDPROC, (LONG)myWndProc );
									TpsAssert( pOSWndProc, "pOSWndProc is NULL!" );
								}
								pThisDialog = this;
							}

	virtual				~RCdSwapDialog()
							{
								if (pActiveWindow && pOSWndProc)
									 ::SetWindowLong( pActiveWindow, GWL_WNDPROC, (LONG)pOSWndProc );

								pActiveWindow = NULL;
								pOSWndProc = NULL;
								pThisDialog = NULL;
							}

	virtual BOOL		OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
							{
								if (nID == IDABORT)
								{
									EndModalLoop(kGBOtherGraphics);
									return TRUE;
								}
								return CDialog::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
							}

private:
	HWND					pActiveWindow;
	WNDPROC				pOSWndProc;

	static RCdSwapDialog* pThisDialog;

	static LRESULT CALLBACK myWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

RCdSwapDialog* RCdSwapDialog::pThisDialog = NULL;

LRESULT CALLBACK RCdSwapDialog::myWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (uMsg == WM_DEVICECHANGE)
	{
		DEV_BROADCAST_VOLUME* dbcv = (DEV_BROADCAST_VOLUME*)lParam;
		int iDrive;

		switch (wParam)
		{
		case DBT_DEVICEARRIVAL:
			if (dbcv->dbcv_devicetype == DBT_DEVTYP_VOLUME &&
				(dbcv->dbcv_flags & DBTF_MEDIA))
			{
				for (iDrive = 0; iDrive < 26; iDrive++)
				{
					if (dbcv->dbcv_unitmask & (1L << iDrive))
					{
						TRACE( "CD inserted in drive %d - end dialog now\n", iDrive );
						TpsAssert( pThisDialog, "pThisDialog is NULL!" );
						pThisDialog->EndDialog( IDOK );
					}
				}
			}
			break;
		}
	}
	TpsAssert( pThisDialog, "pThisDialog is NULL!" );
	TpsAssert( pThisDialog->pOSWndProc, "pThisDialog->pOSWndProc is NULL!" );
	return pThisDialog->pOSWndProc( hWnd, uMsg, wParam, lParam );
}


// ****************************************************************************
//
//  Function Name:	RAlert::AskUserForCdSwap()
//
//  Description:		Ask the user to insert a CD with OK and Cancel buttons.
//
//							For 32-bit CD-ROM drivers, if a CD is inserted before OK is
//							clicked, the dialog automatically terminates and this
//							function returns kAlertOk.
//
//  Returns:			kAlertOk or kAlertCancel
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::AskUserForCdSwap()
{
	return AskUserForCdSwap( DIALOG_CD_SWAP );
}

// ****************************************************************************
//
//  Function Name:	RAlert::AskUserForCdSwap( YResourceId DialogTemplate )
//
//  Description:		Ask the user to insert a CD with OK and Cancel buttons.
//							The alert to use is specified by DialogTemplate.
//
//							For 32-bit CD-ROM drivers, if a CD is inserted before OK is
//							clicked, the dialog automatically terminates and this
//							function returns kAlertOk.
//
//  Returns:			kAlertOk or kAlertCancel
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::AskUserForCdSwap( YResourceId DialogTemplate )
	{
	RCdSwapDialog theDialog;
	if (!theDialog.InitModalIndirect( GetResourceManager().GetResourceDialogTemplate( DialogTemplate ) ))
	{
		TpsAssertAlways( "InitModalIndirect failed!" );
		return kAlertCancel; // shouldn't happen
	}
	if (theDialog.DoModal() == IDOK)
	{
		// set wait cursor on assumption that it may take a little while for
		// calling code to open and search files on the CD and that someone else
		// will set the cursor back to normal later
		::GetCursorManager().SetCursor( IDC_WAIT );

		return kAlertOk;
	}
	else
		return kAlertCancel;
	}

// ****************************************************************************
//
//  Function Name:	RAlert::AskUserForGraphicBrowserCdSwap()
//
//  Description:		Ask the user to insert a CD with OK, Cancel and
//							Other Graphics buttons - suitable for use with bringing
//							up the graphics browser dialog.
//
//							For 32-bit CD-ROM drivers, if a CD is inserted before OK is
//							clicked, the dialog automatically terminates and this
//							function returns kAlertOk.
//
//  Returns:			kGBAlertOk, kGBAlertCancel or kGBOtherGraphics
//
//  Exceptions:		None
//
// ****************************************************************************
//
YGBCDSwapAlertValues RAlert::AskUserForGraphicBrowserCdSwap()
{
	RCdSwapDialog theDialog;
	if (!theDialog.InitModalIndirect( GetResourceManager().GetResourceDialogTemplate( DIALOG_CD_SWAP_GB ) ))
	{
		TpsAssertAlways( "InitModalIndirect failed!" );
		return kGBAlertCancel; // shouldn't happen
	}
	int dialogStatus = theDialog.DoModal();
	if (dialogStatus == IDOK)
	{
		// set wait cursor on assumption that it may take a little while for
		// calling code to open and search files on the CD and that someone else
		// will set the cursor back to normal later
		::GetCursorManager().SetCursor( IDC_WAIT );

		return kGBAlertOk;
	}
	else if (dialogStatus == kGBOtherGraphics)
		return kGBOtherGraphics;
	else
		return kGBAlertCancel;
}

// ****************************************************************************
//
//  Function Name:	RAlert::DisplayMessage( )
//
//  Description:		Display the message in a system specific way.
//
//  Returns:			the users alert value
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAlertValues	RAlert::DisplayMessage( YAlertFlags flags, LPCSZ lpszMessage )
	{
	//	Initialize to Cancel in case of some catastrophic error has occured
	//		cancel is the LEAST distructive option (usually)
	YAlertValues	returnValue = kAlertCancel;

#ifdef	_WINDOWS

	// Get the active window
	HWND hwnd = ::GetActiveWindow( );

	// if we don't have the active window use NULL and MB_TASKMODAL..
	if( !hwnd )
		{
		flags |= MB_TASKMODAL;
		}

	int iChoice = ::MessageBox( hwnd, lpszMessage, m_rAlertTitle, (UINT)flags );
	switch (iChoice)
		{
			case IDOK :
				returnValue = kAlertOk;
				break;

			case IDCANCEL :
				returnValue = kAlertCancel;
				break;

			case IDYES :
				returnValue = kAlertYes;
				break;

			case IDNO :
				returnValue = kAlertNo;
				break;

			default:
				//		eventually change this to a AssetAlways
				TpsAssert( FALSE, "Return value from MessageBox is unknown" );
				break;
		}

#else		//	_WINDOWS

#endif	//	_WINDOWS

	m_rAlertTitle.Empty();

	return returnValue;
	}

// ****************************************************************************
//
//  Function Name:	RAlert::FormatAlertString( )
//
//  Description:		Format an a string with variable text for alert output.
//							Retrieve the message from a resource file and format the
//							subString into the resource string (likely to be a file 
//							name for an error message).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString RAlert::FormatAlertString( YResourceId alertId, const RMBCString& subString )
	{
	LPSZ	lpszString = (LPSZ)GetGlobalBuffer();
	*lpszString = 0;		//	Make sure the string is null terminated...

	RMBCString	string = GetResourceManager().GetResourceString( alertId );
	wsprintf(  lpszString, (LPCSZ)string, (LPCSZ)subString ); 
	
	RMBCString	returnString(lpszString);
	ReleaseGlobalBuffer( (unsigned char*)lpszString );

#ifdef	TPSDEBUG
	char	buffer[10];
	buffer[0] = '\0';
	WinCode( wsprintf( buffer, " - %d", alertId ) );
	returnString += buffer;
#endif

	return returnString;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RAlert::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAlert::Validate( ) const
	{
	RObject::Validate( );
	}

#endif		//	TPSDEBUG
