#pragma once

class CDocWindow;

typedef enum 
{
	RECOLOR_OFF,
	RECOLOR_SOLIDFILL,
	RECOLOR_SWEEPDOWN,
	RECOLOR_SWEEPRIGHT,
	RECOLOR_SWEEPCENTER
} RECOLOR_MODE;

class CDocCommands
{
public:
	CDocCommands(CDocWindow& DocWindow);
	~CDocCommands(void);

	bool DoCommand(LPCTSTR strCommand, LPCTSTR strValue);
	long GetCommand(LPCTSTR strCommand);

private:
	bool StartOver();
	bool Undo();
	bool Redo();
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
	bool Apply();

	bool Open();
	bool Close();
	bool Save();

	bool FlipHorizontal();
	bool FlipVertical();
	bool FlipXY(bool bFlipX, bool bFlipY);
	bool RotateLeft();
	bool RotateRight();
	bool Rotate(bool bCW);
	bool CropStart();

	bool Recolor();
	bool DisableFill();
	bool SolidFill();
	bool SweepDown();
	bool SweepRight();
	bool SweepCenter();
	bool SetPrimaryColor(LPCTSTR pColor);
	bool SetSecondaryColor(LPCTSTR pColor);
	bool SelectColor(LPCTSTR pPrimary);

	bool Sharpen(int iValue);
	bool Smooth(int iValue);
	bool Edges(int iValue);

	bool AutoCorrect();
	bool Contrast(int iValue);
	bool Brightness(int iValue);

	bool HSLAmount(int iValue);
	bool Hue(int iValue);
	bool Saturation(int iValue);
	bool MakeGray();

	bool RedEyeReset();
	bool RedEyeFix();

	bool OpenBorderImage();

	long GetWidth();
	long GetHeight();

private:
	CDocWindow& m_DocWindow;
};
