// ****************************************************************************
//
//  File Name:			Settings.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RSettings class
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
//  $Logfile:: /PM8/Framework/Include/Settings.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_SETTINGS_H_
#define	_SETTINGS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif


// ****************************************************************************
//
//  Class Name:	RSettings
//
//  Description:	The class for storing component specific settings information.
//
// ****************************************************************************
//
class RSettings
	{
	public:
		//	Enums
		enum ESettingState
			{
			kNotSet	= 0,
			kSet,
			kMultiState
			};

	//	Constructor and Destructor
	public :
													RSettings( );
		virtual									~RSettings( );

	//	Operations
	public :
		virtual void							MergeSetting( RSettings* ) = 0;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RSettings::RSettings()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSettings::RSettings( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RSettings::~RSettings()
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RSettings::~RSettings( )
	{
	NULL;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_SETTINGS_H_
