// ****************************************************************************
//
//  File Name:			StorageSupport.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Definition and Implementation of the archive support routines
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Aoftware.
//							One Gateway Center, Auite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Aoftware, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Aoftware. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/StorageSupport.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_STORAGESUPPORT_H_
#define		_STORAGESUPPORT_H_

//
//		Function Definitions
//

FrameworkLinkage RArchive& operator<<( RArchive& archive, const R2dTransform& transform );
FrameworkLinkage RArchive& operator>>( RArchive& archive, R2dTransform& transform );

FrameworkLinkage RArchive& operator<<( RArchive& archive, const RColor& color );
FrameworkLinkage RArchive& operator>>( RArchive& archive, RColor& color );

FrameworkLinkage RArchive& operator<<( RArchive& archive, const YFileFont& font );
FrameworkLinkage RArchive& operator>>( RArchive& archive, YFileFont& font );

FrameworkLinkage RArchive& operator<<( RArchive& archive, const RMBCString& string );
FrameworkLinkage RArchive& operator>>( RArchive& archive, RMBCString& string );

// ****************************************************************************
//
// Function Name:		Operator >> and << ( )
//
// Description:		Read and/or Write the given data to/from the given archive
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
	//
	//						Base Types
	//
	//		Unsigned Bytes
inline RArchive& operator<<( RArchive& archive, uBYTE ub )
{
	archive.Write( sizeof( ub ), reinterpret_cast< LPUBYTE >( &ub ) );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, uBYTE& ub )
{
	archive.Read( sizeof( ub ), reinterpret_cast< LPUBYTE >( &ub ) );
	return archive;
}

	//		Signed Bytes
inline RArchive& operator<<( RArchive& archive, sBYTE sb )
{
	archive.Write( sizeof( sb ), reinterpret_cast< LPUBYTE >( &sb ) );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, sBYTE& sb )
{
	archive.Read( sizeof( sb ), reinterpret_cast< LPUBYTE >( &sb ) );
	return archive;
}

	//	Unsigned Word
inline RArchive& operator<<( RArchive& archive, uWORD uw )
{
	MacCode( tintSwapWord( uw ); );
	archive.Write( sizeof( uw ), reinterpret_cast< LPUBYTE >( &uw ) );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, uWORD& uw )
{
	archive.Read( sizeof( uw ), reinterpret_cast< LPUBYTE >( &uw ) );
	MacCode( tintSwapWord( uw ); );
	return archive;
}

	//	Signed Word
inline RArchive& operator<<( RArchive& archive, sWORD sw )
{
	MacCode( tintSwapWord( sw ); );
	archive.Write( sizeof( sw ), reinterpret_cast< LPUBYTE >( &sw ) );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, sWORD& sw )
{
	archive.Read( sizeof( sw ), reinterpret_cast< LPUBYTE >( &sw ) );
	MacCode( tintSwapWord( sw ); );
	return archive;
}

	//	Unsigned Long
inline RArchive& operator<<( RArchive& archive, uLONG ul )
{
	MacCode( tintSwapLong( ul ); );
	archive.Write( sizeof( ul ), reinterpret_cast< LPUBYTE >( &ul ) );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, uLONG& ul )
{
	archive.Read( sizeof( ul ), reinterpret_cast< LPUBYTE >( &ul ) );
	MacCode( tintSwapLong( ul ); );
	return archive;
}

	//	Signed Long
inline RArchive& operator<<( RArchive& archive, sLONG sl )
{
	MacCode( tintSwapLong( sl ); );
	archive.Write( sizeof( sl ), reinterpret_cast< LPUBYTE >( &sl ) );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, sLONG& sl )
{
	archive.Read( sizeof( sl ), reinterpret_cast< LPUBYTE >( &sl ) );
	MacCode( tintSwapLong( sl ); );
	return archive;
}

	//	float
inline RArchive& operator<<( RArchive& archive, float f )
{
	MacCode( tintSwapFloat( f ); );
	archive.Write( sizeof( f ), reinterpret_cast< LPUBYTE >( &f ) );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, float& f )
{
	archive.Read( sizeof( f ), reinterpret_cast< LPUBYTE >( &f ) );
	MacCode( tintSwapFloat( f ); );
	return archive;
}

	//	double
inline RArchive& operator<<( RArchive& archive, double d )
{
	MacCode( tintSwapDouble( d ); );
	archive.Write( sizeof( d ), reinterpret_cast< LPUBYTE >( &d ) );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, double& d )
{
	archive.Read( sizeof( d ), reinterpret_cast< LPUBYTE >( &d ) );
	MacCode( tintSwapDouble( d ); );
	return archive;
}


	//	unsigned int
inline RArchive& operator<<( RArchive& archive, unsigned int )
{
	TpsAssertAlways( "Cannot write unsigned ints into a stream." );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, unsigned int& )
{
	TpsAssertAlways( "Cannot read unsigned ints from a stream." );
	return archive;
}

	//	int
inline RArchive& operator<<( RArchive& archive, int )
{
	TpsAssertAlways( "Cannot write ints into a stream." );
	return archive;
}
inline RArchive& operator>>( RArchive& archive, int& )
{
	TpsAssertAlways( "Cannot read ints from a stream." );
	return archive;
}


	//
	//						Renaissance Types
	//
	//		RSize< T >
template <class T> inline RArchive& operator<<( RArchive& archive, const RSize< T >& size )
{
	archive << size.m_dx << size.m_dy;
	return archive;
}
template <class T> inline RArchive& operator>>( RArchive& archive, RSize< T >& size )
{
	archive >> size.m_dx >> size.m_dy;
	return archive;
}

	//		RPoint< T >
template <class T> inline RArchive& operator<<( RArchive& archive, const RPoint< T >& point )
{
	archive << point.m_x << point.m_y;
	return archive;
}
template <class T> inline RArchive& operator>>( RArchive& archive, RPoint< T >& point )
{
	archive >> point.m_x >> point.m_y;
	return archive;
}

	//		RRect< T >
template <class T> inline RArchive& operator<<( RArchive& archive, const RRect< T >& rect )
{
	archive << rect.m_Left << rect.m_Top << rect.m_Right << rect.m_Bottom;
	return archive;
}
template <class T> inline RArchive& operator>>( RArchive& archive, RRect< T >& rect )
{
	archive >> rect.m_Left >> rect.m_Top >> rect.m_Right >> rect.m_Bottom;
	return archive;
}

#ifdef _WINDOWS
	//		RVectorRect< T >
inline RArchive& operator<<( RArchive& archive, const RVectorRect< YFloatType >& rect )
{
	archive << rect.m_UntransformedSize;
	archive << rect.m_Transform;
	return archive;
}

inline RArchive& operator>>( RArchive& archive, RVectorRect< YFloatType >& rect )
{
	archive >> rect.m_UntransformedSize;
	archive >> rect.m_Transform;
	rect.Recalculate( );
	return archive;
}
#endif	//	_WINDOWS

#endif	//	_STORAGESUPPORT_H_

