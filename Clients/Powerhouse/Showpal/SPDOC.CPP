// spdoc.cpp : implementation of the CShowPalDoc class
//

#include "stdafx.h"
#include "showpal.h"
#include "dib.h"
#include "dibpal.h"
#include "spdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowPalDoc

IMPLEMENT_DYNCREATE(CShowPalDoc, CDocument)

BEGIN_MESSAGE_MAP(CShowPalDoc, CDocument)
    //{{AFX_MSG_MAP(CShowPalDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowPalDoc construction/destruction

CShowPalDoc::CShowPalDoc()
{
    m_pPal = NULL;
}

CShowPalDoc::~CShowPalDoc()
{
    if (m_pPal) delete m_pPal;
}

BOOL CShowPalDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    if (m_pPal) {
        delete m_pPal;
        m_pPal = NULL;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CShowPalDoc serialization

void CShowPalDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring()) {
        ASSERT(1); // We don't do this.
    } else {
        // Get the file from the archive.
        CFile* fp = ar.GetFile();
        ASSERT(fp);
        ar.Flush();
        // Load the DIB from the file.
        CDIB dib;
        if (!dib.Load(fp)) {
            AfxMessageBox("Failed to load DIB file");
            return;
        }
        // Create the palette from the DIB.
        if (m_pPal) delete m_pPal;
        m_pPal = new CDIBPal;
        ASSERT(m_pPal);
        if (!m_pPal->Create(&dib)) {
            AfxMessageBox("Failed to create palette from DIB file");
            delete m_pPal;
            m_pPal = NULL;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// CShowPalDoc diagnostics

#ifdef _DEBUG
void CShowPalDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CShowPalDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShowPalDoc commands
