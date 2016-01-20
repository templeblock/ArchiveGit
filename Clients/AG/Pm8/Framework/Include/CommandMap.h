// ****************************************************************************
//
//  File Name:			CommandMap.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the command mapper classes
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
//  $Logfile:: /PM8/Framework/Include/CommandMap.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMMANDMAP_H_
#define		_COMMANDMAP_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RCommandUI;
class RCommandTarget;

// ****************************************************************************
//
//  Class Name:	RCommandMapBase
//
//  Description:	Non-template base class for command maps
//
// ****************************************************************************
//
class RCommandMapBase
	{
	// Operations
	public :
		virtual BOOLEAN					OnCommand( YCommandID commandId, RCommandTarget* pCommandTarget, BOOLEAN fDoCommand ) const = 0;
		virtual BOOLEAN					OnCommandUI( RCommandUI& commandUI, const RCommandTarget* pCommmandTarget ) const = 0;
	};

// ****************************************************************************
//
//  Class Name:	RCommandHandler
//
//  Description:	Abstract base class for Renaissance command handlers
//
// ****************************************************************************
//
template <class T, class B> class RCommandHandler
	{
	// Operations
	public :
		virtual BOOLEAN					HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const = 0;
	};

// ****************************************************************************
//
//  Class Name:	RCommandUpdater
//
//  Description:	Abstract base class for Renaissance command updaters
//
// ****************************************************************************
//
template <class T, class B> class RCommandUpdater
	{
	// Operations
	public :
		virtual BOOLEAN					UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const = 0;
	};

// ****************************************************************************
//
//  Class Name:	RCommandMap
//
//  Description:	Renaissance command map. Encapsulates an array of
//						RCommandHandlers
//
// ****************************************************************************
//
template <class T, class B> class RCommandMap : public RCommandMapBase
	{
	// Pointer to member typedefs to make things easier to read
	public :																											
#ifdef _WINDOWS // REVIEW GCB 12/18/96 - not sure why this doesn't compile on the Mac
		typedef void ( T::* YHandlerFunction )( );
		typedef void ( T::* YRangeHandlerFunction ) ( YCommandID );
		typedef void ( T::* YUpdaterFunction ) ( RCommandUI& ) const;
		typedef void ( T::* YRangeUpdaterFunction ) ( RCommandUI& ) const;
		typedef BOOLEAN ( T::* YHandleAlwaysFunction ) ( YCommandID );
		typedef BOOLEAN ( T::* YUpdateAlwaysFunction ) ( RCommandUI& ) const;
		typedef RCommandTarget* T::* YCommandTargetPointer;
		typedef RCommandTarget* ( T::* YCommandTargetFunction )( ) const;
#elif defined MAC
		typedef void ( *YHandlerFunction )( );
		typedef void ( *YRangeHandlerFunction ) ( YCommandID );
		typedef void ( *YUpdaterFunction ) ( RCommandUI& );
		typedef void ( *YRangeUpdaterFunction ) ( RCommandUI& );
		typedef BOOLEAN ( *YHandleAlwaysFunction ) ( YCommandID );
		typedef BOOLEAN ( *YUpdateAlwaysFunction ) ( RCommandUI& );
		typedef RCommandTarget* YCommandTargetPointer;
		typedef RCommandTarget* ( *YCommandTargetFunction )( );
#endif

	// Typedefs for the command maps
	private :
		typedef RArray<RCommandHandler<T,B>*>	YHandlerCollection;
		typedef YHandlerCollection::YIterator	YHandlerIterator;
		typedef RArray<RCommandUpdater<T,B>*>	YUpdaterCollection;
		typedef YUpdaterCollection::YIterator	YUpdaterIterator;

	// Construction & Destruction
	public :
												RCommandMap( );
												~RCommandMap( );

	// Operations
	public :
		virtual BOOLEAN					OnCommand( YCommandID commandId, RCommandTarget* pCommandTarget, BOOLEAN fDoCommand ) const;
		virtual BOOLEAN					OnCommandUI( RCommandUI& commandUI, const RCommandTarget* pCommmandTarget ) const;

		BOOLEAN								OnCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const;
		BOOLEAN								OnCommandUI( RCommandUI& commandUI, const T* pCommmandTarget ) const;

	// Functions to build the maps
	private  :
		void									HandleCommand( YCommandID commandId, YHandlerFunction pmfHandlerFunction );
		void									HandleCommandRange( YCommandID startId, YCommandID endId, YRangeHandlerFunction pmfRangeHandlerFunction );
		void									UpdateCommand( YCommandID commandId, YUpdaterFunction pmfUpdaterFunction );
		void									UpdateCommandRange( YCommandID startId, YCommandID endId, YRangeUpdaterFunction pmfRangeUpdaterFunction );
		void									HandleAndUpdateCommand( YCommandID commandID, YHandlerFunction pmfHandlerFunction, YUpdaterFunction pmfUpdaterFunction );
		void									HandleAndUpdateCommandRange( YCommandID startId, YCommandID endId, YRangeHandlerFunction pmfHandlerFunction, YRangeUpdaterFunction pmfUpdaterFunction );
		void									RouteCommandToObject( YCommandTargetPointer pmCommandTarget );
		void									RouteCommandToFunction( YCommandTargetFunction pmfCommandTargetFunction );

	// Members
	private :
		YHandlerCollection				m_HandlerCollection;
		YUpdaterCollection				m_UpdaterCollection;
	};

// Some defines to make the code easier to read
#define YHandlerFunction RCommandMap<T,B>::YHandlerFunction
#define YRangeHandlerFunction RCommandMap<T,B>::YRangeHandlerFunction
#define YUpdaterFunction RCommandMap<T,B>::YUpdaterFunction
#define YRangeUpdaterFunction RCommandMap<T,B>::YRangeUpdaterFunction
#define YHandleAlwaysFunction RCommandMap<T,B>::YHandleAlwaysFunction
#define YUpdateAlwaysFunction RCommandMap<T,B>::YUpdateAlwaysFunction
#define YCommandTargetPointer RCommandMap<T,B>::YCommandTargetPointer
#define YCommandTargetFunction RCommandMap<T,B>::YCommandTargetFunction

// ****************************************************************************
//
//  Class Name:	RHandleCommand
//
//  Description:	Command handler for a single command
//
// ****************************************************************************
//
template <class T, class B> class RHandleCommand : public RCommandHandler<T,B>
	{
	// Construction
	public :
												RHandleCommand( YCommandID commandId, YHandlerFunction pmfCommandHandler );

	// Operations
	public :
		virtual BOOLEAN					HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const;

	// Members
	private :
		YCommandID							m_CommandId;
		YHandlerFunction					m_pmfCommandHandler;
	};

// ****************************************************************************
//
//  Class Name:	RHandleCommandRange
//
//  Description:	Command handler for a range of commands
//
// ****************************************************************************
//
template <class T, class B> class RHandleCommandRange : public RCommandHandler<T,B>
	{
	// Construction
	public :
												RHandleCommandRange( YCommandID startId, YCommandID endId, YRangeHandlerFunction pmfCommandHandler );

	// Operations
	public :
		virtual BOOLEAN					HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const;

	// Members
	private :
		YCommandID							m_StartId;
		YCommandID							m_EndId;
		YRangeHandlerFunction			m_pmfCommandHandler;
	};

// ****************************************************************************
//
//  Class Name:	RUpdateCommand
//
//  Description:	Update handler for a single command
//
// ****************************************************************************
//
template <class T, class B> class RUpdateCommand : public RCommandUpdater<T,B>
	{
	// Construction
	public :
												RUpdateCommand( YCommandID commandId, YUpdaterFunction pmfCommandUpdater );

	// Operations
	public :
		virtual BOOLEAN					UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const;

	// Members
	private :
		YCommandID							m_CommandId;
		YUpdaterFunction					m_pmfCommandUpdater;
	};

// ****************************************************************************
//
//  Class Name:	RUpdateCommandRange
//
//  Description:	Update handler for a range of commands
//
// ****************************************************************************
//
template <class T, class B> class RUpdateCommandRange : public RCommandUpdater<T,B>
	{
	// Construction
	public :
												RUpdateCommandRange( YCommandID startId, YCommandID endId, YRangeUpdaterFunction pmfUpdateHandler );

	// Operations
	public :
		virtual BOOLEAN					UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const;

	// Members
	private :
		YCommandID							m_StartId;
		YCommandID							m_EndId;
		YRangeUpdaterFunction			m_pmfUpdateHandler;
	};

// ****************************************************************************
//
//  Class Name:	RRoutePointerHandler
//
//  Description:	Handler which searches the command map of another command
//						handler
//
// ****************************************************************************
//
template <class T, class B> class RRoutePointerHandler : public RCommandHandler<T,B>
	{
	// Construction
	public :
												RRoutePointerHandler( YCommandTargetPointer pmCommandTarget );

	// Operations
	public :
		virtual BOOLEAN					HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const;

	// Members
	private :
		YCommandTargetPointer			m_pmCommandTarget;
	};

// ****************************************************************************
//
//  Class Name:	RRoutePointerUpdater
//
//  Description:	Updater which searches the command map of another command
//						handler
//
// ****************************************************************************
//
template <class T, class B> class RRoutePointerUpdater : public RCommandUpdater<T,B>
	{
	// Construction
	public :
												RRoutePointerUpdater( YCommandTargetPointer pmCommandTarget );

	// Operations
	public :
		virtual BOOLEAN					UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const;

	// Members
	private :
		YCommandTargetPointer			m_pmCommandTarget;
	};

// ****************************************************************************
//
//  Class Name:	RRouteFunctionHandler
//
//  Description:	Handler which searches the command map of another command
//						handler
//
// ****************************************************************************
//
template <class T, class B> class RRouteFunctionHandler : public RCommandHandler<T,B>
	{
	// Construction
	public :
												RRouteFunctionHandler( YCommandTargetFunction pmfCommandTargetFunction );

	// Operations
	public :
		virtual BOOLEAN					HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const;

	// Members
	private :
		YCommandTargetFunction			m_pmfCommandTargetFunction;
	};

// ****************************************************************************
//
//  Class Name:	RRouteFunctionUpdater
//
//  Description:	Updater which searches the command map of another command
//						handler
//
// ****************************************************************************
//
template <class T, class B> class RRouteFunctionUpdater : public RCommandUpdater<T,B>
	{
	// Construction
	public :
												RRouteFunctionUpdater( YCommandTargetFunction pmfCommandTargetFunction );

	// Operations
	public :
		virtual BOOLEAN					UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const;

	// Members
	private :
		YCommandTargetFunction			m_pmfCommandTargetFunction;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RCommandMap::~RCommandMap( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RCommandMap<T,B>::~RCommandMap( )
	{
	// Delete the command map entries
	YHandlerIterator handlerIterator = m_HandlerCollection.Start( );
	YHandlerIterator handerEnd = m_HandlerCollection.End( );
	for( ; handlerIterator != handerEnd; ++handlerIterator )
		delete *handlerIterator;
	
	YUpdaterIterator updaterIterator = m_UpdaterCollection.Start( );
	YUpdaterIterator updaterEnd = m_UpdaterCollection.End( );
	for( ; updaterIterator != updaterEnd; ++updaterIterator )
		delete *updaterIterator;
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::HandleCommand( )
//
//  Description:		Adds a handler for a single command to the map 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> void RCommandMap<T,B>::HandleCommand( YCommandID commandId, YHandlerFunction pmfHandlerFunction )
	{
	m_HandlerCollection.InsertAtEnd( new RHandleCommand<T,B>( commandId, pmfHandlerFunction ) );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::HandleCommandRange( )
//
//  Description:		Adds a handler for a command range to the map 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> void RCommandMap<T,B>::HandleCommandRange( YCommandID startId, YCommandID endId, YRangeHandlerFunction pmfHandlerFunction )
	{
	m_HandlerCollection.InsertAtEnd( new RHandleCommandRange<T,B>( startId, endId, pmfHandlerFunction ) );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::UpdateCommand( )
//
//  Description:		Adds an updater for a single command to the map 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> void RCommandMap<T,B>::UpdateCommand( YCommandID commandId, YUpdaterFunction pmfUpdaterFunction )
	{
	m_UpdaterCollection.InsertAtEnd( new RUpdateCommand<T,B>( commandId, pmfUpdaterFunction ) );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::UpdateCommandRange( )
//
//  Description:		Adds an updater for a command range to the map 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> void RCommandMap<T,B>::UpdateCommandRange( YCommandID startId, YCommandID endId, YRangeUpdaterFunction pmfUpdaterFunction )
	{
	m_UpdaterCollection.InsertAtEnd( new RUpdateCommandRange<T,B>( startId, endId, pmfUpdaterFunction ) );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::HandleAndUpdateCommand( )
//
//  Description:		Adds a hander and an updater for a single command to the map 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> void RCommandMap<T,B>::HandleAndUpdateCommand( YCommandID commandId, YHandlerFunction pmfHandlerFunction, YUpdaterFunction pmfUpdaterFunction )
	{
	m_HandlerCollection.InsertAtEnd( new RHandleCommand<T,B>( commandId, pmfHandlerFunction ) );
	m_UpdaterCollection.InsertAtEnd( new RUpdateCommand<T,B>( commandId, pmfUpdaterFunction ) );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::HandleAndUpdateCommandRange( )
//
//  Description:		Adds a hander and an updater for a range of commands to the map 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> void RCommandMap<T,B>::HandleAndUpdateCommandRange( YCommandID startId, YCommandID endId, YRangeHandlerFunction pmfHandlerFunction, YRangeUpdaterFunction pmfUpdaterFunction )
	{
	m_HandlerCollection.InsertAtEnd( new RHandleCommandRange<T,B>( startId, endId, pmfHandlerFunction ) );
	m_UpdaterCollection.InsertAtEnd( new RUpdateCommandRange<T,B>( startId, endId, pmfUpdaterFunction ) );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::RouteCommand( )
//
//  Description:		Adds a handler that routes to another command targets map
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> void RCommandMap<T,B>::RouteCommandToObject( YCommandTargetPointer pmCommandTarget )
	{
	m_HandlerCollection.InsertAtEnd( new RRoutePointerHandler<T,B>( pmCommandTarget ) );
	m_UpdaterCollection.InsertAtEnd( new RRoutePointerUpdater<T,B>( pmCommandTarget ) );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::RouteCommand( )
//
//  Description:		Adds a handler that routes to another command targets map
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> void RCommandMap<T,B>::RouteCommandToFunction( YCommandTargetFunction pmfCommandTargetFunction )
	{
	m_HandlerCollection.InsertAtEnd( new RRouteFunctionHandler<T,B>( pmfCommandTargetFunction ) );
	m_UpdaterCollection.InsertAtEnd( new RRouteFunctionUpdater<T,B>( pmfCommandTargetFunction ) );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::OnCommand( )
//
//  Description:		Handles a command 
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RCommandMap<T,B>::OnCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const
	{
	// Search through the command handlers, looking for one that can handle this command
	YHandlerIterator handlerIterator = m_HandlerCollection.Start( );
	for( ; handlerIterator != m_HandlerCollection.End( ); ++handlerIterator )
		if( ( *handlerIterator )->HandleCommand( commandId, pCommandTarget, fDoCommand ) )
			return TRUE;

	// Try the base class
	TpsAssertIsObject( B, pCommandTarget );

	if( pCommandTarget->B::GetRCommandMap( ) )
		return pCommandTarget->B::GetRCommandMap( )->OnCommand( commandId, static_cast<B*>( pCommandTarget ), fDoCommand );

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::OnCommandUI( )
//
//  Description:		Handles a command UI
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RCommandMap<T,B>::OnCommandUI( RCommandUI& commandUI, const T* pCommandTarget ) const
	{
	// Search through the command updaters, looking for one that can update this command
	YUpdaterIterator updaterIterator = m_UpdaterCollection.Start( );
	for( ; updaterIterator != m_UpdaterCollection.End( ); ++updaterIterator )
		if( ( *updaterIterator )->UpdateCommand( commandUI, pCommandTarget ) )
			return TRUE;

	// Try the base class
	TpsAssertIsObject( B, pCommandTarget );

	if( pCommandTarget->B::GetRCommandMap( ) )
		return pCommandTarget->B::GetRCommandMap( )->OnCommandUI( commandUI, static_cast<const B*>( pCommandTarget ) );

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::OnCommand( )
//
//  Description:		Handles a command 
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RCommandMap<T,B>::OnCommand( YCommandID commandId, RCommandTarget* pCommandTarget, BOOLEAN fDoCommand ) const
	{
	TpsAssertIsObject( T, pCommandTarget );
	return OnCommand( commandId, static_cast<T*>( pCommandTarget ), fDoCommand );
	}

// ****************************************************************************
//
//  Function Name:	RCommandMap::OnCommandUI( )
//
//  Description:		Handles a command UI
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RCommandMap<T,B>::OnCommandUI( RCommandUI& commandUI, const RCommandTarget* pCommandTarget ) const
	{
	TpsAssertIsObject( T, pCommandTarget );
	return OnCommandUI( commandUI, static_cast<const T*>( pCommandTarget ) );
	}

// ****************************************************************************
//
//  Function Name:	RHandleCommand::RHandleCommand( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RHandleCommand<T,B>::RHandleCommand( YCommandID commandId, YHandlerFunction pmfCommandHandler )
	: m_CommandId( commandId ),
	  m_pmfCommandHandler( pmfCommandHandler )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RHandleCommand::HandleCommand( )
//
//  Description:		Handles the command
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RHandleCommand<T,B>::HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const
	{
	if( commandId == m_CommandId )
		{
		if( fDoCommand )
			( pCommandTarget->*m_pmfCommandHandler )( );

		return TRUE;
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RHandleCommandRange::RHandleCommandRange( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RHandleCommandRange<T,B>::RHandleCommandRange( YCommandID startId, YCommandID endId, YRangeHandlerFunction pmfCommandHandler )
	: m_StartId( startId ),
	  m_EndId( endId ),
	  m_pmfCommandHandler( pmfCommandHandler )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RHandleCommandRange::HandleCommand( )
//
//  Description:		Handles the command
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RHandleCommandRange<T,B>::HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const
	{
	if( commandId >= m_StartId && commandId <= m_EndId )
		{
		if( fDoCommand )
			( pCommandTarget->*m_pmfCommandHandler )( commandId );

		return TRUE;
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RUpdateCommand::RUpdateCommand( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RUpdateCommand<T,B>::RUpdateCommand( YCommandID commandId, YUpdaterFunction pmfCommandUpdater )
	: m_CommandId( commandId ),
	  m_pmfCommandUpdater( pmfCommandUpdater )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RUpdateCommand::UpdateCommand( )
//
//  Description:		Handles the command
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RUpdateCommand<T,B>::UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const
	{
	if( commandUI.GetCommandID( ) == m_CommandId )
		{
		( pCommandTarget->*m_pmfCommandUpdater )( commandUI );
		return TRUE;
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RUpdateCommandRange::RUpdateCommandRange( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RUpdateCommandRange<T,B>::RUpdateCommandRange( YCommandID startId, YCommandID endId, YRangeUpdaterFunction pmfUpdateHandler )
	: m_StartId( startId ),
	  m_EndId( endId ),
	  m_pmfUpdateHandler( pmfUpdateHandler )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RUpdateCommandRange::UpdateCommand( )
//
//  Description:		Updates the command
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RUpdateCommandRange<T,B>::UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const
	{
	if( commandUI.GetCommandID( ) >= m_StartId && commandUI.GetCommandID( ) <= m_EndId )
		{
		( pCommandTarget->*m_pmfUpdateHandler )( commandUI );
		return TRUE;
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RRoutePointerHandler::RRoutePointerHandler( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RRoutePointerHandler<T,B>::RRoutePointerHandler( YCommandTargetPointer pmCommandTarget )
	: m_pmCommandTarget( pmCommandTarget )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRoutePointerHandler::HandleCommand( )
//
//  Description:		Handles the command
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RRoutePointerHandler<T,B>::HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const
	{
	RCommandTarget* pCommandTargetToRouteTo = pCommandTarget->*m_pmCommandTarget;
	if( pCommandTargetToRouteTo && pCommandTargetToRouteTo->GetRCommandMap( ) )	
		return pCommandTargetToRouteTo->GetRCommandMap( )->OnCommand( commandId, pCommandTargetToRouteTo, fDoCommand );

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RRoutePointerUpdater::RRoutePointerUpdater( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RRoutePointerUpdater<T,B>::RRoutePointerUpdater( YCommandTargetPointer pmCommandTarget )
	: m_pmCommandTarget( pmCommandTarget )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRoutePointerUpdater::UpdateCommand( )
//
//  Description:		Handles the command
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RRoutePointerUpdater<T,B>::UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const
	{
	RCommandTarget* pCommandTargetToRouteTo = pCommandTarget->*m_pmCommandTarget;
	if( pCommandTargetToRouteTo && pCommandTargetToRouteTo->GetRCommandMap( ) )
		return ( pCommandTarget->*m_pmCommandTarget )->GetRCommandMap( )->OnCommandUI( commandUI, pCommandTarget->*m_pmCommandTarget );
	
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RRouteFunctionHandler::RRouteFunctionHandler( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RRouteFunctionHandler<T,B>::RRouteFunctionHandler( YCommandTargetFunction pmfCommandTargetFunction )
	: m_pmfCommandTargetFunction( pmfCommandTargetFunction )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRouteFunctionHandler::HandleCommand( )
//
//  Description:		Handles the command
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RRouteFunctionHandler<T,B>::HandleCommand( YCommandID commandId, T* pCommandTarget, BOOLEAN fDoCommand ) const
	{
	RCommandTarget* pCommandTargetToRouteTo = ( pCommandTarget->*m_pmfCommandTargetFunction )( );
	if( pCommandTargetToRouteTo && pCommandTargetToRouteTo->GetRCommandMap( ) )	
		return pCommandTargetToRouteTo->GetRCommandMap( )->OnCommand( commandId, pCommandTargetToRouteTo, fDoCommand );

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RRouteFunctionUpdater::RRouteFunctionUpdater( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> RRouteFunctionUpdater<T,B>::RRouteFunctionUpdater( YCommandTargetFunction pmfCommandTargetFunction )
	: m_pmfCommandTargetFunction( pmfCommandTargetFunction )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRouteFunctionUpdater::UpdateCommand( )
//
//  Description:		Handles the command
//
//  Returns:			TRUE if the command is handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class B> BOOLEAN RRouteFunctionUpdater<T,B>::UpdateCommand( RCommandUI& commandUI, const T* pCommandTarget ) const
	{
	RCommandTarget* pCommandTargetToRouteTo = ( pCommandTarget->*m_pmfCommandTargetFunction )( );
	if( pCommandTargetToRouteTo && pCommandTargetToRouteTo->GetRCommandMap( ) )	
		return pCommandTargetToRouteTo->GetRCommandMap( )->OnCommandUI( commandUI, pCommandTargetToRouteTo );

	return FALSE;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _COMMANDMAP_H_
