// ****************************************************************************
//
//  File Name:			Psd3GraphicFileFormat.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Various types and constants for Psd 3 graphics
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
//  $Logfile:: /PM8/GraphicComp/Include/Psd3GraphicFileFormat.h               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_PSD3GRAPHICFILEFORMAT_H_
#define	_PSD3GRAPHICFILEFORMAT_H_

// Graphic library header. There is one of these at the start of all
// graphic libraries

struct RPsd3GraphicLibraryHeader
	{
	char								m_FileIdentity[ 16 ];
	char								m_LibraryName[ 32 ];
	short								m_FileID;
	short								m_Machine;
	char								m_LibraryType;
	char								m_SubType;
	short								m_Count;
	short								m_ClassSize;
	short								m_LayoutNumber;
	char								m_Driver[ 8 ];
	char								m_BroderbundCreated;
	char								m_Unused;
	char								m_Reserved[ 58 ];
	};

// Graphic info struct. An array of these, one for each graphic, follows
// the graphic library header

typedef char YPsd3Subproject;
typedef long YPsd3GraphicID;
const int kGraphicNameLength = 32;

struct RPsd3GraphicInfo
	{
	char								m_Name[ kGraphicNameLength ];
	YPsd3GraphicID					m_GraphicID;
	long								m_GraphicSize;
	long								m_BitmapSize;
	char								m_GraphicCompressionScheme;
	char								m_BitmapCompressionScheme;
	char								m_Mono;
	char								m_Screen;
	YPsd3Subproject				m_SubprojectType;
	char								m_Unused;
	short								m_Code;
	};

const YPsd3Subproject kUnknown = -1;
const YPsd3Subproject kPortrait = 1;
const YPsd3Subproject kLandscape = 2;
const YPsd3Subproject kHorizontalBanner = 3;
const YPsd3Subproject kVerticalBanner = 4;
const YPsd3Subproject kSidespread = 5;
const YPsd3Subproject kTopspread = 6;
const YPsd3Subproject kTile = 7;
const YPsd3Subproject kBusiness = 1;
const YPsd3Subproject kPrintShopEnvelope = 2;
const YPsd3Subproject kInitialCap = 10;
const YPsd3Subproject kNumber = 11;
const YPsd3Subproject kTimepiece = 12;
// REVIEW STA: This define was in panama.h It kinda doesnt fit in a char
//const YPsd3Subproject kCatalog = 15301;
const YPsd3Subproject kNamelist1 = 0;
const YPsd3Subproject kNamelist2 = 1;

// PSD 3 Graphic header structure definition
struct RPsd3GraphicHeader
	{
	long		m_Size;
	long		m_UnpackedSize;
	char		m_Compression;
	char		m_Unused;
	short		m_xSize;
	short		m_ySize;
	short		m_ElementCount;
	long		m_PackedTableSize;
	long		m_ClassSize;
	};

struct RPsd3MultiPartGraphicHeader
{
	uWORD m_PartsList;
	uWORD m_Draw;
	uWORD m_Flip;
	uWORD m_Stretch;
	uWORD m_Unused[24];
};

#endif	// _PSD3GRAPHICFILEFORMAT_H_
