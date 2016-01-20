#include "stdafx.h"
#include "FloodFill.h"
#include "Dib.h"

const BYTE   _BitMask[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

CFloodFill::CFloodFill(void)
{
	m_pVisitList = NULL;
	m_pLineList = NULL;		// List of lines to be visited.
	m_pFreeList = NULL;		// List of free nodes.
	m_bXSortOn = FALSE;		// X-Sorting flag (single visit indicator).
	m_bMemError = FALSE;	// Memory error flag.
	memset(&m_rcInvalid,0,sizeof(m_rcInvalid));
	memset(m_FillMask,0xFF,8);		// Current fill mask
}

CFloodFill::~CFloodFill(void)
{
	FreeList();
	m_DibData.DeleteObject();
}

bool CFloodFill::Fill(
	BITMAPINFOHEADER* pBitmap, LPRECT pRect,int x,int y,
	COLORREF fill_color,COLORREF border_color/*=CLR_INVALID*/)
{
	DWORD ThisColorValue, FillValue, BorderValue;
	int MinY,MinX,MaxY,MaxX,dy;
	int ChildLeft,ChildRight;
	int ParentLeft,ParentRight;

	// Create dib data object
	if( !m_DibData.InitDIB(pBitmap) )
		return false;

	FillValue   = m_DibData.RGBtoValue(fill_color);
	BorderValue = m_DibData.RGBtoValue(border_color);

	m_bXSortOn = m_bMemError = FALSE;
	m_LastY = -1;

	/* Get seed pixel */
	ThisColorValue = GetPixel(x,y);
	
	if (ThisColorValue == CLR_INVALID)
		return false;

	if (FillValue == ThisColorValue)
		return false;

	/* Using macros because we can not use pointers to non-static
	 * member functions.
	 * This makes the code less efficient, but solves the problem.
	 */
#define FindLeft(x,y,xmin,color) \
	((CLR_INVALID != BorderValue) \
	? SearchLeft(x,y,xmin,color) : ScanLeft(x,y,xmin,color))
#define FindRight(x,y,xmax,color) \
	((CLR_INVALID != BorderValue) \
	? SearchRight(x,y,xmax,color) : ScanRight(x,y,xmax,color))
#define SkipRight(x,y,xmax,color) \
	((CLR_INVALID != BorderValue) \
	? ScanRight(x,y,xmax,color) : SearchRight(x,y,xmax,color))

	/* Initialize Line list */
	if( MakeList() ) {
		m_DibData.DeleteObject();
		return false;
	}

    /* Initialize coords */
	MaxX = m_DibData.GetWidth()  - 1;
	MaxY = m_DibData.GetHeight() - 1;
	MinX = MinY = 0;
	if( pRect )
	{
		if( 0 < pRect->left && pRect->left < MaxX )
			MinX = pRect->left;
		if( 0 < pRect->right && pRect->right < MaxX )
			MaxX = pRect->right;
		if( 0 < pRect->top && pRect->top < MaxY )
			MinY = pRect->top;
		if( 0 < pRect->bottom && pRect->bottom < MaxY )
			MaxY = pRect->bottom;
	}

	//if( x < MinX || MaxX < x || y < MinY || MaxY < y ) {
	//	m_DibData.DeleteObject();
	//	return;
	//}

	/* Initialize invalid rectangle */
	m_rcInvalid.left = m_rcInvalid.right  = x;
	m_rcInvalid.top  = m_rcInvalid.bottom = y;

	/* Push starting point on stack.
	 * Durring testing calling FindLeft() & FindRight() here reduced the number
	 * of revisits by 1 and the number of items on the visit list by 2.
	 */
	ChildLeft  = FindLeft(x,y,MinX,ThisColorValue)+1;
	ChildRight = FindRight(x,y,MaxX,ThisColorValue)-1;
	PushLine(ChildLeft,ChildRight,y,+1); /* Needed in one special case */
	PushLine(ChildLeft,ChildRight,++y,-1);

	/* Now start flooding */
	while( m_pLineList ) 
	{
		PopLine(&ParentLeft,&ParentRight,&y,&dy);
		y += dy;
		if( y < MinY || MaxY < y ) 
		{
			continue;
		}
		if( m_bMemError ) 
		{
			continue;
		}
		if( m_bXSortOn && IsRevisit(ParentLeft,ParentRight,y) ) 
		{
			continue;
		}

		if( ParentLeft < m_rcInvalid.left )
			m_rcInvalid.left = ParentLeft;
		else if( ParentRight > m_rcInvalid.right )
			m_rcInvalid.right = ParentRight;
		if( y < m_rcInvalid.top )
			m_rcInvalid.top = y;
		else if( y > m_rcInvalid.bottom )
			m_rcInvalid.bottom = y;
		/* Find ChildLeft end ( ChildLeft>ParentLeft on failure ) */
		ChildLeft = FindLeft(ParentLeft,y,MinX,ThisColorValue)+1;
		if( ChildLeft<=ParentLeft ) 
		{
			/* Find ChildRight end ( this should not fail here ) */
			ChildRight = FindRight(ParentLeft+1,y,MaxX,ThisColorValue)-1;

			/* Fill line */
			if( ChildLeft == ChildRight )
				SetPixel(ChildRight,y,FillValue);
			else
				DrawHorizontalLine(ChildLeft,ChildRight,y,FillValue);

			/* Push unvisited lines */
			if( ParentLeft-1<=ChildLeft && ChildRight<=ParentRight+1 ) 
			{
				PushLine(ChildLeft,ChildRight,y,dy);
			}
			else 
			{
				if( m_bXSortOn ) 
				{
					PushOpposite(ParentLeft,ParentRight,ChildLeft,ChildRight,y,dy);
				}
				else if( ChildLeft == ParentLeft ) 
				{
					PushLine(ParentRight+2,ChildRight,y,-dy);
				}
				else if( ChildRight == ParentRight ) 
				{
					PushLine(ChildLeft,ParentLeft-2,y,-dy);
				}
				else 
				{
					PushLine(ChildLeft,ChildRight,y,-dy);
				}
				PushLine(ChildLeft,ChildRight,y,dy);
			}
			/* Addvance ChildRight end on to border */
			++ChildRight;
		}
		else 
			ChildRight = ParentLeft;

		/* Fill betweens */
		while( ChildRight < ParentRight ) 
		{
			/* Skip to new ChildLeft end ( ChildRight>ParentRight on failure ) */
			ChildRight = SkipRight(ChildRight+1,y,ParentRight,ThisColorValue);
			/* If new ChildLeft end found */
			if( ChildRight<=ParentRight ) 
			{
				ChildLeft = ChildRight;

				/* Find ChildRight end ( this should not fail here ) */
				ChildRight = FindRight(ChildLeft+1,y,MaxX,ThisColorValue)-1;

				/* Fill line */
				if( ChildLeft == ChildRight )
					SetPixel(ChildRight,y,FillValue);
				else
					DrawHorizontalLine(ChildLeft,ChildRight,y,FillValue);

				/* Push unvisited lines */
				if( ChildRight <= ParentRight+1 ) 
				{
					PushLine(ChildLeft,ChildRight,y,dy);
				}
				else 
				{
					if( m_bXSortOn ) 
					{
						PushOpposite(ParentLeft,ParentRight,ChildLeft,ChildRight,y,dy);
					}
					else
						PushLine(ChildLeft,ChildRight,y,-dy);
					PushLine(ChildLeft,ChildRight,y,dy);
				}
				/* Addvance ChildRight end onto border */
				++ChildRight;
			}
		}
	}
	FreeList();
	DWORD dwSrc = m_DibData.GetDibSize() - m_DibData.GetHeaderSize();
	DWORD dwDst = DibSizeImage(pBitmap);

	if (dwSrc != dwDst)
	{
		m_DibData.Detach();
		return false;
	}

	memcpy(pBitmap, m_DibData.GetDibPtr(), dwSrc);
	*pRect = m_rcInvalid;
	m_DibData.Detach();

	return true;
}

DWORD CFloodFill::GetPixel(int x, int y)
{
	return m_DibData.GetPixelValue(x,y);
}

// Arguments: x, y coordinates and new color of pixel.
BOOL CFloodFill::SetPixel(int x, int y, DWORD dwValue)
{
	if( _BitMask[x&7] & m_FillMask[y&7] ) 
	{
		return m_DibData.SetPixelValue(x,y,dwValue);
	}

	return FALSE;
}

// Arguments: Coordinates of horizontal line and new color of line.
void CFloodFill::DrawHorizontalLine(int x1, int x2, int y, DWORD dwValue)
{
	// If doing solid fill
	if( !m_bXSortOn ) {
		m_DibData.DrawLineH(x1,x2,y,dwValue);
	}
	else {
		// Slow but neccessary for patterns and masks
		for( ; x1 <= x2; ++x1 )
			SetPixel(x1,y,dwValue);
	}
}

/*	ScanLeft()
 *	xmin > result -> failure
 */
int CFloodFill::ScanLeft(int x,int y,int xmin,DWORD dwValue)
{
	return m_DibData.ScanLeft(x,y,xmin,dwValue);
}

/*	SearchLeft()
 *	xmin > result -> failure
 */
int CFloodFill::SearchLeft(int x,int y,int xmin,DWORD dwValue)
{
	return m_DibData.SearchLeft(x,y,xmin,dwValue);
}

/*	ScanRight()
 *	xmax < result -> failure
 */
int CFloodFill::ScanRight(int x,int y,int xmax,DWORD dwValue)
{
	return m_DibData.ScanRight(x,y,xmax,dwValue);
}

/*	SearchRight()
 *	xmax < result -> failure
 */
int CFloodFill::SearchRight(int x,int y,int xmax,DWORD dwValue)
{
	return m_DibData.SearchRight(x,y,xmax,dwValue);
}

//----------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------

/* Initialize Line list */
int CFloodFill::MakeList(void)
{
	m_pFreeList = (HLINE_NODE*)calloc(1,sizeof(HLINE_NODE));
	return( m_pFreeList?0:1 );
}

/* Frees the list of free nodes */
void CFloodFill::FreeList(void)
{
	HLINE_NODE *pNext;
	while( m_pFreeList ) 
	{
		pNext = m_pFreeList->pNext;
	  	free(m_pFreeList);
	  	m_pFreeList = pNext;
	}

	while( m_pLineList ) 
	{
		pNext = m_pLineList->pNext;
	  	free(m_pLineList);
	  	m_pLineList = pNext;
	}
	
	while( m_pVisitList ) 
	{
		pNext = m_pVisitList->pNext;
	  	free(m_pVisitList);
	  	m_pVisitList = pNext;
	}

}

/* Push a node onto the line list */
void CFloodFill::PushLine(int x1,int x2,int y,int dy)
{
	HLINE_NODE *pNew = m_pFreeList;
	if( pNew )
		m_pFreeList = m_pFreeList->pNext;
	else
		pNew = (HLINE_NODE*)calloc(1,sizeof(HLINE_NODE));
	/* Add to start of list */
	if( pNew ) {
		pNew->x1 = x1;
		pNew->x2 = x2;
		pNew->y  = y;
		pNew->dy = dy;
		
		if( m_bXSortOn ) 
		{
			HLINE_NODE *pThis,*pPrev=(HLINE_NODE*)0;
			for( pThis=m_pLineList;pThis;pThis=pThis->pNext ) 
			{
				if( x1 <= pThis->x1 )
					break;
				pPrev = pThis;
			}
			if( pPrev ) 
			{
				pNew->pNext = pPrev->pNext;
				pNew->pPrev = pPrev;
				pPrev->pNext = pNew;
				if( pNew->pNext )
					pNew->pNext->pPrev = pNew;
			}
			else 
			{
				pNew->pNext = m_pLineList;
				pNew->pPrev = (HLINE_NODE*)0;
				if( pNew->pNext )
					pNew->pNext->pPrev = pNew;
				m_pLineList = pNew;
			}
		}
		else 
		{
			pNew->pNext = m_pLineList;
			m_pLineList = pNew;
		}
	}
	else m_bMemError = 1;
}

/* Pop a node off the line list */
void CFloodFill::PopLine(int *x1,int *x2,int *y,int *dy)
{
	if( m_pLineList ) 
	{
		HLINE_NODE *pThis,*pPrev;
		pPrev = m_pLineList;
		for( pThis=m_pLineList->pNext;pThis;pThis=pThis->pNext ) 
		{
			if( pThis->y == m_LastY )
				break;
			pPrev = pThis;
		}
		/* If pThis found - remove it from list */
		if( pThis ) 
		{
			pPrev->pNext = pThis->pNext;
			if( pPrev->pNext )
				pPrev->pNext->pPrev = pPrev;
			*x1 = pThis->x1;
			*x2 = pThis->x2;
			*y  = pThis->y;
			*dy = pThis->dy;
		}
		/* Remove from start of list */
		else 
		{
			*x1 = m_pLineList->x1;
			*x2 = m_pLineList->x2;
			*y  = m_pLineList->y;
			*dy = m_pLineList->dy;
			pThis = m_pLineList;
			m_pLineList = m_pLineList->pNext;
			if( m_pLineList )
				m_pLineList->pPrev = (HLINE_NODE*)0;
		}

		pThis->pNext = m_pFreeList;
		m_pFreeList = pThis;
		m_LastY = *y;
	}
}

/* Adds line to visited block list */
void CFloodFill::PushVisitedLine(int x1,int x2,int y)
{
	HLINE_NODE *pNew = m_pFreeList;
	if( pNew )
		m_pFreeList = m_pFreeList->pNext;
	else
		pNew = (HLINE_NODE*)calloc(1,sizeof(HLINE_NODE));
	/* Add to start of list */
	if( pNew ) {
		pNew->x1 = x1;
		pNew->x2 = x2;
		pNew->y  = y;
		pNew->pNext = m_pVisitList;
		m_pVisitList = pNew;
	}
	else m_bMemError = 1;
}

/* Checks if line has already been visited */
BOOL CFloodFill::IsRevisit(int x1,int x2,int y)
{
	HLINE_NODE* pNext = m_pVisitList;
	while( pNext ) {
		if( pNext->y == y && pNext->x1 <= x1 && x2 <= pNext->x2 ) {
			break;
		}
		pNext = pNext->pNext;
	}
	return( pNext != NULL );
}

/* Find next line segment on parent line.
 * Note: This function is designed to be
 *		called until NULL is returned.
 */
CFloodFill::HLINE_NODE* CFloodFill::FindNextLine(int x1,int x2,int y)
{
	static HLINE_NODE *pFindNext=(HLINE_NODE*)0;
	HLINE_NODE *pThis;
	if( !pFindNext )
		pFindNext = m_pLineList;
	for( pThis=pFindNext;pThis;pThis=pThis->pNext ) {
		if( (pThis->y+pThis->dy) == y ) {
			if( x1 < pThis->x1 && pThis->x1 <= x2 ) {
				pFindNext = pThis->pNext;
				return pThis;
			}
		}
	}
	return( pFindNext=(HLINE_NODE*)0 );
}

/* Removes pThis from line list */
void CFloodFill::PopThis(HLINE_NODE *pThis)
{
	if( m_pLineList ) 
	{
		/* If pThis is not start of list */
		if( pThis->pPrev ) 
		{
			HLINE_NODE *pPrev = pThis->pPrev;
			pPrev->pNext = pThis->pNext;
			if( pPrev->pNext )
				pPrev->pNext->pPrev = pPrev;
		}
		/* Remove pThis from start of list */
		else 
		{
			m_pLineList = m_pLineList->pNext;
			if( m_pLineList )
				m_pLineList->pPrev = (HLINE_NODE*)0;
		}
		pThis->pNext = m_pFreeList;
		m_pFreeList = pThis;
	}
}

void CFloodFill::PushOpposite(int OldX1,int OldX2,int x1,int x2,int y,int dy)
{
	/* Find next line on parent line */
	HLINE_NODE *pFind = FindNextLine(x1,x2,y);
	if( !pFind ) 
	{
		/* push cliped left ends */
		if( x1 < --OldX1 )
			PushLine(x1,--OldX1,y,-dy);
		if( x2 > ++OldX2 )
			PushLine(++OldX2,x2,y,-dy);
	}
	else 
	{
		/* push cliped left */
		if( x1 < --OldX1 )
			PushLine(x1,--OldX1,y,-dy);
		/* set test value for right cliping */
		OldX1 = x2+1;
		do {
			/* push valid line only */
			if( ++OldX2 < pFind->x1-2 )
				PushLine(++OldX2,pFind->x1-2,y,-dy);
			OldX2 = pFind->x2;
			/* clip right end if needed */
			if( OldX2 > OldX1 ) {
				pFind->x1 = ++OldX1;
			}
			else /* pop previously visited line */
				PopThis(pFind);
			pFind = FindNextLine(x1,x2,y);
		} while( pFind );

		if( ++OldX2 < x2 )
			PushLine(OldX2,x2,y,-dy);
	}
	PushVisitedLine(x1,x2,y);
}
