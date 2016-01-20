// Color Meter PlusDoc.cpp : implementation of the CColorMeterPlusDoc class
//

#include "stdafx.h"
#include "Color Meter Plus.h"

#include "Color Meter PlusDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusDoc

IMPLEMENT_DYNCREATE(CColorMeterPlusDoc, CDocument)

BEGIN_MESSAGE_MAP(CColorMeterPlusDoc, CDocument)
	//{{AFX_MSG_MAP(CColorMeterPlusDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusDoc construction/destruction

CColorMeterPlusDoc::CColorMeterPlusDoc()
{
	// TODO: add one-time construction code here

}

CColorMeterPlusDoc::~CColorMeterPlusDoc()
{
}

BOOL CColorMeterPlusDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusDoc serialization

void CColorMeterPlusDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusDoc diagnostics

#ifdef _DEBUG
void CColorMeterPlusDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CColorMeterPlusDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusDoc commands
