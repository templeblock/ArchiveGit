#pragma once

#include "Track.h"
#include "Matrix.h"
#include "ImageObject.h"

#define TOOLCODE_ABORT	-1
#define TOOLCODE_UNUSED	 0
#define TOOLCODE_DONE	 1
#define TOOLCODE_UPDATE	 2

class CDocWindow;

class CSelect
{
protected:
    enum SEL_MODE
    {
		NO_MODE = 0,
		RESUME_MODE = 1,
		NORMAL_MODE = 2,
		TRANS_MODE = 3,
		CROP_MODE = 4,
	};

public:
    CSelect(CDocWindow* pDocWnd);
    ~CSelect();

	void Select(CImageObject* pObject, CPoint* pClickPoint = NULL);
    void Unselect(bool bUpdateUI = true);

	void Paint(HDC pDC);
	bool KeyDown(int nChar);
	bool KeyUp(int nChar);
	bool KeyChar(int nChar);
	bool ButtonDown(int x, int y, CImageObject* pHitObject);
	bool ButtonUp(int x, int y);
	bool DoubleClick(int x, int y);
	bool RightClick(int x, int y);
	bool SetCursor(int x, int y, CImageObject* pHitObject);
	bool MouseMove(int x, int y);
	bool SetFocus();
	bool KillFocus();
protected:
	void StartTracking(SEL_MODE uObjectMode);
public:
	void StopTracking(bool bSuccess);
	void SuspendTracking();
	void ResumeTracking();

	void StartNormal();
	void StartTransform();
	void StartCrop();

	bool IsSelected(CImageObject* pObject);
	CImageObject* GetSelected();
	const CMatrix& GetMatrix();
	const CRect& GetDestRect();

    bool InTrackingMode()	{ return (m_uMode != NO_MODE); }
    bool InNormalMode()		{ return (m_uMode == NORMAL_MODE); }
    bool InTransformMode()	{ return (m_uMode == TRANS_MODE); }
	bool InCropMode()		{ return (m_uMode == CROP_MODE); }

protected:
	static void NormalDrawProcEx(HDC hDC, bool bOn, int iTrackingCode, void* pData);
	void NormalDrawProc(HDC hDC, bool bOn, int iTrackingCode);
	static void TransformDrawProcEx(HDC hDC, bool bOn, int iTrackingCode, void* pData);
	void TransformDrawProc(HDC hDC, bool bOn, int iTrackingCode);
	static void CropDrawProcEx(HDC hDC, bool bOn, int iTrackingCode, void* pData);
	void CropDrawProc(HDC hDC, bool bOn, int iTrackingCode);

protected:
    CDocWindow* m_pDocWnd;
	CImageObject* m_pSelObject;
    SEL_MODE m_uMode;
    SEL_MODE m_uModeSuspended;
	CTrack m_Track;
	CRect m_StartingCropRect;
	CMatrix m_StartingMatrix;
};
