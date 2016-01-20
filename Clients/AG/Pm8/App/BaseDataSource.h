/*	
	$Header: /PM8/App/BaseDataSource.h 1     3/03/99 6:03p Gbeddow $

	Base class to handle delayed render for Copy Paste and drag drop operations.

	Copyright 1998 The Learning Company, Inc., all rights reserved

	Revision History:

	$Log: /PM8/App/BaseDataSource.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     1/25/99 3:38p Rgrenfell
// Added support for object descriptors in Drag/Drop
// 
// 1     1/07/99 5:07p Rgrenfel
// New files to allow Drag out of PM
*/

#ifndef BASEDATASOURCE_H
#define BASEDATASOURCE_H

// #include "OleDataList.h"

class CPmwDoc;
class CPtrList;

/* This is a base class which handles delayed render for Copy/Paste and
	Drag/Drop operations.
*/
class CPBaseDataSource : public COleDataSource
{
public:
	// Public interface

	// Class constructor.
	CPBaseDataSource();
	// Class destructor.
	virtual ~CPBaseDataSource();

	// Provide the document linked to this drag process.  (Necessary to render the
	// objects upon request.)
	inline void SetDocument( CPmwDoc *pDoc )
		{ m_pDoc = pDoc; }


protected:
	// Protected methods

	// Pure virtual for rendering the global data.
	virtual BOOL OnRenderGlobalData( LPFORMATETC lpFormatetc, HGLOBAL* phGlobal) = 0;
	
	// Converts shared memory file into an object list.
//	CPOleDataList * DeSerialize( HANDLE hData );

	// Serializes the object list into a shared memory file and
	//			returns a handle to the file.
//	HANDLE Serialize( CPOleDataList* pList );

	// Renders the object list as a single metafile.
	BOOL RenderMetafile( HGLOBAL* phGlobal );

	// Renders the object list as a single enhanced metafile.
	BOOL RenderEnhMetafile( HGLOBAL* phGlobal );

	// Renders the selected object list as a single BMP.
	BOOL RenderBitmap(HGLOBAL* phGlobal );

	// Renders the selected object list as a single DIB.
	BOOL RenderDIB(HGLOBAL* phGlobal );
	
	// Renders the object list as RTF text
	BOOL RenderRTF( HGLOBAL* phGlobal );
	
	// Renders the object list as plain text
	BOOL RenderText( HGLOBAL* phGlobal );
	
	// Renders the object list into chunky storage format
//	BOOL RenderPSDChunk( HGLOBAL* phGlobal /*, CPOleDataList* pObjList*/ );

	// Renders the object descriptor specifying the drag object's layout.
	BOOL RenderObjectDescriptor( HGLOBAL* phGlobal );

	// Allocates a list of selected objects in the document.
	CPtrList *CreateSelectedList();
	// Deallocates the list of selected objects.
	void ReleaseSelectedList( CPtrList *pList );

	// Store the document so we can render the selected objects upon request.
	CPmwDoc *m_pDoc;
};

#endif // BASEDATASOURCE_H
