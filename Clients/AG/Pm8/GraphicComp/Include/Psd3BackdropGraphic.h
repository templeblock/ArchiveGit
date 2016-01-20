// ****************************************************************************
//
//  File Name:			Psd3BackdropGraphic.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RPsd3TiledBackdropGraphic class
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
// ****************************************************************************

#ifndef _PSD3BACKDROPGRAPHIC_H_
#define _PSD3BACKDROPGRAPHIC_H_

#ifndef _PSD3SINGLEGRAPHIC_H_
#include "Psd3SingleGraphic.h"
#endif

const YChunkTag kPsd3BackdropGraphicChunkID =		0x02160008;
const YChunkTag kPsd3TiledBackropGraphicChunkID =	0x02160009;

class RPsd3BackdropGraphic : public RPsd3SingleGraphic
{
public:
	virtual BOOLEAN					GetAdjustLineWidth() {return FALSE;}
	virtual YChunkTag					GetChunkStorageTag() {return kPsd3BackdropGraphicChunkID;}
};

class RPsd3TiledBackdropGraphic : public RPsd3SingleGraphic
{
public:
	virtual BOOLEAN					GetAdjustLineWidth() {return TRUE;}
	virtual YChunkTag					GetChunkStorageTag() {return kPsd3TiledBackropGraphicChunkID;}
};

#endif // _PSD3TILEDBACKDROPGRAPHIC_H_