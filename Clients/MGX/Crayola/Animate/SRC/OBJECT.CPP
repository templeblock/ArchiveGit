/* ®FD1¯®PL1¯®TP0¯®BT0¯®RM255¯
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#include "app.h"
#include "animate.h"
#include "bitmaps.h"
#include "instr.h"
#include "object.h"

#define DEFAULT_SPEED		20
#define DEFAULT_CYCLETIME	100


CAnimateObj::CAnimateObj(HDC hTypeDC, HBITMAP hObjectBitmap, HBITMAP hMaskBitmap,
	LPPOINT lpOffset)
{   
	BITMAP      bm;

	m_ChildHead     = NULL;
    m_Parent        = NULL;

	virginMap		= hTypeDC;
	next            = 0;
	prev            = 0;

	xInc            = 1;
	yInc            = 1;
	movesPerInc     = 0;
	movesTilNextInc = 0;
	incsSoFar       = 0;
	axisShort       = 1;
	xPos            = 0;
	yPos            = 0;
	xOffset			= 0;
	yOffset			= 0;
	if ( lpOffset )
	{ // adder to jump and move instructions
		xOffset		= lpOffset->x;
		yOffset		= lpOffset->y;
		xPos        = xOffset;
		yPos        = yOffset;
	}

	movesSoFar      = 0;
	firstTiccnt     = 0;
	ticsPerMove     = 500;
    m_lCycleTime    = DEFAULT_CYCLETIME;      // measured in milliseconds

	moving          = 1;
	visible         = TRUE;
	needsSpecial    = FALSE;
	inBeginCycle    = FALSE;
	m_bAutoRepeat   = FALSE;
	killMe          = FALSE;

    m_hScript       = NULL;

	headB           = 0;
	currentB        = 0;
	headI           = 0;
	currentI        = 0;

	wNmessage       = 0;                    // NO notification message
	continuous      = 0;                    // don't continually send messages
	Awhich          = 0;                    // send if collide with anyone
	lastContObject  = 0;
	NotifyProc      = NULL;
	m_lpfnSoundCallback = NULL;

	trackLimit      = 0;                   // mouse tracking the object
    ptParent.x      = 0;
    ptParent.y      = 0;
    m_bRepeatCycle  = TRUE;

// create brand new bitmaps

	currentB = new BitObject;
	headB = currentB;

	if ( hObjectBitmap )
	{
		currentB->hObjectBmap = CopyBitmap( hTypeDC, hObjectBitmap );
		GetObject( currentB->hObjectBmap, sizeof(BITMAP), (LPSTR) &bm );
		Owidth  = bm.bmWidth;
		Oheight = bm.bmHeight;
        currentB->m_Width  = bm.bmWidth;
        currentB->m_Height = bm.bmHeight;
	}
	else
	{
		currentB->hObjectBmap = 0;
		Owidth = Oheight = 0;
	}

	if ( !hMaskBitmap )
		hMaskBitmap = MakeBitmapMono( currentB->hObjectBmap );
	else
		hMaskBitmap = MakeBitmapMono( hMaskBitmap );

	if ( hMaskBitmap )
	{
		currentB->hMaskBmap = hMaskBitmap;
		GetObject( currentB->hMaskBmap, sizeof(BITMAP), (LPSTR) &bm);
		Mwidth  = bm.bmWidth;
		Mheight = bm.bmHeight;
	}
	else
	{
		currentB->hMaskBmap = 0;
		Mwidth = Mheight = 0;
	}

	// set the rects to the size of the bitmaps
	xPos -= 10000; yPos -= 10000; // move off screen first
	SetRect(&oldrect, xPos, yPos, xPos+Owidth, yPos+Oheight);
	newrect = oldrect;

	SetSpeed(DEFAULT_SPEED);
}



void CAnimateObj::AddChild(CAnimateObj *pChild)
{
    if (!pChild) return;

// Add to the linked list of child Animation objects.
// Child objects are moved when the parent is moved.
    CAnimateObj *pNextOne = m_ChildHead;
    if (m_ChildHead)
    {
	    while (pNextOne->next)
		    pNextOne = pNextOne->next;
	    pNextOne->next = pChild;   // link in at the end of the list
    }
    else
        m_ChildHead = pChild;    

    pChild->SetParent(this);

// Set the child's parent offset.  This allows the child to use
// coordinates reletive to the parent.
    POINT pt;
    pt.x = newrect.left;
    pt.y = newrect.top;
    pChild->SetParentOffset(pt);
    OffsetRect(&pChild->newrect, -(pChild->newrect.left), -(pChild->newrect.top));
    OffsetRect(&pChild->oldrect, -(pChild->oldrect.left), -(pChild->oldrect.top));
}


/***********************************************************************/
static HPALETTE CopySystemPalette()
/***********************************************************************/
{
#define NUMENTRIES 256

	LPLOGPALETTE lpPal;
	LPPALETTEENTRY lpEntry;
	PALETTEENTRY   TempEntries[256+1];
	HWND hWnd;
	HDC hDC;
	int i, n;
	
	lpPal = (LPLOGPALETTE)TempEntries;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = 256;
	lpEntry = lpPal->palPalEntry;

	if ( !(hWnd = GetDesktopWindow()) )
		return( NULL );

	if ( !(hDC = GetDC( hWnd )) )
		return( NULL );

	n = GetSystemPaletteEntries( hDC, 0, NUMENTRIES, lpEntry );
	ReleaseDC( hWnd, hDC );

	if ( n != NUMENTRIES )
		return( NULL );

	for ( i=0; i<NUMENTRIES; i++ )
		(lpEntry+i)->peFlags = PC_RESERVED;

	return( CreatePalette( lpPal ) );
}


HBITMAP CAnimateObj::MakeBitmapMono( HBITMAP hBMobject )
{
	BITMAP  bm;
	COLORREF cColor, cOldColor;
	HBITMAP hBMmask;
	HDC hDCmask, hDCobject;
	HPALETTE hPal, hOldPal;
	#define SRCNAND 0x00220326

	if ( !hBMobject )
		return( NULL );

	GetObject( hBMobject, sizeof(BITMAP), (LPSTR) &bm );

	if ( !(hBMmask = CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL )) )
		return( NULL );
	if ( !(hDCmask = CreateCompatibleDC( NULL )) )
		{
		DeleteObject( hBMmask );
		return( NULL );
		}
	if ( !(hDCobject = CreateCompatibleDC( NULL )) )
		{
		DeleteDC( hDCmask );
		DeleteObject( hBMmask );
		return( NULL );
		}
	SelectObject( hDCobject, hBMobject );
	SelectObject( hDCmask, hBMmask );
	if ( hPal = CopySystemPalette() )
		{
		hOldPal = SelectPalette( hDCobject, hPal, FALSE );
		RealizePalette( hDCobject );
		cColor = GetPixel( hDCobject, 0, 0 ) | 0x02000000; // make PALETTERGB()
		}
	else
		cColor = GetPixel( hDCobject, 0, 0 );
	cOldColor = SetBkColor( hDCobject, cColor );
	BitBlt( hDCmask, 0, 0, bm.bmWidth, bm.bmHeight, hDCobject, 0, 0, SRCCOPY );
	BitBlt( hDCobject, 0, 0, bm.bmWidth, bm.bmHeight, hDCmask, 0, 0, SRCNAND );
	SetBkColor( hDCobject, cOldColor );
	if ( hPal )
		{
		SelectPalette( hDCobject, hOldPal, FALSE );
		RealizePalette( hDCobject );
		DeleteObject( hPal );
		}
	DeleteDC( hDCobject );
	DeleteDC( hDCmask );
	return( hBMmask );
}

void CAnimateObj::ResetTiming()
{
	firstTiccnt = 0;                                        // a fresh start after speed change
	movesSoFar  = 0;
	incsSoFar   = 0;
	movesTilNextInc = 0;
}


void CAnimateObj::SetNotification(WORD newMsg, int isCont, int newWhich)
{
	wNmessage       = newMsg;                       // message to send back to window
	continuous      = isCont;                       // send for each collision?
	Awhich          = newWhich;                     // 0=All, !0=only one object to notify if hit
}



void CAnimateObj::SetNotifyProc(NOTIFYPROC lpfnNotify)
{
	NotifyProc = lpfnNotify;
}


void CAnimateObj::PlaySound()
{
	if (!m_lpfnSoundCallback)
		return;
		
	(*m_lpfnSoundCallback)(descriptor, m_sSound, m_bLoopSound, m_iChannel);
}


void CAnimateObj::SetPlaySoundProc(PLAYSOUNDPROC lpfnSoundCallback)
{
	m_lpfnSoundCallback = lpfnSoundCallback;
}


void CAnimateObj::SetPlaySound(LPSTR lpSound, BOOL bLoopSound, int iChannel)
{
	lstrcpy(m_sSound, lpSound);
	m_bLoopSound = bLoopSound;
	m_iChannel = iChannel;
}


void CAnimateObj::SetSpeed(int newSpeed)            // change rate that objects move
{
	speed = newSpeed;
	if (speed > 40) speed = 40;
	if (speed <  0) speed = 1;
	
#ifdef USE_TIMERCOUNT
	ticsPerMove = 40 - speed;
	if (ticsPerMove < 1) ticsPerMove = 1;
#else
	ticsPerMove = 2000 - (speed * 50);
	if (speed == 0 ) ticsPerMove = 4000;
	if (speed == 40) ticsPerMove =   20;
#endif
	
	ResetTiming();
}


void CAnimateObj::MaskBitmap( HBITMAP hObject, HBITMAP hMask )
{
}

HBITMAP CAnimateObj::CopyBitmap( HDC hDC, HBITMAP hbmSrc )
{
    HBITMAP hbmOldSrc, hbmOldDest, hbmNew;
    HDC     hdcSrc, hdcDest;
    BITMAP  bm;

    hdcSrc = CreateCompatibleDC(hDC);
    hdcDest = CreateCompatibleDC(hDC);

    hbmOldSrc = SelectObject(hdcSrc, hbmSrc);

    GetObject(hbmSrc, sizeof(BITMAP), &bm);
    hbmNew = CreateCompatibleBitmap(hdcSrc, bm.bmWidth, bm.bmHeight);

    hbmOldDest = SelectObject(hdcDest, hbmNew);

    BitBlt( hdcDest, 0, 0, bm.bmWidth, bm.bmHeight, hdcSrc, 0, 0, SRCCOPY );

    SelectObject(hdcDest, hbmOldDest);
    SelectObject(hdcSrc, hbmOldSrc);

    DeleteDC(hdcDest);
    DeleteDC(hdcSrc);

    return( hbmNew );
}

int CAnimateObj::AddAnimateObj(HDC hTypeDC, HBITMAP hObjectBitmap,
		HBITMAP hMaskBitmap )
{
	BitObject       *newOne;
	BitObject       *nextOne;
    BITMAP          bm;

	newOne = new BitObject;
	if (!newOne)
		return FALSE;

    int iFrameNum = 2;
	nextOne = headB;
	while (nextOne->next)
    {
		nextOne = nextOne->next;
        iFrameNum++;
    }

	nextOne->next = newOne;            // link in at the end of the list
    newOne->m_FrameNum = iFrameNum;    // Set frame num to last one

	if ( hObjectBitmap )
    {
		newOne->hObjectBmap = CopyBitmap( hTypeDC, hObjectBitmap );
	    GetObject( newOne->hObjectBmap, sizeof(BITMAP), (LPSTR) &bm );
        newOne->m_Width  = bm.bmWidth;
        newOne->m_Height = bm.bmHeight;
    }
	else
		newOne->hObjectBmap = 0;

	if ( !hMaskBitmap )
		hMaskBitmap = MakeBitmapMono( newOne->hObjectBmap );
	else
		hMaskBitmap = MakeBitmapMono( hMaskBitmap );

	if ( hMaskBitmap )
		newOne->hMaskBmap = hMaskBitmap;
	else
		newOne->hMaskBmap = 0;

	return TRUE;
}


void CAnimateObj::FlipAllVert()                     // flip all bitmaps
{
    BitObject *thisOne;

    int newYOff;
    int fullHeight;
    thisOne = headB;                                        // linked list of bitmaps
    while (thisOne)
    {
        thisOne->FlipVert();                    // both object and mask

        // invert the offset because the image is inverted.
        // image is now offset reletive to the bottom rather than the top
        fullHeight = newrect.bottom - newrect.top;
        newYOff = fullHeight - (thisOne->m_yOff + thisOne->m_Height);
        thisOne->m_yOff = newYOff;

        thisOne = thisOne->next;                // NULL for last link - end.
    }
    needsSpecial = 0;


// Flip all the children
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
        pChild->FlipAllVert();
        pChild = pChild->next;
    }
}



void CAnimateObj::FlipAllHorz()                     // flip all bitmaps
{
	BitObject *thisOne;

    int newXOff;
    int fullWidth;
	thisOne = headB;                                        // linked list of bitmaps
	while (thisOne)
	{
		thisOne->FlipHorz();                    // both object and mask

        // invert the offset because the image is inverted.
        // image is now offset reletive to the right rather than the left
        fullWidth = newrect.right - newrect.left;
        newXOff = fullWidth - (thisOne->m_xOff + thisOne->m_Width);
        thisOne->m_xOff = newXOff;

		thisOne = thisOne->next;                // NULL for last link - end.
	}
	needsSpecial = 0;

// Flip all the children
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
        pChild->FlipAllHorz();
        pChild = pChild->next;
    }
}




void CAnimateObj::Unlink()
{
	if (prev) prev->next = next;
	if (next) next->prev = prev;
}




void CAnimateObj::DeleteInstr(Instr *newOne)        // recursively delete end of the chain
{
	if ( !newOne ) return;
	if (newOne->next) DeleteInstr(newOne->next);
	delete newOne;
}


void CAnimateObj::DeleteAllInstr()  // instructions to execute
{
	if (headI) DeleteInstr(headI);
	headI    = 0;                   // so next insert will start here
	currentI = 0;

	xPos = newrect.left;            // this is our final destination
	yPos = newrect.top;             // may make another move from this point

	moving = 0;                     // stop all motion!
	inBeginCycle  = FALSE;
}



void CAnimateObj::DeleteChild(CAnimateObj *pChild) // recursively delete end of the chain
{
	if (!pChild) return;
	if (pChild->next) DeleteChild(pChild->next);
	delete pChild;      // this will cause the child's children to be deleted.
}



void CAnimateObj::DeleteAllChildren()
{
	if (m_ChildHead) DeleteChild(m_ChildHead);
}


 
void CAnimateObj::DeleteBitObjects(BitObject *newOne)       // recursively delete end of the chain
{
	if ( !newOne ) return;
	if (newOne->next) DeleteBitObjects(newOne->next);
	delete newOne;
}



void CAnimateObj::DeleteAllBitObjects()
{
	if (headB) DeleteBitObjects(headB);
}



CAnimateObj::~CAnimateObj()
{
	DeleteAllBitObjects();      // bitmaps and objects
	DeleteAllInstr();           // there may still be some instructions left
	DeleteAllChildren();        // child animation objects
    if (m_hScript)
        GlobalFree(m_hScript);
    m_hScript = NULL;
}
					   

void CAnimateObj::IncrementObjectPosition(int times)
{
    if (!times) return;
	if (times > maxBits-2) times = maxBits-2;          // limit times to limit jumping

	for (int i=0; i<times; i++)
	{
		if (axisShort == 2)
			OffsetRect(&newrect, xInc, 0);  // go to the next pos on screen
		else
			OffsetRect(&newrect, 0, yInc);


		if (float(incsSoFar) >= movesTilNextInc)        // increment the short side now
		{
			if (axisShort == 2)
				OffsetRect(&newrect, 0, yInc);
			else
				OffsetRect(&newrect, xInc, 0);

			movesTilNextInc += movesPerInc;
		}

		incsSoFar++;
	}

	movesSoFar += times;
}


void CAnimateObj::JumpObject(HWND hWnd)
{   int saveVisible;

	if (abs(xPos - newrect.left) <= maxBits &&
		abs(yPos - newrect.top ) <= maxBits)
	{
		OffsetRect(&newrect, -(newrect.left), -(newrect.top));  // back to origin
		OffsetRect(&newrect, xPos, yPos);                       // new position
        OffsetChildren();               // make sure the children follow
	    Draw(hWnd);
        ResetChildrenPos();             // child movement will be reletive to this parent
		needsSpecial = 0;
		return;
	}

// long jump
	saveVisible = visible;
	HideObject(hWnd);
	visible = saveVisible;

	OffsetRect(&newrect, -(newrect.left), -(newrect.top));  // back to origin
	OffsetRect(&newrect, xPos, yPos);                       // new position
    OffsetChildren();
	CopyRect(&oldrect, &newrect);                   // just in case it isn't
	if (visible)
		ShowObject(hWnd);
    ResetChildrenPos();

	needsSpecial = 0;
}



void CAnimateObj::ShowObject(HWND hWnd)
{
    ShowChildren(hWnd);

	visible   = TRUE;
	needsSpecial = 0;

	CopyRect(&oldrect, &newrect);                   // just in case it isn't
	Draw(hWnd);
}


void CAnimateObj::HideObject(HWND hWnd)
{
	visible   = FALSE;
	needsSpecial = 0;
	CopyRect(&oldrect, &newrect);                   // just in case it isn't

    RECT rDest;
    ChildToScreen(&rDest, &newrect);
	BltRect( hWnd, &rDest, 0/*left*/, 0/*top*/ );

    HideChildren(hWnd);
}


void CAnimateObj::HideChildren(HWND hWnd)
{
    CAnimateObj *pChild = m_ChildHead;
    while(pChild)
    {
        pChild->HideObject(hWnd);
        pChild = pChild->next;
    }
}


void CAnimateObj::ShowChildren(HWND hWnd)
{
    CAnimateObj *pChild = m_ChildHead;
    while(pChild)
    {
        pChild->ShowObject(hWnd);
        pChild = pChild->next;
    }
}



void CAnimateObj::SetupJump(int newX, int newY)
{
// Adjust the jump position reletive to the first frame offset.
// Find the the first frame and subtract its offset.
    BitObject* pFrame = FindFrameNum(1);
    if (pFrame)
    {
        newX -= pFrame->m_xOff;
        newY -= pFrame->m_yOff;
    }

	newX += xOffset;
	newY += yOffset;
	xPos = newX; // set destination point
	yPos = newY;
}


void CAnimateObj::Where(LPRECT lpRect)                      // just return the current rectangle of the object
{
	CopyRect(lpRect, &newrect);
}


void CAnimateObj::ContinuePausing(unsigned long theTime)
{
	unsigned long diff;

    moving = 0;
	if (!firstTiccnt)
	{
		firstTiccnt = theTime;                          // setup time
		return;
	}

	if (theTime < firstTiccnt) return;              // safety measure (shouldn't be)

	diff = theTime - firstTiccnt;                   // wait a certin amount of time
	if (float(diff) < movesPerInc) return;

	needsSpecial = 0;
	SetupNextInstruction();                                 // time has run out - do next
}


void CAnimateObj::SetupPause(int PauseTime)
{
	if (PauseTime <= 0) PauseTime = 1;
	movesPerInc = float(PauseTime) * 100.0;        // 10th of second intervals
}



void CAnimateObj::CheckLastActiveFrame()
{
// Look for the next active frame in the ordered list.
// If there is none, then this is the last active frame.
	bool bLastActive = FALSE;
	
    BitObject *pBitObject = currentB;
    do
    {
	    if (pBitObject->next)             // go to next bitmap
		    pBitObject = pBitObject->next;
	    else
        {
            bLastActive = TRUE;
			break;
        }
    }
    while (!pBitObject->m_bActive);

	moving = 0;
	if (!bLastActive) return;
	needsSpecial = 0;
	SetupNextInstruction();                                 // time has run out - do next
}


void CAnimateObj::SetupRepeat()             // go back to BEGIN and start over
{
	Instr *newOne;

	newOne = headI;                         // start looking at the head
	while (newOne)
	{
		if (newOne->instruction == I_BEGIN)             // look for begining
		{
			if (newOne->next)
				currentI = newOne->next;                // execute one just after BEGIN
			else
				currentI = newOne;
			return;
		}
		newOne = newOne->next;
	}
}


void CAnimateObj::SetupMoveTo(int newX, int newY)
{
	int             xDiff;
	int             yDiff;

// Adjust the jump position reletive to the first frame offset.
// Find the the first frame and subtract its offset.
    BitObject* pFrame = FindFrameNum(1);
    if (pFrame)
    {
        newX -= pFrame->m_xOff;
        newY -= pFrame->m_yOff;
    }


	newX += xOffset;
	newY += yOffset;
	xDiff = newX - xPos;    // new - old positions
	yDiff = newY - yPos;

	if (xDiff >= 0) xInc = 1; else xInc = -1;       // Inc will always be 1
	if (yDiff >= 0) yInc = 1; else yInc = -1;
	if (!xDiff) xInc = 0;
	if (!yDiff) yInc = 0;

	movesPerInc = 9999;

	if (abs(xDiff) > abs(yDiff))                            // tell which axis is "short"
	{
		axisShort = 2;
		if (yDiff)
			movesPerInc = float( abs(xDiff)) / float( abs(yDiff));
	}
	else
	{
		axisShort = 1;
		if (xDiff)
			movesPerInc = float( abs(yDiff)) / float( abs(xDiff));
	}
	xPos = newX;
	yPos = newY;

	movesTilNextInc = movesPerInc;                          // add 1 next time it reaches this
    moving = 1;
}



void CAnimateObj::SetupTrack(HWND hWnd)             // get ready to track the mouse
{
	POINT	newPoint;
	int		xDiff;
	int		yDiff;
	int		width;
	int		height;

	GetCursorPos(&newPoint);                // where is the mouse now
	ScreenToClient(hWnd, &newPoint);

	if (PtInRect(&newrect, newPoint))       // if already in object, then its ok
	{   xPos  = newPoint.x;
		yPos  = newPoint.y;
		xLast = xPos;
		yLast = yPos;

		return;
	}

	xLast = 9999;                           // force a realignment if outside of object rect
	yLast = 9999;

	width  = newrect.right  - newrect.left;
	height = newrect.bottom - newrect.top;
	xPos   = newPoint.x - (width  / 2);     // must move the object to the mouse
	yPos   = newPoint.y - (height / 2);

	if (xPos < 1) xPos = 1;                 // not off the screen
	if (yPos < 1) yPos = 1;
}


void CAnimateObj::TrackMouse(HWND hWnd)
{
	POINT   newPoint;
	int		xDiff = 0;
	int		yDiff = 0;
	int		width;
	int		height;

	GetCursorPos(&newPoint);
	ScreenToClient(hWnd, &newPoint);
	if (trackLimit == NO_X) newPoint.x = xLast;     // no movement here
	if (trackLimit == NO_Y) newPoint.y = yLast;

	xDiff = newPoint.x - xLast;
	yDiff = newPoint.y - yLast;

	if (xDiff == 0 && yDiff == 0) return;

	xLast = newPoint.x;
	yLast = newPoint.y;

	xPos += xDiff;
	yPos += yDiff;

	if (abs(xDiff) < maxBits && abs(yDiff) < maxBits)
	{
		OffsetRect(&newrect, xDiff, yDiff);
		Draw(hWnd);
	}
	else
	{
		width  = newrect.right  - newrect.left;
		height = newrect.bottom - newrect.top;
		xPos   = newPoint.x - (width  / 2);     // must move the object to the mouse
		yPos   = newPoint.y - (height / 2);

		JumpObject(hWnd);
	}
	needsSpecial = I_TRACK;                     // keep tracking
}



BOOL CAnimateObj::CheckSpecialInstructions(HWND hWnd, unsigned long theTime)
{
	if (!needsSpecial) return FALSE;
    bool bHandled = FALSE;
	
	switch (needsSpecial)                 // rather than normal moving
	{
    	case I_TRACK:
    		TrackMouse(hWnd);
    		bHandled = TRUE;
            break;
    	case I_HIDE:
    		HideObject(hWnd);
    		SetupNextInstruction();
    		bHandled = TRUE;
            break;
    	case I_SHOW:
    		ShowObject(hWnd);
    		SetupNextInstruction();
    		bHandled = TRUE;
            break;
    	case I_JUMP:
    		JumpObject(hWnd);
    		SetupNextInstruction();
    		bHandled = TRUE;
            break;
    	case I_PAUSE:
    		ContinuePausing(theTime);
//    		bHandled = TRUE;
            break;
    	case I_WAITLASTFRAME:
			CheckLastActiveFrame();
            break;
    	case I_FVERT:
    		FlipAllVert();
    		SetupNextInstruction();
    		bHandled = TRUE;
            break;
    	case I_FHORZ:
    		FlipAllHorz();
    		SetupNextInstruction();
    		bHandled = TRUE;
            break;
    	case I_SETFRAME:
            Draw(hWnd);
            needsSpecial = 0;
    		SetupNextInstruction();
    		bHandled = TRUE;
            break;
	}
    return bHandled;
}




void CAnimateObj::CheckMoveAnimateObj(HWND hWnd, unsigned long theTime)
// the object will move only when it is supposed to
{
    CheckMoveChildren(hWnd, theTime);
    if (CheckSpecialInstructions(hWnd, theTime))
        return;

    bool bUpdated = CheckCycleFrames(theTime);

// Moving instructions
	unsigned long   diff;
	int  moves = 0;

	if (!moving && !bUpdated) return;                    // stopped for any reason


	if (!firstTiccnt || (theTime < firstTiccnt))    // happens once per start of instruction
	{
		firstTiccnt = theTime;
        m_lLastCycleTime = theTime;
		movesSoFar = 0;
		if (!bUpdated) return;
	}

	diff = theTime - firstTiccnt;               // total elapsed time since start of instruction
	if (diff >= ticsPerMove)
    {
	    moves = (diff / ticsPerMove) - movesSoFar;  // moves since start of instruction
	    bUpdated |= (moves > 0);                      // nothing to move? on this time
    }

    if (!bUpdated)
        return;

// Something about the object has changed - redraw it
	if (moving)
    {
        if (currentI->instruction == I_MOVE)
        {
            IncrementObjectPosition(moves);
            OffsetChildren();               // make sure the children follow
	        Draw(hWnd);
            ResetChildrenPos();             // child movement will be reletive to this parent
        }

        if (currentI->instruction != I_PAUSE ||
        	currentI->instruction != I_WAITLASTFRAME)
        {
	        if (CheckDestination())
		        SetupNextInstruction();
        }
    }
    else
        Draw(hWnd);
}



void CAnimateObj::Draw(HWND hWnd)
{
	RECT    r;
	int     left;
	int     top;

// 1) get UnionRect of old and new rect positions for the image
// 2) copy fresh "virgin" copy of screen to a loading area with this rect
// 3) SRCAND mask onto loading area to make a "hole" for image
// 4) SRCPAINT (or) the object to the loading area
// 5) copy loading area to the screen

// you will notice that everything in the code below is opposite
// from the above description.  this is because the bits in
// bitmaps are inverted.  i.e. black=0 and white=1.
// you will notice that the mask for each object is also inverted.
// the white area is where the image is displayed.

	UnionRect(&r, &newrect, &oldrect);              // redraw old and new positions

	left = newrect.left - oldrect.left;             // offset within loading area
	if (left < 0) left = 0;
	top  = newrect.top  - oldrect.top ;
	if (top  < 0) top  = 0;

	CopyRect(&oldrect, &newrect);                   // wait until oldrect is used

	if (!visible) return;                           // got to be showing

    RECT rDest;
    ChildToScreen(&rDest, &r);
	BltRect( hWnd, &rDest, left, top );

    DrawChildren(hWnd);                             // allow all the children to draw
}




void CAnimateObj::DrawChildren(HWND hWnd)
{
// Draw all the child animation objects
    CAnimateObj *pAnimateObj = m_ChildHead;
    while(pAnimateObj)
    {
        pAnimateObj->Draw(hWnd);
        pAnimateObj = pAnimateObj->next;
    }
}


void CAnimateObj::CheckMoveChildren(HWND hWnd, unsigned long theTime)
{
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
        pChild->CheckMoveAnimateObj(hWnd, theTime);
        pChild = pChild->next;
    }
}



void CAnimateObj::SetParentOffset(POINT pt)
{
    ptParent = pt;
}


void CAnimateObj::OffsetObjRect(POINT pt)
{
    OffsetRect(&newrect, pt.x, pt.y);
    OffsetChildren();
}


void CAnimateObj::OffsetChildren()
{
// Make sure children track the parent.  Each change to the parent
// is reflected in the children.  They move along with the parent.

    POINT pt;
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
        pt.x = newrect.left - oldrect.left;
        pt.y = newrect.top  - oldrect.top;
        pChild->OffsetObjRect(pt);
        pChild = pChild->next;
    }
}

void CAnimateObj::ResetObjRect(POINT pt)
{
    int xOff = ptParent.x - pt.x;
    int yOff = ptParent.y - pt.y;
    OffsetRect(&newrect, xOff, yOff);
    CopyRect(&oldrect, &newrect);
    ResetChildrenPos();
    ptParent = pt;
}


void CAnimateObj::ResetChildrenPos()
{
// Make sure children track the parent.  Each change to the parent
// is reflected in the children.  They move along with the parent.

    POINT pt;
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
        pt.x = newrect.left;
        pt.y = newrect.top;
        pChild->ResetObjRect(pt);
        pChild = pChild->next;
    }
}


void CAnimateObj::SetChildrenParentOffsets()
{
// Set the Parent offset for each child. This ensures that their
// movement is reletive to the parent.

    POINT pt;
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
        pt.x = newrect.left;
        pt.y = newrect.top;
        pChild->SetParentOffset(pt);
        pChild = pChild->next;
    }
}




void CAnimateObj::BltRect( HWND hWnd, LPRECT lpR, int left, int top )
{
	HDC hDC, loadMap;
	HBITMAP newBitmap, oldBitmap;
	int fullWidth, fullHeight;

	fullWidth  = lpR->right  - lpR->left;
	fullHeight = lpR->bottom - lpR->top;
	if ( !(loadMap = CreateCompatibleDC(NULL)) ) // make a new loading area
		return;
	if ( !(newBitmap = CreateCompatibleBitmap(virginMap, fullWidth, fullHeight)) )
		{
		DeleteDC( loadMap );
		return;
		}
	oldBitmap = SelectObject(loadMap, newBitmap );

	BitBlt(loadMap, 0, 0, fullWidth, fullHeight,
			virginMap, lpR->left, lpR->top, SRCCOPY);

	CopyAllBitmaps(hWnd, loadMap, lpR, left, top);

	if ( hDC = GetDC(hWnd) ) // clear the bitmap by copying virgin to screen
	{
		BitBlt(hDC, 
		    lpR->left, 
		    lpR->top, 
		    fullWidth, fullHeight,
		    loadMap, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hDC);                    // done drawing
	}

	SelectObject( loadMap, oldBitmap );
	DeleteDC( loadMap );
	DeleteObject( newBitmap );
}


void CAnimateObj::ChildToScreen(LPRECT lpRect, LPRECT lpStart)
{
// Get the real screen position of this object.
// Child object coordinates are reletive to the parents.
// Check each parent for an offset.  The cummulative offset is the
// actual screen rect of this object.

    SetRect(lpRect, lpStart->left, lpStart->top, lpStart->right, lpStart->bottom);
    OffsetRect(lpRect, ptParent.x, ptParent.y);

    CAnimateObj *pParent = m_Parent;
    while (pParent)
    {
        OffsetRect(lpRect, pParent->ptParent.x, pParent->ptParent.y);
        pParent = pParent->m_Parent;
    }
}



void CAnimateObj::CopyAllBitmaps (HWND hWnd, HDC loadMap, LPRECT r,
						int left, int top)
{   CAnimateObj  *newOne;
	RECT  destrect, rActual, rThisActual;
	HDC hDC;

	if ( !(hDC = CreateCompatibleDC( NULL )) )
		return;

// Start the chaing back up at the Head of all objects.
// If any intersect with this object, then try must be drawn.
	newOne = this;
	while(newOne->m_Parent)
		newOne = newOne->m_Parent;      // go back to root parent

	while(newOne->prev)
		newOne = newOne->prev;          // go back to the begginning of objects

// Display ANY object that intersects with this rect.
// Place ALL intersecting objects into the loading area for display.
	while(newOne)                                   // newOne will ne NULL at end
	{
        newOne->ChildToScreen(&rActual, &newOne->newrect);
		if (IntersectRect(&destrect, &rActual, r)
		    && newOne->visible)
		{
			if (wNmessage && this != newOne)
				SendNotification(hWnd, newOne->descriptor);

            ChildToScreen(&rThisActual, &newrect);
			int lPlus = rActual.left - rThisActual.left;
			int tPlus = rActual.top  - rThisActual.top;

			SelectObject( hDC, newOne->currentB->hMaskBmap );
			BitBlt(loadMap, 
                left + lPlus + newOne->currentB->m_xOff, 
                top  + tPlus + newOne->currentB->m_yOff, 
                newOne->currentB->m_Width, 
                newOne->currentB->m_Height,
				hDC, 
                0, 
                0, 
                SRCAND);

			SelectObject( hDC, newOne->currentB->hObjectBmap );
			BitBlt(loadMap, 
                left + lPlus + newOne->currentB->m_xOff, 
                top  + tPlus + newOne->currentB->m_yOff, 
                newOne->currentB->m_Width, 
                newOne->currentB->m_Height,
				hDC, 
                0, 
                0, 
                SRCPAINT);
		}

// Select the next object to draw
// Go down through the children and grandchildren first.
		if (newOne->m_ChildHead)
            newOne = newOne->m_ChildHead;
        else if (newOne->next)
            newOne = newOne->next;
        else if (newOne->m_Parent)
            newOne = newOne->m_Parent->next;
        else
            newOne = NULL;
	}

	DeleteDC( hDC );
}


void CAnimateObj::SendNotification(HWND hWnd, int newDesc)  // tell the window a hit has occurred
{
	WORD    theMsg = wNmessage;

	if (!continuous)
		wNmessage = 0;                                  // now he can't send it again!
	else if (lastContObject == newDesc) return;
	lastContObject = newDesc;

	if (!Awhich || Awhich == newDesc)       // any object or just "newDesc" object
	{
		PostMessage(hWnd, theMsg, descriptor, newDesc);
//      SendMessage(hWnd, theMsg, descriptor, newDesc);
	}
}


int CAnimateObj::CheckDestination()
{
	if (newrect.left == xPos && newrect.top == yPos) return TRUE;

	if (xInc > 0 && newrect.left > xPos) return TRUE;
	if (xInc < 0 && newrect.left < xPos) return TRUE;

	if (yInc > 0 && newrect.top  > yPos) return TRUE;
	if (yInc < 0 && newrect.top  < yPos) return TRUE;

	if (xInc == 0 && yInc == 0) return TRUE;                        // same pos as before

	return FALSE;
}




BOOL CAnimateObj::CheckCycleFrames(unsigned long theTime)
{
    if (!m_lCycleTime) return FALSE;                // don't cycle if 0
	if (!headB->next) return FALSE;                 // any more bitmaps
    if ((theTime - m_lLastCycleTime) < m_lCycleTime)
        return FALSE;
    m_lLastCycleTime = theTime;

// Look for the next active frame in the ordered list.
    BitObject *pStart = currentB;
    do
    {
	    if (currentB->next)             // go to next bitmap
		    currentB = currentB->next;
	    else
        {
            if (!m_bRepeatCycle)        // are we allowed to repeat the cycle at the beginning
            {
                currentB = pStart;
                break;
            }
            currentB = headB;          // or head
        }
    }
    while (!currentB->m_bActive && (currentB != pStart));
    
    return (pStart != currentB);
}



void CAnimateObj::SetFramesPerSec(int iFmsPerSec)
{
    if (iFmsPerSec <= 0)
         m_lCycleTime = 0;
    else m_lCycleTime = 1000 / iFmsPerSec;
}



CAnimateObj* CAnimateObj::FindChild(int Child)
{
    int i=1;
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
        if (Child == i)
            return pChild;    
        pChild = pChild->next;
        i++;
    }
    return NULL;
}



void CAnimateObj::SetChildFramesPerSec(int Child, int iFmsPerSec)
{
    CAnimateObj* pChild = FindChild(Child);
    if (pChild)
        pChild->SetFramesPerSec(iFmsPerSec);
}


BitObject* CAnimateObj::FindFrameNum(int iFrame)
{
    BitObject* pFrame = headB;
    while (pFrame && (pFrame->m_FrameNum != iFrame))
        pFrame = pFrame->next;
    return pFrame;
}

void CAnimateObj::ActivateFrame(int iFrame, bool bActivate)
{
    BitObject* pFrame = FindFrameNum(iFrame);
    if (pFrame)
        pFrame->m_bActive = bActivate;
}

              
void CAnimateObj::ActivateChildFrame(int Child, int iFrame, bool bActivate)
{
    CAnimateObj* pChild = FindChild(Child);
    if (pChild)
        pChild->ActivateFrame(iFrame, bActivate);
}


void CAnimateObj::SetCurrentFrame(int nFrame)
{
	if (!headB->next) return;                 // any more bitmaps

// Select the specified frame.
    int i = 1;
    bool bSet = FALSE;
    BitObject *pCurrent = currentB;
    currentB = headB;
    while (currentB)
    {
        if (i == nFrame)
        {
            bSet = TRUE;
            break;
        }
        i++;
	    currentB = currentB->next;
    }
    if (!bSet)
        currentB = pCurrent;            // restore the old one.
}              



void CAnimateObj::SetFrameOrder(int iFrame, int iOrder)
{
// Make sure there is more than one object to be reordered.
    if (!headB->next) return;

// Try to locate the specified frame in the ordered list.
    BitObject* pFrame = headB;
    BitObject* pPrev  = NULL;
    while (pFrame && (pFrame->m_FrameNum != iFrame))
    {
        pPrev = pFrame;
        pFrame = pFrame->next;
    }
    if (!pFrame) return;

// Unlink this frame from the list.  It will be reinserted later.
    if (pFrame == headB)
        headB = pFrame->next;    
    else if (pPrev)
        pPrev->next = pFrame->next;
    pFrame->next = NULL;


// Try to locate this order.
// ReInsert it into another place - changing the order.
    int i = 1;
    BitObject* pInsFrame = headB;
    BitObject* pInsPrev  = NULL;
    while (pInsFrame)
    {
        if (i == iOrder)
        {       
            if (pInsPrev)
                pInsPrev->next = pFrame;
            pFrame->next = pInsFrame;
            if (pInsFrame == headB)
                headB = pFrame;
            return;
        }
        i++;
        pInsPrev  = pInsFrame;
        pInsFrame = pInsFrame->next;
    }

// The Order number could not be found.  Reinsert the frame at the end of the list.
    if (pInsPrev)
        pInsPrev->next = pFrame;
}



void CAnimateObj::SetChildFrameOrder(int Child, int iFrame, int order)
{
    CAnimateObj* pChild = FindChild(Child);
    if (pChild)
        pChild->SetFrameOrder(iFrame, order);
}






void CAnimateObj::SetFrameOffset(int iFrame, int x, int y)
{
// Each frame in an object can be a different size and offset from 0,0
    BitObject* pFrame = FindFrameNum(iFrame);
    if (pFrame)
    {
// Set the frame offset for this frame.  Widen newrect if necessary.
// newrect becomes a union of all the different sizes.
        pFrame->m_xOff = x;
        pFrame->m_yOff = y;

        RECT rFrame;
        SetRect(&rFrame, 
            newrect.left, 
            newrect.top, 
            newrect.left + pFrame->m_Width, 
            newrect.top  + pFrame->m_Height);
        OffsetRect(&rFrame, pFrame->m_xOff,  pFrame->m_yOff);
        UnionRect(&newrect, &rFrame, &newrect);
        CopyRect(&oldrect, &newrect);
    }
}


void CAnimateObj::NormalizeFrameOffsets()
{
// if an offset is negative, loop over all frames and add that amount to each.
// This normalizes all frames to 0.
// Nether pFrame->m_xOff nor pFrame->m_yOff can be less than 0.
    
    int iXCorrect=0;
    int iYCorrect=0;
    BitObject* pFrame = headB;
    while (pFrame)
    {
        if ((pFrame->m_xOff < 0) || (pFrame->m_yOff < 0))
        {
            if (pFrame->m_xOff < 0)
                iXCorrect = -(pFrame->m_xOff);    
            if (pFrame->m_yOff < 0)
                iYCorrect = -(pFrame->m_yOff);
            
            // Correct ALL frames if any one frame is found to be negative.
            BitObject* pNext = headB;    
            while (pNext)
            {
            	pNext->m_xOff += iXCorrect;
            	pNext->m_yOff += iYCorrect;
            	pNext = pNext->next;
            }
        }
        pFrame = pFrame->next;
    }
}


void CAnimateObj::SetChildFrameOffset(int Child, int iFrame, int x, int y)
{
    CAnimateObj* pChild = FindChild(Child);
    if (pChild)
        pChild->SetFrameOffset(iFrame, x, y);
}





CAnimateObj *CAnimateObj::FindChildObj(int dAobject)
{
// Look for this descriptor within the list of children
// If it is found, return the child pointer to the caller.
	CAnimateObj *pChild = m_ChildHead;
    CAnimateObj *pGrandChild;
	while(pChild)
	{
		if (dAobject == pChild->GetDescriptor()) 
		    return pChild;
        if (pGrandChild = pChild->FindChildObj(dAobject))
            return pGrandChild;
		pChild = pChild->next;
	}
	return 0;
}



void CAnimateObj::SetupNextInstruction()    // do the next instruction in chain
{   Instr       *oldOne;

	if (!currentI)
	{   moving = 0;
		return;
	}

	oldOne = currentI;

	currentI = currentI->next;
	if (!inBeginCycle && !m_bAutoRepeat)
	{
		headI = currentI;           // set head to the the one we are going to execute
        delete oldOne;              // kill the one we just finished executing
	}

	if (currentI)
	{ 
	    SetupThisInstruction();
		
		if (!needsSpecial &&
        currentI->instruction != I_MOVE  &&
        currentI->instruction != I_BEGIN &&
        currentI->instruction != I_REPEAT)
    	{
        	SetupNextInstruction();
    	}
	}
	else if (m_bAutoRepeat)
		RestartInstructions();
	else
		moving = 0;                 // stop moving if no more instructions
}


void CAnimateObj::SetupThisInstruction()
{   
    Instr*          newOne;
    CAnimateObj*    pAnimateObj;

	if (!currentI) return;

	switch (currentI->instruction)
	{
		case I_MOVE:
            moving = 1;
			SetupMoveTo(currentI->x, currentI->y);
			break;

		case I_JUMP:
			SetupJump(currentI->x, currentI->y);
			needsSpecial = I_JUMP;                          // do it next "checkMove"
			break;

		case I_HIDE:
			needsSpecial = I_HIDE;                  // next "move" call will hide it
			break;

		case I_SHOW:
			needsSpecial = I_SHOW;
			break;

		case I_BEGIN:
			inBeginCycle = TRUE;
			break;

		case I_REPEAT:                                          // go back to BEGIN instruction
			SetupRepeat();
            if (currentI->instruction != I_REPEAT)
			{
				SetupThisInstruction();                 // go do the first one again
				PlaySound();
			}
            else 
            	SetupNextInstruction();
			break;

		case I_PAUSE:
			SetupPause(currentI->param);
			needsSpecial = I_PAUSE;
			break;

		case I_WAITLASTFRAME:
			needsSpecial = I_WAITLASTFRAME;
			break;

		case I_KILL:
			killMe = TRUE;                                  // will be killed later
			break;

		case I_FVERT:
			needsSpecial = I_FVERT;
			break;

		case I_FHORZ:
			needsSpecial = I_FHORZ;
			break;

		case I_SPEED:
			SetSpeed(currentI->param);
			break;

		case I_INCSPEED:
			SetSpeed(GetSpeed() + 1);
			break;

		case I_DECSPEED:
			SetSpeed(GetSpeed() - 1);
			break;

		case I_ACTFRAME:
            ActivateFrame(currentI->x, currentI->y);
			break;

		case I_ACTCHILDFRAME:
            ActivateChildFrame(currentI->x, currentI->y, currentI->param);
			break;

		case I_SETFRAME:
            SetCurrentFrame(currentI->param);
            needsSpecial = I_SETFRAME;                  // next "move" call will hide it
			break;

		case I_SETCHILDFRAME:
            pAnimateObj = FindChild(currentI->x);
            if (pAnimateObj)
                pAnimateObj->SetCurrentFrame(currentI->y);
			break;
            
		case I_CYCLESPEED:
            SetFramesPerSec(currentI->param);
			break;

		case I_CHILDCYCLESPEED:
            SetChildFramesPerSec(currentI->x, currentI->param);
			break;

		case I_FRAMEORDER:
            SetFrameOrder(currentI->x, currentI->y);
			break;

		case I_CHILDFRAMEORDER:
            SetChildFrameOrder(currentI->x, currentI->y, currentI->param);
			break;

		case I_FRAMEOFFSET:
            SetFrameOffset(currentI->x, currentI->y, currentI->param);
            NormalizeFrameOffsets();
			break;

		case I_CHILDFRAMEOFFSET:
            SetChildFrameOffset(0, currentI->x, currentI->y, currentI->param);
			break;

		case I_REPEATCYCLE:
            m_bRepeatCycle = currentI->x;
			break;
	}

	if (currentI->instruction != I_STOP  &&
		currentI->instruction != I_PAUSE && 
		currentI->instruction != I_WAITLASTFRAME)
        moving = 1;

	firstTiccnt = 0;                                        // make a fresh start after each instruction
	movesSoFar  = 0;
	incsSoFar   = 0;
	movesTilNextInc = 0;
}



void CAnimateObj::InsertNewInstruction(BYTE newInstr, int X, int Y, int newParam)
{
	Instr   *newOne;
	Instr   *nextOne;

	newOne = new Instr(newInstr, X, Y, newParam);                           // make the new instruction
	if (!newOne) return;

// look at head
	nextOne = headI;
	if (!nextOne)                     // if head is not pointing to anything
	{
		headI    = newOne;
		currentI = newOne;
		SetupThisInstruction();
		return;
	}
	
// add to chain
	while (1)                          // loop to last instruction, then add a new one
	{
		if ((nextOne->next) == 0)
		{
			nextOne->next = newOne;
			return;
		}
		nextOne = nextOne->next;
	}
}


void CAnimateObj::SetAutoRepeat(bool bAutoRepeat)
{
// Tell this object to start or stop repeating the instructions it has.
// When it is in AutoRepeat mode it will repeat the instructions when
// they reach the end.  This will go on until the object is deleted.

	m_bAutoRepeat = bAutoRepeat;

    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
		pChild->SetAutoRepeat(bAutoRepeat);
        pChild = pChild->next;
    }
}



void CAnimateObj::RestartInstructions()
{
// This function restarts the object back at the first instruction.
// Instructions begin executing now.

// Activate all the frames, just like it was when the instructions began.
    BitObject* pFrame = headB;
    while (pFrame)
	{
        pFrame->m_bActive = TRUE;
		pFrame = pFrame->next;
	}
	currentB = headB;

// Set the speed back to the default.
	SetSpeed(DEFAULT_SPEED);
	m_lCycleTime = DEFAULT_CYCLETIME;
	
	currentI = headI;
	SetupThisInstruction();

// Restart all the children.
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
		pChild->RestartInstructions();
        pChild = pChild->next;
    }
	
	PlaySound();
}





HBITMAP CAnimateObj::MergeToBackground(HBITMAP hBackBitmap, HBITMAP hObjectBitmap)
{
    HBITMAP     hBitmap;
    HBITMAP     hMask;
    HBITMAP     hOldBackBitmap;
    HBITMAP     hOldObjectBitmap;
    HDC         hMemoryDC;
    HDC         hObjectDC;
    HDC         hBitDC;
    BITMAP      bmBack;

// Create DC and Bitmap
    hMask = MakeBitmapMono(hObjectBitmap);
    if (!hMask) return 0;
    GetObject(hBackBitmap, sizeof(bmBack), &bmBack);    
    hMemoryDC = CreateCompatibleDC(NULL);
    hObjectDC = CreateCompatibleDC(NULL);
    if (!hMemoryDC) 
    {
        DeleteObject(hMask);
        return 0;
    }
    if (!hObjectDC) 
    {
        DeleteDC(hMemoryDC);
        DeleteObject(hMask);
        return 0;
    }
    hBitmap = CreateCompatibleBitmap(hMemoryDC, 
        bmBack.bmWidth,
        bmBack.bmHeight);
    if (!hBitmap)
    {
        DeleteObject(hMask);
        DeleteDC(hMemoryDC);
        return 0;
    }
    
    hOldBackBitmap   = SelectObject(hMemoryDC, hBitmap);
    hOldObjectBitmap = SelectObject(hObjectDC, hBackBitmap);

// Copy in the background bitmap
    BitBlt(
        hMemoryDC, 
        0, 
        0, 
        bmBack.bmWidth, 
        bmBack.bmHeight,
        hObjectDC, 
        0, 
        0, 
        SRCCOPY);

// Merge in the mask and object bitmaps.    
    SelectObject(hObjectDC, hMask);
    BitBlt(
        hMemoryDC, 
        0, 
        0, 
        bmBack.bmWidth, 
        bmBack.bmHeight,
        hObjectDC, 
        0, 
        0, 
        SRCAND);

    SelectObject(hObjectDC, hObjectBitmap);
    BitBlt(
        hMemoryDC, 
        0, 
        0, 
        bmBack.bmWidth, 
        bmBack.bmHeight,
        hObjectDC, 
        0, 
        0, 
        SRCPAINT);
        
// Clean up        
    SelectObject(hMemoryDC, hOldBackBitmap);
    SelectObject(hObjectDC, hOldObjectBitmap);
    DeleteDC(hMemoryDC);
    DeleteDC(hObjectDC);
    DeleteObject(hMask);

    return hBitmap;
}


int CAnimateObj::GetChildObjectID(int Child)
{
    int descriptor = 0;
    int i=1;

// Look for the child of this object
    CAnimateObj *pChild = m_ChildHead;
    while (pChild)
    {
        if (i == Child)
        {
            descriptor = pChild->GetDescriptor();
            break;
        }
        pChild = pChild->next;
    }
    return descriptor;
}
