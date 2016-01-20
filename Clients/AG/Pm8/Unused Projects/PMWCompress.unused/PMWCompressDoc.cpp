// PMWCompressDoc.cpp : implementation of the CPMWCompressDoc class
//

#include "stdafx.h"
#include "PMWCompress.h"

#include "PMWCompressDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPMWCompressDoc

IMPLEMENT_DYNCREATE(CPMWCompressDoc, CDocument)

BEGIN_MESSAGE_MAP(CPMWCompressDoc, CDocument)
	//{{AFX_MSG_MAP(CPMWCompressDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPMWCompressDoc construction/destruction

CPMWCompressDoc::CPMWCompressDoc()
{
	// TODO: add one-time construction code here

}

CPMWCompressDoc::~CPMWCompressDoc()
{
}

BOOL CPMWCompressDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPMWCompressDoc serialization

void CPMWCompressDoc::Serialize(CArchive& ar)
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
// CPMWCompressDoc diagnostics

#ifdef _DEBUG
void CPMWCompressDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPMWCompressDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPMWCompressDoc commands
