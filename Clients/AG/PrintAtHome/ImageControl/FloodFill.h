#pragma once

#include "DibData.h"

class CFloodFill
{
public:
	CFloodFill(void);
	~CFloodFill(void);

	bool Fill(BITMAPINFOHEADER* pBitmap,LPRECT pRect,int x,int y,
		COLORREF fill_color,COLORREF border_color=CLR_INVALID);

	RECT GetInvalidRect() const { return m_rcInvalid; }

protected:
	DWORD GetPixel(int x, int y);
	BOOL SetPixel(int x, int y, DWORD dwValue);
	void DrawHorizontalLine(int x1, int x2, int y, DWORD dwValue);
	int ScanLeft(int x,int y,int xmin,DWORD dwValue);
	int SearchLeft(int x,int y,int xmin,DWORD dwValue);
	int ScanRight(int x,int y,int xmax,DWORD dwValue);
	int SearchRight(int x,int y,int xmax,DWORD dwValue);

	private:
	typedef struct hlineNode_st {
		int x1,x2,y,dy;
		struct hlineNode_st *pNext,*pPrev;
	} HLINE_NODE;

	HLINE_NODE*	m_pVisitList;
	HLINE_NODE*	m_pLineList;
	HLINE_NODE*	m_pFreeList;
	BOOL	m_bXSortOn;
	BOOL	m_bMemError;
	int		m_LastY;

	int MakeList(void);
	void FreeList(void);
	void PushLine(int x1,int x2,int y,int dy);
	void PopLine(int *x1,int *x2,int *y,int *dy);
	HLINE_NODE* FindNextLine(int x1,int x2,int y);
	void PopThis(HLINE_NODE *pThis);
	void PushOpposite(int OldX1,int OldX2,int x1,int x2,int y,int dy);

	void PushVisitedLine(int x1,int x2,int y);
	BOOL ExpandVisitedBlock(int x1,int x2,int y);
	BOOL IsRevisit(int x1,int x2,int y);

	// Private bitmap data members
	CDibData m_DibData;
	BYTE m_FillMask[8];

	// Private information members
	RECT m_rcInvalid;
};
