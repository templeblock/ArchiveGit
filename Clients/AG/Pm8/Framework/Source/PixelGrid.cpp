// ****************************************************************************
//
//  File Name:			PixelGrid.cpp
//
//  Project:			TPS Libraries
//
//  Author:				Greg Brown, Bruce Rosenblum
//
//  Description:		Advanced DIB manipulation functions support routines
//							Maintains a bitmap "hit-table" used to indicate which
//							pixels in an image have been accessed
//
//  Portability:		Platform independent, 32 bit systems only
//
//  Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Turning Point Software         
//
//  Copyright (C) 1996 Turning Point Software, Inc. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/PixelGrid.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "PixelGrid.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	CPixelGrid::CPixelGrid( )
//
//  Description:		Initialize pixel grid IVs
//
//  Returns:			Nothing
//
//  Exceptions:		none
//
// ****************************************************************************
//
CPixelGrid::CPixelGrid ()
{
	m_ppPixelGrid = NULL;		
	m_Width = 0;
	m_Height = 0;
}

// ****************************************************************************
//
//  Function Name:	CPixelGrid::~CPixelGrid( )
//
//  Description:		delete memory used by pixel grid array
//
//  Returns:			Nothing
//
//  Exceptions:		none
//
// ****************************************************************************
//
CPixelGrid::~CPixelGrid()
{
	if(NULL != m_ppPixelGrid)
	{
		for (int i = 0; i < m_Height; i++)
			delete m_ppPixelGrid[i];
		delete m_ppPixelGrid;
	}
}

// ****************************************************************************
//
//  Function Name:	CPixelGrid::Initialize( )
//
//  Description:		Allocate memory used for pixel grid arrays
//
//  Returns:			None
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void CPixelGrid::Initialize(sLONG Width, sLONG Height)
{
	m_Width = Width;
	m_Height = Height;
	m_ppPixelGrid = new PUBYTE [Height];
	memset (m_ppPixelGrid, 0, Height * sizeof(PUBYTE));		
	int ByteWidth = (Width + 7) / 8;
	for (int i = 0; i < Height; i++)
	{
		//	Use discrete memory allocations to avoid multiplies when accessing
		//	grid later to aid in performance.
		m_ppPixelGrid[i] = new uBYTE [ByteWidth];
		memset (m_ppPixelGrid[i], 0, ByteWidth);		
	}
}

