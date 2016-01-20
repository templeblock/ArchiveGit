/*	
	$Header: /PM8/App/FeedbackDropSource.cpp 1     3/03/99 6:05p Gbeddow $

	The CPFeedbackDropSource class allows the program to activate the drop
	source and retain our movement style cursors.  This class overrides
	the feedback opperation of the drop source processing so that we can update
	the UI as the cursor is moved within Windows.

	Copyright 1998 The Learning Company, Inc., all rights reserved

	Revision History:

	$Log: /PM8/App/FeedbackDropSource.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 1     1/07/99 5:07p Rgrenfel
// New files to allow Drag out of PM
*/

#include "stdafx.h"					// precompiled header
#include "FeedbackDropSource.h"	// CPFeedbackDropSource
#include "Resource.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


/*	CPFeedbackDropSource method that provides feedback to the application as the
	drop item is moved.  Over-riding this call allows us to ensure that the
	correct cursor is shown as items are dragged.

	Returns : DRAGDROP_S_USEDEFAULTCURSORS if dragging is in progress and we
				 are not setting the cursor.	NOERROR if dragging is not in proccess
				 or we have set the cursor manually.
*/
SCODE CPFeedbackDropSource::GiveFeedback(
	DROPEFFECT dropEffect )	// The drop effect that should be shown to the
									//		 user.  This is usually the value last
									//		 returned from CView::OnDragEnter or
									//		 CView::OnDragOver.
{
	if( dropEffect == DROPEFFECT_COPY )
	{
//		::SetCursor(AfxGetApp()->LoadCursor( CURSOR_RESOURCE_COPY ));
		::SetCursor(AfxGetApp()->LoadCursor( IDC_FOURARROW_CURSOR ));
		return NOERROR;
	}
	else if ( dropEffect == DROPEFFECT_MOVE )
	{
//		::SetCursor(AfxGetApp()->LoadCursor( CURSOR_RESOURCE_MOVE ));
		::SetCursor(AfxGetApp()->LoadCursor( IDC_FOURARROW_CURSOR ));
		return NOERROR;
	}
	return m_bDragStarted ? DRAGDROP_S_USEDEFAULTCURSORS : NOERROR;
}
