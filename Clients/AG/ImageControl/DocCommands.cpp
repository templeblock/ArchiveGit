#include "stdAfx.h"
#include "atldlgs.h"
#include "DocCommands.h"
#include "ImageObject.h"
#include "DocWindow.h"
#include "MessageBox.h"
#include "Utility.h"
#include "Image.h"
#include "ImageControlConstants.h"
#include "ConvertDibToJpg.h"

#ifdef _DEBUG
	#define SINGLE_IMAGE_EDITOR 0
#else
	#define SINGLE_IMAGE_EDITOR 1
#endif

#define FILTER_WITH_GDIPLUS    "All Supported Files (*.bmp, *.gif, *.jpg, *.tif, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.tif*;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0TIF Files (*.tif)\0*.tif*\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0"
#define FILTER_WITHOUT_GDIPLUS "All Supported Files (*.bmp, *.gif, *.jpg, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0"

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
	CString strSubCommand = strValue;
	strSubCommand.MakeUpper();
	int iValue = _ttoi(strValue);

	if (!m_DocWindow.m_hWnd)
		return (strCommand == "CLOSE" ? true : false);

	bool bOK = false;
	// Commands from the main dialog
	     if (strCommand == "STARTOVER")
		bOK = StartOver();
	else if (strCommand == "ZOOMIN")
		bOK = ZoomIn();
	else if (strCommand == "ZOOMOUT")
		bOK = ZoomOut();
	else if (strCommand == "ZOOMFULL")
		bOK = ZoomFull();

	// Commands from the File Operations tab
	else if (strCommand == "FILETAB")
		bOK = FileTab(strSubCommand);

	// Commands from the Crop tab
	else if (strCommand == "CROPTAB")
		bOK = CropTab(iValue);

	// Commands from the Resize tab
	else if (strCommand == "RESIZETAB")
		bOK = ResizeTab(iValue, NULL);
	else if (strCommand == "RESIZEAPPLY")
		bOK = ResizeTab(EDIT_APPLY, strValue);

	// Commands from the Flip & Rotate tab
	else if (strCommand == "FLIPROTATETAB")
		bOK = FlipRotateTab(iValue);
	else if (strCommand == "FLIPROTATETABSUBCOMMAND")
		bOK = FlipRotateTabSubCommand(strSubCommand);

	// Commands from the Recolor Area Fill tab
	else if (strCommand == "RECOLORTAB")
		bOK = RecolorTab(iValue);
	else if (strCommand == "RECOLORTABSUBCOMMAND")
		bOK = RecolorTabSubCommand(strSubCommand);

	else if (strCommand == "SETPRIMARYCOLOR")
		m_DocWindow.m_ToolSettings.m_PrimaryColor = iValue;
	else if (strCommand == "SETSECONDARYCOLOR")
		m_DocWindow.m_ToolSettings.m_SecondaryColor = iValue;
	else if (strCommand == "TOLERANCE")
		m_DocWindow.m_ToolSettings.m_nTolerance = iValue;

	// Commands from the Special Effects tab
	else if (strCommand == "EFFECTSTAB")
		bOK = EffectsTab(iValue);
	else if (strCommand == "EFFECTSTABSUBCOMMAND")
		bOK = EffectsTabSubCommand(strSubCommand);

	// Commands from the Contrast & Brightness tab
	else if (strCommand == "CONTRASTBRIGHTNESSTAB")
		bOK = ContrastBrightnessTab(iValue);
	else if (strCommand == "AUTOCORRECT")
		bOK = AutoCorrect();

	// Commands from the Hue & Saturation tab
	else if (strCommand == "COLORSTAB")
		bOK = ColorsTab(iValue);
	else if (strCommand == "MAKEGRAY")
		bOK = MakeGray();
	else if (strCommand == "INVERT")
		return Invert();

	// Commands from the Red Eye Removal tab
	else if (strCommand == "REDEYETAB")
		bOK = RedEyeTab(iValue);

	// Commands from the Frames & Borders tab
	else if (strCommand == "BORDERSTAB")
		bOK = BordersTab(iValue);
	else if (strCommand == "BORDERIMAGE")
		bOK = BorderImageMerge(strValue);
	else if (strCommand == "BORDERTYPE")
		bOK = BorderType(strValue);
	else if (strCommand == "BORDERSIZE")
		bOK = BorderSize(iValue);
	else if (strCommand == "BORDERSOFTNESS")
		bOK = SetBorderSoftness(iValue);
	else if (strCommand == "BORDERSTRETCH")
		bOK = SetBorderStretch(!!iValue);
	else
		CMessageBox::Message(String("'%s' is not implemented.", strCommand));

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
	else if (strCommand == "GETANGLE")
		dwResult = GetAngle();
	else if (strCommand == "GETCROPLEFT")
		dwResult = GetCropLeft();
	else if (strCommand == "GETCROPTOP")
		dwResult = GetCropTop();
	else if (strCommand == "GETCROPRIGHT")
		dwResult = GetCropRight();
	else if (strCommand == "GETCROPBOTTOM")
		dwResult = GetCropBottom();
	else if (strCommand == "GETCANUNDO")
		dwResult = GetCanUndo();
	else if (strCommand == "GETMODIFIED")
		dwResult = GetModified();
	else
		CMessageBox::Message(String("'%s' is not implemented.", strCommand));

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::StartOver()
{
	CWaitCursor Wait;

	if (!m_DocWindow.GetSelectedObject())
		return false;

	return 	m_DocWindow.StartOver();
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

	if (!m_DocWindow.GetSelectedObject())
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

	if (!m_DocWindow.GetSelectedObject())
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	return m_DocWindow.ZoomEx(nZoomLevel, fZoomScale);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SetHidden()
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Invalidate the image pre-edit
	m_DocWindow.InvalidateImage(pObject);

	// Change the symbol
	pObject->SetHidden(!pObject->IsHidden());

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SetTransparent()
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Change the symbol
	bool bTransparent = (pObject->GetTransparentColor() != CLR_NONE);
	pObject->SetTransparent(!bTransparent);

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::FileTab(CString& strCommand)
{
	if (strCommand == "OPEN")
		return OpenImage();
	if (strCommand == "CLOSE")
		return CloseImage();
	if (strCommand == "SAVE")
		return SaveImage();

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::OpenImage()
{
	CWaitCursor Wait;

	LPCTSTR pstrFilter = (CImage::IsGdiPlusInstalled() ? FILTER_WITH_GDIPLUS : FILTER_WITHOUT_GDIPLUS);

	CFileDialog FileDialog(true, _T("jpg"), NULL/*pszFileName*/, 
		OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		pstrFilter, m_DocWindow/*hWndParent*/);

	FileDialog.m_ofn.lpstrTitle = _T("Open an Image File");
	if (FileDialog.DoModal(m_DocWindow) == IDCANCEL)
		return false;

	CString strFileName = FileDialog.m_ofn.lpstrFile;

	// Save the path in the registry
//j	if (regkey.m_hKey)
//j		bool bOK = (regkey.SetStringValue(REGVAL_MRU_PICTURE_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_DocWindow);

	if (SINGLE_IMAGE_EDITOR)
	{ // Close the existing image if you want to a single image editor
		if (m_DocWindow.GetSelectedObject())
			CloseImage();
	}

	CImageObject* pObject = m_DocWindow.OpenImage(strFileName, NULL/*pDib*/, NULL/*pMatrix*/);
	if (!pObject)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::OpenImageEx(BITMAPINFOHEADER* pDIB)
{
	if (SINGLE_IMAGE_EDITOR)
	{ // Close the existing image if you want to a single image editor
		if (m_DocWindow.GetSelectedObject())
			CloseImage();
	}

	CImageObject* pObject = m_DocWindow.OpenImage(NULL/*pszFileName*/, pDIB, NULL/*pMatrix*/);
	if (!pObject)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::CloseImage()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	m_DocWindow.CloseImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SaveImage()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	CString strFilePath = StripExtension(pObject->GetSourceFile());
	LPCTSTR pstrFilter = _T("All Supported Files (*.bmp, *.jpg)\0*.bmp;*.jpg\0Bitmap Files (*.bmp)\0*.bmp\0JPG Files (*.jpg)\0*.jpg\0All Files (*.*)\0*.*\0\0");
	CFileDialog FileDialog(false, _T("jpg"), strFilePath,
		OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		pstrFilter, m_DocWindow/*hWndParent*/);

	FileDialog.m_ofn.lpstrTitle = _T("Save an Image File");
	if (FileDialog.DoModal(m_DocWindow) == IDCANCEL)
		return false;

	CString strFileName = FileDialog.m_ofn.lpstrFile;

	bool bOK = false;
	BITMAPINFOHEADER* pDib = pObject->GetDib(true/*bForDraw*/); // DibForDraw gives us the rotated image
	if (pDib)
	{
		CString strExtension = StrExtension(strFileName);
		if (strExtension == ".bmp")
			bOK = DibWrite(pDib, strFileName);
		else
		if (strExtension == ".jpg")
			bOK = ConvertDibToJpg(pDib, 100, strFileName);
	}

	if (!bOK)
		CMessageBox::Message(String("Error saving to file '%s'.", strFileName));

	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::FlipRotateTab(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (iValue == EDIT_START)
	{
	}
	else
	if (iValue == EDIT_END)
	{
	}
	else
	if (iValue == EDIT_APPLY)
	{
	}
	else
	if (iValue == EDIT_RESET)
	{
	}
	else
	if (iValue >= 0)
	{
		m_DocWindow.GetSelect().SuspendTracking();
	}

	if (!pObject->FlipRotateTab(iValue))
		return false;

	// On interactive changes and Reset, we need to update the PageRect
	if (iValue >= 0 || iValue == EDIT_RESET)
	{
		bool bBaseObject = (pObject == m_DocWindow.GetBaseObject());
		if (bBaseObject)
			m_DocWindow.SetPageRect(pObject, false/*bUnZoom*/);

		m_DocWindow.GetSelect().ResumeTracking();
	}

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::FlipRotateTabSubCommand(CString& strCommand)
{
	if (strCommand == "FLIPHORIZONTAL")
		return FlipXY(true, false);
	else if (strCommand == "FLIPVERTICAL")
		return FlipXY(false, true);

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::FlipXY(bool bFlipX, bool bFlipY)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (!pObject->FlipXY(bFlipX, bFlipY))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::CropTab(int iValue)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (iValue == EDIT_START)
	{
		m_DocWindow.GetSelect().StartCrop();
	}
	else
	if (iValue == EDIT_END)
	{
		m_DocWindow.GetSelect().StartNormal();
	}
	else
	if (iValue == EDIT_APPLY)
	{
		if (!m_DocWindow.GetSelect().InCropMode())
			return false;

		bool bOK = CropApply();
		m_DocWindow.GetSelect().StopTracking(false/*bSuccess*/);
		m_DocWindow.GetSelect().StartCrop();
	}
	else
	if (iValue == EDIT_RESET)
	{
		m_DocWindow.GetSelect().StopTracking(false/*bSuccess*/);
		m_DocWindow.GetSelect().StartCrop();
	}
	else
	if (iValue >= 0) // SetCropAspect
	{
		if (!m_DocWindow.GetSelect().InCropMode())
			return false;

		m_DocWindow.GetSelect().SuspendTracking();
		bool bOK = CropSetAspect(iValue);
		m_DocWindow.GetSelect().ResumeTracking();
	}

	if (!pObject->CropTab(iValue))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::CropSetAspect(int iValue)
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	// Constrain the DestRect to the Aspect ratio and set it as the new CropRect
	CSize Aspect(LOWORD(iValue), HIWORD(iValue));
	CRect DestRect = pObject->GetDestRect();
	int dx = DestRect.Width();
	int dy = DestRect.Height();
	ScaleToFit(&dx, &dy, Aspect.cx, Aspect.cy, true/*bUseSmallerFactor*/);
	DestRect.left -= ((dx - DestRect.Width()) / 2);
	DestRect.top  -= ((dy - DestRect.Height()) / 2);
	DestRect.right  = DestRect.left + dx;
	DestRect.bottom = DestRect.top  + dy;

	pObject->SetCropRect(DestRect);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::CropApply()
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	// Invalidate the image pre-edit
	m_DocWindow.InvalidateImage(pObject);
	m_DocWindow.GetSelect().SuspendTracking();

	// Create a new cropped image
	CRect CropRectInPixels;
	GetDibCropRect(CropRectInPixels);
	if (!pObject->CropApply(CropRectInPixels))
		return false;

	// We need to update the PageRect
	bool bBaseObject = (pObject == m_DocWindow.GetBaseObject());
	if (bBaseObject)
		m_DocWindow.SetPageRect(pObject, false/*bUnZoom*/);

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	m_DocWindow.GetSelect().ResumeTracking();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ResizeTab(int iValue, LPCTSTR pSize)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	CSize Size(0,0);
	if (iValue == EDIT_START)
	{
	}
	else
	if (iValue == EDIT_END)
	{
	}
	else
	if (iValue == EDIT_APPLY)
	{
		m_DocWindow.GetSelect().SuspendTracking();

		CString strSize = pSize;
		int iIndex = strSize.Find(',');
		Size.cx = _ttol(strSize);
		Size.cy = _ttol(strSize.Mid(iIndex+1));
	}
	else
	if (iValue == EDIT_RESET)
	{
	}
	else
	if (iValue >= 0)
	{
	}

	// Invalidate the image pre-edit
	m_DocWindow.InvalidateImage(pObject);

	bool bOK = pObject->ResizeTab(iValue, Size);

	// On Apply, we need to update the PageRect
	if (iValue == EDIT_APPLY)
	{
		bool bBaseObject = (pObject == m_DocWindow.GetBaseObject());
		if (bBaseObject)
			m_DocWindow.SetPageRect(pObject, false/*bUnZoom*/);

		m_DocWindow.GetSelect().ResumeTracking();
	}
	
	if (!bOK)
		return false;
	
	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::RecolorTab(int iValue)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (iValue == EDIT_START)
	{
	}
	else
	if (iValue == EDIT_END)
	{
		m_DocWindow.m_ToolSettings.m_SelectedTool = TOOL_OFF;
	}
	else
	if (iValue == EDIT_APPLY)
	{
	}
	else
	if (iValue == EDIT_RESET)
	{
	}
	else
	if (iValue >= 0)
	{
	}

	if (!pObject->RecolorTab(iValue))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::RecolorTabSubCommand(CString& strCommand)
{
	if (strCommand == "SOLIDFILL")
		m_DocWindow.m_ToolSettings.m_SelectedTool = TOOL_SOLIDFILL;
	else
	if (strCommand == "SWEEPDOWN")
		m_DocWindow.m_ToolSettings.m_SelectedTool = TOOL_SWEEPDOWN;
	else
	if (strCommand == "SWEEPRIGHT")
		m_DocWindow.m_ToolSettings.m_SelectedTool = TOOL_SWEEPRIGHT;
	else
	if (strCommand == "SWEEPCENTER")
		m_DocWindow.m_ToolSettings.m_SelectedTool = TOOL_SWEEPCENTER;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::EffectsTab(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (iValue == EDIT_START)
	{
	}
	else
	if (iValue == EDIT_END)
	{
	}
	else
	if (iValue == EDIT_APPLY)
	{
	}
	else
	if (iValue == EDIT_RESET)
	{
	}
	else
	if (iValue >= 0)
	{
	}

	if (!pObject->EffectsTab(iValue))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::EffectsTabSubCommand(CString& strCommand)
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	return pObject->EffectsTabSubCommand(strCommand);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ContrastBrightnessTab(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (iValue == EDIT_START)
	{
	}
	else
	if (iValue == EDIT_END)
	{
	}
	else
	if (iValue == EDIT_APPLY)
	{
	}
	else
	if (iValue == EDIT_RESET)
	{
	}
	else
	if (iValue >= 0)
	{
	}

	if (!pObject->ContrastBrightnessTab(iValue))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::AutoCorrect()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	if (!pObject->AutoCorrect())
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::ColorsTab(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (iValue == EDIT_START)
	{
	}
	else
	if (iValue == EDIT_END)
	{
	}
	else
	if (iValue == EDIT_APPLY)
	{
	}
	else
	if (iValue == EDIT_RESET)
	{
	}
	else
	if (iValue >= 0)
	{
	}

	if (!pObject->ColorsTab(iValue))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::MakeGray()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	if (!pObject->MakeGray())
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::Invert()
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	if (!pObject->Invert())
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::RedEyeTab(int iValue)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (iValue == EDIT_START)
	{
		m_DocWindow.m_ToolSettings.m_SelectedTool = TOOL_REDEYE;
	}
	else
	if (iValue == EDIT_END)
	{
		m_DocWindow.m_ToolSettings.m_SelectedTool = TOOL_OFF;
	}
	else
	if (iValue == EDIT_APPLY)
	{
	}
	else
	if (iValue == EDIT_RESET)
	{
	}
	else
	if (iValue >= 0)
	{
	}

	if (!pObject->RedEyeTab(iValue))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::BordersTab(int iValue)
{
//j The wait cursor is distracting during interactive adjustments
//j	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	if (iValue == EDIT_START)
	{
	}
	else
	if (iValue == EDIT_END)
	{
	}
	else
	if (iValue == EDIT_APPLY)
	{
	}
	else
	if (iValue == EDIT_RESET)
	{
	}
	else
	if (iValue >= 0)
	{
	}

	if (!pObject->BordersTab(iValue))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::BorderImageChoose(CString& strFileName)
{
	LPCTSTR pstrFilter = (CImage::IsGdiPlusInstalled() ? FILTER_WITH_GDIPLUS : FILTER_WITHOUT_GDIPLUS);

	CFileDialog FileDialog(true, _T("jpg"), NULL/*pszFileName*/, 
		OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		pstrFilter, m_DocWindow/*hWndParent*/);

	FileDialog.m_ofn.lpstrTitle = _T("Open an Image File");
	if (FileDialog.DoModal(m_DocWindow) == IDCANCEL)
		return false;

	strFileName = FileDialog.m_ofn.lpstrFile;

	// Save the path in the registry
//j	if (regkey.m_hKey)
//j		bool bOK = (regkey.SetStringValue(REGVAL_MRU_PICTURE_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

	// Redraw the window underneath, before we get to work
	::UpdateWindow(m_DocWindow);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::BorderImageMerge(LPCTSTR pstrFileName)
{
	CWaitCursor Wait;

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	CString strFileName = pstrFileName;
	if (strFileName.IsEmpty() && !BorderImageChoose(strFileName))
		return false;

	if (!pObject->BorderImageMerge(strFileName, m_DocWindow.GetPageRect()))
		return false;

	// Invalidate the image post-edit
	m_DocWindow.InvalidateImage(pObject);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::BorderType(LPCTSTR pstrType)
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	return pObject->BorderType(pstrType);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::BorderSize(int nSize)
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	return pObject->BorderSize(nSize);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SetBorderSoftness(int nSoftness)
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	return pObject->BorderSoftness(nSoftness);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::SetBorderStretch(bool bStretch)
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	return pObject->BorderStretch(bStretch);
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetWidth()
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return 0;

	return pObject->GetWidth();
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetHeight()
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return 0;

	return pObject->GetHeight();
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetAngle()
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return 0;

	return dtoi(pObject->GetAngle());
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::GetDibCropRect(CRect& DibCropRect)
{
	DibCropRect.SetRectEmpty();

	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	return pObject->GetDibCropRect(DibCropRect);
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetCropLeft()
{
	CRect DibCropRect;
	GetDibCropRect(DibCropRect);
	return DibCropRect.left;
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetCropTop()
{
	CRect DibCropRect;
	GetDibCropRect(DibCropRect);
	return DibCropRect.top;
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetCropRight()
{
	CRect DibCropRect;
	GetDibCropRect(DibCropRect);
	return DibCropRect.right;
}

/////////////////////////////////////////////////////////////////////////////
long CDocCommands::GetCropBottom()
{
	CRect DibCropRect;
	GetDibCropRect(DibCropRect);
	return DibCropRect.bottom;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::GetCanUndo()
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	return pObject->EditCanUndo();
}

/////////////////////////////////////////////////////////////////////////////
bool CDocCommands::GetModified()
{
	CImageObject* pObject = m_DocWindow.GetSelectedObject();
	if (!pObject)
		return false;

	return pObject->GetModified();
}

