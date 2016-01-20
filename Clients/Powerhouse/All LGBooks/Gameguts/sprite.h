#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "offscren.h"

class FAR CToon;

typedef struct _cell far * LPCELL;
typedef struct _cell
{
	LPCELL		next;			// chain all the cells of animation together
	PDIB		pSpriteDib;
	BOOL		bActive;		// to activate/deactivate cells
	int			xOffset;		// offset from 0, 0 to draw cell
	int			yOffset;
	int			iWidth;			// Each cell can have a different size
	int			iHeight;
} CELL;

typedef struct _action far * LPACTION;
typedef struct _action
{
	LPACTION	next;			// next one in the chain of Sprite actions
	BYTE		cAction;			// what is the sprite supposed to do
	int			x;				// X, Y point to head for next
	int			y;
	int			iParam;			// the command parameter
} ACTION;

//************************************************************************
// The CSprite class definition
//************************************************************************
// ENUMERATION CONSTANTS
typedef enum
{ 
	SN_DESTROY,
	SN_MOVED,
	SN_MOVEDONE,
	SN_PAUSEAFTER,
	SN_PAUSEDONE
} SPRITE_NOTIFY;


class far CSprite;
typedef class CSprite far * LPSPRITE;
typedef void (CALLBACK* NOTIFYPROC)( LPSPRITE, SPRITE_NOTIFY Notify, DWORD dwNotifyData );
typedef void (CALLBACK* PLAYSOUNDPROC)( LPSPRITE, LPSTR, BOOL, int );

#define ANIMATOR_TIMER_ID 0xDEAD
#define PAUSE_FOREVER 9999

class far CAnimator
{
public:
	CAnimator(HWND hBackWnd, LPOFFSCREEN lpOffScreen);
	CAnimator(CToon FAR* pToon);
	~CAnimator();

	// implementation
	void DeleteAll(void);
	void StartAll(void);
	void PaintAll(void);
	void HideAll(void);
	void DrawRect( LPRECT lpRect );
	void TimeSlice(void);
	int GetNumSprites(void);
	void PlayAllSounds(void);
	LPSPRITE MouseInAnySprite(LPPOINT lpMousePt = NULL, BOOL fCheckTransparent = FALSE);
	void SetWindow( HWND hWnd = NULL, LPOFFSCREEN lpOffScreen = NULL );
	BOOL GetStopped(void);
	void StopAll(void);
	LPSPRITE GetHeadSprite(void) { return m_pHeadSprite; }
	void AdjustTime(DWORD dwTimeDelta);
	LPSPRITE CreateSprite( LPPOINT lpOrigin, DWORD dwData = 0, BOOL fAddToTop = FALSE);
	void AddSprite(LPSPRITE pSprite, BOOL fAddToTop = FALSE);
	void DeleteSprite(LPSPRITE pSprite);
	void OrderSprite(LPSPRITE pSprite, BOOL fTop);
	void Unlink(LPSPRITE pSprite);
	void SetClipRect(LPRECT lpClipRect = NULL);
	BOOL CheckCollision(LPSPRITE lpSprite1, LPSPRITE lpSprite2, LPRECT lpRect1 = NULL, LPRECT lpRect2 = NULL);

protected:
	void Init();

private:
	HWND		m_hBackWnd;			// the window for these sprites
	LPOFFSCREEN m_lpOffScreen;		// the window for these sprites
	CToon FAR*	m_pToon;			// the toon pointer to display these sprites
	LPSPRITE	m_pHeadSprite;		// first in a linked list of sprites
	BOOL		m_bFullStop;		// to stop all sprites from moving
	RECT		m_rClip;			// clipping rectangle
public:
//	DWORD		m_dwCurrentTicks;	// the timeslice time
};
typedef class CAnimator far * LPANIMATOR;

class far CSprite // : public BaseClass
{
	friend class CAnimator;

// CONSTRUCTORS
	public:
		enum CycleDirectionType
		{ 
			CycleForward,
			CycleBackward
		};
		CSprite( LPANIMATOR pAnimator, LPPOINT lpOrigin, DWORD dwData = 0 );

// DESTRUCTOR
	public:
		~CSprite(void);

// CLASS ATTRIBUTES
// OBJECT ATTRIBUTES
	private:
		LPSPRITE next;				// all sprites are chained together
		LPSPRITE prev;

		LPANIMATOR m_pAnimator;		// the animator class pointer 

		// POSITIONING
		int xOrigin;				// the origin is added to each instruction
		int yOrigin;
		int xLocation;				// where is the sprite now?
		int yLocation;
		int xLastDrawn;				// where was the sprite last drawn?
		int yLastDrawn;

		BOOL bVisible;
		BOOL bKill;					// set this and it will die after use

		// MOVEMENT
		BOOL bMoving;				// flag to tell if the sprite is in motion
		BOOL bMoveIncrement;		// flag to tell if the sprite just moved
		int xMoveDest;				// where does the sprite want to move?
		int yMoveDest;
		LFIXED fxMoveRate;
		LFIXED fyMoveRate;
		LFIXED fxMoveLocation;
		LFIXED fyMoveLocation;
		DWORD dwMoveLastTicks;
		DWORD dwMoveEndTicks;

		// TIMING
		int iSpeed;					// speed measured in pixels per second
		DWORD dwPauseTicks;
		LPCELL lpPauseAfterCell;
		int	iPauseAfterCellTenths;

		// CELLS
		LPCELL lpCellHead;			// first cell in the list
		LPCELL lpCellCurrent;		// the current cell in the list
		LPCELL lpCellLastDrawn;		// the last cell drawn on the screen

		// CELL CYCLING
		DWORD dwCycleTimeLast;		// Last time the cell was cycled
		DWORD dwCycleTime;			// milliseconds between each cycle
		BOOL bCycleRepeat;			// Loop back to first cell when last one is displayed
		BOOL bCycleBack;			// Cycle back to first cell when last on is displayed
		CycleDirectionType CycleDirection; // CycleForward or CycleBackward

		// ACTIONS & SCRIPTS
		LPACTION lpActionHead;		// pointer to actions to execute
		LPACTION lpActionCurrent;
		LPSTR lpScript;				// Script to run actions

		// NOTIFICATION
		NOTIFYPROC lpNotifyProc;	// another way to notify the owner
		DWORD		m_dwNotifyData; // data to pass to notify proc

		private:
		// SOUND
		PLAYSOUNDPROC lpSoundProc;	// tell when to play a sound
		STRING szSoundName;			// The sound to play for this sprite
		BOOL bSoundLoop;			// Should the sound keep looping
		int bSoundChannel;			// sound channel #

		// USER DATA
		DWORD	m_dwData;			// data to be used external to sprite engine

		int m_nId;					// Sprite ID (used in debugging)
		
// OBJECT FUNCTIONS
	public:
		BOOL IsVisible() { return bVisible; }
		BOOL MouseInSprite(LPPOINT lpMousePt, BOOL fCheckTransparent = FALSE, LPRECT lpWhereRect = NULL);
		void GetMaxSize(LPINT lpWidth, LPINT lpHeight);
		LPSPRITE GetNextSprite() { return next; }
		LPCELL GetCurrentCell() { return lpCellCurrent; }
		LPCELL GetHeadCell() { return lpCellHead; }
		void SetNotifyProc(NOTIFYPROC lpfnNotify, DWORD dwNotifyData);
		void PlaySound( void );
		void SetPlaySoundProc(PLAYSOUNDPROC lpfnSoundCallback);
		void SetPlaySound(LPSTR lpSound, BOOL bLoopSound, int iChannel);
		BOOL SetSpeed(int newSpeed);
		LPCELL AddCell( PDIB pSpriteDib, int xOffset = 0, int yOffset = 0 );
		LPCELL AddCell( LPCTSTR lpFileName, HPALETTE hPal = NULL, int xOffset = 0, int yOffset = 0, BOOL fDecodeRLE = FALSE );
		BOOL AddCells( PDIB pSpriteDib, int nCells, LPPOINT lpOffsets = NULL);
		BOOL AddCells( LPCTSTR lpFileName, int nCells, HPALETTE hPal = NULL,  LPPOINT lpOffsets = NULL, BOOL fDecodeRLE = FALSE );
		void Draw(void);
		BOOL Show( BOOL bShow );
		BOOL SetCellsPerSec(int iFmsPerSec);
		BOOL ActivateCell(int iCell, BOOL bActivate);
		void ActivateCells( int nStartCell, int nEndCell, BOOL bActivate);
		BOOL SetCurrentCell(int nCell);
		BOOL ReverseCellOrder( void );
		BOOL SetCellOrder(int iCell, int iOrder);
		void SetAllCellOffsets(LPPOINTS lpOffsets, int nPoints);
		void SetCellOffset(int iCell, int x, int y);
		void SetCycleRepeat(BOOL bRepeat);
		void SetCycleBack(BOOL bBack);
		BOOL CycleCells(void);
		void ReExecuteScript(void);
		BOOL ExecuteScript( LPSTR lpScript, LPSTR lpError, int nErrorBytes );
		void Location( LPRECT lpRect );
		void LastDrawn( LPRECT lpRect );
		BOOL Moving( void );
		BOOL Move(int newX, int newY);
		BOOL Jump(int newX, int newY);
		BOOL Pause( int iTecnths );
		BOOL PauseAfterCell( int iCell, int iTenths = PAUSE_FOREVER );
		void ClearPauseAfterCell();
		void Kill();
		void AddCmdMove(int X, int Y);
		void AddCmdBegin(void);
		void AddCmdRepeat(void);
		void AddCmdActivateCells( int nStartCell, int nEndCell, BOOL bActivate);
		void AddCmdSetCellsPerSec(int iFmsPerSec);
		void AddCmdReverseCellOrder( void );
		void AddCmdSetCellOrder(int cell, int order);
		void AddCmdSetRepeatCycle(BOOL bRepeat);
		void AddCmdSetSpeed(int newSpeed);
		void AddCmdIncSpeed(void);
		void AddCmdDecSpeed(void);
		void AddCmdSetCurrentCell(int nCell);
		void AddCmdPause(int tenths);
		void AddCmdPauseAfterCell( int iCell, int tenths = PAUSE_FOREVER );
		void AddCmdKill(void);
		void AddCmdMovePoints( int nPoints, LPPOINT lpPoints );
		void AddCmdJump(int X, int Y);
		void AddCmdHide(void);
		void AddCmdShow(void);
		void AddCmdFlipVertical(void);
		void AddCmdFlipHorizontal(void);
		void SetData(DWORD dwData) { m_dwData = dwData; }
		DWORD GetData() { return m_dwData; }
		DWORD GetNotifyData() { return m_dwNotifyData; }
		void DeleteAllActions( void );
		BOOL FirstCellDrawn();
		BOOL LastCellDrawn();
		void SetId(int nId) { m_nId = nId; }
		
	private:
		void DeleteAllCells( void );
		void Unlink( void );
		BOOL MoveIncrement( void );
		BOOL Repeat( void );
		void Action( BOOL bMoved );
		BOOL ReachedDestination( void );
		BOOL CycleAllCells(void);
		LPCELL FindCellNum(int iCell);
		int GetCellNum( LPCELL pTheCell );
		BOOL PerformNextAction( void );
		BOOL PerformCurrentAction( void );
		void InsertNewAction(BYTE newAction, int X, int Y, int newParam);
		LPCELL GetPreviousCell(LPCELL lpCell);
};

//************************************************************************
// The DEFINITION and/or INITIALIZATION of static members
//************************************************************************

enum INSTR
{
	I_MOVE = 1,				// Move to a screen position
	I_HIDE,					// Stop showing
	I_SHOW,					// Start showing
	I_JUMP,					// Go to a position on the screen
	I_STOP,					// Stop bMoving
	I_START,				// Begin bMoving again
	I_PAUSE,				// Pause for a specified period time (in tenths)
	I_KILL,					// Kill the whole sprite when done
	I_FVERT,				// Flip the sprites and masks upside down
	I_FHORZ,				// Flip left to right
	I_SETCELLSPERSEC,		// Cells per second cycling of animation sprites
	I_SETSPEED,				// Speed of movement
	I_INCSPEED,				// Up 1
	I_DECSPEED,				// Down 1
	I_ACTCELL,				// Activate a cell to cycle through
	I_DEACTCELL,			// Deactivate a cell to
	I_REVERSE_CELLORDER,	// Reverse the cell order
	I_CELLORDER,			// Set the order that cells cycle though
	I_SETCELL,				// Set the cell to begin using
	I_BEGIN,				// Start of a repeat cycle
	I_REPEAT,				// Loop again
	I_REPEATCYCLE,			// Loop back when last cell is played
	I_PAUSEAFTERCELL		// Pause after the desired cell is drawn
};

#endif
