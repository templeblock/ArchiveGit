// instead of always doing a union of the last and current positions
// at display time, check for overlap and do individuals if not intersecting
#include <windows.h>
#include <mmsystem.h>
#include "proto.h"						
#include "sprite.h"
#include "script.h"

static DWORD m_dwCurrentTicks;

//************************************************************************
static LPANIMATOR TestAnimate(HWND hBackWnd, LPOFFSCREEN lpOffScreen)
//************************************************************************
{
	LPANIMATOR lpAnimator = new CAnimator(hBackWnd, lpOffScreen);
	if (!lpAnimator)
		return(NULL);

	lpAnimator->StopAll();

	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0;
	LPSPRITE lpSprite = lpAnimator->CreateSprite( &ptOrigin );
	lpSprite->AddCell( "wheel.bmp" );
	lpSprite->AddCell( "wheell22.bmp" );
	lpSprite->AddCell( "wheell45.bmp" );
	lpSprite->AddCell( "wheell67.bmp" );
	lpSprite->AddCell( "wheell90.bmp" );
	lpSprite->AddCell( "wheelr22.bmp" );
	lpSprite->AddCell( "wheelr45.bmp" );
	lpSprite->AddCell( "wheelr67.bmp" );
	lpSprite->AddCell( "wheelr90.bmp" );
	lpSprite->AddCmdSetSpeed( 500 );
	lpSprite->AddCmdBegin();
	lpSprite->AddCmdJump( 600,	30 );
	lpSprite->AddCmdIncSpeed();
	lpSprite->AddCmdMove( 600, 400 );
	lpSprite->AddCmdIncSpeed();
	lpSprite->AddCmdMove(	0, 400 );
	lpSprite->AddCmdIncSpeed();
	lpSprite->AddCmdMove(	0,	30 );
	lpSprite->AddCmdIncSpeed();
	lpSprite->AddCmdMove(	0, 400 );
	lpSprite->AddCmdIncSpeed();
	lpSprite->AddCmdMove( 600, 400 );
	lpSprite->AddCmdIncSpeed();
	lpSprite->AddCmdMove( 600,	30 );
	lpSprite->AddCmdIncSpeed();
	//int i;
	//for ( i=0; i<1000; i++ )
	//	{
	//	lpSprite->AddCmdIncSpeed();
	//	lpSprite->AddCmdMove( GetRandomNumber(600), GetRandomNumber(400) );
	//	}
	lpSprite->AddCmdRepeat();
	//for ( i = 0; i < 32767; i++ )
	//	CSprite::TimeSlice();
	//delete lpSprite;

	// List of possible commands to queue up
	//	AddCmdJump( x, y );
	//	AddCmdHide();
	//	AddCmdShow();
	//	AddCmdPause( x );
	//	AddCmdPauseAfterCell( x );
	//	AddCmdBegin();
	//	AddCmdRepeat();
	//	AddCmdKill();
	//	AddCmdFlipVertical();
	//	AddCmdFlipHorizontal();
	//	AddCmdSetSpeed( x );
	//	AddCmdIncSpeed();
	//	AddCmdDecSpeed();
	//	AddCmdSetCellsPerSec( x);
	//	AddCmdActivateCells( x, y, TRUE/FALSE );
	//	AddCmdReverseCellOrder();
	//	AddCmdSetCellOrder( x, y );
	//	AddCmdSetCurrentCell( x );
	//	AddCmdSetRepeatCycle( x );

	lpAnimator->StartAll();
	return(lpAnimator);
}

#define DEFAULT_SPEED		100 // in pixels/sec.
#define DEFAULT_CYCLETIME	100 // in ticks
#define I_EOF				-1


// Constructor
//***********************************************************************
CAnimator::CAnimator( HWND hBackWnd, LPOFFSCREEN lpOffScreen )
//***********************************************************************
{
	Init();
	m_hBackWnd = hBackWnd;			// the window for these sprites
	m_lpOffScreen = lpOffScreen;	// the offscreen for these sprites
}

// Constructor
//***********************************************************************
CAnimator::CAnimator( PTOON pToon )
//***********************************************************************
{
	Init();
	m_pToon = pToon;
}

//***********************************************************************
void CAnimator::Init()
//***********************************************************************
{
	m_pToon = NULL;
	m_hBackWnd = NULL;			// the window for these sprites
	m_lpOffScreen = NULL;	// the offscreen for these sprites
	m_pHeadSprite = NULL;			// first in a linked list of sprites
	m_bFullStop = FALSE;			// to stop all sprites from moving
	SetRectEmpty(&m_rClip);
}

// Destructor
//***********************************************************************
CAnimator::~CAnimator()
//***********************************************************************
{
	StopAll();
	DeleteAll();			// wack all the sprites
}

//***********************************************************************
void CAnimator::AddSprite(LPSPRITE pSprite, BOOL fAddToTop)
//***********************************************************************
{
	if ( !pSprite )
		return;

	// Add it into the list
	// link the new sprite into the MAIN list
	if (fAddToTop && m_pHeadSprite)
	{
		// add to end of list (top in z-order)
		LPSPRITE pLink = m_pHeadSprite;
		while (pLink->next)
			pLink = pLink->next;
		pLink->next = pSprite;
		pSprite->prev = pLink;
		pSprite->next = NULL; // make sure list is null terminated
	}
	else
	{
		// add to beginning of list (bottom in z-order)
		LPSPRITE pOldHeadSprite = m_pHeadSprite;
		m_pHeadSprite = pSprite;
		if (pOldHeadSprite)
		{
			pOldHeadSprite->prev = m_pHeadSprite;
			pSprite->next = pOldHeadSprite;
		}
	}
}

//***********************************************************************
LPSPRITE CAnimator::CreateSprite(LPPOINT lpOrigin, DWORD dwData, BOOL fAddToTop)
//***********************************************************************
{
	LPSPRITE pSprite = new CSprite(this, lpOrigin, dwData);
	if (pSprite)
		AddSprite(pSprite, fAddToTop);
	return(pSprite);
}

//***********************************************************************
void CAnimator::DeleteSprite(LPSPRITE pSprite)
//***********************************************************************
{
	if ( !pSprite )
		return;

	if (pSprite->bVisible)
		pSprite->Show( NO );
	Unlink(pSprite);
	delete pSprite;
}

//***********************************************************************
void CAnimator::OrderSprite(LPSPRITE pSprite, BOOL fTop)
//***********************************************************************
{
	if ( !pSprite )
		return;

	Unlink( pSprite );

	LPSPRITE pLink = m_pHeadSprite;

	if ( fTop )
	{ // Move the sprite to the TOP of the list
		if ( m_pHeadSprite )
		{
			pLink->prev = pSprite;
			pSprite->next = pLink;
		}
		m_pHeadSprite = pSprite;
	}
	else
	{ // Move the sprite to the BOTTOM of the list
		if ( m_pHeadSprite )
		{
			while (pLink->next)
				pLink = pLink->next;
			pLink->next = pSprite;
			pSprite->prev = pLink;
		}
		else
			m_pHeadSprite = pSprite;
	}
}

//***********************************************************************
void CAnimator::Unlink(LPSPRITE pSprite)
//***********************************************************************
{
	if (!pSprite)
		return;
	if (pSprite->prev)
		pSprite->prev->next = pSprite->next;
	if (pSprite->next)
		pSprite->next->prev = pSprite->prev;
	if (pSprite == m_pHeadSprite)
		m_pHeadSprite = pSprite->next;
	pSprite->prev = pSprite->next = NULL;
}

//***********************************************************************
void CAnimator::DeleteAll()
//***********************************************************************
{
	LPSPRITE pHead = m_pHeadSprite;
	while (pHead)
	{
		LPSPRITE pSprite = pHead->next;
		delete pHead;
		pHead = pSprite;
	}
	m_pHeadSprite = NULL;
}

//***********************************************************************
void CAnimator::StartAll()
//***********************************************************************
{
	m_bFullStop = FALSE;
	LPSPRITE lpSprite = m_pHeadSprite;
	while (lpSprite)
	{
		lpSprite->dwPauseTicks = 0; // reset any paused sprites
		lpSprite = lpSprite->next;
	}
}

//***********************************************************************
void CAnimator::PaintAll()
//***********************************************************************
{
	LPSPRITE lpSprite;

	lpSprite = m_pHeadSprite;
	while (lpSprite)
	{
		lpSprite->Draw();
		lpSprite = lpSprite->next;
	}
}

//***********************************************************************
void CAnimator::HideAll()
//***********************************************************************
{
	LPSPRITE lpSprite;

	lpSprite = m_pHeadSprite;
	while (lpSprite)
	{
		lpSprite->Show (FALSE);
		lpSprite = lpSprite->next;
	}
}

//***********************************************************************
void CAnimator::DrawRect( LPRECT lpRect )
//***********************************************************************
{
	if (!lpRect)
		return;

	if (IsRectEmpty(lpRect))
		return;

	RECT rArea;
	if (IsRectEmpty(&m_rClip))
		rArea = *lpRect;
	else
	if (!IntersectRect(&rArea, lpRect, &m_rClip))
		return;

	HDC hWinGDC;
	PDIB pdibSrc, pdibDst;
	if (m_lpOffScreen)
	{
		if ( !(pdibSrc = m_lpOffScreen->GetReadOnlyDIB()) )
			return;
		if ( !(pdibDst = m_lpOffScreen->GetWritableDIB()) )
			return;
		hWinGDC = m_lpOffScreen->GetDC();
	}
	#ifndef WIN32
	#ifdef USE_WINTOON
	else
	if (m_pToon)
	{
		if ( !(pdibSrc = m_pToon->GetStageDib()) )
			return;
		if ( !(pdibDst = m_pToon->GetWinGDib()) )
			return;
		hWinGDC = ToonDC(m_pToon->GetToonHandle());
	}
	#endif
	#endif
	else
		return;

	if ( pdibSrc->GetCompression() == BI_RLE8 ||
		 pdibDst->GetCompression() == BI_RLE8 )
//	if ( 1 )
		{ // compressed DIBs must use GDI copying (lose transparency ability)
		pdibSrc->DCBlt( hWinGDC,
						rArea.left, rArea.top,
						rArea.right - rArea.left, rArea.bottom - rArea.top,
						rArea.left, rArea.top,
						rArea.right - rArea.left, rArea.bottom - rArea.top );
		}
	else
		{
		pdibSrc->DibBlt( pdibDst,
						rArea.left, rArea.top,
						rArea.right - rArea.left, rArea.bottom - rArea.top,
						rArea.left, rArea.top,
						rArea.right - rArea.left, rArea.bottom - rArea.top,
						NO /*bTransparent*/ );
		}

	// Copy ALL intersecting sprites into the loading area for display.
	RECT rUpdate, rSprite;
	LPSPRITE lpSprite = m_pHeadSprite;
	while (lpSprite)
		{
		lpSprite->Location( &rSprite );
		if ( lpSprite->bVisible && IntersectRect(&rUpdate, &rSprite, &rArea) )
			{
			if ( lpSprite->lpCellCurrent->pSpriteDib->GetCompression() == BI_RLE8 ||
				 pdibDst->GetCompression() == BI_RLE8 )
//			if ( 1 )
				{ // compressed DIBs must use GDI copying (lose transparency ability)
				lpSprite->lpCellCurrent->pSpriteDib->DCBlt( hWinGDC,
							rUpdate.left, rUpdate.top,
							rUpdate.right - rUpdate.left,
							rUpdate.bottom - rUpdate.top,
							rUpdate.left - rSprite.left,
							rUpdate.top	 - rSprite.top,
							rUpdate.right - rUpdate.left,
							rUpdate.bottom - rUpdate.top );
				}
			else
				{
				lpSprite->lpCellCurrent->pSpriteDib->DibBlt( pdibDst,
							rUpdate.left, rUpdate.top,
							rUpdate.right - rUpdate.left,
							rUpdate.bottom - rUpdate.top,
							rUpdate.left - rSprite.left,
							rUpdate.top	 - rSprite.top,
							rUpdate.right - rUpdate.left,
							rUpdate.bottom - rUpdate.top,
							YES /*bTransparent*/ );
				}
			}

		// Select the next sprite to draw
		lpSprite = lpSprite->next;
		}

	// Copy the offscreen to the screen
	if (m_lpOffScreen)
	{
		if ( m_hBackWnd )
		{
			HDC hDC = GetDC(m_hBackWnd);
			m_lpOffScreen->DrawRect(hDC, &rArea);
			ReleaseDC(m_hBackWnd, hDC);
		}
	}
	else
	{
	#ifndef WIN32
	#ifdef USE_WINTOON
		m_pToon->UpdateArea(&rArea);
	#endif
	#endif
	}
}

//***********************************************************************
void CAnimator::TimeSlice(void) // loop over the entire linked list
//***********************************************************************
{
	if ( m_bFullStop )
		return;

	// Get the current time
	DWORD dwTicks = timeGetTime();
	// Compute the time spent outside this function
	DWORD dwAdjustTicks = ( m_dwCurrentTicks ? dwTicks - m_dwCurrentTicks : 0 );
	// Subtract out the normal timer resolution
	if ( dwAdjustTicks > 55 ) dwAdjustTicks -= 55; else dwAdjustTicks = 0;
	// Set the time to be used for this slice - 
	// All sprites use the same time to avoid drift
	m_dwCurrentTicks = dwTicks;

	// Do any time critical movement stuff first
	LPSPRITE lpSprite = m_pHeadSprite;
	while (lpSprite)
	{
		// Adjust for time spent outside this function
		lpSprite->dwMoveLastTicks += dwAdjustTicks;
		lpSprite->dwMoveEndTicks += dwAdjustTicks;
		lpSprite->dwPauseTicks += dwAdjustTicks;

		// handle the move toward the final destination
		if ( lpSprite->bMoving )
			lpSprite->bMoveIncrement = lpSprite->MoveIncrement();
		else // hasn't moved toward the final destination
			lpSprite->bMoveIncrement = FALSE;

		lpSprite = lpSprite->next;
	}

	// To track time spent outside this function
	m_dwCurrentTicks = timeGetTime();

	// Now perform the real action once time and coordinates are updated
	lpSprite = m_pHeadSprite;
	while (lpSprite)
	{
		lpSprite->Action(lpSprite->bMoveIncrement);

		LPSPRITE lpNextSprite = lpSprite->next;
		if (lpSprite->bKill)
		{ // now check for dead ones; hide and remove them
			if (lpSprite->lpNotifyProc)
				(*(lpSprite->lpNotifyProc))(lpSprite, SN_DESTROY, lpSprite->GetNotifyData());
			DeleteSprite(lpSprite);
		}
		lpSprite = lpNextSprite;
	}
}

//***********************************************************************
int CAnimator::GetNumSprites() // tell how many there are
//***********************************************************************
{
	int NumSprites = 0;
	LPSPRITE lpSprite = m_pHeadSprite;
	while (lpSprite)
	{
		if (!lpSprite->bKill)
			NumSprites++;
		lpSprite = lpSprite->next;
	}
	return NumSprites;
}

//***********************************************************************
void CAnimator::PlayAllSounds()
//***********************************************************************
{ // Have each of the animation sprites play their sound.
	LPSPRITE pSprite = m_pHeadSprite;
	while (pSprite)
	{
		pSprite->PlaySound();
		pSprite = pSprite->next;
	}
}

//***********************************************************************
LPSPRITE CAnimator::MouseInAnySprite(LPPOINT lpMousePt, BOOL fCheckTransparent)
//***********************************************************************
{
	POINT pt;
	if (lpMousePt)
	{
		pt = *lpMousePt;
	}
	else
	{
		GetCursorPos( &pt );
		ScreenToClient( m_hBackWnd, &pt );
	}

	// find the last sprite in the linked list
	LPSPRITE lpSprite = m_pHeadSprite;
	if (lpSprite)
	{
		while (lpSprite->next)
			lpSprite = lpSprite->next;
	}
	while ( lpSprite )
	{
		if (lpSprite->MouseInSprite(&pt, fCheckTransparent))
			return(lpSprite);
		lpSprite = lpSprite->prev;
	}

	return NULL;
}

//***********************************************************************
void CAnimator::SetWindow( HWND hBackWnd, LPOFFSCREEN lpOffScreen )
//***********************************************************************
{
	m_hBackWnd = hBackWnd;
	m_lpOffScreen = lpOffScreen;
}

//***********************************************************************
BOOL CAnimator::GetStopped()
//***********************************************************************
{
	return( m_bFullStop );
}

//***********************************************************************
void CAnimator::StopAll()
//***********************************************************************
{
	m_bFullStop = TRUE;
}

//***********************************************************************
void CAnimator::AdjustTime(DWORD dwTimeDelta)
//***********************************************************************
{
	LPSPRITE lpSprite = m_pHeadSprite;
	while (lpSprite)
	{
		if (lpSprite->bMoving)
		{
			lpSprite->dwMoveLastTicks += dwTimeDelta;
			lpSprite->dwMoveEndTicks += dwTimeDelta;
		}
		lpSprite = lpSprite->next;
	}
}

//***********************************************************************
void CAnimator::SetClipRect(LPRECT lpClipRect)
//***********************************************************************
{
	if (lpClipRect)
		m_rClip = *lpClipRect;
	else
		SetRectEmpty(&m_rClip);
}

//************************************************************************
BOOL CAnimator::CheckCollision(LPSPRITE lpSprite1, LPSPRITE lpSprite2, LPRECT lpRect1, LPRECT lpRect2)
//************************************************************************
{
	if ( !lpSprite1 || !lpSprite2 )
		return(FALSE);

	RECT rSprite1, rSprite2, rSect;
	lpSprite1->Location(&rSprite1);
	lpSprite2->Location(&rSprite2);
	if (!IntersectRect(&rSect, &rSprite1, &rSprite2))
		return(FALSE);

	if (lpRect1)
		if (!IntersectRect(&rSect, &rSect, lpRect1))
			return(FALSE);

	if (lpRect2)
		if (!IntersectRect(&rSect, &rSect, lpRect2))
			return(FALSE);

	LPCELL lpCell1 = lpSprite1->GetCurrentCell();
	LPCELL lpCell2 = lpSprite2->GetCurrentCell();
	if (!lpCell1 || !lpCell2)
		return(FALSE);

	PDIB pDib1 = lpCell1->pSpriteDib;
	PDIB pDib2 = lpCell2->pSpriteDib;
	if (!pDib1 || !pDib2)
		return(FALSE);

	BYTE bTrans1 = *pDib1->GetXY(0, 0);
	BYTE bTrans2 = *pDib2->GetXY(0, 0);
	for (int y = rSect.top; y < rSect.bottom; ++y)
	{
		LPTR lp1 = pDib1->GetXY(rSect.left-rSprite1.left, y-rSprite1.top);
		LPTR lp2 = pDib2->GetXY(rSect.left-rSprite2.left, y-rSprite2.top);
		if (!lp1 || !lp2)
			continue;
		int iWidth = rSect.right - rSect.left;
		while (--iWidth >= 0)
		{
			if (*lp1++ != bTrans1 && *lp2++ != bTrans2)
				return(TRUE);
		}
	}
	return(FALSE);
}

// Constructor
//***********************************************************************
CSprite::CSprite( LPANIMATOR pAnimator, LPPOINT lpOrigin, DWORD dwData )
//***********************************************************************
{
	next = NULL;
	prev = NULL;

	m_pAnimator = pAnimator;

	// POSITIONING
	xLastDrawn = xLocation = xOrigin = ( lpOrigin ? lpOrigin->x : 0 );
	yLastDrawn = yLocation = yOrigin = ( lpOrigin ? lpOrigin->y : 0 );

	bVisible = FALSE;
	bKill = FALSE;

	// MOVEMENT - Not needed until we start moving
	bMoving = NO;
	bMoveIncrement = NO;
	dwMoveEndTicks = dwMoveLastTicks = 0;
	fxMoveRate = fxMoveLocation = xMoveDest = 0;
	fyMoveRate = fyMoveLocation = yMoveDest = 0;

	// TIMING
	iSpeed = DEFAULT_SPEED;
	dwPauseTicks = 0;
	lpPauseAfterCell = NULL;
	iPauseAfterCellTenths = 0;

	// CELLS
	lpCellHead = NULL;
	lpCellCurrent = NULL;
	lpCellLastDrawn = NULL;

	// CELL CYCLING
	dwCycleTimeLast = DEFAULT_CYCLETIME; // measured in milliseconds
	dwCycleTime = DEFAULT_CYCLETIME; // measured in milliseconds
	bCycleRepeat = YES;
	bCycleBack = FALSE;
	CycleDirection = CycleForward;

	// ACTIONS & SCRIPTS
	lpActionHead = NULL;
	lpActionCurrent = NULL;
	lpScript = NULL;

	// NOTIFICATION
	lpNotifyProc = NULL;
	m_dwNotifyData = 0;

	// SOUND
	lpSoundProc = NULL;
	szSoundName[0] = '\0';
	bSoundLoop = NO;
	bSoundChannel = 1;

	// USER DATA
	m_dwData = dwData;

}

// Destructor
//***********************************************************************
CSprite::~CSprite()
//***********************************************************************
{
	DeleteAllCells();		// DIBs and sprites
	DeleteAllActions();		// there may still be some actions left
	if ( lpScript )
	{
		FreeUp( lpScript );
		lpScript = NULL;
	}
}

//***********************************************************************
LPCELL CSprite::AddCell( PDIB pSpriteDib, int xOffset, int yOffset )
//***********************************************************************
{
	LPCELL lpNewCell;
	if ( !(lpNewCell = (LPCELL)Alloc( sizeof(CELL) )) )
		return NULL;

	// initialize the members
	lpNewCell->next			 = NULL;
	lpNewCell->pSpriteDib	 = NULL;
	lpNewCell->bActive		 = TRUE;
	lpNewCell->xOffset		 = xOffset;
	lpNewCell->yOffset		 = yOffset;

	if ( pSpriteDib )
	{
		lpNewCell->pSpriteDib = pSpriteDib;
		lpNewCell->iWidth = lpNewCell->pSpriteDib->GetWidth();
		lpNewCell->iHeight = abs(lpNewCell->pSpriteDib->GetHeight());
	}
	else
	{
		lpNewCell->pSpriteDib = NULL;
		lpNewCell->iWidth = 0;
		lpNewCell->iHeight = 0;
	}

	// Add it into the list
	// look to add at the head first
	LPCELL lpCell;
	if (!(lpCell = lpCellHead)) // if head is not pointing to anything
	{
		lpCellHead = lpNewCell;
		lpCellCurrent = lpNewCell;
	}
	else
	{ // otherwise, add to the end of the chain
		while ( lpCell->next )
		{
			lpCell = lpCell->next;
		}
		lpCell->next = lpNewCell;
	}

	return lpNewCell;
}

//***********************************************************************
LPCELL CSprite::AddCell( LPCTSTR lpFileName, HPALETTE hPal, int xOffset, int yOffset, BOOL fDecodeRLE )
//***********************************************************************
{
return( AddCell( CDib::LoadDibFromFile(lpFileName, hPal, FALSE, fDecodeRLE), xOffset, yOffset ) );
}

//***********************************************************************
BOOL CSprite::AddCells( PDIB pSpriteDib, int nCells, LPPOINT lpOffsets )
//***********************************************************************
{
	if (!pSpriteDib)
		return(FALSE);

	if (nCells == 1)
		return(AddCell(pSpriteDib,
						lpOffsets != NULL ? lpOffsets->x : 0,
						lpOffsets != NULL ? lpOffsets->y : 0) != NULL);

	// determine the height of each cell
	int iHeight = pSpriteDib->GetHeight() / nCells;
	int iWidth = pSpriteDib->GetWidth();
	BOOL fRet = TRUE;

	for (int i = 0; i < nCells; ++i)
	{
		// create dib the same size as original with no data ptr
		PDIB pDib;
		if ( !(pDib = new CDib(pSpriteDib, NULL, FALSE)))
		{
			fRet = FALSE;
			break;
		}

		// reset size of dib and for recalc of size info
		pDib->SetHeight(iHeight);
		pDib->SetSizeImage(0);
		pDib->FixHeader();

		// allocate memory for dib and set new ptr
		LPTR lp;
		if (!(lp = Alloc(pDib->GetSizeImage())))
		{
			delete pDib;
			fRet = FALSE;
			break;
		}
		pDib->SetPtr(lp);

		// copy the data from the source dib to the destination dib
		int y = i * abs(iHeight);
		pSpriteDib->DibBlt( pDib,
							0, 0, iWidth, abs(iHeight),
							0, y, iWidth, abs(iHeight), FALSE);

		int xOffset, yOffset;
		if (lpOffsets)
		{
			xOffset = lpOffsets[i].x;
			yOffset = lpOffsets[i].y;
		}
		else
			xOffset = yOffset = 0;

		if (AddCell(pDib, xOffset, yOffset) == NULL)
		{	
			delete pDib;
			fRet = FALSE;
			break;
		}
	}

	delete pSpriteDib;
	return(fRet);
}

//***********************************************************************
BOOL CSprite::AddCells( LPCTSTR lpFileName, int nCells, HPALETTE hPal, LPPOINT lpOffsets, BOOL fDecodeRLE )
//***********************************************************************
{
return( AddCells( CDib::LoadDibFromFile(lpFileName, hPal, FALSE, fDecodeRLE), nCells, lpOffsets ) );
}

//***********************************************************************
void CSprite::SetNotifyProc(NOTIFYPROC lpfnNotifyCallback, DWORD dwNotifyData)
//***********************************************************************
{
	lpNotifyProc = lpfnNotifyCallback;
	m_dwNotifyData = dwNotifyData;
}

//***********************************************************************
void CSprite::SetPlaySoundProc(PLAYSOUNDPROC lpfnSoundCallback)
//***********************************************************************
{
	lpSoundProc = lpfnSoundCallback;
}

//***********************************************************************
void CSprite::PlaySound()
//***********************************************************************
{
	if ( lpSoundProc )
		(*lpSoundProc)( this, szSoundName, bSoundLoop, bSoundChannel);
}

//***********************************************************************
void CSprite::SetPlaySound(LPSTR lpSound, BOOL bLoopSound, int iChannel)
//***********************************************************************
{
	if (!lpSound)
		return;

	lstrcpy(szSoundName, lpSound);
	bSoundLoop = bLoopSound;
	bSoundChannel = iChannel;
}

//***********************************************************************
BOOL CSprite::SetSpeed( int iPixPerSec )
//***********************************************************************
{ // speed is measured in pixels per second
	#define MAX_PIX_PER_SEC 1000

	iSpeed = iPixPerSec;
	if ( iSpeed > MAX_PIX_PER_SEC )
		iSpeed = MAX_PIX_PER_SEC;
	if ( iSpeed < 0 )
		iSpeed = 1;
	return(FALSE);
}

//***********************************************************************
void CSprite::DeleteAllCells(void)
//***********************************************************************
{
	lpCellCurrent = NULL;
	while ( lpCellHead )
	{
		LPCELL lpCell = lpCellHead;
		lpCellHead = lpCellHead->next;
		if ( lpCell->pSpriteDib )
			delete lpCell->pSpriteDib;
		FreeUp( lpCell );
	}
}

//***********************************************************************
void CSprite::DeleteAllActions() // actions to execute
//***********************************************************************
{
	lpActionCurrent = NULL;
	while ( lpActionHead )
	{
		LPACTION lpAction = lpActionHead;
		lpActionHead = lpActionHead->next;
		FreeUp( lpAction );
	}

	bMoving = NO; // stop all motion!
}

//***********************************************************************
BOOL CSprite::Jump( int newX, int newY )
//***********************************************************************
{
	bMoving = NO; // stop all motion!

	int xLoc = newX + xOrigin;
	int yLoc = newY + yOrigin;
	if (xLoc != xLocation || yLoc != yLocation)
	{
		xLocation = xLoc;
		yLocation = yLoc;
		if (bVisible)
		{
			Draw();
			return(TRUE);
		}
	}
	return(FALSE);
}

//***********************************************************************
BOOL CSprite::Show( BOOL bShow )
//***********************************************************************
{
	if (bShow != bVisible)
	{
		bVisible = bShow;
		Draw();
	}
	return(TRUE);
}

//***********************************************************************
void CSprite::Location( LPRECT lpRect )
//***********************************************************************
{
	if ( !lpRect )
		return;

	if ( lpCellCurrent )
	{
		SetRect( lpRect, 0/*x*/, 0/*y*/, 
			lpCellCurrent->iWidth, lpCellCurrent->iHeight );
		OffsetRect( lpRect, -lpCellCurrent->xOffset, -lpCellCurrent->yOffset );
		OffsetRect( lpRect, lpCellHead->xOffset, lpCellHead->yOffset );
	}
	else
		SetRect( lpRect, 0, 0, 0, 0 );
	OffsetRect( lpRect, xLocation, yLocation );
}

//***********************************************************************
void CSprite::LastDrawn( LPRECT lpRect )
//***********************************************************************
{
	if ( !lpRect )
		return;

	if ( lpCellLastDrawn )
	{
		SetRect( lpRect, 0/*x*/, 0/*y*/, 
			lpCellLastDrawn->iWidth, lpCellLastDrawn->iHeight );
		OffsetRect( lpRect, -lpCellLastDrawn->xOffset, -lpCellLastDrawn->yOffset );
		OffsetRect( lpRect, lpCellHead->xOffset, lpCellHead->yOffset );
	}
	else
		SetRect( lpRect, 0, 0, 0, 0 );
	OffsetRect( lpRect, xLastDrawn, yLastDrawn );
}

//***********************************************************************
BOOL CSprite::Moving(void)
//***********************************************************************
{ // are we moving or cycling through bitmaps?
	return( bMoving || (lpCellHead && lpCellHead->next && dwCycleTime) );
}

//***********************************************************************
BOOL CSprite::Pause( int iTenths )
//***********************************************************************
{ // compute the time to start up again
	if (iTenths)
	{
		if ( iTenths <= 0 )
			iTenths = 1;
		if ( iTenths < PAUSE_FOREVER )
			dwPauseTicks = m_dwCurrentTicks + (DWORD)iTenths * 100;
		else
			dwPauseTicks = 0xEFFFFFFF; // Pause forever
	}
	else
		dwPauseTicks = 0;
	return(FALSE);
}

//***********************************************************************
CSprite::PauseAfterCell( int iCell, int iTenths )
//***********************************************************************
{
	lpPauseAfterCell = FindCellNum( iCell );
	iPauseAfterCellTenths = iTenths; 
	return(FALSE);
}
              
//***********************************************************************
void CSprite::ClearPauseAfterCell()
//***********************************************************************
{        
	lpPauseAfterCell = NULL;      
	iPauseAfterCellTenths = 0;
	dwPauseTicks = 0;
}
              
//***********************************************************************
BOOL CSprite::Repeat() // go back to BEGIN action and start over
//***********************************************************************
{
	LPACTION lpAction = lpActionHead;
	while (lpAction)
	{
		if (lpAction->cAction == I_BEGIN) // look for the begin action
		{
			// set BEGIN to be our current action that way
			// the next action performed will be the one just after
			lpActionCurrent = lpAction;
			//if (lpAction->next)
			//		lpActionCurrent = lpAction->next; // execute one just after BEGIN
			//else	lpActionCurrent = lpAction;
			break;
		}
		lpAction = lpAction->next;
	}

	return(FALSE);
}

//***********************************************************************
BOOL CSprite::Move( int newX, int newY )
//***********************************************************************
{
	bMoving = YES;
	xMoveDest = newX + xOrigin;
	yMoveDest = newY + yOrigin;
	int xDiff = xMoveDest - xLocation;
	int yDiff = yMoveDest - yLocation;
	int iDistance = max( abs(xDiff), abs(yDiff) );
	fxMoveRate = fget( xDiff, iDistance );
	fyMoveRate = fget( yDiff, iDistance );
	fxMoveLocation = TOFIXED(xLocation) + fxMoveRate/2;
	fyMoveLocation = TOFIXED(yLocation) + fyMoveRate/2;

	// compute the time we should be done with this move
	// (done ticks) = (ticks now) + ((1000L ticks/sec * pixels) / (pixels/sec))
	dwMoveLastTicks = m_dwCurrentTicks;
	dwMoveEndTicks = dwMoveLastTicks + ((1000L * iDistance) / iSpeed);
	return(FALSE);
}

//***********************************************************************
BOOL CSprite::SetCellsPerSec( int iFmsPerSec )
//***********************************************************************
{
	if (iFmsPerSec <= 0)
		dwCycleTime = 0;
	else
		dwCycleTime = 1000L / iFmsPerSec;

	return(FALSE);
}

//***********************************************************************
LPCELL CSprite::FindCellNum( int iCell )
//***********************************************************************
{
	LPCELL pCell = lpCellHead;
	while ( (--iCell >= 0) && pCell )
		pCell = pCell->next;
	return pCell;
}

//***********************************************************************
int CSprite::GetCellNum( LPCELL pTheCell )
//***********************************************************************
{
	if ( !pTheCell )
		return(-1);

	int iCellNum = 0;

	LPCELL pCell = lpCellHead;
	while (pCell)
	{
		if (pCell == pTheCell)
			return(iCellNum);
		++iCellNum;
		pCell = pCell->next;
	}
	return(-1);
}

//***********************************************************************
BOOL CSprite::ActivateCell( int iCell, BOOL bActivate )
//***********************************************************************
{
	LPCELL pCell;
	if ( pCell = FindCellNum(iCell) )
		pCell->bActive = bActivate;
	return(FALSE);
}

//***********************************************************************
void CSprite::ActivateCells( int nStartCell, int nEndCell, BOOL bActivate)
//***********************************************************************
{
	nEndCell = max(nStartCell, nEndCell);
	for (int i=nStartCell; i<nEndCell+1; i++)
		ActivateCell( i, bActivate );
	SetCurrentCell( nStartCell );
}

//***********************************************************************
BOOL CSprite::SetCurrentCell( int iCell )
//***********************************************************************
{
	LPCELL pCell;
	if ( pCell = FindCellNum(iCell) )
		lpCellCurrent = pCell;
	return(FALSE);
}

//***********************************************************************
BOOL CSprite::ReverseCellOrder( void )
//***********************************************************************
{
	// Make sure there is more than one sprite to be reordered.
	LPCELL pCell = lpCellHead;
	if ( !pCell || !pCell->next )
		return(FALSE);

	// Reverse the ordered list.
	LPCELL prev = NULL;
	LPCELL next;
	while ( pCell )
	{
		lpCellHead = pCell;
		next = pCell->next;
		pCell->next = prev;
		prev = pCell;
		pCell = next;
	}

	return(FALSE);
}

//***********************************************************************
BOOL CSprite::SetCellOrder( int iCell, int iOrder )
//***********************************************************************
{
	// Try to locate the specified cell in the ordered list.
	LPCELL pCell = FindCellNum( iCell );

	if ( !pCell )
		return(FALSE);

	// Unlink this cell from the list.	It will be reinserted later.
	if ( pCell == lpCellHead )
		lpCellHead = pCell->next;
	else
	{
		LPCELL prev = FindCellNum( iCell-1 );
		if (prev)
			prev->next = pCell->next;
	}
	pCell->next = NULL;

	// Try to locate this order.
	// Reinsert it into another place - changing the order.
	int i = 1;
	LPCELL pInsCell = lpCellHead;
	LPCELL pInsPrev	 = NULL;
	while (pInsCell)
	{
		if (i == iOrder)
		{
			if (pInsPrev)
				pInsPrev->next = pCell;
			pCell->next = pInsCell;
			if (pInsCell == lpCellHead)
				lpCellHead = pCell;
			return(FALSE);
		}
		i++;
		pInsPrev  = pInsCell;
		pInsCell = pInsCell->next;
	}

	// The Order number could not be found.	 Reinsert the cell at the end of the list.
	if (pInsPrev)
		pInsPrev->next = pCell;
	return(FALSE);
}

//***********************************************************************
void CSprite::SetAllCellOffsets( LPPOINTS lpOffsets, int nPoints )
//***********************************************************************
{
	if ( !lpOffsets )
		return;

	for (int i=0; i<nPoints; i++)
		SetCellOffset(i+1, lpOffsets[i].x, lpOffsets[i].y);
}

//***********************************************************************
void CSprite::SetCellOffset( int iCell, int x, int y )
//***********************************************************************
{
	// Each cell in an sprite can be a different size and offset from 0,0
	LPCELL pCell;
	if ( !(pCell = FindCellNum(iCell)) )
		return;

	// Set the cell offset for this cell.
	pCell->xOffset = x;
	pCell->yOffset = y;
}

//***********************************************************************
void CSprite::SetCycleRepeat(BOOL bRepeat)
//***********************************************************************
{
	bCycleRepeat = bRepeat;
}

//***********************************************************************
void CSprite::SetCycleBack(BOOL bBack)
//***********************************************************************
{
	bCycleBack = bBack;
}

//***********************************************************************
void CSprite::ReExecuteScript(void)
//***********************************************************************
{
	// Re-execute the last script that was given to the sprite
	if (lpScript)
	{
		DeleteAllActions();
		ExecuteScript( lpScript, NULL, 0);
	}
}

//***********************************************************************
BOOL CSprite::ExecuteScript( LPSTR lpNewScript, LPSTR lpError, int nErrorBytes )
//***********************************************************************
{ // This function sets up the script for the sprite as well as executes it.
	if ( !lpNewScript )
		return( NO );

	CParser Parser(lpNewScript, lpError, nErrorBytes);

	if ( lpNewScript != lpScript )
	{ // Keep a copy of the script so you can re-execute it later.
		if ( lpScript ) // Free the old script if there is one.
			FreeUp( lpScript );
		if ( lpScript = (LPSTR)Alloc( lstrlen(lpNewScript) + 1 ) )
			lstrcpy(lpScript, lpNewScript); // Copy in the new script
	}

	int x, y, instr;
	while ( instr = Parser.GetNextToken() )
	{
		x = Parser.GetParam(0);
		y = Parser.GetParam(1);

		switch (instr) // token from the script file
		{
			case I_MOVE:			AddCmdMove( x, y );					break;
			case I_JUMP:			AddCmdJump( x, y );					break;
			case I_HIDE:			AddCmdHide();						break;
			case I_SHOW:			AddCmdShow();						break;
			case I_PAUSE:			AddCmdPause( x );					break;
			case I_PAUSEAFTERCELL:	AddCmdPauseAfterCell( x, y ); 		break;
			case I_BEGIN:			AddCmdBegin();						break;
			case I_REPEAT:			AddCmdRepeat();						break;
			case I_KILL:			AddCmdKill();						break;
			case I_FVERT:			AddCmdFlipVertical();				break;
			case I_FHORZ:			AddCmdFlipHorizontal();				break;
			case I_SETSPEED:		AddCmdSetSpeed( x );				break;
			case I_INCSPEED:		AddCmdIncSpeed();					break;
			case I_DECSPEED:		AddCmdDecSpeed();					break;
			case I_SETCELLSPERSEC:	AddCmdSetCellsPerSec( x);			break;
			case I_ACTCELL:			AddCmdActivateCells( x, y, TRUE);	break;
			case I_DEACTCELL:		AddCmdActivateCells( x, y, FALSE );	break;
			case I_REVERSE_CELLORDER:AddCmdReverseCellOrder();			break;
			case I_CELLORDER:		AddCmdSetCellOrder( x, y );			break;
			case I_SETCELL:			AddCmdSetCurrentCell( x );			break;
			case I_REPEATCYCLE:		AddCmdSetRepeatCycle( x );			break;
			case I_EOF:
				#ifdef _DEBUG
				Debug( lpError );
				#endif
				return TRUE;
		}
	}

	#ifdef _DEBUG
	Debug( lpError );
	#endif 

	return TRUE;
}

//***********************************************************************
void CSprite::InsertNewAction( BYTE newAction, int X, int Y, int newParam )
//***********************************************************************
{
	LPACTION lpNewAct;
	if ( !(lpNewAct = (LPACTION)Alloc( sizeof(ACTION) )) )
		return;

	lpNewAct->next		= NULL;
	lpNewAct->cAction	= newAction;
	lpNewAct->x			= X;
	lpNewAct->y			= Y;
	lpNewAct->iParam	= newParam;

	// look to add at the head first
	LPACTION lpAct;
	if (!(lpAct = lpActionHead)) // if head is not pointing to anything
	{
		lpActionHead = lpNewAct;
		lpActionCurrent = lpNewAct;
	}
	else
	{ // otherwise, add to the end of the chain
		while ( lpAct->next )
			lpAct = lpAct->next;
		lpAct->next = lpNewAct;
		if (!lpActionCurrent)
			lpActionCurrent = lpNewAct;
	}
}

//***********************************************************************
void CSprite::AddCmdMove(int X, int Y)
//***********************************************************************
{
	InsertNewAction(I_MOVE, X, Y, 0);
}

//***********************************************************************
void CSprite::AddCmdBegin(void)
//***********************************************************************
{
	InsertNewAction(I_BEGIN, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdRepeat(void)
//***********************************************************************
{
	InsertNewAction(I_REPEAT, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdActivateCells( int nStartCell, int nEndCell, BOOL bActivate)
//***********************************************************************
{
	nEndCell = max(nStartCell, nEndCell);
	for (int i=nStartCell; i<nEndCell+1; i++)
		InsertNewAction(I_ACTCELL, i, bActivate, 0);
}

//***********************************************************************
void CSprite::AddCmdSetCellsPerSec(int iFmsPerSec)
//***********************************************************************
{
	InsertNewAction(I_SETCELLSPERSEC, 0, 0, iFmsPerSec);
}

//***********************************************************************
void CSprite::AddCmdReverseCellOrder(void)
//***********************************************************************
{
	InsertNewAction(I_REVERSE_CELLORDER, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdSetCellOrder(int cell, int order)
//***********************************************************************
{
	InsertNewAction(I_CELLORDER, cell, order, 0);
}

//***********************************************************************
void CSprite::AddCmdSetRepeatCycle( BOOL bRepeat )
//***********************************************************************
{
	// Will the sprite repeat cell cycling at the beginning or not.
	// If this is FALSE, then cell cycling stops at the last active cell.
	InsertNewAction(I_REPEATCYCLE, bRepeat, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdSetSpeed(int newSpeed)
//***********************************************************************
{
	InsertNewAction(I_SETSPEED, 0, 0, newSpeed);
}

//***********************************************************************
void CSprite::AddCmdIncSpeed(void)
//***********************************************************************
{
	InsertNewAction(I_INCSPEED, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdDecSpeed(void)
//***********************************************************************
{
	InsertNewAction(I_DECSPEED, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdSetCurrentCell( int nCell )
//***********************************************************************
{
	InsertNewAction(I_SETCELL, 0, 0, nCell);
}

//***********************************************************************
void CSprite::AddCmdPause( int tenths )
//***********************************************************************
{
	InsertNewAction(I_PAUSE, 0, 0, tenths);
}

//***********************************************************************
void CSprite::AddCmdPauseAfterCell( int iCell, int iTenths )
//***********************************************************************
{
	// Wait for the desired cell to appear.	 Works just like a pause.
	InsertNewAction(I_PAUSEAFTERCELL, iCell, iTenths, 0);
}

//***********************************************************************
void CSprite::AddCmdKill(void)
//***********************************************************************
{
	InsertNewAction(I_KILL, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdMovePoints( int nPoints, LPPOINT lpPoints )
//***********************************************************************
{
	if (nPoints <= 0)
		return;

	if (!lpPoints)
		return;

	for (int i=0; i<nPoints; i++)
		InsertNewAction(I_MOVE, lpPoints[i].x, lpPoints[i].y, 0);
}

//***********************************************************************
void CSprite::AddCmdJump( int X, int Y )
//***********************************************************************
{
	InsertNewAction(I_JUMP, X, Y, 0);
}

//***********************************************************************
void CSprite::AddCmdHide(void)
//***********************************************************************
{
	InsertNewAction(I_HIDE, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdShow(void)
//***********************************************************************
{
	InsertNewAction(I_SHOW, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdFlipVertical(void)
//***********************************************************************
{
	InsertNewAction(I_FVERT, 0, 0, 0);
}

//***********************************************************************
void CSprite::AddCmdFlipHorizontal(void)
//***********************************************************************
{
	InsertNewAction(I_FHORZ, 0, 0, 0);
}

//***********************************************************************
void CSprite::Action( BOOL bMoved )
// handle sprite movement as well as act on and initiate pending commands
//***********************************************************************
{
	BOOL bMovedAndDrawn = FALSE;
	BOOL bMoveDone = FALSE;
	BOOL bCycled;
	if ( bMoving )
	{ // finish doing the move before looking at new actions

		// cycle the bitmaps if need be
		bCycled = CycleAllCells();

		if ( ReachedDestination() )
		{
			bMoveDone = TRUE;
			bMoving = NO;
		}
	}
	else
	{ // do non-moving stuff
		// handle the paused state
		if ( dwPauseTicks )
		{
			if ( dwPauseTicks > m_dwCurrentTicks )
				return; // we're still paused
			dwPauseTicks = 0L; // pause end
			if (lpNotifyProc)
			 	(*lpNotifyProc)(this, SN_PAUSEDONE, m_dwNotifyData);
		}

		// handle the pause after cell state
		if ( lpPauseAfterCell == lpCellCurrent )
		{ // if we've reached the cell to pause at...
		
			// Be sure and set this before the notify in case someone
			// inside the notify sets up another one	
			lpPauseAfterCell = NULL;	 
			if (lpNotifyProc)
			 	(*lpNotifyProc)(this, SN_PAUSEAFTER, m_dwNotifyData);
			Pause( iPauseAfterCellTenths ); // pause begin
			return;
		}

		// handle bitmap cycling
		bCycled = CycleAllCells();

		// perform the next action, if any
		if (!lpPauseAfterCell) // this pause is an action in progress
			bMovedAndDrawn = PerformNextAction();
	}

	// Something about the sprite has changed - redraw it
	if ( bCycled || bMoved ) // OR IF CELL OR LOCATION CHANGED
		Draw();
	if (lpNotifyProc)
	{
		if ( bCycled || bMoved || bMovedAndDrawn )
			(*lpNotifyProc)(this, SN_MOVED, m_dwNotifyData);
		if ( bMoveDone )
		{                
			bMoving = NO;
			(*lpNotifyProc)(this, SN_MOVEDONE, m_dwNotifyData);
		}
	}
}

//***********************************************************************
BOOL CSprite::CycleAllCells(void)
//***********************************************************************
{
	if ( !dwCycleTime ) // get out if we're not cycling
		return FALSE;
	if ( !bVisible ) // get out if we're not visible 
		return FALSE;
	if (!lpCellHead->next) // only one cell
		return FALSE;
	if ((m_dwCurrentTicks - dwCycleTimeLast) < dwCycleTime)
		return FALSE;  
	dwCycleTimeLast = m_dwCurrentTicks;

	return(CycleCells());
}

//***********************************************************************
BOOL CSprite::CycleCells(void)
//***********************************************************************
{
	static int iLastCell = -1, iCell;
	static BOOL fDummy;

	// Look for the next active cell in the ordered list.
	LPCELL lpStart = lpCellCurrent;
	do
	{
		LPCELL prev = GetPreviousCell(lpCellCurrent);
		if (!prev)
			CycleDirection = CycleForward;

		// cycling in forward direction?
		if (CycleDirection == CycleForward)
		{
			if (lpCellCurrent->next)			// go to next bitmap
				lpCellCurrent = lpCellCurrent->next;
			else
			// hit end of list, can we cycle backwards?
			if (bCycleBack && prev)
			{
				lpCellCurrent = prev;
				CycleDirection = CycleBackward;
			}
			else
			{
				if (!bCycleRepeat) // are we allowed to restart the cycle
				{
					lpCellCurrent = lpStart;
					break;
				}
				lpCellCurrent = lpCellHead;
			}
		}
		else
		{
			lpCellCurrent = prev;
		}
	} while (!lpCellCurrent->bActive && (lpCellCurrent != lpStart));
                        
               
	return ( lpCellCurrent != lpStart);
}

//***********************************************************************
LPCELL CSprite::GetPreviousCell(LPCELL lpCell)
//***********************************************************************
{
	if (!lpCell)
		return(NULL);

	LPCELL prev = lpCellHead;
	while (prev)
	{
		if (prev->next == lpCell)
			return(prev);
		prev = prev->next;
	}
	return(NULL);
}

//***********************************************************************
BOOL CSprite::MoveIncrement(void)
//***********************************************************************
{
	// compute the pixels left to travel
	int iDistance = max( abs(xMoveDest - xLocation), abs(yMoveDest - yLocation) );
	if ( !iDistance )
		return( NO );
	
	int nTimesPerCall;
	
	if ( dwMoveEndTicks <= m_dwCurrentTicks ) // time is up
	{
		nTimesPerCall = iDistance; // so go all the way
	}
	else
	{
		// Check for rare case of dwMoveLastTicks getting ahead of m_dwCurrentTicks
		if (dwMoveLastTicks > m_dwCurrentTicks)  
			dwMoveLastTicks = m_dwCurrentTicks;
			
		DWORD dwTicksLeft = dwMoveEndTicks - m_dwCurrentTicks;
		DWORD dwTicksPerCall = m_dwCurrentTicks - dwMoveLastTicks;
		nTimesPerCall = (int)(((dwTicksPerCall * iDistance) + dwTicksLeft/2) / dwTicksLeft);
	}

	dwMoveLastTicks = m_dwCurrentTicks;

	if ( nTimesPerCall <= 0 )
		return( NO );

	while( --nTimesPerCall >= 0 )
	{
		xLocation = WHOLE(fxMoveLocation);
		yLocation = WHOLE(fyMoveLocation);
		fxMoveLocation += fxMoveRate;
		fyMoveLocation += fyMoveRate;
		// are we close enough to be considered done?
		if ( abs(xLocation - xMoveDest) <= 2 && abs(yLocation - yMoveDest) <= 2 )
		{
			xLocation = xMoveDest;
			yLocation = yMoveDest;
			break;
		}
	}

	return( YES );
}

//***********************************************************************
BOOL CSprite::ReachedDestination()
//***********************************************************************
{
// due to change in MoveIncrement() we can now check for exact destination
//	return( abs(xLocation - xMoveDest) <= 2 && abs(yLocation - yMoveDest) <= 2 );
	return( (xLocation == xMoveDest) && (yLocation == yMoveDest) );
}

//***********************************************************************
BOOL CSprite::PerformNextAction(void)
//***********************************************************************
{
	if ( !lpActionHead )
		return(FALSE);

	BOOL bMoved = FALSE;
	if ( lpActionCurrent )
	{
		bMoved = PerformCurrentAction();
		lpActionCurrent = lpActionCurrent->next;
	}
	else
		DeleteAllActions();

	return(bMoved);
}

//***********************************************************************
BOOL CSprite::PerformCurrentAction(void)
//***********************************************************************
{
	if ( !lpActionHead )
		return(FALSE);

	if ( !lpActionCurrent )
		return(FALSE);

	BOOL bMoved = FALSE;
	switch ( lpActionCurrent->cAction )
	{
		case I_MOVE:
			bMoved = Move( lpActionCurrent->x, lpActionCurrent->y );
			break;

		case I_JUMP:
			bMoved = Jump( lpActionCurrent->x, lpActionCurrent->y );
			break;

		case I_HIDE:
			bMoved = Show( NO );
			break;

		case I_SHOW:
			bMoved = Show( YES );
			break;

		case I_BEGIN:
			break;

		case I_REPEAT: // go back to BEGIN action
			bMoved = Repeat();
			break;

		case I_PAUSE:
			bMoved = Pause( lpActionCurrent->iParam );
			break;

		case I_PAUSEAFTERCELL:
			bMoved = PauseAfterCell( lpActionCurrent->x, lpActionCurrent->y );
			break;

		case I_KILL:
			Kill();
			break;

		case I_SETSPEED:
			bMoved = SetSpeed( lpActionCurrent->iParam );
			break;

		case I_INCSPEED:
			bMoved = SetSpeed( iSpeed + 20 );
			break;

		case I_DECSPEED:
			bMoved = SetSpeed( iSpeed - 20 );
			break;

		case I_ACTCELL:
			bMoved = ActivateCell( lpActionCurrent->x, lpActionCurrent->y );
			break;

		case I_SETCELL:
			bMoved = SetCurrentCell( lpActionCurrent->iParam );
			break;

		case I_SETCELLSPERSEC:
			bMoved = SetCellsPerSec( lpActionCurrent->iParam );
			break;

		case I_REVERSE_CELLORDER:
			bMoved = ReverseCellOrder();
			break;

		case I_CELLORDER:
			bMoved = SetCellOrder( lpActionCurrent->x, lpActionCurrent->y );
			break;

		case I_REPEATCYCLE:
			bCycleRepeat = lpActionCurrent->x;
			break;

		case I_FVERT:
		case I_FHORZ:
			break;
	}

	return(bMoved);
}

//***********************************************************************
void CSprite::Draw(void)
//***********************************************************************
{
	RECT rArea, rPosCurrent, rPosLast;

	// 1) get union rect of the old and new rect positions for this sprite
	// 2) use this rect to copy READ-ONLY offscreen to the WRITABLE offscreen
	// 3) copy all sprites within this rect transparently to the WRITABLE offscreen
	// 4) copy the WRITABLE offscreen to the screen

	LastDrawn( &rPosLast );
	Location( &rPosCurrent );

	if ( !IntersectRect( &rArea, &rPosCurrent, &rPosLast ) )
	{
		m_pAnimator->DrawRect( &rPosLast );
		m_pAnimator->DrawRect( &rPosCurrent );
	}
	else
	{
		UnionRect( &rArea, &rPosCurrent, &rPosLast );
		m_pAnimator->DrawRect( &rArea );
	}

	// updates the variables that track the last place we drew
	xLastDrawn = xLocation;
	yLastDrawn = yLocation;
	lpCellLastDrawn = lpCellCurrent;
}

//***********************************************************************
void CSprite::GetMaxSize(LPINT lpWidth, LPINT lpHeight)
//***********************************************************************
{
	if (!lpWidth || !lpHeight)
		return;

	*lpWidth = 0;
	*lpHeight = 0;

	LPCELL lpCell = lpCellHead;
	while (lpCell)
	{
		if (lpCell->iWidth > *lpWidth)
			*lpWidth = lpCell->iWidth;
		if (lpCell->iHeight > *lpHeight)
			*lpHeight = lpCell->iHeight;
		lpCell = lpCell->next;
	}
}

//***********************************************************************
BOOL CSprite::MouseInSprite(LPPOINT lpMousePt, BOOL fCheckTransparent, LPRECT lpWhereRect)
//***********************************************************************
{
	if (!lpMousePt)
		return(FALSE);
	if (!bVisible)
		return(FALSE);
	POINT pt = *lpMousePt;

	RECT rWhere;
	if (lpWhereRect)
		rWhere = *lpWhereRect;
	else
		Location(&rWhere);

	if ( PtInRect( &rWhere, pt ) )
	{
		if (fCheckTransparent)
		{
			// see if mouse is over a transparent area
			LPCELL lpCell = GetCurrentCell();
			if (!lpCell || !lpCell->pSpriteDib)
				return(TRUE);
			BYTE cTrans = *(lpCell->pSpriteDib->GetXY(0, 0));
			BYTE cPixel = *(lpCell->pSpriteDib->GetXY(pt.x-rWhere.left, pt.y-rWhere.top));
			if (cPixel != cTrans)
				return(TRUE);
		}
		else
			return(TRUE);
	}
	return(FALSE);
}

//***********************************************************************
void CSprite::Kill()
//***********************************************************************
{
	bKill = TRUE; // will be killed later
}
//***********************************************************************
BOOL CSprite::FirstCellDrawn()
//***********************************************************************
{     
	if (lpCellHead == lpCellLastDrawn)
		return TRUE;
	else
		return FALSE;
}

//***********************************************************************
BOOL CSprite::LastCellDrawn()
//***********************************************************************
{                   
	if (lpCellLastDrawn->next == NULL)
		return FALSE;
	else
		return TRUE;			
}
