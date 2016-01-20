// ActiveXHostDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ActiveXHost.h"
#include "ActiveXHostDlg.h"
#include ".\activexhostdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CActiveXHostDlg dialog



CActiveXHostDlg::CActiveXHostDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CActiveXHostDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CActiveXHostDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITBOX, Writingbox);
	DDX_Control(pDX, IDC_EDIT_WIDTH, EditWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, EditHeight);
	DDX_Control(pDX, IDC_EDIT_FONTSIZE, EditFontSize);
	DDX_Control(pDX, IDC_BUTTON1, IncrLineSpacing);
	DDX_Control(pDX, IDC_BUTTON2, DecrLineSpacing);
	DDX_Control(pDX, IDC_RADIO_BEZIER, RadioBezier);
	DDX_Control(pDX, IDC_COMBO_SWEEPMODE, SweepMode);
	DDX_Control(pDX, IDC_CHECK_IMAGE_TRANS, ImageBkgd);
	DDX_Control(pDX, IDC_TEXTEDITOR1, TextEditor);
}

BEGIN_MESSAGE_MAP(CActiveXHostDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDITBOX, OnEnChangeEditbox)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnEnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnEnChangeEditHeight)
	ON_EN_CHANGE(IDC_EDIT_FONTSIZE, OnEnChangeEditFontsize)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_INCRCS, OnBnClickedBtnIncrcs)
	ON_BN_CLICKED(IDC_BTN_DECRCS, OnBnClickedBtnDecrcs)
	ON_BN_CLICKED(IDC_RADIO_BEZIER, OnBnClickedRadioBezier)
	ON_BN_CLICKED(IDC_RADIO_CIRCLE, OnBnClickedRadioCircle)
	ON_BN_CLICKED(IDC_RADIO_ELLIPSE, OnBnClickedRadioEllipse)
	ON_BN_CLICKED(IDC_RADIO_SEMICIRCLE, OnBnClickedRadioSemicircle)
	ON_BN_CLICKED(IDC_RADIO_SPIRAL, OnBnClickedRadioSpiral)
	ON_BN_CLICKED(IDC_RADIO_VERTICAL, OnBnClickedRadioVertical)
	ON_BN_CLICKED(IDC_RADIO_HORIZONTAL, OnBnClickedRadioHorizontal)
	ON_BN_CLICKED(IDC_RADIO_INVBEZIER, OnBnClickedRadioInvbezier)
	ON_BN_CLICKED(IDC_RADIO_INVSEMICIRCLE, OnBnClickedRadioInvsemicircle)
	ON_BN_CLICKED(IDC_BTN_CLR1, OnBnClickedBtnClr1)
	ON_BN_CLICKED(IDC_BTN_CLR2, OnBnClickedBtnClr2)
	ON_BN_CLICKED(IDC_BTN_PRVBK, OnBnClickedBtnPrvbk)
	ON_BN_CLICKED(IDC_BTN_IMGBK, OnBnClickedBtnImgbk)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_CBN_SELCHANGE(IDC_COMBO_SWEEPMODE, OnCbnSelchangeComboSweepmode)
	ON_BN_CLICKED(IDC_CHECK_IMAGE_TRANS, OnBnClickedCheckImageTrans)
END_MESSAGE_MAP()


// CActiveXHostDlg message handlers

BOOL CActiveXHostDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString Temp;
	m_fWidth = 2.0f;
	m_fHeight = 2.0f;
	m_iFontSize = 24;
	m_StrType = CTexteditor2::STR_BEZIER;

	Temp.Format("%3.1f", m_fWidth);
	EditWidth.SetWindowText(Temp);
	Temp.Format("%3.1f", m_fHeight);
	EditHeight.SetWindowText(Temp);
	Temp.Format("%ld", m_iFontSize);
	EditFontSize.SetWindowText(Temp);
	RadioBezier.SetCheck(1);

	CTexteditor2::LINEAR_MODES mode = CTexteditor2::SWEEP_RIGHT;
	int Index = SweepMode.AddString("Sweep Right");
	SweepMode.SetItemData(Index, mode);
	mode = CTexteditor2::SWEEP_DOWN;
	Index = SweepMode.AddString("Sweep Down");
	SweepMode.SetItemData(Index, mode);
	mode = CTexteditor2::SWEEP_RIGHT_DIAGONAL;
	Index = SweepMode.AddString("Sweep Forward Diagonal");
	SweepMode.SetItemData(Index, mode);
	mode = CTexteditor2::SWEEP_LEFT_DIAGONAL;
	Index = SweepMode.AddString("Sweep backward Diagonal");
	SweepMode.SetItemData(Index, mode);
	SweepMode.SetCurSel(3);

	TextEditor.SetImageSize(m_fWidth, m_fHeight);
	TextEditor.SetString("", 0);
	TextEditor.SetFontSize(m_iFontSize);
	TextEditor.RefreshPreviewWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CActiveXHostDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CActiveXHostDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CActiveXHostDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CActiveXHostDlg::OnEnChangeEditbox()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SetDrawString();
	TextEditor.Invalidate();
}

//////////////////////////////////////////////////////////////////////
void CActiveXHostDlg::SetDrawString()
{
	CString Str;
	Writingbox.GetWindowText(Str);
		
	int nPos = -1;
	int nLine = 0;
	do
	{
		if (Str.GetLength())
		{
			nPos = Str.FindOneOf("\r\n");
			CString newStr, oldStr;
			if (nPos != -1)
			{
				if (nPos > 0)
					newStr = Str.Left(nPos);
				Str = ((nPos+2) < Str.GetLength()) ? Str.Mid(nPos+2) : "" ;
			}
			else
			{
				newStr = Str;
				Str.Empty();
				nPos = -1;
			}
			if ( !GetCurrentLine(oldStr, nLine) || !(newStr == oldStr) )
			{
				SetCurrentLine(newStr, nLine);
				TextEditor.SetString(newStr, nLine);
			}
			nLine++;
		}
		else
		{
			if ( !GetCurrentLine(Str, nLine) )
			{
				SetCurrentLine(Str, nLine);
				TextEditor.SetString(Str, nLine);
			}

			nPos = -1;
		}
	}while (nPos != -1);

}

//////////////////////////////////////////////////////////////////////
void CActiveXHostDlg::SetCurrentLine(CString& newStr, int nLine)
{
	int nCount = m_CurrStrList.GetCount();

	if (nLine == nCount)
	{
		// Add the line
		m_CurrStrList.AddTail(newStr);
		return;
	}

	if (nLine < nCount)
	{
		// set the line
		POSITION pos = m_CurrStrList.FindIndex(nLine);
		m_CurrStrList.SetAt(pos, newStr);
		return;
	}

	// nLine is greater than nCount+1 - cannot add at this point.
	ATLASSERT(false);
}

//////////////////////////////////////////////////////////////////////
bool CActiveXHostDlg::GetCurrentLine(CString& str, int nLine)
{
	str.Empty();
	int nCount = m_CurrStrList.GetCount();

	if (nCount <= 0)
		return false;

	if (nLine >= nCount)
		return false;

	nCount = 0;
	POSITION pos = m_CurrStrList.GetHeadPosition();
	while (pos)
	{
		str = m_CurrStrList.GetNext(pos);
		nCount++;
		if (nCount == nLine)
			break;
	}

	return true;
}


void CActiveXHostDlg::OnEnChangeEditWidth()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here EditWidth
	CString StrText;
	EditWidth.GetWindowText(StrText);
	if (StrText != "")
	{
		m_fWidth = (float)atof(StrText);
		if (m_fWidth != 0.0F && m_fHeight != 0.0F)
			TextEditor.SetImageSize(m_fWidth, m_fHeight);

		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnEnChangeEditHeight()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString StrText;
	EditHeight.GetWindowText(StrText);
	if (StrText != "")
	{
		m_fHeight = (float)atof(StrText);
		if (m_fWidth != 0.0F && m_fHeight != 0.0F)
			TextEditor.SetImageSize(m_fWidth, m_fHeight);

		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnEnChangeEditFontsize()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString StrText;
	EditFontSize.GetWindowText(StrText);
	if (StrText != "")
	{
		m_iFontSize = atoi(StrText);
		if (m_iFontSize != 0)
		{
			TextEditor.SetFontSize(m_iFontSize);
			TextEditor.RefreshPreviewWindow();
		}

		TextEditor.Invalidate();
	}
}
void CActiveXHostDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	TextEditor.IncrementLineHeight();
	TextEditor.RefreshPreviewWindow();
	TextEditor.Invalidate();
}

void CActiveXHostDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	TextEditor.DecrementLineHeight();
	TextEditor.RefreshPreviewWindow();
	TextEditor.Invalidate();
}

void CActiveXHostDlg::OnBnClickedBtnIncrcs()
{
	// TODO: Add your control notification handler code here
	TextEditor.IncrementCharWidth();
	TextEditor.RefreshPreviewWindow();
	TextEditor.Invalidate();
}

void CActiveXHostDlg::OnBnClickedBtnDecrcs()
{
	// TODO: Add your control notification handler code here
	TextEditor.DecrementCharWidth();
	TextEditor.RefreshPreviewWindow();
	TextEditor.Invalidate();
}

void CActiveXHostDlg::OnBnClickedRadioBezier()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_BEZIER)
	{
		m_StrType = CTexteditor2::STR_BEZIER;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedRadioCircle()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_CIRCLE)
	{
		m_StrType = CTexteditor2::STR_CIRCLE;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedRadioEllipse()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_ELLIPSE)
	{
		m_StrType = CTexteditor2::STR_ELLIPSE;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedRadioSemicircle()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_SEMICIRCLE)
	{
		m_StrType = CTexteditor2::STR_SEMICIRCLE;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedRadioSpiral()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_SPIRAL)
	{
		m_StrType = CTexteditor2::STR_SPIRAL;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedRadioVertical()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_VERTICALLINE)
	{
		m_StrType = CTexteditor2::STR_VERTICALLINE;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedRadioHorizontal()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_HORIZONTALLINE)
	{
		m_StrType = CTexteditor2::STR_HORIZONTALLINE;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedRadioInvbezier()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_INVERTEDBEZIER)
	{
		m_StrType = CTexteditor2::STR_INVERTEDBEZIER;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedRadioInvsemicircle()
{
	// TODO: Add your control notification handler code here
	if (m_StrType != CTexteditor2::STR_INVERTEDSEMICIRCLE)
	{
		m_StrType = CTexteditor2::STR_INVERTEDSEMICIRCLE;
		TextEditor.SetCurveType(m_StrType);
		TextEditor.RefreshPreviewWindow();
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedBtnClr1()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF Clr = dlg.GetColor();
		TextEditor.SetColor1(Clr);
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedBtnClr2()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF Clr = dlg.GetColor();
		TextEditor.SetColor2(Clr);
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedBtnPrvbk()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF Clr = dlg.GetColor();
		TextEditor.SetPreviewBkColor(Clr);
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedBtnImgbk()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF Clr = dlg.GetColor();
		TextEditor.SetImageBkColor(Clr);
		TextEditor.Invalidate();
	}
}

void CActiveXHostDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(FALSE, "BMP", 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "Bitmap (*.bmp)|*.bmp|JPEG (*.jpeg)|*.jpeg|GIF (*.gif)|*.gif|TIFF (*.tiff)|*.tiff|PNG (*.png)|*.png ||", this);
	if (dlg.DoModal() == IDOK)
	{
		CString FilePath = dlg.GetPathName();
		CString FileName = dlg.GetFileTitle();
		CString Ext = FilePath.Mid(FilePath.ReverseFind('.')+1);
		Ext.MakeUpper();
		CTexteditor2::IMAGE_TYPES Type;
		if (Ext == "JPEG")
			Type = CTexteditor2::JPEG;
		else if (Ext == "BMP")
			Type = CTexteditor2::BMP;
		else if (Ext == "GIF")
			Type = CTexteditor2::GIF;
		else if (Ext == "TIFF")
			Type = CTexteditor2::TIFF;
		else if (Ext == "PNG")
			Type = CTexteditor2::PNG;
		else
			return;

		TextEditor.SaveImage(FilePath, Type);
	}
}

void CActiveXHostDlg::OnCbnSelchangeComboSweepmode()
{
	// TODO: Add your control notification handler code here
	int Index = SweepMode.GetCurSel();
	CTexteditor2::LINEAR_MODES mode = (CTexteditor2::LINEAR_MODES)SweepMode.GetItemData(Index);
	TextEditor.SetColorGradientMode(mode);
	TextEditor.Invalidate();
}

void CActiveXHostDlg::OnBnClickedCheckImageTrans()
{
	// TODO: Add your control notification handler code here
	if (ImageBkgd.GetCheck() == BST_CHECKED)
		TextEditor.SetWhiteBkgdTransparent(true);
	else
		TextEditor.SetWhiteBkgdTransparent(false);
}
