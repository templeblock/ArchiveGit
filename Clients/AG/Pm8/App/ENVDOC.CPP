/*
// $Workfile: ENVDOC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVDOC.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     12/23/98 1:50p Psasse
// Code clean up
// 
//    Rev 1.0   14 Aug 1997 15:20:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:18   Fred
// Initial revision.
// 
//    Rev 1.2   03 Apr 1997 14:31:14   Jay
// Resource cleanup
// 
//    Rev 1.1   28 Mar 1997 17:10:44   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.0   14 Mar 1996 13:33:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:18   FRED
// Initial revision.
// 
//    Rev 1.6   06 Oct 1995 11:59:52   JAY
// New print code, etc.
// 
//    Rev 1.5   03 Oct 1995 10:03:44   JAY
// Label/envelope printing fixes
// 
//    Rev 1.4   30 Aug 1995 14:13:38   JAY
// Added PrintPaperInfo() method.
// 
//    Rev 1.3   09 Aug 1995 13:11:24   JAY
// Misc changes.
// 
//    Rev 1.2   28 Jul 1995 12:31:14   JAY
// Printer info. Envelope printing.
// 
//    Rev 1.1   26 Jul 1995 07:41:22   JAY
// Envelope support
// 
//    Rev 1.0   20 Jul 1995 12:52:22   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "envdoc.h"
#include "printer.h"
#include "paper.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeDoc

IMPLEMENT_SERIAL(CEnvelopeDoc, CPmwDoc, 0 /* schema number*/ )

/*
// The poster project panel.
*/

static PANEL_INFO
envelope_panels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{    0,    0,    0,    0 },
		0,
		2
	}
};

/*
// The poster project info.
*/

ProjectInfo CEnvelopeDoc::StandardInfo =
{
	PROJECT_TYPE_Envelope,
	0,
	PORTRAIT,
	sizeof(envelope_panels)/sizeof(envelope_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	envelope_panels
};

CEnvelopeDoc::CEnvelopeDoc()
{
	info = StandardInfo;
}

CEnvelopeDoc::~CEnvelopeDoc()
{
}

int CEnvelopeDoc::DefaultTextSize(int nDefault)
{
	PPNT Dims = get_dimensions();
	PCOORD Min = Dims.x < Dims.y ? Dims.x : Dims.y;
	nDefault = (int)scale_pcoord(Min, 72, PAGE_RESOLUTION);
	if ((nDefault /= 25) < 8)
	{
		nDefault = 8;
	}
	return nDefault;
}

BEGIN_MESSAGE_MAP(CEnvelopeDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CEnvelopeDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeDoc commands

int CEnvelopeDoc::EnvelopeOrientation(CPrinterInfo* pPrinterInfo)
{
	WORD wPosition = pPrinterInfo->GetEnvelopePosition();
	if ((wPosition & ENVELOPE_OMASK) == ENVELOPE_Wide)
	{
	/* Not even landscape. Return 0. */
		return 0;
	}

	int nOrientation = pPrinterInfo->Orientation();

	if (nOrientation == -1)
	{
		if (pPrinterInfo->Tractor() == 1)
		{
		/* Tractor feed can't be a 90 no matter what. */
			nOrientation = 270;
		}
		else if ((wPosition & ENVELOPE_RMASK) == ENVELOPE_RRotate)
		{
		/* Top goes in first. Print at the top. */
			nOrientation = 270;
		}
		else
		{
			nOrientation = 90;
		}
	}

	return nOrientation;
}

/*
// Resize the document to fit on the paper.
*/

void CEnvelopeDoc::size_to_paper(VOID)
{
	CPmwApp* pApp = GET_PMWAPP();
	int nPrintOrientation = GetPrintOrientation();
	CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(nPrintOrientation);
	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(nPrintOrientation);

/* Don't put any constraints on the margins. */

	m_pPaperInfo->SetMarginStyle(CPaperInfo::MARGINS_Minimal, FALSE);

	WORD wPosition = pPrinterInfo->GetEnvelopePosition();
	BOOL fWide = ((wPosition & ENVELOPE_Wide) != 0);

/*
// Remember the original physical margins.
// If they change, we invalidate at least.
*/

	CFixedRect OriginalPhysicalMargins = m_pPaperInfo->PhysicalMargins();

	CFixedRect PaperPhysicalMargins = pPaperInfo->PhysicalMargins();

	// The envelope dimensions are looking at the envelope tall-ways.
	CFixedPoint EnvelopeDims = m_pPaperInfo->PaperDims();
	CFixedPoint PaperDims = pPaperInfo->PaperDims();

	CFixed ExcessX;
	CFixed ExcessY;

	if (fWide)
	{
		ExcessX = EnvelopeDims.y - PaperDims.x;
		ExcessY = EnvelopeDims.x - PaperDims.y;
	}
	else
	{
		ExcessX = EnvelopeDims.x - PaperDims.x;
		ExcessY = EnvelopeDims.y - PaperDims.y;
	}

	CFixedRect PhysicalMargins;

	PhysicalMargins.Top = PaperPhysicalMargins.Top;
#if 0
	PhysicalMargins.Bottom = ExcessY + PaperPhysicalMargins.Bottom;
#else
	PhysicalMargins.Bottom = PaperPhysicalMargins.Bottom;
#endif

/* Figure out how the envelope lands here. */

	BOOL fFlip = FALSE;
  	BOOL fReverseLandscape = !fWide && pPrinterInfo->ReverseLandscape();

	if ((wPosition & ENVELOPE_FMASK) == ENVELOPE_FlapUp)
	{
		fFlip = !fFlip;
	}
	if (fReverseLandscape)
	{
		fFlip = !fFlip;
	}

	if (fFlip)
	{
	/*
	// Flap up reverses left and right.
	// 
	// (Cheat a little... we know this works with the current value scheme.)
	*/
		if (!(wPosition & ENVELOPE_Center))
		{
			wPosition ^= ENVELOPE_Right;
		}
	}
	
	switch (wPosition & ENVELOPE_HMASK)
	{
		case ENVELOPE_Left:
		{
			PhysicalMargins.Left = PaperPhysicalMargins.Left;
			PhysicalMargins.Right = ExcessX + PaperPhysicalMargins.Right;
			break;
		}
		case ENVELOPE_Center:
		{
		/* Evenly distribute it. */
			ExcessX >>= 1;
			PhysicalMargins.Left = ExcessX + PaperPhysicalMargins.Left;
			PhysicalMargins.Right = ExcessX + PaperPhysicalMargins.Right;
			break;
		}
		case ENVELOPE_Right:
		{
			PhysicalMargins.Left = ExcessX + PaperPhysicalMargins.Left;
			PhysicalMargins.Right = PaperPhysicalMargins.Right;
			break;
		}
		default:
		{
			ASSERT(FALSE);
			PhysicalMargins.Left =
				PhysicalMargins.Right = 0;
			break;
		}
	}

	/* Handle any margins that don't exist. */

	if (PhysicalMargins.Left < 0)
	{
		PhysicalMargins.Left = 0;
	}
	if (PhysicalMargins.Right < 0)
	{
		PhysicalMargins.Right = 0;
	}
	if (PhysicalMargins.Top < 0)
	{
		PhysicalMargins.Top = 0;
	}
	if (PhysicalMargins.Bottom < 0)
	{
		PhysicalMargins.Bottom = 0;
	}

	/* Wide envelopes have a different belief system. */

	if (fWide)
	{
		pPrinterInfo->OrientRect(&PhysicalMargins, TRUE, TRUE);
	}
	else
	{
	/*
	// We want the physical margins to be symmetric.
	// This enables us to ignore which direction the landscape is occurring.
	// Ultimatelty, we will want to handle this completely; unfortunately,
	// it will require changes all over the place that I don't have time for
	// right now.
	*/
		if (PhysicalMargins.Left < PhysicalMargins.Right)
		{
			PhysicalMargins.Left = PhysicalMargins.Right;
		}
		else
		{
			PhysicalMargins.Right = PhysicalMargins.Left;
		}
		if (PhysicalMargins.Top < PhysicalMargins.Bottom)
		{
			PhysicalMargins.Top = PhysicalMargins.Bottom;
		}
		else
		{
			PhysicalMargins.Bottom = PhysicalMargins.Top;
		}
	}

	m_pPaperInfo->SetPhysicalMargins(PhysicalMargins);

	SizeToInternalPaper();

/* In case only the physical margins changed. */

	if (PhysicalMargins != OriginalPhysicalMargins)
	{
	/* Cause all views to refresh. */
		UpdateAllViews(NULL, -1);
	}
}

CPaperInfo* CEnvelopeDoc::PrintPaperInfo(void)
{
	return GET_PMWAPP()->GetPaperInfo(GetPrintOrientation());
}

ORIENTATION CEnvelopeDoc::GetPrintOrientation(void)
{
/*
// Orientation depends on the "envelope position" stored in the info
// for this printer.
// A "wide" envelope prints on a portrait page.
// A "tall" envelope prints on a landscape page.
*/
	return (GET_PMWAPP()->GetPrinterInfo(PORTRAIT)->GetEnvelopePosition() & ENVELOPE_Wide)
				? PORTRAIT
				: LANDSCAPE;
}
