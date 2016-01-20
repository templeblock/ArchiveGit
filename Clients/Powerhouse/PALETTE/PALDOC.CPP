// paldoc.cpp : implementation of the CPaletteDoc class
//

#include "stdafx.h"
#include "palette.h"
#include "dib.h"
#include "dibpal.h"

#include "paldoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteDoc

IMPLEMENT_DYNCREATE(CPaletteDoc, CDocument)

BEGIN_MESSAGE_MAP(CPaletteDoc, CDocument)
	//{{AFX_MSG_MAP(CPaletteDoc)
	ON_COMMAND(ID_FILE_FROMDIB, OnFileFromDIB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteDoc construction/destruction

CPaletteDoc::CPaletteDoc()
{
	m_pPal = NULL;
}

CPaletteDoc::~CPaletteDoc()
{
	if (m_pPal) delete m_pPal;
}

BOOL CPaletteDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	// delete any existing palette
	if (m_pPal) {
		delete m_pPal;
		m_pPal = NULL;
	}
	// create a new palette with just one entry
	LOGPALETTE lp;
	lp.palVersion = 0x300;
	lp.palNumEntries = 1;
	lp.palPalEntry[0].peRed = 0;
	lp.palPalEntry[0].peGreen = 0;
	lp.palPalEntry[0].peBlue = 0;
	lp.palPalEntry[0].peFlags = 0;
	m_pPal = new CDIBPal;
	ASSERT(m_pPal);
	m_pPal->CreatePalette(&lp);

	UpdateAllViews(NULL);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPaletteDoc serialization

void CPaletteDoc::Serialize(CArchive& ar)
{
    // Get the file from the archive
    ar.Flush();
    CFile *fp = ar.GetFile();
    ASSERT(fp);
    if (ar.IsStoring()) {
        ASSERT(m_pPal);
        if (!m_pPal->Save(fp)) {
            AfxMessageBox("Failed to save palette file");
            return;
        }
    } else {
        // Load the palette from the file
        if (m_pPal) delete m_pPal;
        m_pPal = new CDIBPal;
        if (!m_pPal->Load(fp)) {
            AfxMessageBox("Failed to load palette file");
            return;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// CPaletteDoc diagnostics

#ifdef _DEBUG
void CPaletteDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPaletteDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPaletteDoc commands

// Create a palette froma DIB file
void CPaletteDoc::OnFileFromDIB()
{
    // Create a DIB to hold the image
    CDIB* pDIB = new CDIB;
    // Show the file open dialog for a DIB
    if (!pDIB->Load()) {
        delete pDIB;
        return;
    }
	// Create the palette from the DIB
	if (m_pPal) delete m_pPal;
	m_pPal = new CDIBPal;
	BOOL bResult = m_pPal->Create(pDIB);
	// done with the DIB
	delete pDIB;
	if (!bResult) {
		AfxMessageBox("Failed to create palette from DIB");
	}
	UpdateAllViews(NULL);
}
