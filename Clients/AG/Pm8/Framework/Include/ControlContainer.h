// ****************************************************************************
//
//  File Name:			ControlContainer.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RControlContainer class
//
//  Portability:		Windows specific
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
//  $Logfile:: /PM8/Framework/Include/ControlContainer.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CONTROLCONTAINER_H_
#define		_CONTROLCONTAINER_H_

#include "StandaloneDocument.h"
#include "WindowView.h"
#include "MessageDispatch.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentAttributes;
class RControlDocument;

// ****************************************************************************
//
//  Class Name:		RControlView
//
//  Description:		Control view class
//
// ****************************************************************************
//
class FrameworkLinkage RControlView : public RMessageDispatch<CWnd>, public RWindowView
	{
	//
	// Windows Message map
	DECLARE_MESSAGE_MAP()
	public :
		afx_msg UINT OnGetDlgCode();
		afx_msg void OnContextMenu( CWnd*, CPoint );

	// Constuctor
	public :
												RControlView( CDialog* pDialog, int nControlId, RControlDocument* pDocument );

	// Operations
	public :
		virtual void						UpdateScrollBars( EScrollUpdateReason );
		virtual void						SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;
		virtual BOOLEAN						SubclassControl( CDialog* pDialog, int nControlId );
		virtual RColor						GetBackgroundColor( ) const;

		// Platform specific conversion operators
	public :
		virtual CWnd&						GetCWnd( ) const;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:		RControlDocument
//
//  Description:		Control document class
//
// ****************************************************************************
//
class FrameworkLinkage RControlDocument : public RStandaloneDocument 
	{
	// Construction/Destruction
	public :
													RControlDocument( );
		virtual									~RControlDocument( );

	// Operations
	public :
		virtual RControlView*				CreateView( CDialog* pDialog, int nControlId );
		virtual RRealSize						GetMinObjectSize( const YComponentType&, BOOLEAN fMaintainAspect = FALSE );
		virtual RRealSize						GetMaxObjectSize( const YComponentType& );
		virtual RRealSize						GetDefaultObjectSize( const YComponentType& );
		virtual const RFontSizeInfo*		GetFontSizeInfo( ) const;
		virtual void							GetSaveFileFormats( RFileFormatCollection& ) const;
		virtual void							SetDefaultFileFormat( );

	// Members
	public :
		RFontSizeInfo							m_FontInfo;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void							Validate( ) const;
#endif
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _CONTROLCONTAINER_H_
