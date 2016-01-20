// EditImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tek.h"
#include "EditImageDlg.h"
#include "CalibrateDlg.h"
#include "ToneAdjustDlg.h"
#include "TuningDlg.h"
#include "Dibapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditImageDlg dialog

BEGIN_MESSAGE_MAP(CEditImageDlg, CDialog)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_CREATE()
	//{{AFX_MSG_MAP(CEditImageDlg)
	ON_BN_CLICKED(IDC_EDIT_IMAGE_CURVE1, OnCurve1)
	ON_BN_CLICKED(IDC_EDIT_IMAGE_CURVE2, OnCurve2)
	ON_BN_CLICKED(IDC_EDIT_IMAGE_TUNING, OnTuning)
	ON_BN_CLICKED(IDC_PRINTIMAGE, OnPrint)
	ON_BN_CLICKED(IDC_HELPME, OnHelp)
	ON_BN_CLICKED(IDC_ZOOM, OnZoom)
	ON_BN_CLICKED(IDC_UNZOOM, OnUnzoom)
	ON_BN_CLICKED(IDC_OPENIMAGE, OnOpenImage)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BEGIN_EVENTSINK_MAP(CEditImageDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CEditImageDlg)
	ON_EVENT(CEditImageDlg, IDC_IMAGE, -601 /* DblClick */, OnImageDoubleClick, VTS_NONE)
	ON_EVENT(CEditImageDlg, IDC_IMAGE, -606 /* MouseMove */, OnImageMouseMove, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CEditImageDlg, IDC_IMAGE, -605 /* MouseDown */, OnImageMouseDown, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CEditImageDlg, IDC_IMAGE, -607 /* MouseUp */, OnImageMouseUp, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CEditImageDlg, IDC_IMAGE, -600 /* Click */, OnImageClick, VTS_NONE)
	ON_EVENT(CEditImageDlg, IDC_IMAGE, 6 /* ViewStatusChanged */, OnImageViewStatusChanged, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CEditImageDlg, IDC_IMAGE, 5 /* BufStatusChanged */, OnImageBufStatusChanged, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CEditImageDlg, IDC_IMAGE, 7 /* TimerTick */, OnImageTimerTick, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
CEditImageDlg::CEditImageDlg(CString& szImageFile, CWnd* pParent /*=NULL*/)
	: CDialog(CEditImageDlg::IDD, pParent)
{
	m_dwBackupSize = 0;
	m_pBackupBuffer = NULL;
	m_pCalibrateDlg = NULL;
	m_pToneAdjustDlg = NULL;
	m_pTuningDlg = NULL;
	m_szImageFile = szImageFile;
	m_ptMouseOnImage = 0;
	m_hDib = NULL;
	//{{AFX_DATA_INIT(CEditImageDlg)
	m_bZoom = false;
	m_bUnzoom = false;
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CEditImageDlg::~CEditImageDlg()
{
	if (m_pBackupBuffer)
		GlobalFree(m_pBackupBuffer);

	if (m_pCalibrateDlg)
		delete m_pCalibrateDlg;

	if (m_pToneAdjustDlg)
		delete m_pToneAdjustDlg;

	if (m_pTuningDlg)
		delete m_pTuningDlg;
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditImageDlg)
	DDX_Control(pDX, IDC_IMAGE, m_ctlImage);
	DDX_Check(pDX, IDC_ZOOM, m_bZoom);
	DDX_Check(pDX, IDC_UNZOOM, m_bUnzoom);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
BOOL CEditImageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ptMouseOnImage = 0;

	// The license to distribute the Imagn control
	m_ctlImage.UL( 136509823, 1211332160, 1341321342, 6802 );
	m_ctlImage.SetScrollbars(true);
	m_ctlImage.SetViewScale(0); // ShrinkToFit(0), GrowToFit(-1), StretchToFit(-2), ScaleToSides(-3), ScaleToHeight(-4)
	m_ctlImage.SetPopupMenu(true);
	m_ctlImage.SetToolControl(2); // None(0), Zoom(1), ZoomRect(2), Hand(3)
	m_ctlImage.SetToolShift(3); // None(0), Zoom(1), ZoomRect(2), Hand(3)
	m_ctlImage.SetToolControlShift(1); // None(0), Zoom(1), ZoomRect(2), Hand(3)
	m_ctlImage.SetTool(0); // None(0), Zoom(1), ZoomRect(2), Hand(3)
	m_ctlImage.SetDither(2/*Dither Fast*/);
	m_ctlImage.SetRotate(0/*Normal (no rotation)*/);
	m_ctlImage.SetTimer(0);
	m_ctlImage.SetEnabled(true);
	m_ctlImage.SetViewProgressive(true);

	m_bZoom = true;
	m_bUnzoom = !m_bZoom;
	UpdateData(false);

	BeginWaitCursor();

	// Read the file into the backup buffer
	CFile ImageFile(m_szImageFile, CFile::modeReadWrite);
	m_dwBackupSize = ImageFile.SeekToEnd();
	ImageFile.SeekToBegin();
	m_pBackupBuffer = GlobalAlloc(GMEM_FIXED, m_dwBackupSize);
	// Lock it down, and read into it
	BYTE* pBackupBuffer = (BYTE*)GlobalLock(m_pBackupBuffer);
	if (pBackupBuffer)
	{
		UINT uiBytesRead = 0;
		if (m_pBackupBuffer)
			uiBytesRead = ImageFile.Read(pBackupBuffer, (UINT)m_dwBackupSize);
		GlobalUnlock(m_pBackupBuffer);
	}
	ImageFile.Close();

	// Load the main buffer with the image
	m_ctlImage.Load(m_ctlImage.GetViewBufferId(), m_szImageFile);

	EndWaitCursor();

	CenterWindow();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnCurve1() 
{
	if (m_pCalibrateDlg && ::IsWindow(m_pCalibrateDlg->m_hWnd))
	{
		if (m_pCalibrateDlg->IsWindowVisible())
			m_pCalibrateDlg->ShowWindow(SW_HIDE);
		else
		{
			m_pCalibrateDlg->SetActiveWindow();
			m_pCalibrateDlg->ShowWindow(SW_NORMAL);
		}
		return;
	}

	m_pCalibrateDlg = new CCalibrateDlg(this);
	if (!m_pCalibrateDlg)
		return;

	m_pCalibrateDlg->Create(CCalibrateDlg::IDD, this/*pParent*/);
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnCurve2() 
{
	if (m_pToneAdjustDlg && ::IsWindow(m_pToneAdjustDlg->m_hWnd))
	{
		if (m_pToneAdjustDlg->IsWindowVisible())
			m_pToneAdjustDlg->ShowWindow(SW_HIDE);
		else
		{
			m_pToneAdjustDlg->SetActiveWindow();
			m_pToneAdjustDlg->ShowWindow(SW_NORMAL);
		}
		return;
	}

	m_pToneAdjustDlg = new CToneAdjustDlg(this);
	if (!m_pToneAdjustDlg)
		return;

	m_pToneAdjustDlg->Create(CToneAdjustDlg::IDD, this/*pParent*/);
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnTuning() 
{
	if (m_pTuningDlg && ::IsWindow(m_pTuningDlg->m_hWnd))
	{
		if (m_pTuningDlg->IsWindowVisible())
			m_pTuningDlg->ShowWindow(SW_HIDE);
		else
		{
			m_pTuningDlg->SetActiveWindow();
			m_pTuningDlg->ShowWindow(SW_NORMAL);
		}
		return;
	}

	m_pTuningDlg = new CTuningDlg(this);
	if (!m_pTuningDlg)
		return;

	m_pTuningDlg->Create(CTuningDlg::IDD, this/*pParent*/);
}

static bool bModified;

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnPrint() 
{
	if (bModified)
	{
		AfxMessageBox("Add your Help code here!\n\nFor now, we'll RESTORE the image.");
		RestoreImage();
	}
	else
	{
		AfxMessageBox("Add your Printing code here!\n\nFor now, we'll toggle the colors of the LEFT HALF of the image.");
		ModifyImage(true);
	}
	bModified = !bModified;
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnHelp() 
{
	if (bModified)
	{
		AfxMessageBox("Add your Help code here!\n\nFor now, we'll RESTORE the image.");
		RestoreImage();
	}
	else
	{
		AfxMessageBox("Add your Help code here!\n\nFor now, we'll toggle the colors of the ENTIRE image");
		ModifyImage(false);
	}
	bModified = !bModified;
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnZoom() 
{
	m_bZoom = true;
	m_bUnzoom = !m_bZoom;
	UpdateData(false);
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnUnzoom() 
{
	m_bZoom = false;
	m_bUnzoom = !m_bZoom;
	UpdateData(false);
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnOpenImage() 
{
	CString szImageFile;
	if (!GetImageFileName(szImageFile))
		return;

	// Free the backup buffer
	if (m_pBackupBuffer)
	{
		GlobalFree(m_pBackupBuffer);
		m_pBackupBuffer = NULL;
		m_dwBackupSize = 0;
	}

	m_szImageFile = szImageFile;

	BeginWaitCursor();

	// Read the file into the backup buffer
	CFile ImageFile(m_szImageFile, CFile::modeReadWrite);
	m_dwBackupSize = ImageFile.SeekToEnd();
	ImageFile.SeekToBegin();
	m_pBackupBuffer = GlobalAlloc(GMEM_FIXED, m_dwBackupSize);
	// Lock it down, and read into it
	BYTE* pBackupBuffer = (BYTE*)GlobalLock(m_pBackupBuffer);
	if (pBackupBuffer)
	{
		UINT uiBytesRead = 0;
		if (m_pBackupBuffer)
			uiBytesRead = ImageFile.Read(pBackupBuffer, (UINT)m_dwBackupSize);
		GlobalUnlock(m_pBackupBuffer);
	}
	ImageFile.Close();

	// Load the main buffer with the image
	m_ctlImage.Load(m_ctlImage.GetViewBufferId(), m_szImageFile);

	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEditImageDlg::GetImageFileName(CString& szImageFile)
{
	CFileDialog FileDialog(
		/*bOpenFileDialog*/	true,
		/*lpszDefExt*/		"tif",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Image Files (*.tif;*.bmp)|*.tif;*.bmp|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	if (FileDialog.DoModal() != IDOK)
		return false;

	szImageFile = FileDialog.GetPathName();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnImageMouseMove(short Button, short Shift, long X, long Y) 
{
	X = m_ctlImage.ViewToPixelX(X);
	Y = m_ctlImage.ViewToPixelY(Y);

	CString sz;

	if (X < 0 || X >= m_ctlImage.GetSizeX() ||
		Y < 0 || Y >= m_ctlImage.GetSizeY())
		sz = "";
	else
	{
		m_ptMouseOnImage.x = X;
		m_ptMouseOnImage.y = Y;
		sz.Format("(%ld,%ld)", X, Y);
	}

	CWnd* pControl = GetDlgItem(IDC_MOUSE_MOVE);
	if (pControl)
		pControl->SetWindowText(sz);

	if (!Button)
		return;

	m_ctlImage.SetTimer(33);
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnImageClick()
{
//	long xSize = m_ctlImage.GetSizeX();
//	long ySize = m_ctlImage.GetSizeY();
//	long x = m_ptMouseOnImage.x;
//	long y = m_ptMouseOnImage.y;

	long lScale = m_ctlImage.GetViewScale();
	if (lScale <= 0) lScale = 100;
	long lAmount = max(1, lScale/10);
	lScale += (m_bZoom ? lAmount : -lAmount);
	if (lScale <= 0) lScale = 1;
	m_ctlImage.SetViewScale(lScale); // ShrinkToFit(0), GrowToFit(-1), StretchToFit(-2), ScaleToSides(-3), ScaleToHeight(-4)
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnImageDoubleClick() 
{
	m_ctlImage.SetTimer(33);
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnImageMouseDown(short Button, short Shift, long X, long Y) 
{
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnImageMouseUp(short Button, short Shift, long X, long Y) 
{
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnImageViewStatusChanged(long ViewId, long TypeCode, long Data) 
{
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnImageBufStatusChanged(long BufId, long Status, long Data) 
{
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnImageTimerTick() 
{
	if (!GetAsyncKeyState(VK_LBUTTON))
	{
		m_ctlImage.SetTimer(0);
		return;
	}

	long lScale = m_ctlImage.GetViewScale();
	if (lScale <= 0) lScale = 100;
	long lAmount = max(1, lScale/10);
	lScale += (m_bZoom ? lAmount : -lAmount);
	if (lScale <= 0) lScale = 1;
	m_ctlImage.SetViewScale(lScale); // ShrinkToFit(0), GrowToFit(-1), StretchToFit(-2), ScaleToSides(-3), ScaleToHeight(-4)
}


/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::OnSize(UINT nType, int cx, int cy) 
{
	if (!::IsWindow(m_ctlImage.m_hWnd) || !m_ctlImage.IsWindowVisible())
	{
		CDialog::OnSize(nType, cx, cy);
		return;
	}

	// The dialog has already been sized to (cx,cy)
	// Get the image window rect in client coordinates
	CRect rectImg;
	m_ctlImage.GetWindowRect(&rectImg);
	ScreenToClient(&rectImg);

	CDialog::OnSize(nType, cx, cy);
	
	int w = max(0, cx - rectImg.left - rectImg.left);
	int h = max(0, cy - rectImg.top - rectImg.left);
	m_ctlImage.MoveWindow(rectImg.left, rectImg.top, w, h);
}

// Buffer status flags
#define BUF_DEFINED		0x00000001L		// an image filename has been defined.
#define BUF_OPENED		0x00000002L		// the image header and palette information has been successfully read and decoded.
										// If this flag is set, size and colors information is available.
#define BUF_DECODED		0x00000004L		// the image has been decoded.
#define BUF_DECODING	0x00000010L		// the image is in the process of decoding.
#define BUF_ERROR 		0x00000008L		// an error occurred during the open or decode steps.
										// Check the BUF_OPENED and BUF_DECODED flags to determine where the error occurred.
										// If this flag is set, you can get the error code from the GetRetCode method. 

// This header is followed by a palette array containing the actual palette entries.
// The number of entries in the array is exactly PalColors.
// The image data immediately follows the palette.
// The above header information is provided only to describe the image.
// Changes to anything other than the actual image data are ignored.

// The IMW_BITMAPHEADER lOptions flags
#define IMWB_FILL_ORDER	1				// This bit describes how the image data is ordered. It is only valid for 1-bit and 24-bit images. If this bit is set for 1-bit images, the image data is stored with the most significant bit first. If this bit is set for 24-bit images, the image data is stored in Blue, Green, Red (BGR) order instead of Red, Green, Blue (RGB) order.
#define IMWB_SIDEWAYS	2				// If this bit is set, the image orientation is sideways. (The first scan line in the image is the first column of the image.)
#define IMWB_INVERT		4				// If this bit is set, the image is inverted vertically. (BMP files usually have this bit set).
#define IMWB_MIRROR		8				// If this bit is set, the image is mirrored horizontally.

typedef struct
{
	long			lWidth;				// The Width of the Image (in pixels).
	long			lHeight;			// The Height of the Image (in pixels).
	long			lBitsPerPixel;		// The number of bits used for each pixel. This may be 1, 2, 4, 8, 15, 16, or 24.
	long			lTotalBitsPerPixel;	// The total number of bits required for each pixel. This may be 1, 2, 4, 8, 16, 24, or 32. 
	long			lBytesPerLine;		// The number of bytes used for each scan line of the image. Note:  This is not necessarily 4-byte aligned.
	long			lOptions;			// Bit flags that further describe the image data. They can be:
	long			lPalColors;			// This is the number of colors in the image palette. The valid range is 0 through 256.
	PALETTEENTRY	Palette;			// This is an array containing the actual palette entries. The number of entries in the array is exactly PalColors.
} IMW_BITMAPHEADER;

typedef struct
{
	BYTE r;
	BYTE g;
	BYTE b;
} SRGB;

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::RestoreImage() 
{
	// Get the main buffer id
	long lViewBufferId = m_ctlImage.GetViewBufferId();
	if (!lViewBufferId)
		return;

	// Load the backup data into the main buffer
	BeginWaitCursor();
	m_ctlImage.LoadData(lViewBufferId, m_dwBackupSize, (LPUNKNOWN)m_pBackupBuffer);

	EndWaitCursor();
	m_ctlImage.InvalidateBuf(lViewBufferId);
}

/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::ModifyImage(bool bSplit) 
{
	// Get the main buffer id
	long lViewBufferId = m_ctlImage.GetViewBufferId();
	if (!lViewBufferId)
		return;

	long lBufferStatus = m_ctlImage.BufStatus(lViewBufferId);
	if (lBufferStatus & (BUF_DECODING | BUF_ERROR))
		return;

	long lOldBufferStatus = lBufferStatus;
	lBufferStatus |= (BUF_DEFINED | BUF_OPENED | BUF_DECODED);
	if (lBufferStatus != lOldBufferStatus)
		return;

	// Get the main buffer's memory handle 
	HGLOBAL hBuffer = (HGLOBAL)m_ctlImage.GetBufHand(lViewBufferId);
	if (!hBuffer)
		return;

	// Lock it down
	BYTE* pBuffer = (BYTE*)GlobalLock(hBuffer);
	if (!pBuffer)
		return;

	IMW_BITMAPHEADER* pHeader = (IMW_BITMAPHEADER*)pBuffer;
	pBuffer += sizeof(IMW_BITMAPHEADER);
	PALETTEENTRY* pPalette = (PALETTEENTRY*)pBuffer;
	pBuffer += sizeof(PALETTEENTRY) * pHeader->lPalColors;

	if (pHeader->lBitsPerPixel != 24)
	{
		GlobalUnlock(hBuffer);
		pBuffer = NULL;
		return;
	}

	long lBytesPerPixel = pHeader->lTotalBitsPerPixel / 8;

	if (pHeader->lHeight < 0) // A negative height indicates a top-down DIB
		pHeader->lHeight = -pHeader->lHeight;

	BeginWaitCursor();

	for (long y=0; y<pHeader->lHeight-1; y++)
	{
		SRGB* p = (SRGB*)pBuffer;
												 
		long lWidth = (bSplit ? pHeader->lWidth / 2 : pHeader->lWidth-1);
		for (long x=0; x<lWidth; x++)
		{
			BYTE c = p->g;
			p->g = p->r;
			p->r = c;
			p = (SRGB*)((BYTE*)p + lBytesPerPixel);
		}
		
		pBuffer += pHeader->lBytesPerLine;
		
	}

	// Unlock the data
	GlobalUnlock(hBuffer);
	pBuffer = NULL;

	EndWaitCursor();
	m_ctlImage.InvalidateBuf(lViewBufferId);
}

#ifdef comment
// Add this function to the class, if needed
/////////////////////////////////////////////////////////////////////////////
void CEditImageDlg::CopyToDib() 
{
	// Grab the DIB
	BeginWaitCursor();

	// Free any previously allocated DIB
	if (m_hDib)
	{
		GlobalFree(m_hDib);
		m_hDib = NULL;
	}

	// Create the DIB
	m_hDib = (HGLOBAL)m_ctlImage.CreateDIB(m_ctlImage.GetViewBufferId());

	EndWaitCursor();

	// Make sure that the DIB is legit
	if (!m_hDib)
		return;
	PBITMAPINFO pDib = (PBITMAPINFO)GlobalLock(m_hDib);
	if (!pDib)
		return;

	int iWidth = DIBWidth((LPSTR)pDib);
	int iHeight = DIBHeight((LPSTR)pDib);

	// We are done with the DIB
	GlobalUnlock(m_hDib);
	pDib = NULL;

	int iControlWidth = 0;
	CRect rect;
	HWND hWnd = (HWND)m_ctlImage.GetHWnd();
	if (hWnd)
	{
		::GetClientRect(hWnd, &rect);
		iControlWidth = rect.Width();
	}

	if (!iControlWidth)
		iControlWidth = 500;
}
#endif

