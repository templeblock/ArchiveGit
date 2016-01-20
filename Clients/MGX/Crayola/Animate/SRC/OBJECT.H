/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#ifndef _OBJECT_H_
#define _OBJECT_H_

enum INSTR
{
    I_ERROR     = -2,		// an error occured in the script
    I_MOVE      =  1,		// move to a screen position
    I_HIDE,		            // stop showing
    I_SHOW,		            // start showing
    I_JUMP,		            // go to a position on the screen
    I_SPEED,		        // change speeds
    I_STOP,		            // stop moving
    I_START,		        // begin moving again
    I_PAUSE,		        // pause for a specified period of time
    I_BEGIN,		        // start of a repeat cycle
    I_REPEAT,		        // loop again
    I_TRACK,		        // tracking an object by the mouse
    I_STOPTRACK,		    // tracking an object by the mouse
    I_KILL,		            // kill the whole object when done
    I_FVERT,		        // flip the objects and masks upside down
    I_FHORZ,		        // flip left to right
    I_SETLIMIT,		        // limit the axes for mouse tracking
    I_SETSPEED,		        // speed of movement
    I_INCSPEED,		        // up 1
    I_DECSPEED,		        // down 1
    I_CYCLESPEED,           // frames per second cycling of animation objects    
    I_CHILDCYCLESPEED,      // frames per second cycling of children    
    I_ACTFRAME,             // activate a frame to cycle through
    I_DEACTFRAME,           // deactivate a frame to 
    I_ACTCHILDFRAME,        // activate a child frame
    I_DEACTCHILDFRAME,      // deactivate child frame
    I_FRAMEORDER,           // set the order that frames cylce though
    I_CHILDFRAMEORDER,      // set the order for children
    I_FRAMEOFFSET,          // offset from 0,0 to draw frame
    I_CHILDFRAMEOFFSET,     // offset to draw child frames
    I_SETFRAME,             // Set the frame to begin using
    I_SETCHILDFRAME,        // Set the Child frame to begin using
    I_JUMPCHILD,            // Relocate a child animation
    I_REPEATCYCLE,          // Loop back when last frame is played
    I_WAITLASTFRAME         // Wait for the last active frame, works just like Pause
};     





#define maxBits     50		// maximum movement of an object
#define MAX_SOUND_BUF	30
 
// ------------------------Objects to move---------------------------

class BitObject;
class Instr;


class CAnimateObj                  // a single object to be moved on screen
{   friend class CAnimator;


public:
	CAnimateObj(HDC hTypeDC, HBITMAP hObjectBitmap, HBITMAP hMaskBitmap,
		LPPOINT lpOffset);
	~CAnimateObj();

private:
	HDC         virginMap;         // virgin copy of the backgroung screen
	CAnimateObj *next;             // all these objects chained together
	CAnimateObj *prev;
	CAnimateObj *m_ChildHead;      // Child animation objects
	CAnimateObj *m_Parent;         // The parent of this object

	Instr   *headI;                // pointer to instructions to execute
	Instr   *currentI;

	BitObject *headB;              // first bitmap to move on the scree
	BitObject *currentB;

	int    Owidth;                 // size of the bitmaps
	int    Oheight;
	int    Mwidth;
	int    Mheight;

	int    xInc;                   // +/- for X direction
	int    yInc;
	float  movesPerInc;    		   // how many moves before incrementing short axis
	float  movesTilNextInc;        // look ahead to next time we will inc short side
	int    incsSoFar;              // how many moves made so far on short axis
	int    axisShort;              // which one is the "short" axis

	int    xOffset;                // add to the blt calls
	int    yOffset;
	int    xPos;                   // start xPos;
	int    yPos;
	int    xLast;                  // mouse point for tracking object
	int    yLast;
    POINT  ptParent;               // Offset all moves from parent

	int    speed;                  // how fast the object moves
	RECT   oldrect;                // last position on the screen
	RECT   newrect;                // current rect position

	int    movesSoFar;             // incremented as we move along
	int    ticsPerMove;            // how many tics before we must move a pixel

	int    moving;                 // flag to tell if the object is in motion
	int    visible;
	int    needsSpecial;           // tells when the object needs to be cleared
	int    inBeginCycle;           // begin repeat cycle doing it over and over again
	int	   m_bAutoRepeat;          // repeat the instructions when they are finished.
	int    killMe;                 // set this and it will die afer use

	WORD   wNmessage;              // the message to send back to the window if collision
	int    continuous;             // keep sending or stop first time
	int    Awhich;                 // which object to collide with
	int    lastContObject;         // last object hit while in continous mode - one message per object

	NOTIFYPROC NotifyProc;         // another way to notify the owner
	PLAYSOUNDPROC m_lpfnSoundCallback;	// tell when to play a sound

	unsigned long   firstTiccnt;
	int             descriptor;    // unique identifier to tell it to do stuff
	int             trackLimit;    // which axis is track

    unsigned long   m_lLastCycleTime;  // Last time the bitmap was cylced
    unsigned long   m_lCycleTime;       // milliseconds between each cycle

    HGLOBAL     m_hScript;          // Script to run instructions
    bool        m_bRepeatCycle;     // Loop back to first frame when last one is displayed
	
	char		m_sSound[MAX_SOUND_BUF];	// The sound to play for this object
	BOOL 		m_bLoopSound;		// Should the sound keep looping
	int			m_iChannel;			// sound channel #

public:
    static HBITMAP MergeToBackground(HBITMAP hBackBitmap, HBITMAP hObjectBitmap);

private:
	static HBITMAP MakeBitmapMono( HBITMAP hBMobject );
	static void MaskBitmap(HBITMAP hObject, HBITMAP hMask);
	static HBITMAP CopyBitmap(HDC hDC, HBITMAP hBitmap);

    void AddChild(CAnimateObj *pChildObj);
	void CheckMoveAnimateObj(HWND hWnd, unsigned long theTime);
    void CheckMoveChildren(HWND hWnd, unsigned long theTime);
    BOOL CheckSpecialInstructions(HWND hWnd, unsigned long theTime);
	void IncrementObjectPosition(int times);
	void Draw(HWND hWnd);
	void DrawChildren(HWND hWnd);
	int  AddAnimateObj(HDC hTypeDC, HBITMAP hObjectBitmap, HBITMAP hMaskBitmap);
	int  GetDescriptor()                    { return descriptor;    };
	void SetDescriptor(int theDesc)         { descriptor = theDesc; };
    void OffsetObjRect(POINT pt);
    void OffsetChildren();
    void SetChildrenParentOffsets();
    void SetParentOffset(POINT pt);
    void ResetChildrenPos();
    void ResetObjRect(POINT pt);
    CAnimateObj *FindChildObj(int dAobject);
    void ChildToScreen(LPRECT lpRect, LPRECT lpStart);

	void SetupMoveTo(int newX, int newY);
	void SetupJump  (int newX, int newY);
	void SetupTrack(HWND hBackWnd);
	void SetupRepeat();
	void SetupPause(int PauseTime);
	void ContinuePausing(unsigned long theTime);

	void SetSpeed(int newSpeed);
	int  GetSpeed() { return speed; };
	void ResetTiming();

    CAnimateObj* FindChild(int Child);
    void SetFramesPerSec(int iFmsPerSec);
    void SetChildFramesPerSec(int Child, int iFmsPerSec);
    BOOL CheckCycleFrames(unsigned long theTime);
    void SetFrameOrder(int frame, int order);
    void SetChildFrameOrder(int Child, int frame, int order);
    void ActivateFrame(int frame, bool bActivate);              
    void ActivateChildFrame(int Child, int frame, bool bActivate);              
    void SetCurrentFrame(int nFrame);
    void SetFrameOffset(int iFrame, int x, int y);
    void NormalizeFrameOffsets();
    void SetChildFrameOffset(int Child, int iFrame, int x, int y);
    BitObject* FindFrameNum(int frame);

	void HideObject(HWND hWnd);
	void ShowObject(HWND hWnd);
    void HideChildren(HWND hWnd);
    void ShowChildren(HWND hWnd);
	void JumpObject(HWND hWnd);
	void TrackMouse(HWND hWnd);
	void SetTrackLimit(int t) {trackLimit = t; };
	int  CheckDestination();

	void SetupThisInstruction();
	void SetupNextInstruction();
	void InsertNewInstruction(BYTE newInstr, int X, int Y, int newParam);

	void DeleteAllInstr();
	void DeleteInstr(Instr *newOne);
	void DeleteAllBitObjects();
	void DeleteBitObjects(BitObject *newOne);
    void DeleteAllChildren();
    void DeleteChild(CAnimateObj *pChild);

	void Where(LPRECT lpRect);

	void BltRect( HWND hWnd, LPRECT lpR, int left, int top );
	void CopyAllBitmaps (HWND hWnd, HDC loadMap, LPRECT r,
						int left, int top);

	void SetNotification(WORD newMsg, int isCont, int newWhich);
	void SendNotification(HWND hWnd, int newDesc);
	void SetNotifyProc(NOTIFYPROC lpfnNotify);
	void SetPlaySoundProc(PLAYSOUNDPROC lpfnSoundCallback);
	void SetPlaySound(LPSTR lpSound, BOOL bLoopSound, int iChannel);

	void FlipAllVert();                                     // objects and masks
	void FlipAllHorz();

	void Unlink();                                          // get ready to delete it

    CAnimateObj *GetParent()                { return m_Parent; }
    void SetParent(CAnimateObj *pParent)    { m_Parent = pParent; } 
    int GetChildObjectID(int Child);

	void RestartInstructions();
	void SetAutoRepeat(bool bAutoRepeat);
	BOOL GetAutoRepeat() { return m_bAutoRepeat; }

	void CheckLastActiveFrame();
	void PlaySound();

};

#endif





 



