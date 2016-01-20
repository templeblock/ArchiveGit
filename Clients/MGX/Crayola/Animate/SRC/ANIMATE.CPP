/*
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#include "app.h"
#include "animate.h"
#include "object.h"
#include "bitmaps.h"


// various instructions you can send to an animation object
			   
/*
Regulating the speed of moving objects:
The goal of animating moving objects on the screen is to keep a
constant speed for all objects.  There are a few factors that affect this:

	1) the number of objects on the screen at any time
	2) the size of the bitmaps being moved
	3) the speed of the processor
	4) the frequency at which they are moved

The Animator object attemps to regulate the movement of the Aobjects
by making certain that each object moves 1 bit per every so # of tics.
This means that for every so many timer "tics" the object should move
1 bit on the screen.  Since the timer is consistant regardless of
the processor or its load, it can be depended upon to regulate
the speed of moving objects.

Each time you launch a new bitmap, it will take up more processor
time, but the timer will still be consistant.  The object MUST
move 1 bit for every so many tics, so the objects will continue to
move at the same speed.  They will begin to jump because they have
to move many bits to keep up with the timer.  The more objects
on the screen, the more jerky they will be.  Really fast processors
will be able to keep up the movement of the objects without jumping.
*/


// -----------------------master Animator-----------------------------------

#define LAST_DESCRIPTOR     32000
#define RESTART_DESCRIPTOR  100

int bEnhanced;				// messy problem with reading ports



CAnimator::CAnimator()                            // constructor
{
	virginMap = CreateCompatibleDC(NULL);
	hvirginBitmap = 0;
	bBackDestroy = 0;
	hBackWnd  = 0;
	headA     = 0;

	lastANum  = 0;
	stopped   = FALSE;

#ifdef WIN32
	bEnhanced = TRUE;
#else
	bEnhanced = GetWinFlags() & WF_ENHANCED;
#endif
	CVAnimateInit();			// make sure precompiled header OBJ is linked
}




void CAnimator::HideAndDeleteAobjects(CAnimateObj *newOne)   // recursively delete end of the chain
{
	if ( !newOne )
		return;
	if (newOne->next) HideAndDeleteAobjects(newOne->next);
	DeleteAobject(newOne->GetDescriptor());
}





void CAnimator::DeleteAllObjects()
{
	while (headA)
		DeleteAobject(headA->GetDescriptor());
}







void CAnimator::DeleteAobjects(CAnimateObj *newOne)  // recursively delete end of the chain
{
	if ( !newOne )
		return;
	if (newOne->next) DeleteAobjects(newOne->next);
	delete newOne;
}



CAnimator::~CAnimator()
{

	if (virginMap)		DeleteDC(virginMap);            // delete DC THEN the bitmaps
	virginMap = 0;
	if (hvirginBitmap)	DeleteObject(hvirginBitmap);
	hvirginBitmap = 0;
	if ( bBackDestroy && hBackWnd )
	{ // hide the window first so there's no redraw when it's destroyed
		SetWindowPos( hBackWnd, NULL, 0, 0, 0, 0, // No move or size
			SWP_HIDEWINDOW | SWP_NOREDRAW );
		DestroyWindow( hBackWnd );
	}
	hBackWnd = NULL;

	if (headA) DeleteAobjects(headA);
}



void CAnimator::DeleteInstructions(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->DeleteAllInstr();
}




BOOL CAnimator::Where(int dAobject, LPRECT lpRect)       // return the rectangle
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return(FALSE);

	if ( lpRect )
		thisOne->Where(lpRect);

	return(thisOne->moving || 
        (thisOne->headB &&
         thisOne->headB->next &&
         thisOne->m_lCycleTime) );      // moving and/or cycling through bitmaps
}






void CAnimator::CopyBackgroundScreen(HWND hWnd, LPRECT lpClipRect)
{ // make a new virgin copy
	HDC     hDC;
	RECT    r;
	HBITMAP hNewBitmap;
	HWND	hWndClip;

	if ( !virginMap )
		return;
	if ( bBackDestroy && hBackWnd )
	{ // hide the window first so there's no redraw when it's destroyed
		SetWindowPos( hBackWnd, NULL, 0, 0, 0, 0, // No move or size
			SWP_HIDEWINDOW | SWP_NOREDRAW );
		DestroyWindow( hBackWnd );
	}

	hBackWnd = hWnd;
	bBackDestroy = FALSE;
	if ( lpClipRect )
	{ // create a clipping window
		if ( hWndClip = CreateWindowEx( WS_EX_TOPMOST, "static",
			"",
			WS_POPUP|WS_DISABLED|WS_VISIBLE,
			lpClipRect->left, lpClipRect->top,			// x, y
			lpClipRect->right - lpClipRect->left + 1,	// dx
			lpClipRect->bottom - lpClipRect->top + 1,	// dy
			HWND_DESKTOP,								// parent
			NULL,										// hMenu
#ifdef WIN32			
			(HINSTANCE)
			GetWindowLong( GetDesktopWindow(), GWL_HINSTANCE ), // hInstance
#else
			GetWindowWord( GetDesktopWindow(), GWW_HINSTANCE ), // hInstance
#endif
			NULL ) )									// lpvParam
			{
				hBackWnd = hWndClip;
				bBackDestroy = TRUE;
			}
	}

	if ( !(hDC = GetDC(hBackWnd)) )
		return;
	GetClientRect(hBackWnd, &r); // find out how big it is
	if ( !(hNewBitmap = CreateCompatibleBitmap(hDC, r.right, r.bottom)) )
	{
		ReleaseDC(hBackWnd, hDC);
		return;
	}

	SelectObject(virginMap, hNewBitmap);
	if (hvirginBitmap) DeleteObject(hvirginBitmap);
	hvirginBitmap = hNewBitmap;

	BitBlt(virginMap, r.left, r.top, r.right, r.bottom, hDC, 0, 0, SRCCOPY);        // make copy of the backgound screen
	ReleaseDC(hBackWnd, hDC);
}




void CAnimator::GiveBackgroundBitmap(HWND hWnd, HBITMAP hNewBitmap, BOOL bTile)
{
    if (!hNewBitmap) return;

// If the bitmap is to be tiled, then blt row/colm into the virgin HDC.
	hBackWnd = hWnd;
    if (bTile)
    {
        if (!hvirginBitmap)
            return;

        BITMAP bm;
        GetObject(hvirginBitmap, sizeof(BITMAP), (LPSTR)&bm);
        RECT r;
        SetRect(&r, 0, 0, bm.bmWidth, bm.bmHeight);
        GetObject(hNewBitmap, sizeof(BITMAP), (LPSTR)&bm);

        HDC hMemoryDC = CreateCompatibleDC(virginMap);
        if (!hMemoryDC) return;

        HBITMAP hOld = SelectObject(hMemoryDC, hNewBitmap);
        int x, y, width, height;
        for (x=r.left; x<r.right; x += bm.bmWidth)
            for (y=r.top; y<r.bottom; y += bm.bmHeight)
            {
                width  = min(r.right-x,  bm.bmWidth); 
                height = min(r.bottom-y, bm.bmHeight); 
                BitBlt(virginMap, x, y, width, height, hMemoryDC, 0, 0, SRCCOPY);
            } 

        SelectObject(hMemoryDC, hOld);
        DeleteDC(hMemoryDC);
        DeleteObject(hNewBitmap);
    }
    else
    {
// Simply use the bitmap as is.
        SelectObject(virginMap, hNewBitmap);
        if (hvirginBitmap) DeleteObject(hvirginBitmap);
        hvirginBitmap = hNewBitmap;
    }
}



HBITMAP CAnimator::RepairBackgroundBitmap(
HBITMAP hBackBitmap, 
RECT rSrcRepair, 
RECT rDstRepair,
bool bGetPrevious)
{
/*
	This function is used to "repair" the current virgin background bitmap.
	This eliminates the need to first hide the object, then make a copy of the
	background screen.
	Since it it necessary to have a virgin background screen that DOES NOT have the
	image of what is being animated, this allows you to "remove" just that
	portion from background bitmap.

	1) An object on the screen is needed to be animated.
	2) Register the object and copy the entire screen as it is.  (including the object itself).
	3) Replace the object rect of the virgin "copied" background with a "repair"
	   bitmap.
*/
	HBITMAP hPrevious = NULL;
	HDC 	hMemoryDC;
	HBITMAP hOld;

// Create a new bitmap that has the previous bits in it.
// The caller may have asked to get the previous bits back.
// It is the callers responsibility to delete the HBITMAP that is returned.	
	if (bGetPrevious)
	{
		hPrevious = CreateCompatibleBitmap(
			virginMap, 
			rDstRepair.right  - rDstRepair.left,
			rDstRepair.bottom - rDstRepair.top);
		if (!hPrevious) return NULL;
		hMemoryDC = CreateCompatibleDC(virginMap);
    	if (!hMemoryDC) return hPrevious;

    	hOld = SelectObject(hMemoryDC, hPrevious);

		BitBlt(
			hMemoryDC, 
			0, 
			0, 
			rDstRepair.right  - rDstRepair.left, 
			rDstRepair.bottom - rDstRepair.top, 
			virginMap, 
			rSrcRepair.left, 
			rSrcRepair.top, 
			SRCCOPY);
			
    	SelectObject(hMemoryDC, hOld);
    	DeleteDC(hMemoryDC);
	}

// Blt the given bitmap into the virgin bitmap to repair it.
// What ever was there before will now be replaced with new bits from the repair bitmap.
// After all, this is the main purpose of this function.
    hMemoryDC = CreateCompatibleDC(virginMap);
    if (!hMemoryDC) return hPrevious;
    hOld = SelectObject(hMemoryDC, hBackBitmap);

	BitBlt(
		virginMap, 
		rDstRepair.left, 
		rDstRepair.top, 
		rDstRepair.right  - rDstRepair.left, 
		rDstRepair.bottom - rDstRepair.top, 
		hMemoryDC, 
		rSrcRepair.left, 
		rSrcRepair.top, 
		SRCCOPY);
		
    SelectObject(hMemoryDC, hOld);
    DeleteDC(hMemoryDC);
	
	return hPrevious;
}



void CAnimator::RepaintBackground()
{
// Repaint the virgin background bitmap on to the owner window.
	RECT	rWnd;
	HDC		hDC;
	
	hDC = GetDC(hBackWnd);
	GetClientRect(hBackWnd, &rWnd);
	
	BitBlt(
		hDC, 
		0, 
		0, 
		rWnd.right  - rWnd.left, 
		rWnd.bottom - rWnd.top, 
		virginMap, 
		0, 
		0, 
		SRCCOPY);

	ReleaseDC(hBackWnd, hDC);
}


CAnimateObj *CAnimator::FindAobject(int dAobject)    // find the one with this descriptor
{   
	CAnimateObj *newOne, *pChild;

	newOne = headA;
	while(newOne)
	{
		if (dAobject == newOne->GetDescriptor()) 
		    return newOne;
        if (pChild = newOne->FindChildObj(dAobject))
            return pChild;
		newOne = newOne->next;
	}
	return 0;
}

int CAnimator::GetChildObjectID(int dAobject, int Child)
{
    CAnimateObj *pParent = FindAobject(dAobject);
    int descriptor = 0;
    int i=1;

// Look for the child of this object
    if (pParent)
    {
        CAnimateObj *pChild = pParent->m_ChildHead;
        while (pChild)
        {
            if (i == Child)
            {
                descriptor = pChild->GetDescriptor();
                break;
            }
            pChild = pChild->next;
        }
    }
    return descriptor;
}



void CAnimator::StartAll()
{       
	CAnimateObj *newOne;

	stopped = FALSE;
	newOne = headA;
	while(newOne)
	{
		newOne->ResetTiming();
		newOne = newOne->next;
	}
}




int CAnimator::RegisterAobject(
HBITMAP hObjectBitmap, 
HBITMAP hMaskBitmap,
LPPOINT lpOffset)
{   
// link in a new animated object

// try to make a new one.

	CAnimateObj *prevOne;
	CAnimateObj *newOne = 0;

	newOne = new CAnimateObj(virginMap, hObjectBitmap, hMaskBitmap, lpOffset);
	if (!&newOne) return 0;

// link new object into list

	prevOne = headA;
	headA   = newOne;
	if (prevOne)                             // set up next and prev for "delete" function
	{       prevOne->prev = headA;
		newOne ->next = prevOne;
	}

	lastANum++;
	if (lastANum > LAST_DESCRIPTOR) 
	    lastANum = RESTART_DESCRIPTOR;    // wrap to beginning
	newOne->SetDescriptor(lastANum);         // remember who it is for later use

	return lastANum;
}



int CAnimator::RegisterAChild(
int dAobject, 
HBITMAP hObjectBitmap, 
HBITMAP hMaskBitmap,
LPPOINT lpOffset)
{
	CAnimateObj *thisOne, *newOne;
	thisOne = FindAobject(dAobject);                // look up the object
	if (!thisOne) return 0;

	newOne = new CAnimateObj(virginMap, hObjectBitmap, hMaskBitmap, lpOffset);
	if (!&newOne) return 0;

	lastANum++;
	if (lastANum > LAST_DESCRIPTOR) 
	    lastANum = RESTART_DESCRIPTOR;    // wrap to beginning
	newOne->SetDescriptor(lastANum);         // remember who it is for later use

    thisOne->AddChild(newOne);              // add a child object to this object

	return lastANum;
}





int  CAnimator::AddAobject(
int dAobject,
HBITMAP hObjectBitmap,
HBITMAP hMaskBitmap)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);                // look up the object
	if (!thisOne) return FALSE;

	return thisOne->AddAnimateObj(virginMap, hObjectBitmap, hMaskBitmap);
}



#ifndef USE_TIMERCOUNT

int far *lpTicks = (int far *)0x0040006C;	// address of system tickcount

static unsigned long highTics = 0;              // high 16 bits of ticcnt
static unsigned long lowTics  = 0;
static unsigned long lastHighTics  = 0;

unsigned long ticcnt()          // read PIT timer hardware ports directly
{
	int             lowByte  = 0;
	int             highByte = 0;
	unsigned long   lowWord  = 0;

//      disable();
//      outportb(0x43, 0);                                      // read command
//      lowByte  = inportb(0x40);                       // get low byte, then high byte
//      highByte = inportb(0x40);
//      highTics = peek(0x0040, 0x006C);        // address of tics stored in memory

	_outp(0x43, 0);                                 // read command
	lowByte  = inp(0x40);                   // get low byte, then high byte
	highByte = inp(0x40);

	highTics = *lpTicks;					// address of system tickcount

	lowWord = 0xFFFF - ((highByte << 8) | lowByte); // it counts down

	if ((lowWord < lowTics) && (lastHighTics == highTics) && bEnhanced)
	{						// enhanced mode does not update this var frequently enough
		highTics++;
		*lpTicks = highTics;                    // poke a corrected value back in
	}

//      enable();

	lowTics = lowWord;
	lastHighTics = highTics;
				
	return ((highTics << 16) | lowTics) >> 4;
}

#endif // ifndef USE_TIMERCOUNT




void CAnimator::PaintAllObjects()
{   
	CAnimateObj *newOne;

	newOne = headA;
	while(newOne)
	{
		newOne->Draw(hBackWnd);
		newOne = newOne->next;
	}
}


#ifndef WIN32
#include <toolhelp.h>
#endif

void CAnimator::MoveAllObjects()         // loop over the entire linked list
{   
	CAnimateObj		*newOne;
	unsigned long	thisTic;
	
// call ALL animated objects

	if (stopped) return;
	
#ifdef USE_TIMERCOUNT
 #ifdef WIN32
   #ifdef _MAC
    thisTic = GetTickCount();
   #else
	thisTic = timeGetTime();
   #endif
 #else
	TIMERINFO ti;
	ti.dwSize = sizeof(TIMERINFO);
	TimerCount(&ti);
	thisTic = ti.dwmsSinceStart;
 #endif
#else
	thisTic = ticcnt();                             // get the time this call was made
#endif

	newOne = headA;
	while(newOne)
	{
		newOne->CheckMoveAnimateObj(hBackWnd, thisTic);

// now check for dead ones.  hide and remove them
		if (newOne->killMe)
		{
//			FARPROC lpfnCallback;
			if (newOne->NotifyProc)
			{
				(*(newOne->NotifyProc))(newOne->GetDescriptor());
		     //           lpfnCallback = newOne->NotifyProc;
		     //           (*lpfnCallback)(newOne->GetDescriptor());
			}
			DeleteAobject(newOne->GetDescriptor());
			return;
		}
		newOne = newOne->next;
	}
}




void CAnimator::DeleteAobject(int dAobject)      // recursively delete end of the chain
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	if (thisOne->visible) thisOne->HideObject(hBackWnd);
	thisOne->Unlink();

	if (thisOne == headA) headA = thisOne->next;

	delete thisOne;
}
 

void CAnimator::StartTrack(int dAobject)         // set the object to track mouse
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);                // look up the object
	if (!thisOne) return;

	thisOne->SetupTrack(hBackWnd);                  // next "move" will track mouse
	thisOne->needsSpecial = I_TRACK;
	thisOne->moving = TRUE;
}



void CAnimator::StopTrack(int dAobject)          // stop tracking mouse
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->needsSpecial = 0;                              // stop tracking mouse
	thisOne->SetupNextInstruction();                // go to next instruction
}




void CAnimator::BeginCycle(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->InsertNewInstruction(I_BEGIN, 0, 0, 0);
}


void CAnimator::RepeatCycle(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->InsertNewInstruction(I_REPEAT, 0, 0, 0);
}


int CAnimator::GetNumObjects()           // tell how many there are
{
	CAnimateObj             *newOne;
	int NumObjects = 0;

	newOne = headA;
	while(newOne)
	{
		NumObjects++;
		newOne = newOne->next;
	}
	return NumObjects;
}


void CAnimator::ActivateFrame(
int dAobject, 
int nStartFrame, 
int nEndFrame, 
BOOL bActivate)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

    nEndFrame = max(nStartFrame, nEndFrame);
    for (int i=nStartFrame; i<nEndFrame+1; i++)
	    thisOne->InsertNewInstruction(I_ACTFRAME, i, bActivate, 0);
}


void CAnimator::ActivateChildFrame(
int dAobject, 
int Child, 
int nStartFrame, 
int nEndFrame, 
BOOL bActivate)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

    nEndFrame = max(nStartFrame, nEndFrame);
    for (int i=nStartFrame; i<nEndFrame+1; i++)
	    thisOne->InsertNewInstruction(I_ACTCHILDFRAME, Child, i, bActivate);
}



void CAnimator::SetFramesPerSec(int dAobject, int iFmsPerSec)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->InsertNewInstruction(I_CYCLESPEED, 0, 0, iFmsPerSec);
}



void CAnimator::SetChildFramesPerSec(int dAobject, int Child, int iFmsPerSec)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->InsertNewInstruction(I_CHILDCYCLESPEED, Child, 0, iFmsPerSec);
}


void CAnimator::SetFrameOrder(int dAobject, int frame, int order)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->InsertNewInstruction(I_FRAMEORDER, frame, order, 0);
}


void CAnimator::SetChildFrameOrder(int dAobject, int Child, int frame, int order)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->InsertNewInstruction(I_CHILDFRAMEORDER, Child, frame, order);
}




void CAnimator::SetFrameOffset(int dAobject, int frame, int x, int y)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->InsertNewInstruction(I_FRAMEOFFSET, frame, x, y);
}


void CAnimator::SetChildFrameOffset(int dAobject, int Child, int frame, int x, int y)
{
//	CAnimateObj *thisOne;
//
//	thisOne = FindAobject(dAobject);
//	if (!thisOne) return;
//
//	thisOne->InsertNewInstruction(I_CHILDFRAMEOFFSET, Child, x, y);
}


void CAnimator::SetAllFrameOffsets(int dAobject, LPPOINTS lpOffsets, int nPoints)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

    for (int i=0; i<nPoints; i++)
        thisOne->SetFrameOffset(i+1, lpOffsets[i].x, lpOffsets[i].y);

// Make sure all frames are normalized to 0.
    thisOne->NormalizeFrameOffsets();
}




void CAnimator::SetRepeatCycle(int dAobject, bool bRepeat)
{
// Will the object repeat frame cycling at the beginning or not.
// If this is FALSE, then frame cycling stops at the last active frame.
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->InsertNewInstruction(I_REPEATCYCLE, bRepeat, 0, 0);
}


void CAnimator::SetAutoRepeat(int dAobject, bool bAutoRepeat)
{
// Tell this object to start or stop repeating its instructions when they finish.
// When an object is AutoRepeating it will start back at the beginning of
// its instructions when it reaches the end of the list.

	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	thisOne->SetAutoRepeat(bAutoRepeat);
}




void CAnimator::SetNotification(int dAobject, WORD newMsg, int isCont, int newWhich)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->SetNotification(newMsg, isCont, newWhich);
}


void CAnimator::SetNotifyProc(int dAobject, NOTIFYPROC lpfnNotify)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->SetNotifyProc(lpfnNotify);
}


void CAnimator::SetPlaySoundProc(int dAobject, PLAYSOUNDPROC lpfnSoundCallback)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->SetPlaySoundProc(lpfnSoundCallback);
}


void CAnimator::SetPlaySound(int dAobject, LPSTR lpSound, BOOL bLoopSound,
  int iChannel)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->SetPlaySound(lpSound, bLoopSound, iChannel);
}


void CAnimator::PlayAllSounds()
{
// Have each of the animation objects play their sound.

	CAnimateObj* pAnimateObj;

	pAnimateObj = headA;
	while (pAnimateObj)
	{
		pAnimateObj->PlaySound();
		pAnimateObj = pAnimateObj->next;
	}
}



void CAnimator::SetSpeed(int dAobject, int newSpeed)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_SPEED, 0, 0, newSpeed);
}


int  CAnimator::GetSpeed(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return 0;
	return thisOne->GetSpeed();
}



void CAnimator::IncSpeed(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_INCSPEED, 0, 0, 0);
}



void CAnimator::DecSpeed(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_DECSPEED, 0, 0, 0);
}



void CAnimator::SetCurrentFrame(int dAobject, int nFrame)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_SETFRAME, 0, 0, nFrame);
}


void CAnimator::SetCurrentChildFrame(int dAobject, int nChild, int nFrame)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_SETCHILDFRAME, nChild, nFrame, 0);
}


void CAnimator::Pause(int dAobject, int time)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_PAUSE, 0, 0, time);
}


void CAnimator::WaitLastFrame(int dAobject)
{
// Wait for the last active frame to appear.
// Works just like a pause.
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_WAITLASTFRAME, 0, 0, 0);
	thisOne->InsertNewInstruction(I_PAUSE, 0, 0, 2);	
	// pause .1 seconds.  Give this frame some time to display
}





void CAnimator::Kill(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_KILL, 0, 0, 0);
}






void CAnimator::Move(int dAobject, int X, int Y)         // instr to move to point
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_MOVE, X, Y, 0);
	thisOne->InsertNewInstruction(I_JUMP, X, Y, 0); // guaranteed ending
}



void CAnimator::MovePoints(
int dAobject, 
int nPoints, 
LPPOINT lpPoints)          // list of points to move to
{
	CAnimateObj *thisOne;
	int             i;

	if (nPoints <= 0) return;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;

	for (i=0; i<nPoints; i++)
	{
		thisOne->InsertNewInstruction(I_MOVE, lpPoints[i].x, lpPoints[i].y, 0);
	}
	thisOne->InsertNewInstruction(I_JUMP, lpPoints[nPoints-1].x, lpPoints[nPoints-1].y, 0);
}



void CAnimator::Jump(int dAobject, int X, int Y)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_JUMP, X, Y, 0);
}




void CAnimator::Hide(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_HIDE, 0, 0, 0);
}





void CAnimator::Show(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_SHOW, 0, 0, 0);
}




void CAnimator::FlipVertical(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_FVERT, 0, 0, 0);
}


void CAnimator::FlipHorizontal(int dAobject)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->InsertNewInstruction(I_FHORZ, 0, 0, 0);
}



void CAnimator::SetTrackLimit(int dAobject, int limit)
{
	CAnimateObj *thisOne;

	thisOne = FindAobject(dAobject);
	if (!thisOne) return;
	thisOne->SetTrackLimit(limit);
}


int  CAnimator::MouseInAnyObject()
{   
	CAnimateObj *newOne;
	POINT   pt;

	GetCursorPos(&pt);                                      // the current pos
	ScreenToClient(hBackWnd, &pt);

	newOne = headA;
	while(newOne)
	{
		if (PtInRect(&newOne->newrect, pt)) return newOne->descriptor;
		newOne = newOne->next;
	}

	return 0;
}







// -------------------------- Standard C interface ----------------------

extern "C"                                                                      // make available for C routines
{

typedef void FAR *LPAOBJECT;

LPAOBJECT FAR PASCAL ACreateNewAnimator()               // master animator object
{
	CAnimator *newOne;

	newOne = new CAnimator();                       // return the pointer
	return (LPAOBJECT)newOne;                       // use this pointer when doing stuff
}

//--------------------------------------------------------------------------

void FAR PASCAL ADeleteAnimator(LPAOBJECT theObject)
{
    CAnimator *thisOne;

	if ( !theObject )
		return;
	thisOne = (CAnimator *)theObject;
	delete thisOne;
}



//--------------------------------------------------------------------------

int  FAR PASCAL ARegisterAobject( LPAOBJECT A,
			HBITMAP hObjectBitmap, HBITMAP hMaskBitmap, LPPOINT lpOffset)
{
	if (!A) return 0;
	return ((CAnimator *)A)->RegisterAobject(hObjectBitmap, hMaskBitmap,
		lpOffset);
}


//--------------------------------------------------------------------------

int  FAR PASCAL ARegisterAChild( LPAOBJECT A, int dAobject,
			HBITMAP hObjectBitmap, HBITMAP hMaskBitmap, LPPOINT lpOffset)
{
	if (!A) return 0;
	return ((CAnimator *)A)->RegisterAChild(dAobject, hObjectBitmap, hMaskBitmap,
		lpOffset);
}


//--------------------------------------------------------------------------


int  FAR PASCAL AAddAobject(LPAOBJECT A, int dAobject,
		HBITMAP hObjectBitmap, HBITMAP hMaskBitmap)
{
	if (!A) return 0;
	return ((CAnimator *)A)->AddAobject(dAobject, hObjectBitmap, hMaskBitmap);
}






//--------------------------------------------------------------------------
void FAR PASCAL ACopyBackgroundScreen(LPAOBJECT A, HWND hWnd, LPRECT lpClipRect)
{
	if (!A) return;
	((CAnimator *)A)->CopyBackgroundScreen(hWnd, lpClipRect);
}


//--------------------------------------------------------------------------
void FAR PASCAL AGiveBackgroundBitmap(LPAOBJECT A, HWND hWnd, HBITMAP hNewBitmap, BOOL bTile)
{
	if (!A) return;
	((CAnimator *)A)->GiveBackgroundBitmap(hWnd, hNewBitmap, bTile);
}


//--------------------------------------------------------------------------
void FAR PASCAL AMoveAllObjects(LPAOBJECT A)
{
	if (!A) return;
	((CAnimator *)A)->MoveAllObjects();
}


//--------------------------------------------------------------------------
void FAR PASCAL ADeleteAllObjects(LPAOBJECT A)
{
	if (!A) return;
	((CAnimator *)A)->DeleteAllObjects();
}


//--------------------------------------------------------------------------
void FAR PASCAL APaintAllObjects(LPAOBJECT A)
{
	if (!A) return;
	((CAnimator *)A)->PaintAllObjects();
}


//--------------------------------------------------------------------------
void FAR PASCAL AMove(LPAOBJECT A, int dAobject, int X, int Y)
{
	if (!A) return;
	((CAnimator *)A)->Move(dAobject, X, Y);
}

//--------------------------------------------------------------------------
void FAR PASCAL ADeleteAobject(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->DeleteAobject(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL ADeleteInstructions(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->DeleteInstructions(dAobject);
}


//--------------------------------------------------------------------------
void FAR PASCAL AJump(LPAOBJECT A, int dAobject, int X, int Y)
{
	if (!A) return;
	((CAnimator *)A)->Jump(dAobject, X, Y);
}

//--------------------------------------------------------------------------
void FAR PASCAL APause(LPAOBJECT A, int dAobject, int time)
{
	if (!A) return;
	((CAnimator *)A)->Pause(dAobject, time);
}

//--------------------------------------------------------------------------
void FAR PASCAL AHide(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->Hide(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL AShow(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->Show(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL AKill(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->Kill(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL ABeginCycle(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->BeginCycle(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL ARepeatCycle(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->RepeatCycle(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL AStartTrack(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->StartTrack(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL AStopTrack(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->StopTrack(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL ASetSpeed(LPAOBJECT A, int dAobject, int theSpeed)
{
	if (!A) return;
	((CAnimator *)A)->SetSpeed(dAobject, theSpeed);
}

//--------------------------------------------------------------------------
int  FAR PASCAL AGetSpeed(LPAOBJECT A, int dAobject)
{
	if (!A) return 0;
	return ((CAnimator *)A)->GetSpeed(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL AIncSpeed(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->IncSpeed(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL ADecSpeed(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->DecSpeed(dAobject);
}

//--------------------------------------------------------------------------
void FAR PASCAL AStopAll(LPAOBJECT A)
{
	if (!A) return;
	((CAnimator *)A)->StopAll();
}

//--------------------------------------------------------------------------
void FAR PASCAL AStartAll(LPAOBJECT A)
{
	if (!A) return;
	((CAnimator *)A)->StartAll();
}

//--------------------------------------------------------------------------
BOOL FAR PASCAL AWhere(LPAOBJECT A, int dAobject, LPRECT lpRect)
{
	if (!A) return(FALSE);
	return( ((CAnimator *)A)->Where(dAobject, lpRect) );
}


//-------------------------------------------------------------------------
void FAR PASCAL ASetNotification(LPAOBJECT A,int dAobject, WORD newMsg,
									int isCont, int newWhich)
{
	if (!A) return;
	((CAnimator *)A)->SetNotification(dAobject, newMsg, isCont, newWhich);
}


//-------------------------------------------------------------------------
void FAR PASCAL ASetNotifyProc(LPAOBJECT A,int dAobject, NOTIFYPROC lpfnNotify)
{
	if (!A) return;
	((CAnimator *)A)->SetNotifyProc(dAobject, lpfnNotify);
}


//-------------------------------------------------------------------------
void FAR PASCAL ASetPlaySoundProc(LPAOBJECT A,int dAobject, PLAYSOUNDPROC lpfnSoundCallback)
{
	if (!A) return;
	((CAnimator *)A)->SetPlaySoundProc(dAobject, lpfnSoundCallback);
}


//-------------------------------------------------------------------------
void FAR PASCAL ASetPlaySound(LPAOBJECT A,int dAobject, LPSTR lpSound,
  BOOL bLoopSound, int iChannel)
{
	if (!A) return;
	((CAnimator *)A)->SetPlaySound(dAobject, lpSound, bLoopSound, iChannel);
}

//-------------------------------------------------------------------------
int  FAR PASCAL AGetStopped(LPAOBJECT A)
{
	if (!A) return 0;
	return ((CAnimator *)A)->GetStopped();
}


//-------------------------------------------------------------------------
void FAR PASCAL ASetTrackLimit(LPAOBJECT A, int dAobject, int limit)
{
	if (!A) return;
	((CAnimator *)A)->SetTrackLimit(dAobject, limit);
}


//-------------------------------------------------------------------------
int  FAR PASCAL AMouseInAnyObject(LPAOBJECT A)
{
	if (!A) return 0;
	return ((CAnimator *)A)->MouseInAnyObject();
}


//-------------------------------------------------------------------------
bool FAR PASCAL AExecuteScript(
	LPAOBJECT	A,
	int 	dAobject, 
	char 	far *lpScript,
	char 	far *lpError,
	int 	nErrorBytes)
{
	if (!A) return 0;
	return ((CAnimator *)A)->ExecuteScriptSetup(
						dAobject, 
						lpScript,
						lpError,
						nErrorBytes);
}


//-------------------------------------------------------------------------
bool FAR PASCAL AReExecuteScript(LPAOBJECT A, int dAobject)
{
	if (!A) return 0;
	((CAnimator *)A)->ReExecuteScript(dAobject);
    return TRUE;
}


//-------------------------------------------------------------------------
void FAR PASCAL ASetCurrentScript(LPAOBJECT A, int dAobject, char far *lpScript)
{
	if (!A) return;
	((CAnimator *)A)->SetCurrentScript(dAobject, lpScript);
}


//-------------------------------------------------------------------------
void FAR PASCAL ASetAutoRepeat(LPAOBJECT A, int dAobject, bool bAutoRepeat)
{
	if (!A) return;
	((CAnimator *)A)->SetAutoRepeat(dAobject, bAutoRepeat);
}



//-------------------------------------------------------------------------
void FAR PASCAL AFlipVertical(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->FlipVertical(dAobject);
}

//-------------------------------------------------------------------------
void FAR PASCAL AFlipHorizontal(LPAOBJECT A, int dAobject)
{
	if (!A) return;
	((CAnimator *)A)->FlipHorizontal(dAobject);
}


//-------------------------------------------------------------------------
void FAR PASCAL AActivateFrame(
LPAOBJECT A, 
int dAobject, 
int nFrame, 
BOOL bActivate)
{
	if (!A) return;
	((CAnimator *)A)->ActivateFrame(dAobject, nFrame, nFrame, bActivate);
}


//-------------------------------------------------------------------------
void FAR PASCAL AActivateChildFrame(
LPAOBJECT A, 
int dAobject, 
int Child, 
int nFrame, 
BOOL bActivate)
{
	if (!A) return;
	((CAnimator *)A)->ActivateChildFrame(dAobject, nFrame, nFrame, Child, bActivate);
}

//-------------------------------------------------------------------------

void FAR PASCAL ASetFramesPerSec(LPAOBJECT A, int dAobject, int iFmsPerSec)
{
	if (!A) return;
	((CAnimator *)A)->SetFramesPerSec(dAobject, iFmsPerSec);
}


//-------------------------------------------------------------------------

void FAR PASCAL ASetChildFramesPerSec(LPAOBJECT A, int dAobject, int Child, int iFmsPerSec)
{
	if (!A) return;
	((CAnimator *)A)->SetChildFramesPerSec(dAobject, Child, iFmsPerSec);
}


//-------------------------------------------------------------------------

void FAR PASCAL ASetFrameOrder(LPAOBJECT A, int dAobject, int frame, int order)
{
	if (!A) return;
	((CAnimator *)A)->SetFrameOrder(dAobject, frame, order);
}


//-------------------------------------------------------------------------

void FAR PASCAL ASetChildFrameOrder(LPAOBJECT A, int dAobject, int Child, int frame, int order)
{
	if (!A) return;
	((CAnimator *)A)->SetChildFrameOrder(dAobject, Child, frame, order);
}

//-------------------------------------------------------------------------

void FAR PASCAL ASetFrameOffset(LPAOBJECT A, int dAobject, int frame, int x, int y)
{
	if (!A) return;
	((CAnimator *)A)->SetFrameOffset(dAobject, frame, x, y);
}



//-------------------------------------------------------------------------

void FAR PASCAL ASetChildFrameOffset(LPAOBJECT A, int dAobject, int Child, int frame, int x, int y)
{
	if (!A) return;
	((CAnimator *)A)->SetChildFrameOffset(dAobject, Child, frame, x, y);
}


//-------------------------------------------------------------------------

void FAR PASCAL ASetAllFrameOffsets(LPAOBJECT A, int dAobject, LPPOINTS lpOffsets, int nPoints)
{
	if (!A) return;                      
	((CAnimator *)A)->SetAllFrameOffsets(dAobject, lpOffsets, nPoints);
}


HBITMAP FAR PASCAL AMergeToBackground(HBITMAP hBackBitmap, HBITMAP hObjectBitmap)
{
    return CAnimateObj::MergeToBackground(hBackBitmap, hObjectBitmap);
}

HBITMAP FAR PASCAL ARepairBackgroundBitmap(
LPAOBJECT A, 
HBITMAP hBackBitmap, 
RECT rSrcRepair, 
RECT rDstRepair,
bool bGetPrevious)
{
	if (!A) return NULL;                      
	return ((CAnimator *)A)->RepairBackgroundBitmap(
		hBackBitmap, 
		rSrcRepair, 
		rDstRepair, 
		bGetPrevious);
}

void FAR PASCAL ARepaintBackground(LPAOBJECT A)
{
	if (!A) return;
	((CAnimator *)A)->RepaintBackground();
}

void FAR PASCAL APlayAllSounds(LPAOBJECT A)
{
	if (!A) return;
	((CAnimator *)A)->PlayAllSounds();
}


// extern "C"
}

