// ****************************************************************************
//
//  File Name:			ComponentManager.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RComponentManager class
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
//  $Logfile:: /PM8/Framework/Include/ComponentManager.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_COMPONENTMANAGER_H_
#define	_COMPONENTMANAGER_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentApplication;
class RComponentDocument;
class RDocument;
class RComponentAttributes;

// ****************************************************************************
//
//  Class Name:		RComponentServerInfo
//
//  Description:		A class containing useful information about a component
//							server.
//
// ****************************************************************************
//
class RComponentServerInfo
	{
	// Member data
	public :
		RComponentApplication*			m_pApplication;

#ifdef	_WINDOWS
		HINSTANCE							m_hInstance;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RComponentManager
//
//  Description:	Class to manage Renaissance components.
//
// ****************************************************************************
//
class FrameworkLinkage RComponentManager : public RArray<RComponentServerInfo>
	{
	// Construction
	public :
												RComponentManager( );

	// Operations
	public :
		RComponentDocument*				CreateNewComponent( const YComponentType& componentType,
																		  RDocument* pParentDocument,
																		  const RComponentAttributes& componentAttributes,
																		  BOOLEAN fLoading );

		RComponentDocument*				CreateNewComponent( const YComponentType& componentType,
																		  RDocument* pParentDocument,
																		  RView* pParentView,
																		  const RComponentAttributes& componentAttributes,
																		  const YComponentBoundingRect& boundingRect,
																		  BOOLEAN fLoading );

		BOOLEAN								IsComponentAvailable( const YComponentType& componentType ) const;
		BOOLEAN								MapDataFormatToComponentType( YDataFormat dataFormat, YComponentType& componentType ) const;
		const YComponentType				GetNonstandardComponentType( YComponentIndex componentIndex ) const;
		void									LoadComponentServers( const RMBCString& componentPath, YMenuIndex componentSubMenuId );
		void									UnloadComponentServers( );

	// Members
	private :
		BOOLEAN								m_fLoadComponentsCalled;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _COMPONENTMANAGER_H_
