#pragma once
#include "AGDC.h"
#include "Dib.h"
#include "AGMatrix.h"

// CImageObject flags
#define SYMIMAGE_DIB_WRITTEN	1

class CAGDocIO
{
public:
	CAGDocIO() {}
	~CAGDocIO() {}
	void Read(void* pData, int iCount) {}
	void Write(void* pData, int iCount) {}
};

class CImageObject
{
public:
	CImageObject(const CRect& PageRect);
	CImageObject(const CRect& PageRect, const BITMAPINFOHEADER* pDIB);
	CImageObject(const CRect& PageRect, HGLOBAL hMemory);
	~CImageObject();
private:
	void Init();
	void FreeDIB();

public:
	void Close();

	CImageObject* Duplicate();
	void Draw(CAGDC& dc, DWORD dwFlags);
	bool HitTest(CPoint Pt, DWORD dwFlags);
private:
	bool LoadDIB(const BITMAPINFOHEADER* pDIB, bool bTakeOverMemory);
	CString DuplicateSourceFile();
	LPCSTR DetermineSourceType(BYTE* pMemory, DWORD dwSize);
	bool ReadStream(CAGDocIO* pInput);
	bool WriteStream(CAGDocIO* pOutput);
public:
	bool IsCoverAllowed() { return m_bCoverAllowed; }
	void GetDestInflateSize(SIZE& Size);
	const CRect& GetDestRect() const
		{ return m_DestRect; }
	void SetDestRect(const CRect& DestRect)
		{ m_DestRect = DestRect; }

	CRect GetDestRectTransformed()
		{
			CRect DestRect = GetDestRect();
			m_Matrix.Transform(DestRect);
			return DestRect;
		}

	int GetID() const
		{ return m_nID; }
	void SetID(int nID)
		{ m_nID = nID; }
	bool IsHidden(DWORD dwFlags = 0)
		{ return m_bDeleted /*| m_bHidden*/; } //j
	void SetHidden(bool bHidden)
		{ m_bHidden = bHidden; }
	bool IsDeleted()
		{ return m_bDeleted; }
	void SetDeleted(bool bDeleted)
		{ m_bDeleted = bDeleted; }
	const CAGMatrix& GetMatrix() const
		{ return m_Matrix; }
	void SetMatrix(const CAGMatrix& Matrix)
		{ m_Matrix = Matrix; }
	void Transform(const CAGMatrix& Matrix)
		{ m_Matrix *= Matrix; }
	CRect& GetPageRect()
		{ return m_PageRect; }
	CRect& GetCropRect()
		{ return m_CropRect; }
	void SetCropRect(CRect& CropRect)
		{ m_CropRect = CropRect; ::IntersectRect(&m_CropRect, &m_CropRect, &m_DestRect); }
	BITMAPINFOHEADER* GetDib()
		{ return m_pDIB;}
	void SetDib(BITMAPINFOHEADER* pDIB)
		{ m_pDIB = pDIB; }
	void SetTransparent(bool bOn)
		{ m_TransparentColor = (bOn ? m_LastHitColor : CLR_NONE); }
	COLORREF GetTransparentColor()
		{ return m_TransparentColor; }
	bool DoCoverDraw()
		{ return m_TransparentColor != CLR_NONE; }
	COLORREF GetLastHitColor()
		{ return m_LastHitColor; }
	void OrientChangeState(int iRotateDirection, bool bFlipX, bool bFlipY)
		{ CDib::OrientChangeState(m_cOrientation, iRotateDirection, bFlipX, bFlipY); }
	bool GetOrient(int& iRotateDirection, bool& bFlipX, bool& bFlipY)
		{ return CDib::OrientDecode(m_cOrientation, iRotateDirection, bFlipX, bFlipY); }
	CString& GetSourceType()
		{ return m_strSourceType; }
	bool CanChangeOrientation()
		{ return m_strSourceType == "DIB"; }
	void SetModified(bool bModified)
		{ m_bModified = bModified; }

	bool WriteFile(LPCTSTR strFileName, HGLOBAL hMemory, CString* pstrSourceType = NULL);
	bool Open(LPCTSTR strFileName);
	bool ReOpen();
	bool MakeGray();
	bool Sharpen(int iValue);
	bool Smooth(int iValue);
	bool Edges(int iValue);

	bool AutoCorrect();
	bool Contrast(int iValue);
	bool Brightness(int iValue);

	bool HSLAmount(int iValue);
	bool Hue(int iValue);
	bool Saturation(int iValue);

	bool RedEyeReset();
	bool RedEyeFix();

protected:
	int m_nID;
	bool m_bHidden;
	bool m_bDeleted;
	bool m_bModified;
	bool m_bCoverAllowed;
	bool m_bSourceFileIsTemporary;
	CRect m_PageRect;
	CRect m_DestRect;
	CRect m_CropRect;
	CAGMatrix m_Matrix;
	COLORREF m_LastHitColor;
	COLORREF m_TransparentColor;
	BITMAPINFOHEADER* m_pDIB;
	BITMAPINFOHEADER* m_pDIBPrevious;
	BYTE m_cOrientation;
	CString m_strSourceFile;
	CString m_strSourceType;
};
