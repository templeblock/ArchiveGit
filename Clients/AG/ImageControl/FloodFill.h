#pragma once

#include "FloodFill2.h"
#include "DocCommands.h"

class CFloodFill
{
public:
	CFloodFill(void);
	~CFloodFill(void);

	bool Init(BITMAPINFOHEADER* pDibToEdit, BITMAPINFOHEADER* pDibToRead,
		SelectedTool selectedTool, int nTolerance);
	bool Fill(RECT* pRect,int x,int y, SelectedTool selectedTool, 
		COLORREF FromColor, COLORREF ToColor);
	bool FillDryRun(RECT* pRect, int x, int y, SelectedTool selectedTool,
		COLORREF FromColor, COLORREF ToColor);

	RECT GetInvalidRect() const { return m_rcInvalid; }

protected:
	bool DoFill(RECT* pRect, int x, int y, COLORREF FromColor, CFloodFill2& DibToEdit);

	typedef struct hlineNode_st {
		int x1,x2,y,dy;
		struct hlineNode_st *pNext,*pPrev;
	} HLINE_NODE;

	bool MakeList(void);
	void FreeList(void);
	void PushLine(int x1,int x2,int y,int dy);
	void PopLine(int *x1,int *x2,int *y,int *dy);

	void PushVisitedLine(int x1,int x2,int y);
	BOOL ExpandVisitedBlock(int x1,int x2,int y);
	BOOL IsRevisit(int x1,int x2,int y);

	HLINE_NODE*	m_pVisitList;
	HLINE_NODE*	m_pLineList;
	HLINE_NODE*	m_pFreeList;
	bool m_bMemError;
	int m_LastY;
	CFloodFill2 m_DibToRead;
	CFloodFill2 m_DibToEdit;
	CFloodFill2 m_BufferDibData;
	CRect m_rcInvalid;
};
