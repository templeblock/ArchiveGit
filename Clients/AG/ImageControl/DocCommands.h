#pragma once
#include "ImageObject.h"

class CDocWindow;

class CDocCommands
{
public:
	friend CDocWindow;
public:
	CDocCommands(CDocWindow& DocWindow);
	~CDocCommands(void);

	bool DoCommand(LPCTSTR strCommand, LPCTSTR strValue);
	long GetCommand(LPCTSTR strCommand);
	bool OpenImageEx(BITMAPINFOHEADER* pDIB);

private:
	// Commands from the Main Dialog
	bool StartOver();
	bool ZoomIn();
	bool ZoomOut();
	bool Zoom(int iOffset);
	bool ZoomFull();
	bool ZoomLevel1();
	bool ZoomLevel2();
	bool ZoomLevel3();
	bool ZoomLevel4();
	bool ZoomLevel5();
	bool ZoomEx(UINT nZoomLevel, double fZoomScale);
	bool SetHidden();
	bool SetTransparent();

	// Commands from the File Operations tab
	bool FileTab(CString& strCommand);
	bool OpenImage();
	bool CloseImage();
	bool SaveImage();

	// Commands from the Flip & Rotate tab
	bool FlipRotateTab(int iValue);
	bool FlipRotateTabSubCommand(CString& strCommand);
	bool FlipXY(bool bFlipX, bool bFlipY);

	// Commands from the Crop tab
	bool CropTab(int iValue);
	bool CropSetAspect(int iValue);
	bool CropApply();

	// Commands from the Resize tab
	bool ResizeTab(int iValue, LPCTSTR pSize);

	// Commands from the Recolor Area Fill tab
	bool RecolorTab(int iValue);
	bool RecolorTabSubCommand(CString& strCommand);

	// Commands from the Special Effects tab
	bool EffectsTab(int iValue);
	bool EffectsTabSubCommand(CString& strCommand);

	// Commands from the Hue & Saturation tab
	bool ColorsTab(int iValue);
	bool MakeGray();
	bool Invert();

	// Commands from the Contrast & Brightness tab
	bool ContrastBrightnessTab(int iValue);
	bool AutoCorrect();

	// Commands from the Red Eye Removal tab
	bool RedEyeTab(int iValue);

	// Commands from the Frames & Borders tab
	bool BordersTab(int iValue);
	bool BorderImageChoose(CString& strFileName);
	bool BorderImageMerge(LPCTSTR pstrFileName);
	bool BorderType(LPCTSTR pstrType);
	bool BorderSize(int nSize);
	bool SetBorderSoftness(int nSoftness);
	bool SetBorderStretch(bool bStretch);

	// GetCommands
	long GetWidth();
	long GetHeight();
	long GetAngle();
	bool GetDibCropRect(CRect& DibCropRect);
	long GetCropLeft();
	long GetCropTop();
	long GetCropRight();
	long GetCropBottom();
	bool GetCanUndo();
	bool GetModified();

private:
	CDocWindow& m_DocWindow;
};
