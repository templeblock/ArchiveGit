#pragma once

#include "Track.h"
#define IDD_IME_BORDERS_TAB                 20101
#define IDD_IME_COLORS_TAB                  20102
#define IDD_IME_CROP_TAB                    20103
#define IDD_IME_CONTRAST_BRIGHTNESS_TAB     20104
#define IDD_IME_EFFECTS_TAB                 20105
#define IDD_IME_FILE_TAB                    20106
#define IDD_IME_FLIP_ROTATE_TAB             20107
#define IDD_IME_MAIN_TAB                    20108
#define IDD_IME_RECOLOR_TAB                 20109
#define IDD_IME_REDEYE_TAB                  20110
#define IDD_IME_RESIZE_TAB                  20111


#define TOOLCODE_ABORT	-1
#define TOOLCODE_UNUSED	 0
#define TOOLCODE_DONE	 1
#define TOOLCODE_UPDATE	 2

class CAGSym;
class CAGSymImage;
class CAGSymGraphic;
class CAGSymCalendar;
class CAGSymText;
class CAGSymLine;
class CDocWindow;

class CSelect
{
protected:
    enum SEL_MODE
    {
		NO_MODE = -1,
		EDIT_MODE = 1,
		TRANS_MODE = 2,
		CROP_MODE = 3
	};

public:
    CSelect(CDocWindow* pDocWnd);
    ~CSelect();

	void ResumeDrawing();
	void PauseDrawing();
    void SymbolSelect(CAGSym* lpSym, POINT* pClickPoint = NULL);
    void SymbolUnselect(bool bClearPointer, bool fSetPanel = true);
	void SetTextEditMode(bool bOn, POINT* pClickPoint = NULL);
	void SuspendTracking();
	void ResumeTracking();
	void StartTransform();
    void StartTextRotate();
	void StartCrop();
	void Paint(HDC pDC);
	bool KeyDown(int nChar);
	bool KeyUp(int nChar);
	bool KeyChar(int nChar);
	bool ButtonDown(int x, int y, CAGSym* pHitSym);
	bool ButtonUp(int x, int y);
	bool ButtonDblClick(int x, int y);
	bool SetCursor(int x, int y, CAGSym* pHitSym);
	bool MouseMove(int x, int y);
	bool Timer(WPARAM wParam);
	bool SetFocus();
	bool KillFocus();
	bool LaunchImageEditor(UINT iTabID=IDD_IME_MAIN_TAB);
	void ShowTextSelection(HDC hDC, bool bCaretOnly, bool bShow);

	bool NoSymSelected();
	bool SymSelected();
	bool SymIsAddAPhoto();
	bool SymIsText();
	bool SymIsImage();
	bool SymIsCalendar();
	bool SymIsGraphic();
	bool SymIsShape();
	bool SymIsLine();
	bool SymIsLocked();
	bool SymIsClipArt();

	// Current Symbol functions and Wrappers
	void SetSym(CAGSym* pSym) { m_pSelSym = pSym; }
	CAGSym* GetSym() { return m_pSelSym; }
	CAGSymImage* GetImageSym();
	CAGSymGraphic* GetGraphicSym();
	CAGSymCalendar* GetCalendarSym();
	CAGSymText* GetTextSym();
	CAGSymLine* GetLineSym();
	CString& GetImgNativeType();
	const CAGMatrix& GetMatrix();
	const RECT& GetDestRect();

protected:
	bool IsTextSymbolInEditMode() { return ((SymIsText() || (SymIsAddAPhoto() && SpecialsEnabled())) && GetTextSym()->InEditMode()); }
    bool InEditMode()		{ return (m_uMode == EDIT_MODE); }
    bool InTransformMode()	{ return (m_uMode == TRANS_MODE); }
	bool InCropMode()		{ return (m_uMode == CROP_MODE); }
    bool InTrackingMode()	{ return (m_uMode != NO_MODE); }

	void StartTracking(SEL_MODE uSymMode);
	void StopTracking(bool bSuccess);
	bool SymIsEqual(CAGSym* pSym);
	bool SpecialsEnabled();

	static void TransformDrawProcEx(HDC hDC, bool bOn, int iTrackingCode, void* pData);
	void TransformDrawProc(HDC hDC, bool bOn, int iTrackingCode);
	static void CropDrawProcEx(HDC hDC, bool bOn, int iTrackingCode, void* pData);
	void CropDrawProc(HDC hDC, bool bOn, int iTrackingCode);

protected:
    CDocWindow* m_pDocWnd;
	CAGSym* m_pSelSym;
    SEL_MODE m_uMode;
    SEL_MODE m_uModeSuspended;
	CTrack m_Track;
	RECT m_LastCheckPointCropRect;
	CAGMatrix m_LastCheckPointMatrix;
};
