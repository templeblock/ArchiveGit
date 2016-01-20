// ****************************************************************************
//
//  File Name:			Application.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RApplication class
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
//  $Logfile:: /PM8/Framework/Include/Application.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_APPLICATION_H_
#define		_APPLICATION_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Forward References
class RApplicationGlobals;

// ****************************************************************************
//
//  Class Name:	RApplication
//
//  Description:	The base Renaissance Application class. Component and
//						standalone application classes are derived off of this.
//						User application classes should never inherit directly
//						from this class.
//
//						The application class is responsible for creating and
//						managing documents and components.
//
// ****************************************************************************
//
class FrameworkLinkage RApplication : public RCommandTarget
	{
	// Construction, destruction & initialization
	public :
															RApplication( );
		virtual											~RApplication( );
		virtual BOOLEAN								Initialize( RApplicationGlobals* pGlobals );
		virtual void									Shutdown( );

	// Operations
	public :
		virtual RAction*								CreateScriptAction( const YActionId& actionId, RScript& script );
		virtual void									OnXPrinterChanged( const RMBCString& rPrinterChangeInfo );
		virtual void									OnXPreferencesChanged( const RMBCString& rPreferencesChangedInfo );

	// Acessors
	public :
		static RApplicationGlobals*				m_gsApplicationGlobals;
		static RApplicationGlobals*				GetApplicationGlobals( );

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void									Validate( ) const = 0;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RApplicationBase::GetAppliationGlobals( )
//
//  Description:		Returns the Application Globals
//
//  Returns:			m_gsApplicationGlobals
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RApplicationGlobals* RApplication::GetApplicationGlobals( )
	{
	TpsAssert( m_gsApplicationGlobals != NULL, "The Application Base is NULL." );
	return m_gsApplicationGlobals;
	}


// ****************************************************************************
// 					Useful Macros
// ****************************************************************************

#ifdef	_WINDOWS

#define CreateComponent( ComponentApplicationClass )								\
		ComponentApplicationClass theApp;												\
		extern "C" __declspec( dllexport )												\
		RApplication* InitializeComponent( RApplicationGlobals* pGlobals )	\
			{																						\
			theApp.Initialize( pGlobals );												\
			return &theApp;																	\
			}

#define CreateApplication( ApplicationClass ) ApplicationClass theApp;

#else		// ! _WINDOWS

#define CreateComponent( )		ComponentApplicationClass* p##ComponentApplicationClass##App;
										
#define CreateApplication( )

#endif	// _WINDOWS


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _APPLICATION_H_
