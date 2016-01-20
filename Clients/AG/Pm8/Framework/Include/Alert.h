// ****************************************************************************
//
//  File Name:			Alert.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RAlert class
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
//  $Logfile:: /PM8/Framework/Include/Alert.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ALERT_H_
#define		_ALERT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RAlert
//
//  Description:	The class for Managing all alerts that will be presented to the
//						user.
//
// ****************************************************************************
//
class FrameworkLinkage RAlert : public RObject
	{
	// Construction, destruction & initialization
	public :
												RAlert( );
		virtual								~RAlert( );

	// Operations
	public :
		void									AlertUser( YResourceId alertId );
		YAlertValues						WarnUser( YResourceId warningId );
		YAlertValues						QueryUser( YResourceId queryId );
		YAlertValues						ConfirmUser( YResourceId confirmId );
		YAlertValues						InformUser( YResourceId informId );
		YAlertValues						AskUser( YResourceId askId );
		void									SetTitle( YResourceId alertId );

		void									AlertUser( LPCSZ lpszAlert );
		YAlertValues						WarnUser( LPCSZ lpszWarning );
		YAlertValues						QueryUser( LPCSZ lpszQuery );
		YAlertValues						ConfirmUser( LPCSZ lpszConfirm );
		YAlertValues						InformUser( LPCSZ lpszConfirm );
		YAlertValues						AskUser( LPCSZ lpszAsk );
		void									SetTitle( LPCSZ lpszTitle );

		RMBCString							FormatAlertString( YResourceId alertId, const RMBCString& subString );

		YAlertValues						AskUserForCdSwap();
		YAlertValues						AskUserForCdSwap( YResourceId DialogTemplate );
		YGBCDSwapAlertValues				AskUserForGraphicBrowserCdSwap();

	//	Implementation
	private :
		YAlertValues						DisplayMessage( YAlertFlags flags, LPCSZ lpszMessage );
		
	//	member data
	protected:
		RMBCString							m_rAlertTitle;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_ALERT_H_

