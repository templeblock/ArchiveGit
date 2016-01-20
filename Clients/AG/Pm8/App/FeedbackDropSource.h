/*	
	$Header: /PM8/App/FeedbackDropSource.h 1     3/03/99 6:05p Gbeddow $

	The CPFeedbackDropSource class allows the program to activate the drop
	source and retain our movement style cursors.  This class overrides
	the feedback opperation of the drop source processing so that we can update
	the UI as the cursor is moved within Windows.

	Copyright 1998 The Learning Company, Inc., all rights reserved

	Revision History:

	$Log: /PM8/App/FeedbackDropSource.h $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 1     1/07/99 5:07p Rgrenfel
// New files to allow Drag out of PM
*/

#ifndef FEEDBACKDROPSOURCE_H
#define FEEDBACKDROPSOURCE_H


/*	For overridding the display of cursors during a drag drop operation
*/
class CPFeedbackDropSource : public COleDropSource
{
public:
	// Public functions

	// Inline constructor.
	CPFeedbackDropSource( )
	{	}
	
	// Overload called by the framework to provide feedback to the view
	//	as the drop object is moved.
	virtual SCODE GiveFeedback( DROPEFFECT dropEffect );

private:

};

#endif	// FEEDBACKDROPSOURCE_H
