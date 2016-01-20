// YywbDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "frameobj.h"
#include "grafobj.h"
#include "YywbDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Character codes
const BYTE LF = 0x0A;
const BYTE BULLET = 0x95;

extern SHORT days_in_a_month(SHORT year, SHORT month);

/////////////////////////////////////////////////////////////////////////////
// CYywbCategory and derived classed
CPoint CYywbCategory::m_ptPush;
CSize CYywbCategory::m_WorldSize;

CYywbCategory::CYywbCategory(CYywbCategory* pNeighborAbove)
{
	// Viewable objects
	m_bSaveObjects = FALSE;
	m_pFrameObject = NULL;
	m_pGraphicObject = NULL;

	// Placement
	m_pNeighborAbove = pNeighborAbove;

	// Init category codes
	m_dwTitle = 0;
	m_dwArticle = 0;
	m_dwGraphic = 0;

	m_bChange = FALSE;

}

CYywbCategory::~CYywbCategory()
{	
	if (!m_bSaveObjects)
	{
		FreeData();
	}
}

void CYywbCategory::FreeData()
{
	// Free pointers
	if (m_pFrameObject != NULL)
	{
		delete m_pFrameObject;
		m_pFrameObject = NULL;
	}
	if (m_pGraphicObject != NULL)
	{
		delete m_pGraphicObject;
		m_pGraphicObject = NULL;
	}
}

BOOL CYywbCategory::IsValid(CYearDllInterface* pYearDllInterface)
{
	// Determine if the date returns valid data
	CString csData;
	if (m_dwGraphic != 0)
	{
		pYearDllInterface->GetData(m_dwGraphic, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
			return TRUE;
	}
	if (m_dwTitle != 0)
	{
		pYearDllInterface->GetData(m_dwTitle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
			return TRUE;
	}
	if (m_dwArticle != 0)
	{
		pYearDllInterface->GetData(m_dwArticle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
			return TRUE;
	}

	return FALSE;
}

void CYywbCategory::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	m_bChange = FALSE;
}

void CYywbCategory::ReCreate(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Free pointers, then create again
	FreeData();
	m_bChange = TRUE;
	Create(pDoc, pYearDllInterface);
}

void CYywbCategory::Display()
{

}

void CYywbCategory::UpdateDoc(CPmwDoc* pDoc)
{
	// Add graphic & frame to document
	m_bSaveObjects = TRUE;
	Attach(pDoc);
}

void CYywbCategory::Attach(CPmwDoc* pDoc)
{
	if (m_pFrameObject)
	{
		// Set the panel for the object.
		m_pFrameObject->set_panel(pDoc->get_current_panel());
		
		pDoc->append_object(m_pFrameObject);
		pDoc->refresh_object(m_pFrameObject);
	}
	if (m_pGraphicObject)
	{
		// Set the panel for the object.
		m_pGraphicObject->set_panel(pDoc->get_current_panel());
		
		pDoc->append_object(m_pGraphicObject);
		pDoc->refresh_object(m_pGraphicObject);
	}
}

void CYywbCategory::Detach(CPmwDoc* pDoc)
{
	if (m_pFrameObject)
	{
		pDoc->detach_object(m_pFrameObject);
	}
	if (m_pGraphicObject)
	{
		pDoc->detach_object(m_pGraphicObject);
	}
}

void CYywbCategory::ComputeWorldBound(CPmwDoc* pDoc)
{
	// Get the world for this panel.
	PBOX World;
	pDoc->get_panel_world(&World);

	// If world is smaller than standard, use world
	CSize nStandardDPI((int)(STANDARD_W * PAGE_RESOLUTION), (int)(STANDARD_H * PAGE_RESOLUTION));
	long nWorldWidth = World.x1 - World.x0;
	long nWorldHeight = World.y1 - World.y0;
	if (nWorldWidth < nStandardDPI.cx)
	{
		m_WorldSize.cx = nWorldWidth;
		m_ptPush.x = 0;
	}
	else
	{
		m_WorldSize.cx = nStandardDPI.cx;
		m_ptPush.x = (nWorldWidth - nStandardDPI.cx) / 2;
	}
	if (nWorldHeight < nStandardDPI.cy)
	{
		m_WorldSize.cy = nWorldHeight;
		m_ptPush.y = 0;
	}
	else
	{
		m_WorldSize.cy = nStandardDPI.cy;
		m_ptPush.y = (nWorldHeight - nStandardDPI.cy) / 2;
	}
}

void CYywbCategory::ComputeLocalBound(CPmwDoc* pDoc, InchFracBoundT& InchBound, PBOX& Bound, int& nFontSize, BOOL bKeepAspect /*= FALSE*/, enum GraphicLinesE GLine /*= NO_G_LINE*/)
{
	// Get the world for this panel.
	PBOX World;
	pDoc->get_panel_world(&World);

	// Set the dimensions
	Bound.x0 = World.x0 + m_ptPush.x + (long)(InchBound.left * m_WorldSize.cx);
	Bound.y0 = World.y0 + m_ptPush.y + (long)(InchBound.top * m_WorldSize.cy);
	Bound.x1 = Bound.x0 + (long)(InchBound.width * m_WorldSize.cx);
	Bound.y1 = Bound.y0 + (long)(InchBound.height * m_WorldSize.cy);
	if (bKeepAspect)
	{
		Bound.y1 = Bound.y0 + (long)((InchBound.width * m_WorldSize.cx) * (InchBound.height / InchBound.width));
	}

	// Determine a minimum font size
	nFontSize = (int)(STANDARD_PT_SIZE * (double)m_WorldSize.cy / (STANDARD_H * PAGE_RESOLUTION));
	
	// What the hell was I thinking?!
#if 0
	if (nFontSize < 6) 
	{
		// Too small-- put in middle
		if (GLine == NO_G_LINE)
		{
			// Adjust bound to keep aspect ratio
			Bound = World;
			if (InchBound.width > InchBound.height)
			{
				long nH = (long)((World.x1 - World.x0) * (InchBound.height / InchBound.width));
				Bound.y0 = ((Bound.y1 - Bound.y0) - nH) / 2;
				Bound.y1 = Bound.y0 + nH;
			}
			else
			{
				long nW = (long)((World.y1 - World.y0) * (InchBound.width / InchBound.height));
				Bound.x0 = ((Bound.x1 - Bound.x0) - nW) / 2;
				Bound.x1 = Bound.x0 + nW;
			}
		}
		else
		{
			// Center along x or y
			if (GLine == G_LINE_X)
			{
				// Fix x, center using y
				long nH = Bound.y1 - Bound.y0;
				Bound.y0 = ((World.y1 - World.y0) - nH) / 2;
				Bound.y1 = Bound.y0 + nH;
			}
			if (GLine == G_LINE_Y)
			{
				// Fix y, center using x
				long nW = Bound.x1 - Bound.x0;
				Bound.x0 = ((World.x1 - World.x0) - nW) / 2;
				Bound.x1 = Bound.x0 + nW;
			}
		}

		// Try again
		nFontSize = (int)(STANDARD_PT_SIZE * (double)(Bound.y1 - Bound.y0) / (STANDARD_H * PAGE_RESOLUTION));
		if (nFontSize < 6) nFontSize = 6;
	}
#endif
}

BOOL CYywbCategory::CreateFrame(CPmwDoc* pDoc, enum ALIGN_TYPE align, CTxp& Txp, int& nFontSize)
{
	// End immediately if not dirty
	if (!m_bChange)
		return FALSE;

	// Get bounds
	PBOX Bound;
	ComputeLocalBound(pDoc, m_InchBound, Bound, nFontSize);

	FRAMEOBJ_CREATE_STRUCT fcs;
	fcs.bound = Bound;
	fcs.alignment = align;
	fcs.vert_alignment = ALIGN_top;

	// Do the creation
	if (m_pFrameObject != NULL)
	{
		delete m_pFrameObject;
	}
	if ((m_pFrameObject = pDoc->create_frame_object(&fcs)) != NULL)
	{
		// Initialize text
		Txp.Init(m_pFrameObject, 0);
		Txp.SetHorizontalAlignment(align);
		Txp.CalcStyle();

		return TRUE;
	}

	return FALSE;	
}

BOOL CYywbCategory::CreateGraphic(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface, InchFracBoundT& InchBound, enum GraphicLinesE GLine)
{
	// End immediately if not dirty
	if (!m_bChange)
		return FALSE;

	// Get bounds, preserving aspect ratio
	PBOX Bound;
	int nDum;
	ComputeLocalBound(pDoc, InchBound, Bound, nDum, TRUE, GLine);

	// Create the graphic
	GRAPHIC_CREATE_STRUCT gcs;
	gcs.proof = TRUE;
	
	CString csData;
	pYearDllInterface->GetData(m_dwGraphic, csData);
	CString csFilename = GetGlobalPathManager()->ExpandPath("YYWB\\") + csData;
	gcs.m_csFileName = csFilename;
	gcs.m_csFriendlyName.Empty();

	if (m_pGraphicObject != NULL)
	{
		delete m_pGraphicObject;
	}
	if ((m_pGraphicObject = (pDoc->get_database())->create_graphic_object(&gcs)) != NULL)
	{
		// Set bounds.
		m_pGraphicObject->set_bound(Bound);
		m_pGraphicObject->calc();

		return TRUE;
	}

	return FALSE;	
}

BOOL CYywbCategory::CreateBulletFrame(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	enum ALIGN_TYPE align = ALIGN_left;
	int nFontSize;
	CTxp Txp(pDoc->get_database());
	if (CYywbCategory::CreateFrame(pDoc, align, Txp, nFontSize))
	{
		CTextStyle Style(pDoc->get_database());
		Style.SetDefault();
		int nBigFontSize = (int)(nFontSize * 1.8);

		// Find the text
		CString csData;
		
		// Title
		pYearDllInterface->GetData(m_dwTitle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
		{
			csData += "\n";
			Style.BaseSize(MakeFixed(nBigFontSize));
			Style.Bold(TRUE);
			Txp.Style(Style);
			Txp.InsertString(csData);
		}

		// Article (here's where it gets interesting)
		pYearDllInterface->GetData(m_dwArticle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
		{
			Style.BaseSize(MakeFixed(nFontSize));
			Style.Bold(FALSE);
			Txp.Style(Style);

			// Search for bullet lines
			CString csString = csData;
			CString csBullet;
			while (!csString.IsEmpty())
			{
				int nCountIn = csString.Find(LF);
				if (nCountIn != -1)
				{
					csBullet = csString.Left(nCountIn+1);
					csString = csString.Mid(nCountIn+1);	// trim off
				}
				else	// tail end
				{
					csBullet = csString;
					csString.Empty();
				}

				// Add the bullet and insert
				csBullet = "  " + csBullet;
				csBullet.SetAt(0, BULLET);
				Txp.InsertString(csBullet);
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CYywbCategory::CreateParagraphFrame(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface, UINT nID, BOOL bArticle /* = TRUE*/)
{
	enum ALIGN_TYPE align = ALIGN_left;
	int nFontSize;
	CTxp Txp(pDoc->get_database());
	if (CYywbCategory::CreateFrame(pDoc, align, Txp, nFontSize))
	{
		CTextStyle Style(pDoc->get_database());
		Style.SetDefault();
		int nBigFontSize = (int)(nFontSize * 1.8);
		
		// Find the text
		CString csData;
		
		// Title
		pYearDllInterface->GetData(m_dwTitle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
		{
			// Add the prefix string
			CString csTitle;
			if (nID)
			{
				CString csHalfTitle;
				csHalfTitle.LoadString(nID);
				csTitle.Format((const char*)csHalfTitle, (const char*)csData);
			}
			else
			{
				csTitle = csData;
			}
			csTitle += "\n";

			Style.BaseSize(MakeFixed(nBigFontSize));
			Style.Bold(TRUE);
			Txp.Style(Style);
			Txp.InsertString(csTitle);
		}

		// Article
		if (bArticle)
		{
			pYearDllInterface->GetData(m_dwArticle, csData);
			if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
			{
				Style.BaseSize(MakeFixed(nFontSize));
				Style.Bold(FALSE);
				Txp.Style(Style);
				Txp.InsertString(csData);
			}
		}

		return TRUE;
	}

	return FALSE;
}

// CYywbTime
CYywbTime::CYywbTime(CYywbCategory* pNeighborAbove)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName.LoadString(IDS_TIME_CATEGORY);

	m_dwTitle = catIntroductionTitle;
	m_dwArticle = catTimeArticle;

	// Set bounds
	m_InchBound.left =		0.75	/ STANDARD_W;	// from left
	m_InchBound.top =		0.25	/ STANDARD_H;	// from top
	m_InchBound.width =		6.25	/ STANDARD_W;	// width
	m_InchBound.height =	0.875	/ STANDARD_H;	// height
}

void CYywbTime::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Create the frame
	enum ALIGN_TYPE align = ALIGN_center;
	int nFontSize;
	CTxp Txp(pDoc->get_database());
	if (CYywbCategory::CreateFrame(pDoc, align, Txp, nFontSize))
	{
		CTextStyle Style(pDoc->get_database());
		Style.SetDefault();
		
		// Find the text
		CString csData;
		
		// Title
		pYearDllInterface->GetData(m_dwTitle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
		{
			csData += "\n";
			Style.BaseSize(MakeFixed((int)(nFontSize * 1.8)));
			Style.Bold(TRUE);
			Txp.Style(Style);
			Txp.InsertString(csData);
		}

		// Article
		pYearDllInterface->GetData(m_dwArticle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
		{
			Style.BaseSize(MakeFixed((int)(nFontSize * 1.2)));
			Style.Bold(TRUE);
			Txp.Style(Style);
			Txp.InsertString(csData);
		}
	}

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbTime::Display()
{

}

// CYywbNews
CYywbNews::CYywbNews(CYywbCategory* pNeighborAbove)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName.LoadString(IDS_NEWS_CATEGORY);

	m_dwTitle = catNewsTitle;
	m_dwArticle = catNewsArticle;

	// Set bounds
	m_InchBound.left =		0.625	/ STANDARD_W;	// from left
	m_InchBound.top =		0.125	/ STANDARD_H + m_pNeighborAbove->GetBottom();	// from top
	m_InchBound.width =		6.625	/ STANDARD_W;	// width
	m_InchBound.height =	1.875	/ STANDARD_H;	// height
}

void CYywbNews::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Create the frame
	enum ALIGN_TYPE align = ALIGN_left;
	int nFontSize;
	CTxp Txp(pDoc->get_database());
	if (CYywbCategory::CreateFrame(pDoc, align, Txp, nFontSize))
	{
		CTextStyle Style(pDoc->get_database());
		Style.SetDefault();
		int nBigFontSize = (int)(nFontSize * 1.8);

		// Find the text
		CString csData;
		
		// Title
		pYearDllInterface->GetData(m_dwTitle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
		{
			csData += "\n";
			Style.BaseSize(MakeFixed(nBigFontSize));
			Style.Bold(TRUE);
			Txp.Style(Style);
			Txp.InsertString(csData);
		}

		// Article
		pYearDllInterface->GetData(m_dwArticle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
		{
			Style.BaseSize(MakeFixed(nFontSize));
			Style.Bold(FALSE);
			Txp.Style(Style);
			Txp.InsertString(csData);
		}
	}

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbNews::Display()
{

}

// CYywbEntertainment
CYywbEntertainment::CYywbEntertainment(CYywbCategory* pNeighborAbove)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName.LoadString(IDS_ENTERTAINMENT_CATEGORY);

	m_dwTitle = catEntertainmentTitle;
	m_dwArticle = catEntertainmentArticle;

	// Set bounds
	m_InchBound.left =		0.625	/ STANDARD_W;	// from left
	m_InchBound.top =		0.0625	/ STANDARD_H + m_pNeighborAbove->GetBottom();	// from top
	m_InchBound.width =		3.25	/ STANDARD_W;	// width
	m_InchBound.height =	2.875	/ STANDARD_H;	// height
}

void CYywbEntertainment::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Do it "bullet style"
	CreateBulletFrame(pDoc, pYearDllInterface);

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbEntertainment::Display()
{

}

// CYywbSports
CYywbSports::CYywbSports(CYywbCategory* pNeighborAbove)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName.LoadString(IDS_SPORTS_CATEGORY);

	m_dwTitle = catSportTitle;
	m_dwArticle = catSportArticle;

	// Set bounds
	m_InchBound.left =		4.0		/ STANDARD_W;	// from left
	m_InchBound.top =		0.0625	/ STANDARD_H + m_pNeighborAbove->GetBottom();	// from top
	m_InchBound.width =		3.25	/ STANDARD_W;	// width
	m_InchBound.height =	2.875	/ STANDARD_H;	// height	
}

void CYywbSports::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Do it "bullet style"
	CreateBulletFrame(pDoc, pYearDllInterface);

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbSports::Display()
{

}

// CYywbFamous
CYywbFamous::CYywbFamous(CYywbCategory* pNeighborAbove)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName.LoadString(IDS_FAMOUS_CATEGORY);

	m_dwTitle = catFamousTitle;
	m_dwArticle = catFamousArticle;

	// Set bounds
	m_InchBound.left =		2.125	/ STANDARD_W;	// from left
	m_InchBound.top =		0.0625	/ STANDARD_H + m_pNeighborAbove->GetBottom();	// from top
	m_InchBound.width =		3.625	/ STANDARD_W;	// width
	m_InchBound.height =	1.625	/ STANDARD_H;	// height	
}

void CYywbFamous::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Do it "bullet style"
	CreateBulletFrame(pDoc, pYearDllInterface);

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbFamous::Display()
{

}

// CYywbZodiac
CYywbZodiac::CYywbZodiac(CYywbCategory* pNeighborAbove)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName.LoadString(IDS_ZODIAC_CATEGORY);

	m_dwTitle = catZodiacTitle;
	m_dwArticle = catZodiacArticle;
	m_dwGraphic = catZodiacGraphic;

	// Set bounds
	m_InchBound.left =		2.25	/ STANDARD_W;	// from left
	m_InchBound.top =		0.0625	/ STANDARD_H + m_pNeighborAbove->GetBottom();	// from top
	m_InchBound.width =		4.75	/ STANDARD_W;	// width
	m_InchBound.height =	1.25	/ STANDARD_H;	// height	
}

void CYywbZodiac::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Build the text frame
	CreateParagraphFrame(pDoc, pYearDllInterface, IDS_ZODIAC_TITLE);

	// Build the graphic
	InchFracBoundT InchBound = {
		m_InchBound.left - 1.5 / STANDARD_W,	// from left
		m_InchBound.top + 0.125 / STANDARD_H,	// from top
		1.25 / STANDARD_W,	// width
		1.25 / STANDARD_H,	// height
	};
	CreateGraphic(pDoc, pYearDllInterface, InchBound, G_LINE_X);

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbZodiac::Display()
{

}

// CYywbChinese
CYywbChinese::CYywbChinese(CYywbCategory* pNeighborAbove)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName.LoadString(IDS_CHINESE_CATEGORY);

	m_dwTitle = catChineseYearTitle;
	m_dwArticle = catChineseYearArticle;
	m_dwGraphic = catChineseYearGraphic;

	// Set bounds
	m_InchBound.left =		2.25	/ STANDARD_W;	// from left
	m_InchBound.top =		0.0625	/ STANDARD_H + m_pNeighborAbove->GetBottom();	// from top
	m_InchBound.width =		3.25	/ STANDARD_W;	// width
	m_InchBound.height =	0.75	/ STANDARD_H;	// height
}

void CYywbChinese::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Build the text frame
	CreateParagraphFrame(pDoc, pYearDllInterface, IDS_CHINESE_TITLE);

	// Build the graphic
	InchFracBoundT InchBound = {
		m_InchBound.left + m_InchBound.width + 0.25 / STANDARD_W,	// from left
		m_InchBound.top - 0.125 / STANDARD_H,	// from top
		1.25 / STANDARD_W,	// width
		1.25 / STANDARD_H,	// height
	};
	CreateGraphic(pDoc, pYearDllInterface, InchBound, G_LINE_X);

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbChinese::Display()
{

}

// CYywbStoned
CYywbStoned::CYywbStoned(CYywbCategory* pNeighborAbove)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName.LoadString(IDS_BIRTHSTONE_CATEGORY);

	m_dwTitle = catBirthstoneTitle;

	// Set bounds
	m_InchBound.left =		1.0		/ STANDARD_W;	// from left
	m_InchBound.top =		0.0625	/ STANDARD_H + m_pNeighborAbove->GetBottom();	// from top
	m_InchBound.width =		5.5		/ STANDARD_W;	// width
	m_InchBound.height =	0.5		/ STANDARD_H;	// height
}

void CYywbStoned::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Build the text frame
	CreateParagraphFrame(pDoc, pYearDllInterface, IDS_BIRTHSTONE_TITLE, FALSE);

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbStoned::Display()
{

}

// CYywbExternal
CYywbExternal::CYywbExternal(CYywbCategory* pNeighborAbove, CString& csName, DWORD dwCode)
	: CYywbCategory(pNeighborAbove)
{
	// Init name & category codes
	m_csName = csName;
	m_dwTitle = dwCode;

	// Set bounds
	m_InchBound.left =		1.5		/ STANDARD_W;	// from left
	m_InchBound.top =		4.0		/ STANDARD_H;	// from top
	m_InchBound.width =		5.0		/ STANDARD_W;	// width
	m_InchBound.height =	0.5		/ STANDARD_H;	// height
}

void CYywbExternal::Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface)
{
	// Build the text frame
	int nFontSize;
	CTxp Txp(pDoc->get_database());
	if (CYywbCategory::CreateFrame(pDoc, ALIGN_center, Txp, nFontSize))
	{
		CTextStyle Style(pDoc->get_database());
		Style.SetDefault();
		int nBigFontSize = (int)(nFontSize * 1.8);

		// Find the text
		CString csData;
		
		// Title
		pYearDllInterface->GetData(m_dwTitle, csData);
		if (pYearDllInterface->GetErrorCode() == ERR_STATUSOK)
		{
			csData += "\n";

			Style.BaseSize(MakeFixed(nBigFontSize));
			Style.Bold(TRUE);
			Txp.Style(Style);
			Txp.InsertString(csData);
		}
	}

	CYywbCategory::Create(pDoc, pYearDllInterface);
}

void CYywbExternal::Display()
{
	CYywbCategory::Display();
}

/////////////////////////////////////////////////////////////////////////////
// CCategoryListBox

CCategoryListBox::CCategoryListBox()
{
}

CCategoryListBox::~CCategoryListBox()
{
}

void CCategoryListBox::AddCategory(CYywbCategory* pCategory)
{
	CString csCategoryName;
	csCategoryName.Format(" %s", pCategory->GetCategoryName());
	int nIndex = AddString(csCategoryName);
	if (nIndex >= 0)
	{
		SetItemDataPtr(nIndex, (void*)pCategory);
	}
}

BEGIN_MESSAGE_MAP(CCategoryListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CCategoryListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYywbDlg dialog
CString	CYywbDlg::m_csName = _T("");
CString	CYywbDlg::m_csBirthdate = _T("");

CYywbDlg::CYywbDlg(CPmwView* pView, CWnd* pParent /*=NULL*/)
	: CDialog(CYywbDlg::IDD, pParent)
{
	// Members
	m_pView = pView;
	m_bValidDate = FALSE;
	m_nCategory = -1;

	// Set default birthdate
	m_SystemTime.wYear = 1973;
	m_SystemTime.wMonth = 3;
	m_SystemTime.wDayOfWeek = 0;
	m_SystemTime.wDay = 5;
	m_SystemTime.wHour = 0;
	m_SystemTime.wMinute = 0;
	m_SystemTime.wSecond = 0;
	m_SystemTime.wMilliseconds = 0;

	// Create the array of category objects
	int nCategories = 8;
	m_aCategories.SetSize(nCategories);
	m_aCategories[0] = new CYywbTime(NULL);
	m_aCategories[1] = new CYywbNews((CYywbCategory*)m_aCategories[0]);
	m_aCategories[2] = new CYywbEntertainment((CYywbCategory*)m_aCategories[1]);
	m_aCategories[3] = new CYywbSports((CYywbCategory*)m_aCategories[1]);
	m_aCategories[4] = new CYywbFamous((CYywbCategory*)m_aCategories[2]);
	m_aCategories[5] = new CYywbZodiac((CYywbCategory*)m_aCategories[4]);
	m_aCategories[6] = new CYywbChinese((CYywbCategory*)m_aCategories[5]);
	m_aCategories[7] = new CYywbStoned((CYywbCategory*)m_aCategories[6]);

	// Scrap doc
	m_pScrapDoc = GET_PMWAPP()->NewHiddenDocument();
	m_pScrapDoc->size_to_paper();

	//{{AFX_DATA_INIT(CYywbDlg)
	//}}AFX_DATA_INIT
}

CYywbDlg::~CYywbDlg()
{
	// Delete category objects
	int i;
	int nCategories = m_aCategories.GetSize();
	for (i = 0; i < nCategories; i++)
	{
		delete (CYywbCategory*)m_aCategories[i];
	}

	// Delete scrap doc
	delete m_pScrapDoc;
}

void CYywbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CYywbDlg)
	DDX_Text(pDX, IDC_ED_NAME, m_csName);
	DDV_MaxChars(pDX, m_csName, 64);
	DDX_Text(pDX, IDC_ED_BIRTHDATE, m_csBirthdate);
	DDV_MaxChars(pDX, m_csBirthdate, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CYywbDlg, CDialog)
	//{{AFX_MSG_MAP(CYywbDlg)
	ON_EN_CHANGE(IDC_ED_BIRTHDATE, OnChangeEdBirthdate)
	ON_EN_CHANGE(IDC_ED_NAME, OnChangeEdName)
	ON_LBN_SELCHANGE(IDC_CATEGORY_LIST, OnSelchangeCategoryList)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_SELECT_NONE, OnSelectNone)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYywbDlg member functions

void CYywbDlg::EnableCategories(BOOL bEnabled)
{
	GetDlgItem(IDC_CATEGORIES)->EnableWindow(bEnabled);
	GetDlgItem(IDC_CATEGORY_LIST)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SELECT_ALL)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SELECT_NONE)->EnableWindow(bEnabled);
}

// Parse date string to determine a valid system date
void CYywbDlg::ParseBirthdate()
{	
	int nMonth = -1;
	int nDay = -1;
	int nYear = -1;

	m_bValidDate = FALSE;	
	TRY
	{
		// Load the separators which can be used to separate elements of the date.
		CString csSeparators;
		csSeparators.LoadString(IDS_DATE_SEPARATORS);
		
		// Collect all the elements in the day string.
		CString csDay = m_csBirthdate;
		CWordArray cwaElements;
		CString csElement;
		while (GetNextElement(csElement, csDay, csSeparators))
		{
			BOOL fKeep = FALSE;
				
			// If we see a month name go by, remember it.
			if (!csElement.IsEmpty())
			{
				if (isdigit(csElement[0]))
				{
					DWORD dwTest = (DWORD)atol(csElement);
					if((dwTest < 65535) && (dwTest >= 0))
					  cwaElements.Add((WORD)atoi(csElement));
				}
				else
				{
					// Only keep the first month name.
					if (nMonth == -1)
					{
						nMonth = GetMonth(csElement)+1;
					}
				}
			}
		}
		
		// Make sure the number of elements makes sense.
		int nElements = cwaElements.GetSize();
		
		if (nMonth != -1)
		{
			// We saw a real month name, so all we have to do is
			// find the day and year number. For now, just use the first number
			// for the day, and the second for the year
			if (nElements >= 1)
			{
				nDay = (int)cwaElements.GetAt(0);
				
				if(cwaElements.GetSize() > 1)
				  nYear = (int)cwaElements.GetAt(1);
			}
		}
		else
		{
			// We did not see an explicit month name. Thus we could have a date
			// composed of only numbers.
			if (nElements == 3)
			{
				int nM = -1;
				int nD = -1;
				int nY = -1;
				int nIndex = 0;
				int i;
				
				// Get the preferred date format.
				CString csDateFormat;
				csDateFormat.LoadString(IDS_DATE_FORMAT);
				
				// Get MDY ordering
				for (i = 0; i < csDateFormat.GetLength(); i++)
				{
					if (csDateFormat[i] == 'M')
					{
						if (nM == -1)
						{
							nM = nIndex++;
						}
					}
					else if (csDateFormat[i] == 'D')
					{
						if (nD == -1)
						{
							nD = nIndex++;
						}
					}
					else if (csDateFormat[i] == 'Y')
					{
						if (nY == -1)
						{
							nY = nIndex++;
						}
					}
				}

				// Assign default values.				
				if (nM == -1)
				{
					nM = nIndex++;
				}
				if (nD == -1)
				{
					nM = nIndex++;
				}
				if (nY == -1)
				{
					nY = nIndex++;
				}
								
				// Search for the month, day, and year.
				for (i = 0; i <= 2; i++)
				{
					for (int nElement = 0; nElement < cwaElements.GetSize(); nElement++)
					{
						int nValue = cwaElements.GetAt(nElement);
						if (i == nD)
						{
							// Found day.
							nDay = nValue;
							cwaElements.RemoveAt(nElement);
							break;
						}
						if (i == nM)
						{
							// Found month.
							nMonth = nValue;
							cwaElements.RemoveAt(nElement);
							break;
						}
						if (i == nY)
						{
							// Found year.
							nYear = nValue;
							cwaElements.RemoveAt(nElement);
							break;
						}
					}
				}
			}
		}
	}
	END_TRY

	// Bounds check
	if (nDay < 1 || nDay > 31) nDay = -1;
	if (nMonth < 1 || nMonth > 12) nMonth = -1;
	if (nYear > 2500) return;	// behave!
	if (nYear < 100) nYear += 1900;	// Assume 20th century-- may need to change in a few years!

	if(nDay <= days_in_a_month(nYear, (nMonth - 1)))
	{

		// At this point we should have a month, day, and year. If we do, celebrate.
		if ((nDay != -1) && (nMonth != -1) && (nYear != -1))
		{
			// Fill int the time structure
			m_SystemTime.wDay = nDay;
			m_SystemTime.wMonth = nMonth;
			m_SystemTime.wYear = nYear;

			m_bValidDate = TRUE;
		}
	}
}

// Try to figure out what month the given string refers to.
// Return -1 on no match.
int CYywbDlg::GetMonth(const CString& csMonth) const
{
	static UINT MonthNames[] =
	{
		IDS_JANUARY,
		IDS_FEBRUARY,
		IDS_MARCH,
		IDS_APRIL,
		IDS_MAY,
		IDS_JUNE,
		IDS_JULY,
		IDS_AUGUST,
		IDS_SEPTEMBER,
		IDS_OCTOBER,
		IDS_NOVEMBER,
		IDS_DECEMBER
	};
	
	int nMonth = -1;
	
	if (csMonth.GetLength() >= 3)
	{
		// Have at least three characters of month name.
		for (int nMonthName = 0; nMonthName < sizeof(MonthNames)/sizeof(MonthNames[0]); nMonthName++)
		{
			// Check for a month name match.
			CString csMonthName;
			csMonthName.LoadString(MonthNames[nMonthName]);
			if (csMonthName.GetLength() >= csMonth.GetLength())
			{
				if (csMonth.CompareNoCase(csMonthName.Left(csMonth.GetLength())) == 0)
				{
					// Got the month.
					nMonth = nMonthName;
				}
			}
		}
	}
	
	return nMonth;
}
#if 0
void CYywbDlg::IsCharacterLast(CString& csString, const CString& csCharacters) const
{
	TRY
	{
		if (!csString.IsEmpty())
		{
			// Strip off any run of the given characters.
			CString csRemove = csString.SpanIncluding(csCharacters);
			if (!csRemove.IsEmpty())
			{
				char c = csRemove[csRemove.GetLength()];
				csString = csString.Mid(csRemove.GetLength());
			}
		}
	}
	END_TRY
}
#endif

void CYywbDlg::RemoveCharacters(CString& csString, const CString& csCharacters) const
{
	TRY
	{
		if (!csString.IsEmpty())
		{
			// Strip off any run of the given characters.
			CString csRemove = csString.SpanIncluding(csCharacters);
			if (!csRemove.IsEmpty())
			{
				csString = csString.Mid(csRemove.GetLength());
			}
		}
	}
	END_TRY
}

BOOL CYywbDlg::GetNextElement(CString& csElement, CString& csString, const CString& csSeparators) const
{
	BOOL fResult = FALSE;
	
	TRY
	{
		csElement.Empty();
		
		// Make sure the string is not empty, otherwise, we're done.
		if (!csString.IsEmpty())
		{
			// Strip off any separator characters.
			RemoveCharacters(csString, csSeparators);
			
			if(csString != "")
			{
				// Get the element (run of digits or non-separator characters.)
				if (isdigit(csString[0]))
				{
					csElement = csString.SpanIncluding("0123456789");
				}
				else
				{
					csElement = csString.SpanExcluding(csSeparators);
				}
				if (!csElement.IsEmpty())
				{
					// Remove element from original string.
					csString = csString.Mid(csElement.GetLength());
					
					// Sweet smell of success!
					fResult = TRUE;
				}
			}
		}
	}
	END_TRY
	
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CYywbDlg message handlers

BOOL CYywbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// NOTE: library REQUIRES trailing backslash on category paths!
	CString csPath = GetGlobalPathManager()->ExpandPath("YYWB\\");
	if (m_YearDll.Initialize(csPath, csPath, csPath, m_pYearDllInterface) != ERR_STATUSOK)
	{
		TRACE0("Error: Couldn't find Yywb.dll!\n");
		EndDialog(ID_YYWB_ERROR);
		return FALSE;
	}

	// Set up catergories
	if (!m_lbCategory.SubclassDlgItem(IDC_CATEGORY_LIST, this))
	{
		TRACE0("Error: Couldn't subclass IDC_CATEGORY_LIST!\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Find external categories
	CategoryData* pCategories = NULL;
	int nLength = m_pYearDllInterface->GetCategories(NULL);
	if (nLength > 0)

	pCategories = (CategoryData*)(new char[nLength]);
	if (pCategories != NULL)
	{
		nLength = m_pYearDllInterface->GetCategories(pCategories);
		if (nLength >= 0)
		{	
			CategoryData* pCategory = pCategories;
			while (pCategory->szCategoryName[0] != '\0')
			{
				if (pCategory->dwCategoryCode >= catFirstExternal)
				{
					CString csCategoryName = pCategory->szCategoryName;
					CYywbCategory* pExternalCategory = new CYywbExternal(NULL, csCategoryName, pCategory->dwCategoryCode);
					m_aCategories.Add(pExternalCategory);
				}
				pCategory++;
			}
		}
		delete [] (char*)pCategories;
	}

	// Add the categories to the list box
	int i;
	int nCategories = m_aCategories.GetSize();
	m_lbCategory.ResetContent();
	for (i = 0; i < nCategories; i++)
	{
		m_lbCategory.AddCategory((CYywbCategory*)m_aCategories[i]);
	}

	// Set up picture preview
	m_ppCategoryPict.Initialize(this, IDC_CATEGORY_PREVIEW);

	// Set focus to name field
	GetDlgItem(IDC_ED_NAME)->SetFocus();

	CheckForEnabledState();

	// Show that guy
	GET_PMWAPP()->ShowHelpfulHint("YYWB", this);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CYywbDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// Detach the preview
	if (m_nCategory != -1)
	{
		CYywbCategory* pCategory = (CYywbCategory*)m_lbCategory.GetItemDataPtr(m_nCategory);
		pCategory->Detach(m_pScrapDoc);
	}	
}


void CYywbDlg::OnChangeEdBirthdate() 
{
	CheckForEnabledState();
}

void CYywbDlg::OnChangeEdName() 
{
	CheckForEnabledState();
}

void CYywbDlg::CheckForEnabledState()
{
	// Check for a valid birthdate
	UpdateData(TRUE);
	ParseBirthdate();
	
	if (m_bValidDate && (((CEdit*)GetDlgItem(IDC_ED_NAME))->LineLength()))
	{
		// Enable & initialize categories
		EnableCategories(TRUE);
		m_pYearDllInterface->SetDateAndName(m_SystemTime, m_csName);

		// Change view objects
		int i;
		int nCategories = m_aCategories.GetSize();
		for (i = 0; i < nCategories; i++)
		{
			CYywbCategory* pCategory = (CYywbCategory*)m_lbCategory.GetItemDataPtr(i);
			pCategory->Change(TRUE);

			// Check for validity
			m_lbCategory.Enable(i, pCategory->IsValid(m_pYearDllInterface));
		}
		
		// Update the current selection
		if (m_nCategory != -1)
		{
			PreviewSelection(m_nCategory, m_nCategory);
		}
	}
	else
	{
		EnableCategories(FALSE);
	}
	
}

void CYywbDlg::OnSelchangeCategoryList() 
{
	int nCategory = m_lbCategory.GetCurSel();
	if (nCategory != LB_ERR && nCategory != m_nCategory)
	{
		// Create, append & display the new data
		PreviewSelection(m_nCategory, nCategory);
	}
	m_nCategory = nCategory;
}

void CYywbDlg::PreviewSelection(int nOldCategory, int nNewCategory)
{
	CYywbCategory* pCategory;
	
	// Detach the old preview
	if (nOldCategory != -1)
	{
		pCategory = (CYywbCategory*)m_lbCategory.GetItemDataPtr(nOldCategory);
		pCategory->Detach(m_pScrapDoc);
	}

	CYywbCategory::ComputeWorldBound(m_pScrapDoc);
	pCategory = (CYywbCategory*)m_lbCategory.GetItemDataPtr(nNewCategory);
	pCategory->Create(m_pScrapDoc, m_pYearDllInterface);
	pCategory->Attach(m_pScrapDoc);
	m_ppCategoryPict.NewDocPreview(m_pScrapDoc, TRUE);
	GetDlgItem(IDC_CATEGORY_PREVIEW)->Invalidate();
	UpdateData(FALSE);
}

void CYywbDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (nIDCtl == IDC_CATEGORY_PREVIEW)
	{
		m_ppCategoryPict.DrawPreview(lpDrawItemStruct);
	}
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CYywbDlg::OnSelectAll() 
{
	// Select all categories
	int i;
	int nCount = m_lbCategory.GetCount();
	for (i = 0; i < nCount; i++)
	{
		if (m_lbCategory.IsEnabled(i))
		{
			m_lbCategory.SetCheck(i, 1);
		}
	}
}

void CYywbDlg::OnSelectNone() 
{
	// Un-select all categories
	int i;
	int nCount = m_lbCategory.GetCount();
	for (i = 0; i < nCount; i++)
	{
		m_lbCategory.SetCheck(i, 0);
	}
	
}

void CYywbDlg::OnOK() 
{
	CYywbCategory* pCategory;

	// This may take awhile
	HCURSOR hCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));

	// Detach the old preview
	if (m_nCategory != -1)
	{
		pCategory = (CYywbCategory*)m_lbCategory.GetItemDataPtr(m_nCategory);
		pCategory->Detach(m_pScrapDoc);
		m_nCategory = -1;
	}

	// Update document with selected categories
	int i;
	int nCategories = m_aCategories.GetSize();
	CPmwDoc* pDoc = (CPmwDoc*)m_pView->GetDocument();
	CYywbCategory::ComputeWorldBound(pDoc);
	for (i = 0; i < nCategories; i++)
	{
		pCategory = (CYywbCategory*)m_lbCategory.GetItemDataPtr(i);
		if (m_lbCategory.GetCheck(i) == 1)
		{
			// Create & pass to the doc
			pCategory->ReCreate(pDoc, m_pYearDllInterface);
			pCategory->UpdateDoc(pDoc);
		}
	}
	
	CDialog::OnOK();

	// Restore
	::SetCursor(hCursor);
}
