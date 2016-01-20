//****************************************************************************
//
// File Name:		GrcEnums.h
//
// Project:			Renaissance Application Component
//
// Author:			Michael Kelson
//
// Description:	Enums used in GRC Browser
//
// Portability:	Win32
//
// Developed by:	Broderbund Software
//						500 Redwood Blvd
//						Novato, CA 94948
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//****************************************************************************

#ifndef _GRCENUMS_H_
#define _GRCENUMS_H_


enum RGraphicTypeOption
{
	kAllGraphics = 0,
	kSquareGraphics,
	kRowGraphics,
	kColumnGraphics,
	kPhotoGraphics,
	kFineArtGraphics
};

enum RGrcView
{
	kNoTabView = -1,
	kCategoryView = 0,
	kKeywordView,
	kImportView
};

/*
enum EGraphicFilterType 
{ 
	kAll = 0,
	kSquare, 
	kRow, 
	kColumn, 
	kImage, 
	kFineArt 
};
*/


enum RExpandedSearchOption
{
	kNotExpandedSearch = 0,
	kExpandedSearch
};


#endif