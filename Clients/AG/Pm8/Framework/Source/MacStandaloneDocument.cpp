// ****************************************************************************
//
//  File Name:			MacStandaloneDocument.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RStandaloneDocument class
//
//  Portability:		Mac Specific
//
//  Developed by:		Turning Point Software.
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/MacStandaloneDocument.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"StandaloneDocument.h"
#include	"StandaloneView.h"


// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::Initialize( )
//
//  Description:	Initializer. We must use an initializer rather than a
//					constructor, as this document will be created by MFC's
//					document template mechanism.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneDocument::Initialize( RApplication* pApp )
	{
	///xxx	TODO: Validate pApp and pView if it is not done in RDocument...
	RDocument::Initialize( pApp );
	}
