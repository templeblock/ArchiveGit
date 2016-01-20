/*
// $Workfile: borddef.h $
// $Revision: 2 $
// $Date: 3/08/99 1:13p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/BORDER/borddef.h $
// 
// 2     3/08/99 1:13p Rgrenfel
// Added a fixup method for record types used by the borders.
// 
// 21    3/05/99 6:52p Rgrenfel
// Added a fixup method for record types used by the borders.  Fixes
// serialization for the use of component objects.
// 
// 20    11/19/98 2:54p Mwilson
// added call to get bitmap for toolbar
// 
// 19    8/10/98 4:41p Hforman
// changes for cropping copyright
// 
// 18    6/02/98 4:11p Rlovejoy
// Code to remove helpful hint from Border Editor.
// 
// 17    6/02/98 1:18p Rlovejoy
// Add DisplayHelpfulHint() to the callback interface.
// 
// 16    2/24/98 2:17p Hforman
// 
// 15    2/24/98 11:04a Hforman
// add GetPalette()
// 
// 14    2/20/98 12:06p Rlovejoy
// Made GetInnerRect() pure virtual.
// 
// 13    2/20/98 12:01p Hforman
// added enum for return values
// 
// 12    2/20/98 11:49a Hforman
// add GetInnerRect(), put color back into CBEDrawInfo
// 
// 11    2/19/98 11:00a Hforman
// added CBEBackgroundInfo struct, and DrawBackground() method
// 
// 10    2/17/98 12:24p Rlovejoy
// 
// 9     2/17/98 12:15p Hforman
// changed m_pClientDrawState and m_pBEDrawState to just m_pDrawState
// 
// 8     2/11/98 3:03p Rlovejoy
// Latest update.
// 
// 7     1/13/98 6:13p Hforman
// add pDatabase param to Notify()
// 
// 6     1/09/98 11:03a Hforman
// add HWND to ChooseGraphic()
// 
// 5     1/07/98 3:20p Hforman
// removed m_Origin, added m_pClip to CBEDrawInfo
// 
// 4     1/07/98 2:49p Rlovejoy
// Added the SetData() definition.
// 
// 3     1/06/98 4:30p Hforman
// made everything public in CBEDrawInfo class
// 
// 2     1/06/98 9:54a Rlovejoy
// Fixed compilation errors.
// 
// 1     1/05/98 4:16p Rlovejoy
// Initial creation.
*/

#ifndef __BORDDEF_H__
#define __BORDDEF_H__

class CBEInterface;				// Methods exposed by the border DLL.
class CBECallback;				// Methods exposed by the client.
class CBEObject;					// Instance of a border object.

class CBEDrawInfo;				// Information passed to Draw().
class CBEBackgroundInfo;		// Information passed to DrawBackground().
class CBEGraphicInfo;			// Information returned by GetGraphicInfo().

/////////////////////////////////////////////////////////////////////////////
// Interfaces:
// The following abstract interface classes define the function protocols
// for the underlying classes (derived from the base interface classes). To
// preserve the vtable structure, the derived classes should not have
// virtual destructors.


// Return codes for interface calls -
// Negative values signify errors
enum
{
	BorderInvalidParams	= -2,
	BorderError				= -1,	// general error condition
	BorderSuccess			= 0,	// all's well
	BorderEmpty				= 1	// border contains no graphics and no background color
};


/////////////////////////////////////////////////////////////////////////////
// CBEInterface - interface definition for border DLL methods
//                This object is created by the border DLL.

class CBEInterface
{
public:
	// Release this interface. The interface is not valid after the call.
	virtual int Release(void) = 0;

	// Create a new instance of a border object.
	// The object will have default settings unless creation data is passed in.
	virtual int NewObject(CBEObject*& pObject,
								 void* pDatabase,
								 int nLength = 0,
								 void* pData = NULL) = 0;

	// Free a DLL-created draw state.
	virtual int FreeDrawState(void* pDrawState) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CBEObject - interface definition for border object
//             These objects are created and destroyed by the border DLL in
//             response to requests from the client (via NewObject()).

class CBEObject
{
public:
	// Release the memory for this object.
	// The object pointer is no longer valid after this call.
	virtual int Release(void) = 0;

	// Get the "serialization data" for this object.
	virtual int GetData(int& nLength, void* pData = NULL) = 0;

	// Set the "serialization data" for this object.
	virtual int SetData(int nLength, void* pData) = 0;

	// Destroy the object (remove all graphic references).
	// This implicitly does a Release().
	// The object pointer is no longer valid after this call.
	virtual int Destroy(void) = 0;

	// Duplicate this border object.
	// The new object is returned.
	virtual int Duplicate(CBEObject*& pObject, void* pDatabase = NULL) = 0;

	// Set the size (aspect). Units are inches.
	virtual int SetSize(double dXSize, double dYSize) = 0;

	// Get inner rectangle dimensions
	virtual void GetInnerRect(RECT* pRect) = 0;

	// Draw the border.
	virtual int Draw(CBEDrawInfo* pDrawInfo) = 0;

	// Edit the border. This launches the editor interface.
	virtual int Edit(HWND hParent) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CBECallbackInterface - interface for functions provided by the client.
//                        This object is created by the client and passed
//                        to the DLL during the Initialize() call.

class CBECallbackInterface
{
public:
	// Release this interface. The pointer is not valid after this call.
	virtual int Release(void) = 0;

	// Create a new graphic.
	virtual int NewGraphic(DWORD& dwID,
								  void* pDatabase,
								  LPCSTR pszFileName,
								  LPCSTR pszFriendlyName = NULL,
								  BOOL fCropCopyright = FALSE) = 0;

	virtual HBITMAP GetToolbarBitmap() = 0;
	// Duplicate a graphic reference.
	virtual int Duplicate(DWORD& dwDupID,
								 void* pDupDatabase,
								 DWORD dwID,
								 void* pDatabase) = 0;

	// Add a reference to a graphic.
	virtual int AddRef(DWORD dwID, void* pDatabase) = 0;
	
	// Remove a reference to a graphic.
	virtual int RemoveRef(DWORD dwID, void* pDatabase) = 0;
	
	// Choose a graphic using the client interface.
	virtual int ChooseGraphic(HWND hParent, DWORD& dwID, void* pDatabase) = 0;

	// Get the info for a graphic.
	virtual int GetGraphicInfo(DWORD dwID, void* pDatabase, CBEGraphicInfo& Info) = 0;
	
	// Draw a graphic.
	virtual int DrawGraphic(DWORD dwID, void* pDatabase, CBEDrawInfo* pDrawInfo) = 0;

	// Draw entire background
	virtual int DrawBackground(CBEBackgroundInfo* pBackgroundInfo) = 0;

	// Get PrintMaster Palette
	virtual HPALETTE GetPalette() = 0;

	// Display/remove helpful hints for the border editor
	virtual void DisplayHelpfulHint(CWnd* pParent = NULL) = 0;
	virtual void RemoveHelpfulHint() = 0;

	// Free a client-created draw state.
	virtual int FreeDrawState(void* pDrawState) = 0;

	// Ensures that the record referenced is of the correct type and resets the
	// ID value to a converted record if it isn't.
	virtual void MatchupRecordType( DWORD *pID, void *pVoidDatabase ) = 0;

	// Notifications.
	enum
	{
		BENotifyBorderChange = 1,
		BESaveBorder = 2
	};

	// Notify the client of something. See above for notifications.
	virtual int Notify(int nNotification, void* pDatabase, void* pData) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CBEGraphicInfo - Graphic info.

class CBEGraphicInfo
{
public:
	double m_dXSize;		// width in inches
	double m_dYSize;		// height in inches
};

/////////////////////////////////////////////////////////////////////////////
// CBEDrawInfo - Drawing info.

class CBEDrawInfo
{
public:
	HDC		m_hdc;
	RECT		m_Bounds;				// unrotated bounds
	RECT*		m_pClip;					// clipping rect (may be NULL)
	double	m_dAngle;				// rotation angle (in radians)
	BOOL		m_fXFlipped;
	BOOL		m_fYFlipped;
	COLORREF	m_clBackground;		// -1L means transparent.
	HWND		m_hWnd;
	BOOL		m_fAllowInterrupt;
	void*		m_pDrawState;			// used for interrupt handling
	void*		m_pClientData;
};

/////////////////////////////////////////////////////////////////////////////
// CBEBackgroundInfo - Info for drawing Background color

class CBEBackgroundInfo
{
public:
	HDC		m_hdc;
	RECT		m_OuterBounds;			// unrotated bounds of entire border
	RECT		m_InnerBounds;			// unrotated bounds of inner rectangle
	double	m_dAngle;
	COLORREF	m_clBackground;		// -1L means transparent.
	void*		m_pClientData;
};


#endif
