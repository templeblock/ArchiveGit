// ****************************************************************************
//
//  File Name:			DefaultDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RDefaultDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Source/DefaultDrawingSurface.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "DefaultDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef _WINDOWS
// ****************************************************************************
//
//  Function Name:	RDefaultDrawingSurface::RDefaultDrawingSurface()
//
//  Description:		Creates a DS compatible with the screen.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
RDefaultDrawingSurface::RDefaultDrawingSurface(BOOLEAN fIsPrinting)
: BaseDrawingSurface(FALSE, fIsPrinting)
{
	try
	{
		m_hScreenDC = ::GetDC(NULL);
		if (!m_hScreenDC) throw ::kMemory;
		if (!RDcDrawingSurface::Initialize(m_hScreenDC, m_hScreenDC)) throw ::kMemory;
	}
	catch(YException)
	{
		if (m_hScreenDC) ::ReleaseDC(NULL, m_hScreenDC);
		m_hScreenDC = NULL;
		throw;
	}
}

// ****************************************************************************
//
//  Function Name:	RDefaultDrawingSurface::~RDefaultDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RDefaultDrawingSurface::~RDefaultDrawingSurface()
{
	DetachDCs();
	if (m_hScreenDC) ::ReleaseDC(NULL, m_hScreenDC);
	
	// NULL out the dcs so we don't call DetachDCs() again
	m_hScreenDC = m_hOutputDC = m_hAttributeDC = NULL;
}
#endif //_WINDOWS

#ifdef MAC
// ****************************************************************************
//
//  Function Name:	RDefaultDrawingSurface::RDefaultDrawingSurface()
//
//  Description:		Creates a DS compatible with the screen.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
RDefaultDrawingSurface::RDefaultDrawingSurface(BOOLEAN fIsPrinting) : BaseDrawingSurface()
{
	// REVEIW MikeH this wont work for printing on the Mac
}

// ****************************************************************************
//
//  Function Name:	RDefaultDrawingSurface::~RDefaultDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RDefaultDrawingSurface::~RDefaultDrawingSurface()
{
}
#endif //MAC