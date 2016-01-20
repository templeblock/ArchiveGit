// ****************************************************************************
//
//  File Name:			PathInterfaceImp.h
//
//  Project:			Path Component
//
//  Author:				Lance Wilson
//
//  Description:		Declaration of the RPathInterfacesImp class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software.
//							500 Redwood Blvd
//							Novato, CA 94948
//							(415) 482-4400
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/PathComp/Include/PathInterfaceImp.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PATHINTERFACEIMP_H_
#define		_PATHINTERFACEIMP_H_

#include "PathInterface.h"
#include "PropertyInterface.h"

class RPathDocument;
class RPathView;

// ****************************************************************************
//
//  Class Name:	RPathHolderImp
//
//  Description:	Implementation of RPathHolderImp
//
// ****************************************************************************
//
class RPathHolderImp : public RPathHolder
{
//	Construction and Destruction
public :
												RPathHolderImp( );
	virtual									~RPathHolderImp( );

//	Member data
private :

	friend class RPathInterfacesImp;

#ifdef	TPSDEBUG
// Debugging stuff
public :
	virtual void							Validate( ) const;
#endif
} ;

// ****************************************************************************
//
//  Class Name:	RPathInterfaceImp
//
//  Description:	Implementation of IPathInterface
//
// ****************************************************************************
//
class RPathInterfaceImp : public IPathInterface
{
// Constructor 
public:
											RPathInterfaceImp( const RPathView* pView );
	static	RInterface*				CreateInterface( const RComponentView* pView );

// Interfaces
public:

	// RComponentDataInterface
	//
	virtual RComponentDataHolder*	GetData( );
	virtual void						EditData( );
	virtual void						SetData( RComponentDataHolder* );
	virtual void						ClearData( );
	virtual void						Read( RChunkStorage& );
	virtual void						Write( RChunkStorage& );
	virtual void						Render( RDrawingSurface& , const R2dTransform&, const RIntRect&, const RIntRect& );

	// IPathInterface
	//
	virtual RPath*						GetPath( );
	virtual void						SetPath( const RPath& rPath, BOOLEAN fResize = TRUE );

private:

	RPathDocument*						m_pPathDocument;
};

// ****************************************************************************
//
//  Class Name:	RPathObjectPropertiesImp
//
//  Description:	Definition of the RPathObjectPropertiesImp class.
//
// ****************************************************************************
//
class RPathObjectPropertiesImp : public IObjectProperties
{
//	Construction & Destruction
public :
									RPathObjectPropertiesImp( const RComponentView* pView );
	static RInterface*		CreateInterface( const RComponentView* pView );

public:

	virtual BOOLEAN			FindProperty( YInterfaceId iid, RInterface** ppInterface );
//	virtual void				EnumProperties(  );

private:

	RPathDocument*				m_pPathDocument;
};

#endif // _PATHINTERFACEIMP_H_
