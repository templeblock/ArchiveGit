/*	
	$Header: /PM8/App/DragDataSource.h 1     3/03/99 6:05p Gbeddow $

	Derived class to handle delayed render for Drag Drop operations.

	Copyright 1998 The Learning Company, Inc., all rights reserved

	Revision History:

	$Log: /PM8/App/DragDataSource.h $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 1     1/07/99 5:07p Rgrenfel
// New files to allow Drag out of PM
*/


#ifndef DragDataSource_h
#define DragDataSource_h

#include "BaseDataSource.h"

/*	This class provides delayed render support for Drag/Drop opperations.
	The implementation of this class is specific to drag and drop based on the
	support provided by CPBaseDataSource.
*/
class CPDragDataSource : public CPBaseDataSource
{
public:
	// @access Public interface

	// @cmember Class constructor.
	CPDragDataSource();
	// @cmember Class destructor.
	virtual ~CPDragDataSource();

	// @cmember initializes member variable
//	virtual void Init( CPOleDataList* pList );

protected:
	// @access Protected methods.

	// @cmember	Deteremines which format is requested and calls the appropriate
	//			render function.
	virtual BOOL OnRenderGlobalData( LPFORMATETC lpFormatetc, HGLOBAL* phGlobal);

	// Determines how to render the data, its medium and format.
	virtual BOOL OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium);
	
	// @cmember Serializes the object list and sets the handle to the shared memory file.
//	BOOL GetHandle( HANDLE* pHandle );

private:
	// @cmember Stores the object list.
//	CPOleDataList* m_pObjList;
};

#endif //DragDataSource_h
