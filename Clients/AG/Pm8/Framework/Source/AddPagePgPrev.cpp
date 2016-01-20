//****************************************************************************
//
// File Name:		AddPagePgPrev.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Lisa Wu
//
// Description:		Add Page Page Preview   
//
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "AddPagePgPrev.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage


#ifndef	_WINDOWS
	#error	This file must be compiled only on Windows
#endif	//	_WINDOWS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// AddPageWizard

// ****************************************************************************
//
//  Function Name:	RPagePreview::RPagePreview
//
//  Description:	ctor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RPagePreview::RPagePreview()
{
};


// ****************************************************************************
//
//  Function Name:	RPagePreview::~RPagePreview
//
//  Description:	dtor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPagePreview::~RPagePreview( )
{
}

/////////////////////////////////////////////////////////////////////////////
// RDocPagePreviews

// ****************************************************************************
//
//  Function Name:	RDocPagePreviews::RDocPagePreviews
//
//  Description:	ctor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RDocPagePreviews::RDocPagePreviews()
{
};


// ****************************************************************************
//
//  Function Name:	RDocPagePreviews::~RDocPagePreviews
//
//  Description:	dtor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDocPagePreviews::~RDocPagePreviews( )
{
    for (POSITION pos = m_pagePreviewList.GetHeadPosition(); pos != NULL; )
    {
        RPagePreview *pPgPrev = (RPagePreview*)m_pagePreviewList.GetNext(pos);
		m_pagePreviewList.RemoveHead();
		delete pPgPrev;
    }
}

/////////////////////////////////////////////////////////////////////////////
// RAppQuickStartPages

// ****************************************************************************
//
//  Function Name:	RAppQuickStartPages::RAppQuickStartPages
//
//  Description:	ctor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RAppQuickStartPages::RAppQuickStartPages()
{
};


// ****************************************************************************
//
//  Function Name:	RAppQuickStartPages::~RAppQuickStartPages
//
//  Description:	dtor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAppQuickStartPages::~RAppQuickStartPages( )
{
}