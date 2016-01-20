// ****************************************************************************
//
//  File Name:			EGreetingDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Definition of the REGreetingDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Source/EGreetingDrawingSurface.cpp              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "EGreetingDrawingSurface.h"

// ****************************************************************************
//
//  Function Name:	REGreetingDrawingSurface::REGreetingDrawingSurface( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
REGreetingDrawingSurface::REGreetingDrawingSurface() 
: RPrinterDrawingSurface(TRUE, FALSE)
{
}

// ****************************************************************************
//
//  Function Name:	REGreetingDrawingSurface::~REGreetingDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
REGreetingDrawingSurface::~REGreetingDrawingSurface( ) 
{
	DetachDCs();
	m_rODS.ReleaseImage();
}


// ****************************************************************************
//
//  Function Name:	REGreetingDrawingSurface::Initialize()
//
//  Description:		Prepare the drawing surface for use.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN REGreetingDrawingSurface::Initialize(const RIntSize& rBitmapSize)
{
	try
	{
		// Initialize the bitmap and set up our offscreen DS
		m_rBitmap.Initialize(rBitmapSize.m_dx, rBitmapSize.m_dy, 24);
		m_rODS.SetImage(&m_rBitmap);
		
		//	Proceed with our initialization.
#ifdef _WINDOWS
		HDC hMemDC = (HDC)m_rODS.GetSurface();
		if (!RPrinterDrawingSurface::Initialize(hMemDC, hMemDC)) throw ::kMemory;
#endif
#ifdef MAC
		UnimplementedCode();
#endif
	}
	catch(YException)
	{
		DetachDCs();
		m_rODS.ReleaseImage();
		throw;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	REGreetingDrawingSurface::BeginPage()
//
//  Description:		Starts a page on the printer.
//
//  Returns:			TRUE: call succeeded.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN REGreetingDrawingSurface::BeginPage()
{
	// Call the base member
	return RPrinterDrawingSurface::BeginPage();
}

// ****************************************************************************
//
//  Function Name:	REGreetingDrawingSurface::EndPage()
//
//  Description:		Ends a page on the printer.
//
//  Returns:			TRUE: call succeeded.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN REGreetingDrawingSurface::EndPage()
{
	// REVIEW Send the message

	// Call the base member
	return RPrinterDrawingSurface::EndPage();
}

