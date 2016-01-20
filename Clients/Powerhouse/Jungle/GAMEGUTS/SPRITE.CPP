// instead of always doing a union of the last and current positions
// at display time, check for overlap and do individuals if not intersecting
#include <windows.h>
#include <mmsystem.h>
#include "proto.h"						
#include "sprite.h"
#include "script.h"

//************************************************************************
static LPANIMATOR TestAnimate(HWND hBackWnd, LPOFFSCREEN lpOffScreen)
//************************************************************************
{
POINT ptOrigin;
LPSPRITE lpSprite;
LPANIMATOR lpAnimator;
//int i;


lpAnimator = new CAnimator(hBackWnd, lpOffScreen);
if (!lpAnimator)
	return(NULL);

lpAnimator->StopAll();

ptOrigin.x = 0;
ptOrigin.y = 0;
lpSprite = lpAnimator->CreateSprite( &ptOrigin );
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
//	AddCmdActivateCell( x, y, TRUE/FALSE );
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
void CAnimator::AddSprite(LPSPRITE pSprite)
//***********************************************************************
{
	LPSPRITE pOldHeadSprite;

	// Add it into the list
	// link the new sprite into the MAIN list
	pOldHeadSprite = m_pHeadSprite;
	m_pHeadSprite = pSprite;
	if (pOldHeadSprite)
	{
		pOldHeadSprite->prev = m_pHeadSprite;
		pSprite->next = pOldHeadSprite;
	}
}

//***********************************************************************
LPSPRITE CAnimator::CreateSprite( LPPOINT lpOrigin, DWORD dwData)
//***********************************************************************
{
	LPSPRITE pSprite = new CSprite(this, lpOrigin, dwData);
	if (pSprite)
		AddSprite(pSprite);
	return(pSprite);
}

//***********************************************************************
void CAnimator::DeleteSprite(LPSPRITE pSprite)
//***********************************************************************
{
	if (pSprite->bVisible)
		pSprite->Show( NO );
	Unlink(pSprite);
	delete pSprite;
}

//***********************************************************************
void CAnimator::Unlink(LPSPRITE pSprite)
//***********************************************************************
{
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
	LPSPRITE pSprite, pHead;

	pHead = m_pHeadSprite;
	while (pHead)
	{
		pSprite = pHead->next;
		delete pHead;
		pHead = pSprite;
	}
	m_pHeadSprite = NULL;
}

//***********************************************************************
void CAnimator::StartAll()
//***********************************************************************
{
	LPSPRITE lpSprite;

	m_bFullStop = FALSE;
	lpSprite = m_pHeadSprite;
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
void CAnimator::DrawRect( LPRECT lpRect )
//***********************************************************************
{
	RECT rArea, rUpdate, rSprite;
	LPSPRITE lpSprite;
	PDIB pdibSrc, pdibDst;
	HDC hWinGDC;

	if (IsRectEmpty(lpRect))
		return;

	if (IsRectEmpty(&m_rClip))
		rArea = *lpRect;
	else
	if (!IntersectRect(&rArea, lpRect, &m_rClip))
		return;

	//Debug( "DrawRect %d %d %d %d\n",
	//	rArea.left, rArea.top, rArea.right, rArea.bottom);

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
	lpSprite = m_pHeadSprite;
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
		//Debug( "DrawRect = %d %d %d %d\n", rArea.left, rArea.top, rArea.right, rArea.bottom );
		InvalidateRect(m_pToon->GetWindow(), &rArea, FALSE);
		UpdateWindow(m_pToon->GetWindow());
		//ToonClearRects(m_pToon->GetToonHandle());
		//ToonMarkRect(m_pToon->GetToonHandle(), &rArea);
		//ToonPaintRects(m_pToon->GetToonHandle());
	#endif
	#endif
	}
}

//***********************************************************************
void CAnimator::TimeSlice(void) // loop over the entire linked list
//***********************************************************************
{
	LPSPRITE lpSprite;

	if ( m_bFullStop )
		return;

	lpSprite = m_pHeadSprite;
	while (lpSprite)
	{
		lpSprite->Action();

		if (lpSprite->bKill)
		{ // now check for dead ones; hide and remove them
			if (lpSprite->lpNotifyProc)
				(*(lpSprite->lpNotifyProc))(lpSprite, SN_DESTROY, lpSprite->GetNotifyData());
			DeleteSprite(lpSprite);
 			return;
		}
		lpSprite = lpSprite->next;
	}
}

//***********************************************************************
int CAnimator::GetNumSprites() // tell how many there are
//***********************************************************************
{
	LPSPRITE lpSprite;
	int NumSprites;

	NumSprites = 0;
	lpSprite = m_pHeadSprite;
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
{
	// Have each of the animation sprites play their sound.

	LPSPRITE pSprite;

	pSprite = m_pHeadSprite;
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
	LPSPRITE lpSprite;
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
	lpSprite = m_pHeadSprite;
	while (lpSprite->next)
		lpSprite = lpSprite->next;
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
	LPSPRITE lpSprite;

	lpSprite = m_pHeadSprite;
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
	LPCELL lpNewCell, lpCell;

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
	int iHeight, iWidth;
	int i, y, xOffset, yOffset;
	BOOL fRet = TRUE;
	PDIB pDib;
	LPTR lp;

	if (!pSpriteDib)
		return(FALSE);

	if (nCells == 1)
		return(AddCell(pSpriteDib,
						lpOffsets != NULL ? lpOffsets->x : 0,
						lpOffsets != NULL ? lpOffsets->y : 0) != NULL);

	// determine the height of each cell
	iHeight = pSpriteDib->GetHeight() / nCells;
	iWidth = pSpriteDib->GetWidth();
	for (i = 0; i < nCells; ++i)
	{
		// create dib the same size as original with no data ptr
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
		if (!(lp = Alloc(pDib->GetSizeImage())))
		{
			delete pDib;
			fRet = FALSE;
			break;
		}
		pDib->SetPtr(lp);

		// copy the data from the source dib to the destination dib
		y = i * abs(iHeight);
		pSpriteDib->DibBlt( pDib,
							0, 0, iWidth, abs(iHeight),
							0, y, iWidth, abs(iHeight), FALSE);

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
	LPCELL lpCell;

	lpCellCurrent = NULL;
	while ( lpCellHead )
		{
		lpCell = lpCellHead;
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
	LPACTION lpAction;

	lpActionCurrent = NULL;
	while ( lpActionHead )
		{
		lpAction = lpActionHead;
		lpActionHead = lpActionHead->next;
		FreeUp( lpAction );
		}

	bMoving = NO; // stop all motion!
}

//***********************************************************************
BOOL CSprite::Jump( int newX, int newY )
//***********************************************************************
{
	int xLoc, yLoc;

	xLoc = newX + xOrigin;
	yLoc = newY + yOrigin;

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
	if ( lpRect )
		{
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
}

//***********************************************************************
void CSprite::LastDrawn( LPRECT lpRect )
//***********************************************************************
{
	if ( lpRect )
		{
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
}

//***********************************************************************
BOOL CSprite::Moving(void)
//***********************************************************************
{
	// are we moving or cycling through bitmaps?
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
			dwPauseTicks = timeGetTime() + (DWORD)iTenths * 100;
		else
			dwPauseTicks = 0xEFFFFFFF; // Pause forever
	}
	else
		dwPauseTicks = 0;
	return(FALSE);
}

//***********************************************************************
BOOL CSprite::PauseAfterCell( int iCell, int iTenths )
//***********************************************************************
{
	lpPauseAfterCell = FindCellNum( iCell );
	iPauseAfterCellTenths = iTenths; 
	return(FALSE);
}

//***********************************************************************
BOOL CSprite::Repeat() // go back to BEGIN action and start over
//***********************************************************************
{
	LPACTION lpAction;

	lpAction = lpActionHead;
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
	int xDiff, yDiff, iDistance;

	bMoving = YES;
	xMoveDest = newX + xOrigin;
	yMoveDest = newY + yOrigin;
	xDiff = xMoveDest - xLocation;
	yDiff = yMoveDest - yLocation;
	iDistance = max( abs(xDiff), abs(yDiff) );
	fxMoveRate = fget( xDiff, iDistance );
	fyMoveRate = fget( yDiff, iDistance );
	fxMoveLocation = TOFIXED(xLocation) + fxMoveRate/2;
	fyMoveLocation = TOFIXED(yLocation) + fyMoveRate/2;

	// compute the time we should be done with this move
	// (done ticks) = (ticks now) + ((1000L ticks/sec * pixels) / (pixels/sec))
	dwMoveLastTicks = timeGetTime();
	dwMoveEndTicks = dwMoveLastTicks + ((1000L * iDistance) / iSpeed);
	return(FALSE);
}

//***********************************************************************
BOOL CSprite::SetCellsPerSec( int iFmsPerSec )
//***********************************************************************
{
	if (iFmsPerSec <= 0)
			dwCycleTime = 0;
	else	dwCycleTime = 1000L / iFmsPerSec;
	return(FALSE);
}

//***********************************************************************
LPCELL CSprite::FindCellNum( int iCell )
//***********************************************************************
{
	LPCELL pCell;

	pCell = lpCellHead;
	while ( (--iCell >= 0) && pCell )
		pCell = pCell->next;
	return pCell;
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
	LPCELL pCell = lpCellHead;

	// Make sure there is more than one sprite to be reordered.
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
	for (int i=0; i<nPoints; i++)
		SetCellOffset(i+1, lpOffsets[i].x, lpOffsets[i].y);
}

//***********************************************************************
void CSprite::SetCellOffset( int iCell, int x, int y )
//***********************************************************************
{
	LPCELL pCell;

	// Each cell in an sprite can be a different size and offset from 0,0
	if ( !(pCell = FindCellNum(iCell)) )
		return;

	// Set the cell offset for this cell.
	pCell->xOffset = x;
	pCell->yOffset = y;
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
	int x, y, instr;

	CParser Parser(lpNewScript, lpError, nErrorBytes);

	if ( lpNewScript != lpScript )
		{ // Keep a copy of the script so you can re-execute it later.
		if ( lpScript ) // Free the old script if there is one.
			FreeUp( lpScript );
		if ( lpScript = (LPSTR)Alloc( lstrlen(lpNewScript) + 1 ) )
			lstrcpy(lpScript, lpNewScript); // Copy in the new script
		}

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
			case I_ACTCELL:			AddCmdActivateCell( x, y, TRUE);	break;
			case I_DEACTCELL:		AddCmdActivateCell( x, y, FALSE );	break;
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
	LPACTION lpNewAct, lpAct;

	if ( !(lpNewAct = (LPACTION)Alloc( sizeof(ACTION) )) )
		return;

	lpNewAct->next		= NULL;
	lpNewAct->cAction	= newAction;
	lpNewAct->x			= X;
	lpNewAct->y			= Y;
	lpNewAct->iParam	= newParam;

	// look to add at the head first
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
void CSprite::AddCmdActivateCell( int nStartCell, int nEndCell, BOOL bActivate)
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
	int i;

	if (nPoints <= 0)
		return;

	for (i=0; i<nPoints; i++)
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
void CSprite::Action(void)
// handle sprite movement as well as act on and initiate pending commands
//***********************************************************************
{
	DWORD dwCurrentTicks;
	BOOL bCycled, bMoved, bMovedAndDrawn, bMoveDone;

	// Get the current time
	dwCurrentTicks = timeGetTime();

	bMovedAndDrawn = bMoveDone = FALSE;
	if ( bMoving )
		{ // finish doing the move before looking at new actions

		// cycle the bitmaps if need be
		bCycled = CycleCells( dwCurrentTicks );

		// handle the move toward the final destination
		bMoved = MoveIncrement( dwCurrentTicks );

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
			if ( dwPauseTicks > dwCurrentTicks )
				return; // we're still paused
			// pause complete
			dwPauseTicks = 0L;
			}

		// handle the pause after cell state
		if ( lpPauseAfterCell == lpCellCurrent )
			{ // if we've reached the cell to pause at...
			if (lpNotifyProc)
			 	(*lpNotifyProc)(this, SN_PAUSEAFTER, m_dwNotifyData);
			lpPauseAfterCell = NULL;
			Pause( iPauseAfterCellTenths ); // pause
			return;
			}

		// handle bitmap cycling
		bCycled = CycleCells( dwCurrentTicks );

		// perform the next action, if any
		if (!lpPauseAfterCell) // this pause is an action in progress
			bMovedAndDrawn = PerformNextAction();

		// hasn't moved toward the final destination
		bMoved = NO;
		}

	// Something about the sprite has changed - redraw it
	if ( bCycled || bMoved ) // OR IF CELL OR LOCATION CHANGED
		Draw();
	if (lpNotifyProc)
	{
		if ( bCycled || bMoved || bMovedAndDrawn )
			(*lpNotifyProc)(this, SN_MOVED, m_dwNotifyData);
		if ( bMoveDone )
			(*lpNotifyProc)(this, SN_MOVEDONE, m_dwNotifyData);
	}
}

//***********************************************************************
BOOL CSprite::CycleCells( DWORD dwCurrentTicks )
//***********************************************************************
{
	if ( !dwCycleTime ) // get out if we're not cycling
		return FALSE;
	if (!lpCellHead->next) // only one cell
		return FALSE;
	if ((dwCurrentTicks - dwCycleTimeLast) < dwCycleTime)
		return FALSE;
	dwCycleTimeLast = dwCurrentTicks;

	// Look for the next active cell in the ordered list.
	LPCELL lpStart = lpCellCurrent;
	do
		{
		if (lpCellCurrent->next)			// go to next bitmap
			lpCellCurrent = lpCellCurrent->next;
		else
			{
			if (!bCycleRepeat) // are we allowed to restart the cycle
				{
				lpCellCurrent = lpStart;
				break;
				}
			lpCellCurrent = lpCellHead;
			}
		} while (!lpCellCurrent->bActive && (lpCellCurrent != lpStart));

	return ( lpCellCurrent != lpStart);
}

//***********************************************************************
BOOL CSprite::MoveIncrement( DWORD dwCurrentTicks )
//***********************************************************************
{
	DWORD dwTicksLeft, dwTicksPerCall;
	int nTimesPerCall, iDistance;

	// compute the pixels left to travel
	iDistance = max( abs(xMoveDest - xLocation), abs(yMoveDest - yLocation) );
	if ( !iDistance )
		return( NO );
	
	if ( dwMoveEndTicks <= dwCurrentTicks ) // time is up
		nTimesPerCall = iDistance; // so go all the way
	else
		{
		dwTicksLeft = dwMoveEndTicks - dwCurrentTicks;
		dwTicksPerCall = dwCurrentTicks - dwMoveLastTicks;
		nTimesPerCall = (int)(((dwTicksPerCall * iDistance) + dwTicksLeft/2) / dwTicksLeft);
		}

//	Debug( "n=%d, d=%d, t=%ld, tlast=%ld, tleft=%ld, t/call=%ld, tdone=%ld\n",
//		nTimesPerCall, iDistance, dwCurrentTicks, dwMoveLastTicks, dwTicksLeft, dwTicksPerCall, dwMoveEndTicks );

	dwMoveLastTicks = dwCurrentTicks;

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
	BOOL bMoved = FALSE;

	if ( lpActionHead ) // if no actions at all
	{
		if ( lpActionCurrent )
			{
			bMoved = PerformCurrentAction();
			lpActionCurrent = lpActionCurrent->next;
			}
		else
			DeleteAllActions();
	}
	return(bMoved);
}

//***********************************************************************
BOOL CSprite::PerformCurrentAction(void)
//***********************************************************************
{
	BOOL bMoved = FALSE;

	if ( !lpActionCurrent )
		return(bMoved);

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
	// tedr - this test should be opposite - check with the CHIEF!!!
	// I changed this to !IntersectRect().
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
	BYTE cTrans, cPixel;
	LPCELL lpCell;
	POINT pt;
	RECT rWhere;

	pt = *lpMousePt;
	if (lpWhereRect)
		rWhere = *lpWhereRect;
	else
		Location(&rWhere);

	if ( PtInRect( &rWhere, pt ) )
	{
		if (fCheckTransparent)
		{
			// see if mouse is over a transparent area
			lpCell = GetCurrentCell();
			if (!lpCell || !lpCell->pSpriteDib)
				return(TRUE);
			cTrans = *(lpCell->pSpriteDib->GetXY(0, 0));
			cPixel = *(lpCell->pSpriteDib->GetXY(pt.x-rWhere.left, pt.y-rWhere.top));
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
