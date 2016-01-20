/*
// Paper info.
//
// $Header: /PM8/App/PAPER.H 1     3/03/99 6:08p Gbeddow $
//
// $Log: /PM8/App/PAPER.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 5     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 4     12/09/97 8:31a Jstoner
// modified desktop so that label outline is dotted and shows thru objects
// drawn on top of desktop
// affects label and sticker projects
// 
// 3     12/08/97 9:27a Jstoner
// dashed lines for label slots on desktop
// 
// 2     12/04/97 11:00a Jstoner
// added parameter to drawpage which allows label and page outlines to be
// drawn on top of all objects
// 
//    Rev 1.1   30 Sep 1997 11:14:00   johno
// Added ORIENTATION_INVALID
// 
//    Rev 1.0   14 Aug 1997 15:23:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:58   Fred
// Initial revision.
// 
//    Rev 1.1   31 Jan 1997 15:50:54   Jay
// New desktop background color
// 
//    Rev 1.0   14 Mar 1996 13:45:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:44   FRED
// Initial revision.
// 
//    Rev 1.8   30 Aug 1995 11:00:26   FRED
// Added OriginOffset()
// 
//    Rev 1.7   28 Aug 1995 20:31:22   FRED
// Print alignment
// 
//    Rev 1.6   11 Aug 1995 09:46:02   JAY
// Added SetMinimumMargins().
// 
//    Rev 1.5   10 Aug 1995 12:59:36   JAY
// Highlight parameter to DrawSlot().
// 
//    Rev 1.4   09 Aug 1995 15:20:18   JAY
// Added a paremeter to DrawPage3D.
// 
//    Rev 1.3   31 Jul 1995 10:22:42   JAY
// Added InitAsLabels().
// 
//    Rev 1.2   28 Jul 1995 12:28:14   JAY
// Added some access functions.
// 
//    Rev 1.1   20 Jul 1995 12:48:34   JAY
// Support for drawing, labels, etc.
// 
//    Rev 1.0   30 Jun 1995 16:33:22   JAY
// Initial revision.
*/

#ifndef __PAPER_H__
#define __PAPER_H__

#include "pagedefs.h"
#include "cfixed.h"

// what should draw page do
typedef enum {
	DRAW_PREVIEW,
	DRAW_BACKGROUND,
	DRAW_FOREGROUND
} DRAW_PAGE_MODE;

/*
// Paper info.
//
// This class describes a single sheet of paper. This can be used to describe
// the actual paper in a printer or to describe the design set for a project
// (e.g. labels created for a specific label stock).
//
// Most of the values in the following structure are CFixed. The units are
// inches (so, for example, MakeFixed(1) is one inch).
*/

class CPaperInfo
{
public:
	CPaperInfo();

/*
// Enumerations.
*/
	// Paper orientations.
	enum
	{
      PAPER_OrientationInvalid = ORIENTATION_INVALID,
      PAPER_Portrait = PORTRAIT,
	   PAPER_Landscape = LANDSCAPE
	};

	// Paper types.
	enum
	{
		PAPER_CutSheet,
		PAPER_Continuous
	};

	// Paper classes.
	enum
	{
		CLASS_Sheet,
		CLASS_Labels
	};

	// Slot classes.
	enum
	{
		SLOT_Rectangular	= 0,				/* Rectangle */
		SLOT_Rounded		= 1,				/* Rounded rectangle */
		SLOT_Circular		= 2,				/* Circle */
	// Other types.
		SLOT_Avery5198		= 100,			/* Avery #5198 - Audio Tape */
		SLOT_Avery5385		= 101,			/* Avery #5385 - Rotary Card */
		SLOT_Avery5386		= 102,			/* Avery #5386 - Rotary Card */
	};

	// Margin styles.
	enum
	{
		MARGINS_Minimal = 0,
		MARGINS_Symmetric = 1,
		MARGINS_Symmetric2 = 2,		// Do not use internally.
		MARGINS_Equal = 3
	};
/*
// Access functions.
*/

	/*
	// Orientation of paper.
	*/

	int Orientation(void) const
		{ return (int)m_Record.m_nOrientation; }

	/*
	// Type of paper (cut-sheet, continuous).
	*/

	int Type(void) const
		{ return (int)m_Record.m_nType; }

	/*
	// Class of paper.
	*/

	int Class(void) const
		{ return (int)m_Record.m_nClass; }

	/*
	// Return the offset of the origin. This is usually 0, but it's here in
	// case we need it.
	*/

	CFixedPoint OriginOffset(void) const
		{ return m_Record.m_OriginOffset; }

	/*
	// Set the offset of the origin.
	*/
	
	void OriginOffset(const CFixedPoint& Origin)
		{ m_Record.m_OriginOffset = Origin; }

	/*
	// Set the dimensions of the paper.
	*/
	
	void PaperDims(const CFixedPoint& Dims)
		{ m_Record.m_PaperDims = Dims; }

	/*
	// Return the dimensions of the paper.
	*/

	CFixedPoint PaperDims(void) const
		{ return m_Record.m_PaperDims; }

	/*
	// The number of slots.
	*/

	int Slots(void) const
		{ return (int)m_Record.m_nSlotsAcross*m_Record.m_nSlotsDown; }

	/*
	// The number of slots across.
	*/

	int SlotsAcross(void) const
		{ return m_Record.m_nSlotsAcross; }

	/*
	// The number of slots down.
	*/

	int SlotsDown(void) const
		{ return m_Record.m_nSlotsDown; }

	/*
	// Return the global slot class.
	// This is like "round", "rectangle", etc.
	*/

	int SlotClass(void) const
		{ return (int)m_Record.m_nSlotClass; }

	/*
	// Return the radius of the corner of a rounded rectangle slot.
	*/

	CFixed SlotCornerRadius(void) const
		{ return m_Record.m_Radius; }

	/*
	// Return the half page perf skip distance.
	*/

	CFixed HalfPagePerfSkip(void) const
		{ return m_Record.m_HalfPagePerfSkip; }

	/*
	// Return the number of rows on a half page.
	*/

	int HalfPageRows(void) const
		{ return m_Record.m_nHalfPageRows; }

	/*
	// Return the page area of a slot.
	*/

	CFixedRect SlotBounds(int nSlot) const;

	/*
	// Return the dimensions of a slot.
	*/

	CFixedPoint SlotDims(void) const
		{ return m_Record.m_SlotDims; }

	/*
	// Return the dimensions to the next slot.
	*/

	CFixedPoint SlotStep(void) const
		{ return m_Record.m_SlotStep; }

	/*
	// Return the origin of the slots
	*/

	CFixedPoint SlotOrigin(void) const
		{ return m_Record.m_SlotOrigin; }

	/*
	// Return the margin style.
	*/

	int MarginStyle(void) const
		{ return m_Record.m_nMarginStyle; }

	/*
	// Set the margin style.
	*/

	void SetMarginStyle(int nMarginStyle, BOOL fRecompute = TRUE);

	/*
	// Return the physical margins for the slots.
	// This describes the printable area of a slot (by saying what
	// part on each side is not printable).
	*/

	CFixedRect PhysicalMargins(void) const
		{ return m_Record.m_PhysicalMargins; }

	/*
	// Set the physical margins.
	*/

	void SetPhysicalMargins(const CFixedRect& PhysicalMargins, BOOL fRecompute = TRUE);

	/*
	// Return the desired margins for a slot.
	// These can be set by the end-user or by the programmer (or both!).
	*/

	CFixedRect MinimumMargins(void) const
		{ return m_Record.m_MinimumMargins; }

	/*
	// Set the desired margins for a slot.
	// These can be set by the end-user or by the programmer (or both!).
	*/

	void MinimumMargins(CFixedRect MinimumMargins)
		{ m_Record.m_MinimumMargins = MinimumMargins; }

	/*
	// Return the margins to use for a slot.
	*/

	CFixedRect Margins(void) const
		{ return m_Record.m_Margins; }

	/*
	// Set this up to describe a single sheet of paper.
	// This is useful for describing what's in a printer.
	*/

	void InitAsSheet(
		const CFixedPoint& Size,
		const CFixedPoint& Offset,
		const CFixedRect& Margins,
		int nType,
		int nMarginStyle = MARGINS_Equal);

	/*
	// Set this up to describe a sheet of labels.
	*/

	void InitAsLabels(CFixed Width, CFixed Height,
							CFixed SlotWidth, CFixed SlotHeight,
							int nAcross, int nDown,
							CFixed SlotOriginX, CFixed SlotOriginY,
							CFixed SlotXStep, CFixed SlotYStep);

	void SetProjectDims(const PPNT ppnt)
		{ m_ProjectDims = ppnt; }

	/*
	// Compute the dimensions for the project.
	*/

	PPNT GetProjectDims(void)
		{ return m_ProjectDims; }

	/*
	// Get the margins in page resolution.
	*/

	PBOX GetPageMargins(void)
		{ return m_PageMargins; }

	void SetPageMargins(const PBOX pbox)
		{ m_PageMargins = pbox; }

/*
// Routines to update the internal state when things change.
*/

	/*
	// Update the variables stored at page resolution.
	*/

	void UpdatePageValues(void);

	/*
	// Compute the actual margins from the physical and minimum ones.
	*/

	void ComputeMargins(void);

	/*
	// Get the paper's name.
	*/

	const CString& GetName(void) const
		{ return m_csName; }

	/*
	// Set the paper's name.
	*/

	void SetName(LPCSTR pName)
		{ m_csName = pName; }

/*
// Routines to draw this info in various ways.
*/

	/*
	// Draw the page to a DC.
	*/
	// bPaintPage added for drawing stiker desktop
	void DrawPage(CDC* pDC, const CRect& crClient, BOOL fLandscape = FALSE, CRect* pDrawnRect = NULL, short drawmode = DRAW_PREVIEW);

	void DrawPage3D(CDC* pDC, const CRect& crClient, int nGap, BOOL fLandscape = FALSE, CRect* pDrawnRect = NULL, CRect* pEraseRect = NULL);

	/*
	// Draw a slot to a DC.
	*/

	void DrawSlot(HDC hDC, const CRect& crSlot, BOOL fLandscape = FALSE, BOOL fSave = TRUE, COLORREF* pHighlightColor = NULL, COLORREF clPage = RGB(255, 255, 255), BOOL bDashed = FALSE);

/*
// Routines to put this guy away and bring him back.
*/

	ERRORCODE ReadData(StorageDevice* pDevice);
	ERRORCODE WriteData(StorageDevice* pDevice);
	WORD SizeofData(StorageDevice* pDevice);

protected:
	CString m_csName;

	struct
	{
		WORD	m_wFlags;
		short m_nType;
		short m_nOrientation;
		short m_nClass;
		short m_nMarginStyle;

	/*
	// Paper size (full sheet)
	*/

		CFixedPoint m_PaperDims;

	/*
	// Is the origin offset from (0, 0)? If so, by how much?
	*/

		CFixedPoint m_OriginOffset;

	/*
	// A slot is a portion of the page which is *addressable*. This can be
	// the full sheet for regular paper or transparencies, or it can be little
	// portions of the page in the case of labels. Though addressable, the
	// entire slot may not be printable. That's where the margins come in
	// further on.
	*/

		short m_nSlotClass;

		short m_nSlotsAcross;
		short m_nSlotsDown;

	/*
	// We can generate all slots by starting at the top left slot (positioned
	// at m_SlotOrigin), then stepping across m_SlotStep.x and down by
	// m_SlotStep.y. Each slot will have dimensions of m_SlotDims.
	// We need to do something special for "half page perf skip" sheets, but
	// besides that, it's mostly straightforward.
	*/

		CFixedPoint m_SlotOrigin;		// Origin of top left slot
		CFixedPoint m_SlotDims;			// Width and height of slot
		CFixedPoint m_SlotStep;			// Step to next slot
		CFixed m_Radius;					// Rounded corner radius
		CFixed m_HalfPagePerfSkip;		// Distance to skip at middle perf
		short m_nHalfPageRows;			// How many rows to the perf skip

	/*
	// Physical margins.
	// These margins delimit the printable area of a slot. All slots share
	// the same physical margins.
	*/

		CFixedRect m_PhysicalMargins;

	/*
	// Minimum margins.
	// These margins contain the ideal margins for the project. These are
	// normally 1/4" for full-page projects; they will probably be smaller
	// for labels (like maybe 1/10").
	*/

		CFixedRect m_MinimumMargins;

	/*
	// Computed margins.
	// These margins are the margins which will be used. They are a combination
	// of the desired margins and the physical margins (as well as some margin
	// flags which control symmetry).
	*/

		CFixedRect m_Margins;

	} m_Record;

	// Computed project dimensions (at page resolution).
	PPNT m_ProjectDims;

	// Computed slot margins (at page resolution).
	PBOX m_PageMargins;
};

#endif				/* #ifndef __PAPER_H__ */
