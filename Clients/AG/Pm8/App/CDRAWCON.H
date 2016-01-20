/*
// $Workfile: CDRAWCON.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDRAWCON.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:27:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:04   FRED
// Initial revision.
// 
//    Rev 2.3   06 Jun 1995 16:54:48   JAY
// Project previews in the album.
// 
//    Rev 2.2   06 Jun 1995 10:12:18   JAY
// Moved more stuff over from CATALOG.
// 
//    Rev 1.4   23 Dec 1994 11:05:44   JAY
// Moved Breathe() method to here.
// 
//    Rev 1.3   23 Dec 1994 10:23:44   JAY
// CDrawException.
// 
//    Rev 1.2   15 Nov 1994 08:35:40   JAY
// Support for multiple palettes.
// 
//    Rev 1.1   03 Nov 1994 15:53:06   JAY
//  
// 
//    Rev 1.7   02 Nov 1994 15:09:28   JAY
// Support for palettes; cleanup in CCGMItemData::OnDelete
// 
//    Rev 1.6   27 Oct 1994 14:50:08   FRED
// Removed state stuff, Push, and Pop.
// Added new stuff for painting.
// 
// 
//    Rev 1.5   24 Oct 1994 09:19:18   FRED
// Added prototypes for Init(CWnd*) methods.
// 
// 
//    Rev 1.4   11 Oct 1994 08:38:46   JAY
// Reimplemented CDrawContext with states
// 
//    Rev 1.3   10 Oct 1994 17:43:10   JAY
//  
// 
//    Rev 1.2   14 Sep 1994 10:07:16   JAY
// Toggle support in draw context.
// 
//    Rev 1.1   31 Aug 1994 16:01:44   FRED
// Added GetDC() and ReleaseDC() methods.
// 
// 
//    Rev 1.0   30 Aug 1994 14:59:24   JAY
// Initial revision.
*/

class CDrawContextState;
class CDrawContext;
class CToggleArray;
class CToggleElement;

#ifndef __CDRAWCON_H__
#define __CDRAWCON_H__

#include "craster.h"

typedef BOOL (*TOGGLE_FUNC)(CDrawContext* pContext, BOOL fTurnOn, const CRect* pcrArea, LPVOID pData);

class CDrawState;
class CUpdateList;
class CInterfaceObject;
class CDrawException;

class CToggleElement
{
public:
	TOGGLE_FUNC m_pFunction;
	LPVOID m_pData;
};

class CToggleArray
{
public:
	CToggleArray();
	virtual ~CToggleArray();

	int GetSize(void) const;
	void Add(const CToggleElement& Element);
	int Find(const CToggleElement& Element) const;
	void Remove(int nIndex);
	void Remove(const CToggleElement& Element);
	const CToggleElement* GetAt(int nIndex) const;
	void RemoveAll(void);

protected:
	CPtrArray m_cpaElements;
};

/*
// A silly little class to make palette management simple.
*/

class CSharedPalette
{
public:
	CSharedPalette() { m_pPalette = NULL; }
	~CSharedPalette() { delete m_pPalette; }
	CPalette* GetPalette(void);

protected:
	CPalette* m_pPalette;

	struct BigLogPal
	{
		WORD palVersion;
		WORD palNumEntries;
//		PALETTEENTRY palPalEntry[NumColors];
	};
};

/*
// This class defines a context for drawing.
// This class is more or less struct-accessible (i.e. public data members).
*/

class CDrawContext
{
public:
	CDrawContext();
	virtual ~CDrawContext();

	CDC* GetDC(void);
	void ReleaseDC(void);

	CDC* GetDestinationDC(BOOL fIncrement = FALSE);
	void ReleaseDestinationDC(BOOL fDecrement = FALSE);

	CDC* GetSourceDC(void);

	void BeginPaint(CDrawState* pState);
	void EndPaint(void);

	virtual void Invalidate(CInterfaceObject* pObject, CUpdateList* pMaskUpdate = NULL, CUpdateList* pImageUpdate = NULL);

	virtual BOOL Breathe(void);

	void Window(CWnd* pWnd);
	CWnd* Window(void);

	void DC(CDC* pDC);
	CDC* DC(void);

	void DrawState(CDrawState* pDrawState);
	CDrawState* DrawState(void);

	void ClipRect(LPRECT pcrClip);
	LPRECT ClipRect(void);

	void BeginDraw(CDC* pDrawDC, LPRECT pcrClip = NULL);
	void EndDraw(void);

	void Init(CDC* pDC);
	void Init(CWnd* pWnd);
	void UseDC(CDC* pDC);

	CRasterInfo& Info(void);

/* Thumbnail support. */

	void UseThumbNails(BOOL fUseThumbNails)
		{ m_fUseThumbNails = fUseThumbNails; }
	BOOL UseThumbNails(void) const
		{ return m_fUseThumbNails; }

/* Palette support. */

	static CPalette* Palette(void);
	static void PushPalette(HPALETTE hPalette);
	static void PopPalette(void);

/* Toggle support. */
public:
	void RegisterToggle(TOGGLE_FUNC pFunc, LPVOID pData);
	/* Pass NULL for pData to match any instance of pFunc. */
	void UnregisterToggle(TOGGLE_FUNC pFunc, LPVOID pData = NULL);

	void Toggle(BOOL fTurnOn, const CRect* pcrArea);

/* General attributes. */
protected:

	CDC* MakeCompatibleDC(void);

	// Data

	CRasterInfo m_info;			/* The raster information. */
	int m_nDCCount;				/* DC reference count. */

	int m_nDestinationDCLevel;

/* Drawing-specific (transient) values. */

	CDC*		m_pDC;				/* Destination DC. */
	CDC*		m_pDestinationDC;	/* Compatible destination DC. */
	CDC*		m_pSourceDC;		/* Compatible source DC. */
	LPRECT	m_pClipRect;		/* Rectangle for clipping. NULL for none. */

	CWnd*		m_pWnd;				/* Corresponding window, if appropriate. */

	BOOL		m_fUseThumbNails;

	CDrawState* m_pDrawState;

	CToggleArray m_ToggleArray;

	static CSharedPalette near m_GlobalPalette;
	static CWordArray near m_PushedPalettes;

	CPalette* m_pOldPalette;
};

/*
// A draw exception.
*/

class CDrawException : public CException
{
	DECLARE_DYNAMIC(CDrawException);
public:
/* Cause values. */
	enum
	{
		none = 0,
		user_interrupt = 1,
		update_locked = 2
	};

/* Constructor */

	CDrawException(int nCause);

/* Attributes */

	int m_nCause;
};

extern void ThrowDrawException(int nCause = CDrawException::user_interrupt);

#endif			// __CDRAWCON_H__
