#include "stdAfx.h"
#include "atldlgs.h"
#include "DocCommands.h"
#include "ImageObject.h"
#include "DocWindow.h"
#include "MessageBox.h"
#include "Utility.h"
#include "Image.h"

#define SINGLE_IMAGE_EDITOR 0

/////////////////////////////////////////////////////////////////////////////
CDocCommands::CDocCommands(CDocWindow& DocWindow) :
	m_DocWindow(DocWindow)
{
}

/////////////////////////////////////////////////////////////////////////////
CDocCommands::~CDocCommands(void)
{
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::DoCommand(LPCTSTR rstrCommand, LPCTSTR strValue)
{
	CString strCommand = rstrCommand;
	strCommand.MakeUpper();
	int iValue = atoi(strValue);

	if (!m_DocWindow.m_hWnd)
		return (strCommand == "CLOSE" ? true : false);

	bool bOK = false;
	     if (strCommand == "STARTOVER")
		bOK = StartOver();
//j	else if (strCommand == "UNDO")
//j		bOK = Undo();
//j	else if (strCommand == "REDO")
//j		bOK = Redo();
	else if (strCommand == "ZOOMIN")
		bOK = ZoomIn();
	else if (strCommand == "ZOOMOUT")
		bOK = ZoomOut();
	else if (strCommand == "ZOOMFULL")
		bOK = ZoomFull();
//j	else if (strCommand == "APPLY")
//j		bOK = Apply();

	else if (strCommand == "OPEN")
		bOK = Open();
	else if (strCommand == "CLOSE")
		bOK = Close();
//j	else if (strCommand == "SAVE")
//j		bOK = Save();

	else if (strCommand == "CROP")
		bOK = CropStart();

	else if (strCommand == "FLIPHORIZONTAL")
		bOK = FlipHorizontal();
	else if (strCommand == "FLIPVERTICAL")
		bOK = FlipVertical();
	else if (strCommand == "ROTATELEFT")
		bOK = RotateLeft();
	else if (strCommand == "ROTATERIGHT")
		bOK = RotateRight();

	else if (strCommand == "RECOLOR")
		bOK = Recolor();
	else if (strCommand == "DISABLEFILL")
		bOK = DisableFill();
	else if (strCommand == "SOLIDFILL")
		bOK = SolidFill();
	else if (strCommand == "SWEEPDOWN")
		bOK = SweepDown();
	else if (strCommand == "SWEEPRIGHT")
		bOK = SweepRight();
	else if (strCommand == "SWEEPCENTER")
		bOK = SweepCenter();
	else if (strCommand == "SETPRIMARYCOLOR")
		bOK = SetPrimaryColor(strValue);
	else if (strCommand == "SETSECONDARYCOLOR")
		bOK = SetSecondaryColor(strValue);
	else if (strCommand == "SELECTCOLOR")
		bOK = SelectColor(strValue);

	else if (strCommand == "SHARPEN")
		bOK = Sharpen(iValue);
	else if (strCommand == "SMOOTH")
		bOK = Smooth(iValue);
	else if (strCommand == "EDGES")
		bOK = Edges(iValue);

	else if (strCommand == "AUTOCORRECT")
		bOK = AutoCorrect();
	else if (strCommand == "CONTRAST")
		bOK = Contrast(iValue);
	else if (strCommand == "BRIGHTNESS")
		bOK = Brightness(iValue);

	else if (strCommand == "HSLAMOUNT")
		bOK = HSLAmount(iValue);
	else if (strCommand == "HUE")
		bOK = Hue(iValue);
	else if (strCommand == "SATURATION")
		bOK = Saturation(iValue);
	else if (strCommand == "MAKEGRAY")
		bOK = MakeGray();

	else if (strCommand == "REDEYERESET")
		bOK = RedEyeReset();
	else if (strCommand == "REDEYEFIX")
		bOK = RedEyeFix();

	else if (strCommand == "OPENBORDERIMAGE")
		bOK = OpenBorderImage();

	else
		CMessageBox::Message(String("'%s' is not a legal command.", strCommand));

	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetCommand(LPCTSTR rstrCommand)
{
	CString strCommand = rstrCommand;
	strCommand.MakeUpper();

	// The return value of this function is a set of bitwise flags about the command
	// Bit 1 = Enabled
	// Bit 2 = Checked
	const long Enable = 1;
	const long Disable = 0;
	const long Checked = 2;
	const long Unchecked = 0;
	long lDocDependent = false;

	DWORD dwResult = 0;
	     if (strCommand == "OPEN")
		dwResult = Enable;
	else if (strCommand == "CLOSE")
		dwResult = lDocDependent;
	else if (strCommand == "GETWIDTH")
		dwResult = GetWidth();
	else if (strCommand == "GETHEIGHT")
		dwResult = GetHeight();
	else
		CMessageBox::Message(String("'%s' is not a legal command.", strCommand));

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::StartOver()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	pObject->ReOpen();

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Undo()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Redo()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomIn()
{
	return Zoom(+1);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomOut()
{
	return Zoom(-1);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomFull()
{
	return Zoom(0);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Zoom(int iOffset)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	return m_DocWindow.Zoom(iOffset);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomLevel1()
{
	return ZoomEx(0, 0.0); // Page width zoom passes a scale of 0.0
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomLevel2()
{
	return ZoomEx(0, -1.0); // Selection width zoom passes a scale of -1.0
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomLevel3()
{
	return ZoomEx(3-1, 3.0);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomLevel4()
{
	return ZoomEx(4-1, 4.0);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomLevel5()
{
	return ZoomEx(5-1, 5.0);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ZoomEx(UINT nZoomLevel, double fZoomScale)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	return m_DocWindow.ZoomEx(nZoomLevel, fZoomScale);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Apply()
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Open()
{
	#define FILTER_WITH_GDIPLUS    "All Supported Files (*.bmp, *.gif, *.jpg, *.tif, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.tif*;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0TIF Files (*.tif)\0*.tif*\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0"
	#define FILTER_WITHOUT_GDIPLUS "All Supported Files (*.bmp, *.gif, *.jpg, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0"
	LPCTSTR pstrFilter = (CImage::IsGdiPlusInstalled() ? FILTER_WITH_GDIPLUS : FILTER_WITHOUT_GDIPLUS);

	CFileDialog FileDialog(true, _T("jpg"), NULL/*pszFileName*/, 
		OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		pstrFilter, m_DocWindow/*hWndParent*/);

	strcpy_s(FileDialog.m_szFileTitle, sizeof(FileDialog.m_szFileTitle), _T("Open an Image File"));
	if (FileDialog.DoModal(m_DocWindow) == IDCANCEL)
		return false;

	CString strFileName = FileDialog.m_ofn.lpstrFile;

	// Save the path in the registry
//j	if (regkey.m_hKey)
//j		bool bOK = (regkey.SetStringValue(REGVAL_MRU_PICTURE_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_DocWindow);

	CImageObject* pObject = NULL;
	
	if (SINGLE_IMAGE_EDITOR)
	{ // Close the existing image if you want to a single image editor
		pObject = m_DocWindow.GetSelect().GetSelected();
		if (pObject)
			Close();
	}

	pObject = m_DocWindow.CreateImage(strFileName);
	if (!pObject)
		return false;

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Close()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	m_DocWindow.DeleteImage(pObject);
	pObject->Close();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Save()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::FlipHorizontal()
{
	return FlipXY(true, false);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::FlipVertical()
{
	return FlipXY(false, true);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::FlipXY(bool bFlipX, bool bFlipY)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Change the image
	BITMAPINFOHEADER* pDIB = pObject->GetDib();
	if (!pDIB)
		return false;

	BITMAPINFOHEADER* pDIBNew = CDib::Orient(pDIB, 0/*iRotateDirection*/, bFlipX, bFlipY);
	if (!pDIBNew)
		return false;

//j	if (!pObject->CanChangeOrientation())
//j		pObject->OrientChangeState(0/*iRotateDirection*/, bFlipX, bFlipY);

//j	// Unselect the current symbol, since we are changing the dest rect or the transform matrix
//j	m_DocWindow.GetSelect().Unselect();

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	pObject->SetModified(true);

	if (pDIBNew)
	{
		pObject->SetDib(pDIBNew);
		free(pDIB);
	}
	else
	{
		// Adjust the image's matrix
		CRect DestRect = pObject->GetDestRect();
		CAGMatrix Matrix = pObject->GetMatrix();
		Matrix.Transform(DestRect);
		Matrix.Scale((bFlipX ? -1 : 1), (bFlipY ? -1 : 1),
			(DestRect.left + DestRect.right) / 2,
			(DestRect.top + DestRect.bottom) / 2);

		pObject->SetMatrix(Matrix);
	}

//j	// Select the newly modified image
//j	m_DocWindow.GetSelect().Select(pObject);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::RotateLeft()
{
	return Rotate(false/*bCW*/);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::RotateRight()
{
	return Rotate(true/*bCW*/);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Rotate(bool bCW)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Change the image
	BITMAPINFOHEADER* pDIB = pObject->GetDib();
	if (!pDIB)
		return false;

	int iRotateDirection = (bCW ? 1 : -1);
	BITMAPINFOHEADER* pDIBNew = CDib::Orient(pDIB, iRotateDirection, false/*bFlipX*/, false/*bFlipY*/);
	if (!pDIBNew)
		return false;
//j	if (!pObject->CanChangeOrientation())
//j		pObject->OrientChangeState(iRotateDirection, false/*bFlipX*/, false/*bFlipY*/);

//j	// Unselect the current symbol, since we are changing the dest rect or the transform matrix
//j	m_DocWindow.GetSelect().Unselect();

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	pObject->SetModified(true);

	if (pDIBNew)
	{
		// Because we already rotated the dib, flip the matrix to avoid having the DIB being flipped when it is drawn
		CRect DestRect = pObject->GetDestRect();
		CAGMatrix Matrix = pObject->GetMatrix();
		bool bFlipX = bCW;
		bool bFlipY = !bCW;
		Matrix.Scale((bFlipX ? -1 : 1), (bFlipY ? -1 : 1),
			(DestRect.left + DestRect.right) / 2,
			(DestRect.top + DestRect.bottom) / 2);

		pObject->SetDib(pDIBNew);
		free(pDIB);
	}
	else
	{
		// Adjust the symbol's matrix
		CRect DestRect = pObject->GetDestRect();
		CAGMatrix Matrix = pObject->GetMatrix();
		Matrix.Transform(DestRect);
		double fAngle = (bCW ? 90 : -90);
		Matrix.Rotate(fAngle, fAngle,
			(DestRect.left + DestRect.right) / 2,
			(DestRect.top + DestRect.bottom) / 2);

		pObject->SetMatrix(Matrix);
	}

//j	// Select the newly modified image
//j	m_DocWindow.GetSelect().Select(pObject);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::CropStart()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

//j	// Unselect the current symbol, since we are changing the dest rect or the transform matrix
//j	m_DocWindow.GetSelect().Unselect();

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	// Change the image
	m_DocWindow.m_Select.StartCrop();

//j	// Select the newly modified image
//j	m_DocWindow.GetSelect().Select(pObject);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Recolor()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	// Change the image
//j TBD

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SetPrimaryColor(LPCTSTR pColor)
{
	COLORREF Color = (COLORREF)atoi(pColor);
	m_DocWindow.SetPrimaryColor(Color);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SetSecondaryColor(LPCTSTR pColor)
{
	COLORREF Color = (COLORREF)atoi(pColor);
	m_DocWindow.SetSecondaryColor(Color);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SelectColor(LPCTSTR pPrimary)
{
	CString szColor(pPrimary);
	m_DocWindow.UsePrimaryColor(!szColor.CompareNoCase(_T("Primary")));

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::DisableFill()
{
	m_DocWindow.SetRecolorMode(RECOLOR_OFF);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SolidFill()
{
	m_DocWindow.SetRecolorMode(RECOLOR_SOLIDFILL);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SweepDown()
{
	m_DocWindow.SetRecolorMode(RECOLOR_SWEEPDOWN);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SweepRight()
{
	m_DocWindow.SetRecolorMode(RECOLOR_SWEEPRIGHT);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SweepCenter()
{
	m_DocWindow.SetRecolorMode(RECOLOR_SWEEPCENTER);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::MakeGray()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->MakeGray();
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Sharpen(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->Sharpen(iValue);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Smooth(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->Smooth(iValue);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Edges(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->Edges(iValue);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::AutoCorrect()
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->AutoCorrect();
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Contrast(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->Contrast(iValue);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Brightness(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->Brightness(iValue);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::HSLAmount(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->HSLAmount(iValue);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Hue(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->Hue(iValue);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Saturation(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->Saturation(iValue);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::RedEyeReset()
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->RedEyeReset();
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::RedEyeFix()
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image's current location
	m_DocWindow.InvalidateImage(pObject);

	return pObject->RedEyeFix();
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::OpenBorderImage()
{
	#define FILTER_WITH_GDIPLUS    "All Supported Files (*.bmp, *.gif, *.jpg, *.tif, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.tif*;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0TIF Files (*.tif)\0*.tif*\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0"
	#define FILTER_WITHOUT_GDIPLUS "All Supported Files (*.bmp, *.gif, *.jpg, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0"
	LPCTSTR pstrFilter = (CImage::IsGdiPlusInstalled() ? FILTER_WITH_GDIPLUS : FILTER_WITHOUT_GDIPLUS);

	CFileDialog FileDialog(true, _T("jpg"), NULL/*pszFileName*/, 
		OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		pstrFilter, m_DocWindow/*hWndParent*/);

	strcpy_s(FileDialog.m_szFileTitle, sizeof(FileDialog.m_szFileTitle), _T("Open an Image File"));
	if (FileDialog.DoModal(m_DocWindow) == IDCANCEL)
		return false;

	CString strFileName = FileDialog.m_ofn.lpstrFile;

	// Save the path in the registry
//j	if (regkey.m_hKey)
//j		bool bOK = (regkey.SetStringValue(REGVAL_MRU_PICTURE_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_DocWindow);

//j TDB

	return true;
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetWidth()
{
	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
		return 0;

	BITMAPINFOHEADER* pDIB = pObject->GetDib();
	if (!pDIB)
		return 0;

	return pDIB->biWidth;
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetHeight()
{
	CImageObject* pObject = m_DocWindow.GetSelect().GetSelected();
	if (!pObject)
		return 0;

	BITMAPINFOHEADER* pDIB = pObject->GetDib();
	if (!pDIB)
		return 0;

	return pDIB->biHeight;
}

