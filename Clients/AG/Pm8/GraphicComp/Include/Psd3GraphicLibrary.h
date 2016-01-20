// ****************************************************************************
//
//  File Name:			Psd3GraphicLibrary.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//							G. Brown
//
//  Description:		Declaration of the RPsd3GraphicLibrary class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Include/Psd3GraphicLibrary.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PSD3GRAPHICLIBRARY_H_
#define		_PSD3GRAPHICLIBRARY_H_

#ifndef	_PSD3GRAPHIC_H_
#include	"Psd3Graphic.h"
#endif

#ifndef	_PSD3GRAPHICFILEFORMAT_H_
#include "Psd3GraphicFileFormat.h"
#endif

#include "GraphicInterface.h"

class RStream;
struct RPsd3GraphicInfo;

class RPsd3GraphicLibrary : public RObject
{
// Supported PSD3 library types
public:
	enum ELibraryType
	{
		kBackdropLibrary,
		kBorderLibrary,
		kCertificateBorderLibrary,
		kSquareGraphicLibrary,
		kColumnGraphicLibrary,
		kRowGraphicLibrary,
		kRuledLineLibrary,
		kSealExteriorLibrary,
		kSealInteriorLibrary,
		kTimepieceLibrary,
		kSignatureLibrary,
		kSpeechBubbleLibrary,
		kUnknownLibrary
	};

// Construction, destruction & Initialization
public :
	RPsd3GraphicLibrary( );
	virtual ~RPsd3GraphicLibrary( );

// Operations
public :
	BOOLEAN									OpenLibrary( RStream& stream );
	RPsd3Graphic*							GetGraphic( RStream& stream, const char* pszGraphicName, BOOLEAN fKeepRawData = FALSE ) const;
	RPsd3Graphic*							GetGraphic( RStream& stream, YPsd3GraphicID graphicID, BOOLEAN fKeepRawData = FALSE ) const;		
	uBYTE*									GetRawData( RStream& stream, const char* pszGraphicName, uLONG& ulRawDataLength ) const;
	uBYTE*									GetRawData( RStream& stream, YPsd3GraphicID graphicID, uLONG& ulRawDataLength ) const;
	char**									GetGraphicNames( uWORD& numGraphics ) const;
	YPsd3GraphicID*						GetGraphicIDs(uWORD& numGraphics) const;

// Implementation
private :
	ELibraryType							GetLibraryType( ) const;		
	RPsd3Graphic*							GetGraphicByIndex( RStream& stream, uWORD uwGraphicIndex, BOOLEAN fKeepRawData ) const;
	uBYTE*									GetRawDataByIndex( RStream& stream, uWORD uwGraphicIndex, uLONG& ulRawDataLength ) const;
	RGraphicInterface::EGraphicType	GetGraphicType( ELibraryType libraryType, YPsd3Subproject subProject ) const;

// Member data
private :
	RPsd3GraphicLibraryHeader			m_LibraryHeader;
	RPsd3GraphicInfo*						m_GraphicInfoArray;	
	char**									m_pGraphicNames;
	YPsd3GraphicID*						m_pGraphicIDs;
	//uWORD									m_uwNumValidGraphics;
	YStreamPosition						m_LibraryStartPosition;

#ifdef	TPSDEBUG
public:
	// Debugging support
	void Validate( ) const;
#endif	// TPSDEBUG
};

#endif // _PSD3GRAPHICLIBRARY_H_
