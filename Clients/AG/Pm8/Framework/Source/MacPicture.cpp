// *****************************************************************************
//
// File name:			MacPicture.cpp
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Definition of the RMacPicture class.
//
// Portability:		Macintosh
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
// Client:				Broderbund Software, Inc.
//
// Copyright (c) 1995-1996 Turning Point Software.  All rights reserved.
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef MAC

#include "MacPicture.h"

// ****************************************************************************
//
//  Function Name:		RMacPicture::RMacPicture()
//
//  Description:			Constructor
//
//  Returns:				Nothing
//
//  Exceptions:			
//
// ****************************************************************************
RMacPicture::RMacPicture()
{	
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::RMacPicture()
//
//  Description:			Destructor
//
//  Returns:				Nothing
//
//  Exceptions:			None
//
// ****************************************************************************
RMacPicture::~RMacPicture()
{	
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::RMacPicture()
//
//  Description:			Constructor
//
//  Returns:				Nothing
//
//  Exceptions:			
//
// ****************************************************************************
BOOLEAN RMacPicture::Initialize(sLONG, sLONG)
{		
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::Load()
//
//  Description:			Loads a metafile from disk
//
//  Returns:				Success or failure
//
//  Exceptions:			
//
// ****************************************************************************
BOOLEAN RMacPicture::Load(char*)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::Save()
//
//  Description:			Saves a metafile to disk
//
//  Returns:				Success or failure
//
//  Exceptions:			
//
// ****************************************************************************
BOOLEAN RMacPicture::Save(char*)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::SetPictureHandle()
//
//  Description:			Sets the handle of the metafile, deleting the old one
//
//  Returns:				Success or failure
//
//  Exceptions:			
//
// ****************************************************************************
BOOLEAN RMacPicture::SetPictureHandle(uLONG)
{	
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::GetPictureHandle()
//
//  Description:			Returns the handle of the metafile
//
//  Returns:				^
//
//  Exceptions:			
//
// ****************************************************************************
uLONG RMacPicture::GetPictureHandle() const
{
	return 0;
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::GetWidth()
//
//  Description:			Returns the width of the metafile in RLUs
//
//  Returns:				^
//
//  Exceptions:			
//
// ****************************************************************************
uLONG RMacPicture::GetWidth() const
{	
	return m_sWidth;
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::GetHeight()
//
//  Description:			Returns the height of the metafile in RLUs
//
//  Returns:				^
//
//  Exceptions:			
//
// ****************************************************************************
uLONG RMacPicture::GetHeight() const
{	
	return m_sHeight;
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::GetRawDataSize()
//
//  Description:			Returns the size (in bytes) of the buffer required
//								to store the internal representation of the metafile
//								data.
//
//  Returns:				^
//
//  Exceptions:			
//
// ****************************************************************************
uLONG RMacPicture::GetRawDataSize() const
{
	return 0;
}

// ****************************************************************************
//
//  Function Name:		RMacPicture::GetRawData()
//
//  Description:			Returns by reference the internal data of the metafile.
//
//  Returns:				^
//
//  Exceptions:			
//
// ****************************************************************************
BOOLEAN RMacPicture::GetRawData(uBYTE*& pBuffer, uLONG uBufferSize) const
{
	return FALSE;
}

#endif //MAC
