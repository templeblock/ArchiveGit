// DispText.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DispText.h"
#include "util.h"

static LPBYTE  s_pFileBuffer = NULL;
static DWORD   s_dwCurOffset = 0;
static DWORD   s_dwBytesRemaining = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayTextDialog dialog


CDisplayTextDialog::CDisplayTextDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayTextDialog::IDD, pParent)
{
	EnableAutomation();

	//{{AFX_DATA_INIT(CDisplayTextDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDisplayTextDialog::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CDisplayTextDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayTextDialog)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_TEXT_CONTROL, m_ctrlTextFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayTextDialog, CDialog)
	//{{AFX_MSG_MAP(CDisplayTextDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDisplayTextDialog, CDialog)
	//{{AFX_DISPATCH_MAP(CDisplayTextDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDisplayTextDialog to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {9DB12F9A-4308-11D2-8BEF-CB3B5D93F18E}
static const IID IID_IDisplayTextDialog =
{ 0x9db12f9a, 0x4308, 0x11d2, { 0x8b, 0xef, 0xcb, 0x3b, 0x5d, 0x93, 0xf1, 0x8e } };

BEGIN_INTERFACE_MAP(CDisplayTextDialog, CDialog)
	INTERFACE_PART(CDisplayTextDialog, IID_IDisplayTextDialog, Dispatch)
END_INTERFACE_MAP()

static DWORD CALLBACK EditStreamCallback(DWORD dwCookie, LPBYTE pBuffer, LONG cb, LONG *pcb)
   {
      ASSERT(s_pFileBuffer);
      if(s_pFileBuffer == NULL)
         return 1;   // Failure
      ASSERT(pBuffer);
      if(pBuffer)
         {
            LONG  lBytesToRead = cb;

            // Comments on variables:
            // lBytesToRead         Number of bytes requested by RTF control to get
            // s_dwBytesRemaining   Number of bytes remaining from our file buffer
            //                      that have not been read yet.
            // lBytesToRead         Actual number of bytes to be transferred from our
            //                      buffer to callers buffer
            // pcb                  Actual number of bytes transferred to callers
            //                      buffer.

            if(cb > (long)s_dwBytesRemaining)
               lBytesToRead = s_dwBytesRemaining;
            memcpy(pBuffer, &s_pFileBuffer[s_dwCurOffset], lBytesToRead);
            s_dwCurOffset += cb;
            s_dwBytesRemaining -= lBytesToRead;
            *pcb = lBytesToRead;
         }
      return 0;
   }

/////////////////////////////////////////////////////////////////////////////
// CDisplayTextDialog message handlers

int CDisplayTextDialog::DoModal(LPCSTR szTitle, LPCSTR szFileName)
   {
      m_csTitle = szTitle;
      if(!Util::FileExists(szFileName))
         {
            ASSERT(0);
            return IDABORT;
         }
      m_csFileName = szFileName;
      CString csExtension;
      Util::SplitPath(m_csFileName,NULL,NULL,NULL,&csExtension);
      if(csExtension.CompareNoCase(".rtf") == 0)
         m_enFileType = filetypeRTF;
      else
         m_enFileType = filetypeText;
      return INHERITED::DoModal();
   }

int CDisplayTextDialog::DoModal()
   {
      return IDABORT;
   }

BOOL CDisplayTextDialog::OnInitDialog() 
{
   BOOL  bErrorOccurred = TRUE;
   int   nRichEditFileType;

   SetWindowText(m_csTitle);
   s_dwCurOffset = 0;
   s_pFileBuffer = NULL;
   s_dwBytesRemaining = 0;

	CDialog::OnInitDialog();

   m_btnOK.GetWindowRect(&m_crOKButton);
   ScreenToClient(&m_crOKButton);
	
   CRect crClient, crTextControl;
   GetClientRect(&crClient);
   m_ctrlTextFrame.GetWindowRect(&m_crTextFrame);
   ScreenToClient(&m_crTextFrame);

   // Compute distance between Frame and window edge
   m_csizeBorder.cx = crClient.Width() - m_crTextFrame.Width();
   m_csizeBorder.cy = crClient.bottom  - m_crTextFrame.bottom;

   // Allow for 3D border
   crTextControl = m_crTextFrame;
   crTextControl.InflateRect(-4, -4);
   BOOL bCreated = m_ctrlRichEdit.Create(
      WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL,
      crTextControl,
      this,
      IDC_RICHEDIT);
   ASSERT(bCreated);
   if(bCreated)
      {
	      CFileStatus Status;

         // Set up control so that user cannot type in it.
         m_ctrlRichEdit.SetReadOnly();

         CFile cfileText(m_csFileName, CFile::modeRead);
         if (cfileText.GetStatus(Status))
         {
            DWORD dwFileLength = 0;
            TRY
            {
               s_pFileBuffer = (LPBYTE) new char[Status.m_size];
               dwFileLength = s_dwBytesRemaining = cfileText.ReadHuge((void*)s_pFileBuffer, Status.m_size);
            }
            END_TRY
            cfileText.Close();
      
            if (dwFileLength > 0)
            {
               EDITSTREAM  editStream;
               editStream.dwCookie=0;
               editStream.dwError=0;
               editStream.pfnCallback=EditStreamCallback;
               if(m_enFileType == filetypeRTF)
                  nRichEditFileType = SF_RTF;
               else
                  nRichEditFileType = SF_TEXT;
               m_ctrlRichEdit.StreamIn(nRichEditFileType, editStream);
               bErrorOccurred = FALSE;
            }
            delete [] s_pFileBuffer;
            s_pFileBuffer = NULL;
         }
      }
   if (bErrorOccurred)
      EndDialog(IDCANCEL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDisplayTextDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CDisplayTextDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

   if(cx > 0 && cy > 0 && m_ctrlRichEdit.GetSafeHwnd())
   {
      // Resize Frame
      m_ctrlTextFrame.SetWindowPos(
         NULL, 
         m_crTextFrame.left, 
         m_crTextFrame.top, 
         cx - m_csizeBorder.cx,
         cy - m_crTextFrame.top - m_csizeBorder.cy,
         SWP_NOZORDER);

      // Resize RTF control relative to frame
      CRect crFrameClient;
      m_ctrlTextFrame.GetWindowRect(&crFrameClient);
      ScreenToClient(&crFrameClient);
      crFrameClient.InflateRect(-3,-3);
      m_ctrlRichEdit.SetWindowPos(
         NULL, 
         crFrameClient.left, 
         crFrameClient.top, 
         crFrameClient.Width(),
         crFrameClient.Height() ,
         SWP_NOZORDER);

      // Move OK Button
      CRect crClient, crNewTextFrame;
      m_ctrlTextFrame.GetWindowRect(&crNewTextFrame);
      ScreenToClient(&crNewTextFrame);
      GetClientRect(&crClient);
      m_btnOK.SetWindowPos(
         NULL, 
         crClient.left + ((crClient.Width() - m_crOKButton.Width()) / 2), 
         crNewTextFrame.bottom + ((crClient.bottom - crNewTextFrame.bottom - m_crOKButton.Height()) / 2), 
         0,
         0,
         SWP_NOZORDER | SWP_NOSIZE);
   }
}
