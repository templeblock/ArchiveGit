#include "stdafx.h"
#include "FloodFill.h"
#include "Dib.h"

/////////////////////////////////////////////////////////////////////////////
CFloodFill::CFloodFill(void)
{
	m_pVisitList = NULL;
	m_pLineList = NULL;		// List of lines to be visited.
	m_pFreeList = NULL;		// List of free nodes.
	m_bMemError = false;	// Memory error flag.
	::SetRectEmpty(&m_rcInvalid);
}

/////////////////////////////////////////////////////////////////////////////
CFloodFill::~CFloodFill(void)
{
	FreeList();
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill::Init(BITMAPINFOHEADER* pDibToEdit, BITMAPINFOHEADER* pDibToRead, SelectedTool selectedTool, int nTolerance)
{
	// Create dib holders
	if (!m_DibToEdit.InitDIB(pDibToEdit, false/*bCopy*/))
		return false;

	if (!m_DibToRead.InitDIB(pDibToRead, false/*bCopy*/))
		return false;

	bool bNeedBufferDib = (selectedTool != TOOL_SOLIDFILL && selectedTool != TOOL_REDEYE);
	if (bNeedBufferDib)
	{
		if (!m_BufferDibData.InitDIB(pDibToRead, true/*bCopy*/))
			return false;

		m_DibToEdit.SetBufferDib(&m_BufferDibData);
		m_DibToRead.SetBufferDib(&m_BufferDibData);
	}

	bool bRedEye = (selectedTool == TOOL_REDEYE);
	m_DibToEdit.SetRedEye(bRedEye);
	m_DibToRead.SetRedEye(bRedEye);
	m_BufferDibData.SetRedEye(bRedEye);

	m_DibToEdit.SetTolerance(nTolerance);
	m_DibToRead.SetTolerance(nTolerance);
	m_BufferDibData.SetTolerance(nTolerance);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill::Fill(RECT* pRect, int x, int y, SelectedTool selectedTool, COLORREF FromColor, COLORREF ToColor)
{
	if (selectedTool != TOOL_SOLIDFILL && selectedTool != TOOL_REDEYE)
	{
		if (selectedTool == TOOL_SWEEPCENTER)
			m_BufferDibData.DrawRadialGradient(pRect, FromColor, ToColor, true);
		else
			m_BufferDibData.DrawGradRect(pRect, FromColor, ToColor, (selectedTool == TOOL_SWEEPDOWN));
	}

	bool bOK = DoFill(pRect, x, y, FromColor, m_DibToEdit/*DibToEdit*/);

	if (selectedTool == TOOL_REDEYE)
		m_DibToEdit.DrawRedEyeCircle(pRect);
	
	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill::FillDryRun(RECT* pRect,int x,int y, SelectedTool selectedTool, COLORREF FromColor, COLORREF ToColor)
{
	if (selectedTool == TOOL_SOLIDFILL || selectedTool == TOOL_REDEYE)
		return true;

	return DoFill(pRect, x, y, FromColor, m_BufferDibData/*DibToEdit*/);
}

/////////////////////////////////////////////////////////////////////////////
bool CFloodFill::DoFill(RECT* pRect, int x, int y, COLORREF FromColor, CFloodFill2& DibToEdit)
{
	m_LastY = -1;

	// Get seed pixel
	COLORREF Color = m_DibToRead.GetPixel(x, y);
	if (Color == CLR_NONE)
		return false;

	// Initialize Line list
	if (!MakeList())
		return false;

    // Initialize coords
	int MaxX = DibToEdit.GetWidth()  - 1;
	int MaxY = DibToEdit.GetHeight() - 1;
	int MinX = 0;
	int MinY = 0;
	if (pRect)
	{
		if (0 < pRect->left && pRect->left < MaxX)
			MinX = pRect->left;
		if (0 < pRect->right && pRect->right < MaxX)
			MaxX = pRect->right;
		if (0 < pRect->top && pRect->top < MaxY)
			MinY = pRect->top;
		if (0 < pRect->bottom && pRect->bottom < MaxY)
			MaxY = pRect->bottom;
	}

	// Push starting point on stack.
	// During testing calling ScanLeft() & ScanRight() here reduced the number
	// of revisits by 1 and the number of items on the visit list by 2
	int ChildLeft  = m_DibToRead.ScanLeft(x, y, MinX, Color, false/*bMatch*/) + 1;
	int ChildRight = m_DibToRead.ScanRight(x, y, MaxX, Color, false/*bMatch*/) - 1;
	PushLine(ChildLeft, ChildRight, y, +1); // Needed in one special case
	PushLine(ChildLeft, ChildRight, ++y, -1);

	// Initialize invalid rectangle
	m_rcInvalid.left = m_rcInvalid.right  = x;
	m_rcInvalid.top  = m_rcInvalid.bottom = y;

	// Now start flooding
	int dy = 0;
	int ParentLeft = 0;
	int ParentRight = 0;
	while (m_pLineList) 
	{
		PopLine(&ParentLeft, &ParentRight, &y, &dy);
		y += dy;
		if (y < MinY || MaxY < y) 
			continue;

		if (m_bMemError) 
			continue;

		if (IsRevisit(ParentLeft, ParentRight, y)) 
			continue;

		// Add line to the visited list
		PushVisitedLine(ParentLeft, ParentRight, y);

		if (ParentLeft < m_rcInvalid.left)
			m_rcInvalid.left = ParentLeft;
		else
		if (ParentRight > m_rcInvalid.right)
			m_rcInvalid.right = ParentRight;
		if (y < m_rcInvalid.top)
			m_rcInvalid.top = y;
		else
		if (y > m_rcInvalid.bottom)
			m_rcInvalid.bottom = y;

		// Find ChildLeft end (ChildLeft>ParentLeft on failure)
		ChildLeft = m_DibToRead.ScanLeft(ParentLeft, y, MinX, Color, false/*bMatch*/) + 1;
		if (ChildLeft <= ParentLeft) 
		{
			// Find ChildRight end (this should not fail here)
			ChildRight = m_DibToRead.ScanRight(ParentLeft+1, y, MaxX, Color, false/*bMatch*/) - 1;

			// Fill line
			DibToEdit.DrawLineH(ChildLeft, ChildRight, y, FromColor, pRect);

			// Push unvisited lines
			if (ParentLeft-1<=ChildLeft && ChildRight<=ParentRight+1) 
				PushLine(ChildLeft,ChildRight,y,dy);
			else
			{
				if (ChildLeft == ParentLeft) 
					PushLine(ParentRight+2,ChildRight,y,-dy);
				else
				if (ChildRight == ParentRight) 
					PushLine(ChildLeft,ParentLeft-2,y,-dy);
				else
					PushLine(ChildLeft,ChildRight,y,-dy);

				PushLine(ChildLeft,ChildRight,y,dy);
			}

			// Addvance ChildRight end on to border
			++ChildRight;
		}
		else
			ChildRight = ParentLeft;

		// Fill betweens
		while (ChildRight < ParentRight) 
		{
			// Skip to new ChildLeft end (ChildRight>ParentRight on failure)
			ChildRight = m_DibToRead.ScanRight(ChildRight+1, y, ParentRight, Color, true/*bMatch*/);
			// If new ChildLeft end found
			if (ChildRight<=ParentRight) 
			{
				ChildLeft = ChildRight;

				// Find ChildRight end (this should not fail here)
				ChildRight = m_DibToRead.ScanRight(ChildLeft+1, y, MaxX, Color, false/*bMatch*/) - 1;

				// Fill line
				DibToEdit.DrawLineH(ChildLeft, ChildRight, y, FromColor, pRect);

				// Push unvisited lines
				if (ChildRight <= ParentRight+1) 
					PushLine(ChildLeft,ChildRight,y,dy);
				else
				{
					PushLine(ChildLeft,ChildRight,y,-dy);
					PushLine(ChildLeft,ChildRight,y,dy);
				}

				// Advance ChildRight end onto border
				++ChildRight;
			}
		}
	}

	FreeList();
	*pRect = m_rcInvalid;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize Line list
bool CFloodFill::MakeList(void)
{
	m_pFreeList = (HLINE_NODE*)calloc(1,sizeof(HLINE_NODE));
	return (m_pFreeList != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Frees the list of free nodes
void CFloodFill::FreeList(void)
{
	HLINE_NODE* pNext = NULL;
	while (m_pFreeList) 
	{
		pNext = m_pFreeList->pNext;
	  	free(m_pFreeList);
	  	m_pFreeList = pNext;
	}

	while (m_pLineList) 
	{
		pNext = m_pLineList->pNext;
	  	free(m_pLineList);
	  	m_pLineList = pNext;
	}
	
	while (m_pVisitList) 
	{
		pNext = m_pVisitList->pNext;
	  	free(m_pVisitList);
	  	m_pVisitList = pNext;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Push a node onto the line list
void CFloodFill::PushLine(int x1,int x2,int y,int dy)
{
	HLINE_NODE *pNew = m_pFreeList;
	if (pNew)
		m_pFreeList = m_pFreeList->pNext;
	else
		pNew = (HLINE_NODE*)calloc(1,sizeof(HLINE_NODE));
	// Add to start of list
	if (pNew)
	{
		pNew->x1 = x1;
		pNew->x2 = x2;
		pNew->y  = y;
		pNew->dy = dy;
		pNew->pNext = m_pLineList;
		m_pLineList = pNew;
	}
	else
		m_bMemError = true;
}

/////////////////////////////////////////////////////////////////////////////
// Pop a node off the line list
void CFloodFill::PopLine(int *x1,int *x2,int *y,int *dy)
{
	//ATLTRACE(_T("PopLine\n"));
	if (!m_pLineList) 
		return;

	HLINE_NODE *pThis,*pPrev;
	pPrev = m_pLineList;
	for (pThis=m_pLineList->pNext;pThis;pThis=pThis->pNext) 
	{
		if (pThis->y == m_LastY)
			break;
		pPrev = pThis;
	}
	// If pThis found - remove it from list
	if (pThis) 
	{
		pPrev->pNext = pThis->pNext;
		if (pPrev->pNext)
			pPrev->pNext->pPrev = pPrev;
		*x1 = pThis->x1;
		*x2 = pThis->x2;
		*y  = pThis->y;
		*dy = pThis->dy;
	}
	// Remove from start of list
	else
	{
		*x1 = m_pLineList->x1;
		*x2 = m_pLineList->x2;
		*y  = m_pLineList->y;
		*dy = m_pLineList->dy;
		pThis = m_pLineList;
		m_pLineList = m_pLineList->pNext;
		if (m_pLineList)
			m_pLineList->pPrev = (HLINE_NODE*)0;
	}

	pThis->pNext = m_pFreeList;
	m_pFreeList = pThis;
	m_LastY = *y;
}

/////////////////////////////////////////////////////////////////////////////
// Adds line to visited block list
void CFloodFill::PushVisitedLine(int x1,int x2,int y)
{
	//ATLTRACE(_T("PushVisitedLine\n"));
	HLINE_NODE *pNew = m_pFreeList;
	if (pNew)
		m_pFreeList = m_pFreeList->pNext;
	else
		pNew = (HLINE_NODE*)calloc(1,sizeof(HLINE_NODE));
	// Add to start of list
	if (pNew)
	{
		pNew->x1 = x1;
		pNew->x2 = x2;
		pNew->y  = y;
		pNew->pNext = m_pVisitList;
		m_pVisitList = pNew;
	}
	else
		m_bMemError = true;
}

/////////////////////////////////////////////////////////////////////////////
// Checks if line has already been visited
BOOL CFloodFill::IsRevisit(int x1,int x2,int y)
{
	//ATLTRACE(_T("IsRevisit\n"));
	HLINE_NODE* pNext = m_pVisitList;
	while (pNext)
	{
		if (pNext->y == y && pNext->x1 <= x1 && x2 <= pNext->x2)
			break;

		pNext = pNext->pNext;
	}

	return (pNext != NULL);
}
