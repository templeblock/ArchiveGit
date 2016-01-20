/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// fdview.cpp : implementation of the CFdView class
//

#include "stdafx.h"
#include <windowsx.h>
#include <mmsystem.h>
#include "fd.h"

#include "fddoc.h"
#include "fdview.h"
#include "fdmm.h"
#include "fdstatus.h"
#include "fdmap.h"
#include "voldlg.h"

#include "utils.h"
#include <wing.h>
#include "ack\ack3d.h"
#include "ack\ackeng.h"
#include "ack\ackext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void	TBitBlts(UCHAR huge *pDestBits,UCHAR huge *pSrcBits,
			UINT DRowOff,int DCol,int bmWidth,int bmHeight,int ScrWidth,int ViewLowRes);
long	long_divide(long denom,long divisor);

void	AckBuildHeightTables(ACKENG *ae);

struct
{
	WORD					Version;
	WORD					NumberOfEntries;
	PALETTEENTRY	palEntry[256];
} LogicalPalette={0x300,256};

CFdView					*OurView;

extern BOOL			ViewForward;
extern int			ViewLowRes, ViewUpDown, ViewUDCenter;
extern int			jog, jump;
extern BOOL			fActivateApp;	
extern BOOL			fIsIconic;
extern int			rapid_fire, disp_shot;

extern int			MusicActive, MusicWanted;

extern CFdStatus	*fds;
extern int			fdsActive;
extern RECT			fdsRect;
extern int			fdsWidth, fdsHeight;

extern CFdMap		*fdm;
extern int			fdmActive;
extern RECT			fdmRect;
extern int			fdmWidth, fdmHeight;

int							CrossHairs;
int							BufferX=FULL_CX, BufferY=FULL_CY;
long						GrowthX = 1, GrowthY = 1;
long						Orientation = 1;			// assume bottom-up DIBs
unsigned char		TransparentColor = 0x00;	// for transparent Blts
CPalette				CPalApp, *CPalPtr=NULL;

DibHeader				DibBufferInfo;
void far				*pBufferBits = NULL;
HBITMAP					hbmMonoBuffer = NULL;
HBITMAP					hbmBuffer = NULL;
HDC							hdcBuffer = NULL;

UINT						TimerOne=0;
unsigned long		TimerTicks=0, NumberOfPaints=0;

CTime						StartTime, StopTime;
CTimeSpan				ElapsedTime;
#ifdef	_DEBUG
UINT						fsr1, fsr2;
long						fgm1, fgm2, fcm1, fcm2;
#endif

int				sysNames[21]={
					COLOR_ACTIVEBORDER,		//	Active window border. 
					COLOR_ACTIVECAPTION,	//	Active window title. 
					COLOR_APPWORKSPACE,		//	Background color of multiple document interface (MDI) applications. 
					COLOR_BACKGROUND,		//	Desktop. 
					COLOR_BTNFACE,			//	Face shading on push buttons. 
					COLOR_BTNHIGHLIGHT,		//	Selected button in a control. 
					COLOR_BTNSHADOW,		//	Edge shading on push buttons. 
					COLOR_BTNTEXT,			//	Text on push buttons. 
					COLOR_CAPTIONTEXT,		//	Text in title bar, size button, scroll-bar arrow button. 
					COLOR_GRAYTEXT,			//	Grayed (dimmed) text. This color is zero if the current display driver does not support a solid gray color. 
					COLOR_HIGHLIGHT,		//	Background of selected item in a control. 
					COLOR_HIGHLIGHTTEXT,	//	Text of selected item in a control. 
					COLOR_INACTIVEBORDER,	//	Inactive window border. 
					COLOR_INACTIVECAPTION,	//	Inactive window title. 
					COLOR_INACTIVECAPTIONTEXT,	//	Color of text in an inactive title. 
					COLOR_MENU,				//	Menu background. 
					COLOR_MENUTEXT,			//	Text in menus. 
					COLOR_SCROLLBAR,		//	Scroll-bar gray area. 
					COLOR_WINDOW,			//	Window background. 
					COLOR_WINDOWFRAME,		//	Window frame. 
					COLOR_WINDOWTEXT};		//	Text in windows. 
COLORREF	sysColors[21];
#define		BLACK	0x00000000
#define		WHITE	0x00FFFFFF
COLORREF	sysValues[21]={
					BLACK,	//	COLOR_ACTIVEBORDER
					WHITE,	//	COLOR_ACTIVECAPTION
					BLACK,	//	COLOR_APPWORKSPACE
					BLACK,	//	COLOR_BACKGROUND
					BLACK,	//	COLOR_BTNFACE
					BLACK,	//	COLOR_BTNHIGHLIGHT
					BLACK,	//	COLOR_BTNSHADOW
					WHITE,	//	COLOR_BTNTEXT
					BLACK,	//	COLOR_CAPTIONTEXT
					WHITE,	//	COLOR_GRAYTEXT
					WHITE,	//	COLOR_HIGHLIGHT
					BLACK,	//	COLOR_HIGHLIGHTTEXT
					BLACK,	//	COLOR_INACTIVEBORDER
					BLACK,	//	COLOR_INACTIVECAPTION
					WHITE,	//	COLOR_INACTIVECAPTIONTEXT
					BLACK,	//	COLOR_MENU
					WHITE,	//	COLOR_MENUTEXT
					BLACK,	//	COLOR_SCROLLBAR
					BLACK,	//	COLOR_WINDOW
					WHITE,	//	COLOR_WINDOWFRAME
					WHITE};	//	COLOR_WINDOWTEXT

#define CEILING_COLOR	4
#define FLOOR_COLOR		0

#define PLAYER_X			390	    // Initial X coordinate
#define PLAYER_Y			260	    // Initial Y coordinate
#define PLAYER_ANGLE	480	    // Initial POV angle

#define DOORSPEED			4

HGLOBAL			hae, hao;
ACKENG			far *ae = NULL;
char				*BgdFileName = "LBM\\BACK5.LBM";
char				*PalFileName = "LBM\\FD.PAL";
HGLOBAL			hOverlayPic;
UCHAR far   *OverlayPic;

typedef struct
{
	int		Number;
	int		Type;
	char	*Name;
} BMTABLE;

BMTABLE bmTable[] = {
                1		,TYPE_WALL		,"lbm\\swall12.lbm",
                2		,TYPE_WALL		,"lbm\\swall13.lbm",
                3		,TYPE_WALL		,"lbm\\swall14.lbm",
                4		,TYPE_WALL		,"lbm\\swall15.lbm",
                5		,TYPE_WALL    ,"lbm\\swall16.lbm",
                6		,TYPE_WALL    ,"lbm\\swall17.lbm",
                7		,TYPE_WALL    ,"lbm\\swall18.lbm",
                8		,TYPE_WALL    ,"lbm\\swall19.lbm",
                9		,TYPE_WALL		,"lbm\\swall20.lbm",
                10	,TYPE_WALL		,"lbm\\swall21.lbm",
                11	,TYPE_WALL		,"lbm\\swall22.lbm",
                12	,TYPE_WALL		,"lbm\\swall23.lbm",
                13	,TYPE_WALL		,"lbm\\swall24.lbm",
                14	,TYPE_WALL		,"lbm\\swall25.lbm",
                15	,TYPE_WALL		,"lbm\\swall26.lbm",
                16	,TYPE_WALL		,"lbm\\swall27.lbm",
                17	,TYPE_WALL		,"lbm\\heart1.lbm",
                18	,TYPE_WALL		,"lbm\\column1.lbm",
                19	,TYPE_WALL		,"lbm\\swall28.lbm",
                20	,TYPE_WALL		,"lbm\\swall29.lbm",
                21	,TYPE_WALL		,"lbm\\swall30.lbm",
                22	,TYPE_WALL		,"lbm\\cyberd.lbm",
                DOOR_SIDECODE		,TYPE_WALL	,"lbm\\sjam.lbm",
                DOOR_SIDECODE+1	,TYPE_WALL	,"lbm\\sjam1.lbm",
                DOOR_SIDECODE+2	,TYPE_WALL	,"lbm\\sjam2.lbm",
                DOOR_XCODE			,TYPE_WALL	,"lbm\\sdoor.lbm",
                DOOR_XCODE+1		,TYPE_WALL	,"lbm\\sdoor1.lbm",
                DOOR_XCODE+2		,TYPE_WALL	,"lbm\\sdoor2.lbm",
                DOOR_YCODE			,TYPE_WALL	,"lbm\\sdoor.lbm",
                DOOR_YCODE+1		,TYPE_WALL	,"lbm\\sdoor1.lbm",
                DOOR_YCODE+2		,TYPE_WALL	,"lbm\\sdoor2.lbm",
                1   ,TYPE_OBJECT    ,"lbm\\sman1.lbm",
                2   ,TYPE_OBJECT    ,"lbm\\can1.lbm",
                3   ,TYPE_OBJECT    ,"lbm\\astro1.lbm",
                4   ,TYPE_OBJECT    ,"lbm\\smanl1.lbm",
                5   ,TYPE_OBJECT    ,"lbm\\smanr1.lbm",
                6   ,TYPE_OBJECT    ,"lbm\\bluekey1.lbm",
                7   ,TYPE_OBJECT    ,"lbm\\grenkey1.lbm",
                8   ,TYPE_OBJECT    ,"lbm\\redkey1.lbm",
                9   ,TYPE_OBJECT    ,"lbm\\gore1.lbm",
                10  ,TYPE_OBJECT    ,"lbm\\medkit1.lbm",
                11  ,TYPE_OBJECT    ,"lbm\\ammo1.lbm",		//	Pistol, Rifle and Machine Gun
                12  ,TYPE_OBJECT    ,"lbm\\ammo2.lbm",		//	Shotgun
                13  ,TYPE_OBJECT    ,"lbm\\rifle1.lbm",		//	Rifle
                14  ,TYPE_OBJECT    ,"lbm\\shotgun2.lbm",	//	Shotgun
                15  ,TYPE_OBJECT    ,"lbm\\colobj1.lbm",	//	Roman column
                16  ,TYPE_OBJECT    ,"lbm\\sman2.lbm",
                17  ,TYPE_OBJECT    ,"lbm\\sman3.lbm",
                18  ,TYPE_OBJECT    ,"lbm\\ball1.lbm",		//	Rocket
                19  ,TYPE_OBJECT    ,"lbm\\badman1.lbm",	//	Touch bad guy
                20  ,TYPE_OBJECT    ,"lbm\\smanl2.lbm",
                21  ,TYPE_OBJECT    ,"lbm\\smanr2.lbm",
                22  ,TYPE_OBJECT    ,"lbm\\bhole1.lbm",
                23  ,TYPE_OBJECT    ,"lbm\\bhole2.lbm",
                24  ,TYPE_OBJECT    ,"lbm\\bhole3.lbm",
                -1  ,-1             ,""             			//	End of table
                };

typedef struct
{
	int							Number;
	char						*Name;
	PDIB						pGridDib;		//	Pointer to Device Independent Bitmap
	BITMAPINFO far	*pGridInfo;
	char huge				*pGridBits;

} SCRTABLE;

SCRTABLE scrTable[] = {
					0,	"bmp\\grid1.bmp",		NULL,	NULL,	NULL,
					1,	"bmp\\pishnd1.bmp",	NULL,	NULL,	NULL,
					2,	"bmp\\pishnd2.bmp",	NULL,	NULL,	NULL,
					3,	"bmp\\shghnd1.bmp",	NULL,	NULL,	NULL,
					4,	"bmp\\shghnd2.bmp",	NULL,	NULL,	NULL,
					5,	"bmp\\shghnd3.bmp",	NULL,	NULL,	NULL,
					6,	"bmp\\shghnd4.bmp",	NULL,	NULL,	NULL,
					-1,	NULL,								NULL,	NULL,	NULL};

/////////////////////////////////////////////////////////////////////////////
// CFdView

IMPLEMENT_DYNCREATE(CFdView, CView)

BEGIN_MESSAGE_MAP(CFdView, CView)
	//{{AFX_MSG_MAP(CFdView)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(WM_USER_IDLE, OnUserIdle)
	ON_REGISTERED_MESSAGE(WM_USER_ACTION, OnUserAction)
	ON_REGISTERED_MESSAGE(WM_USER_SOUND, OnUserSound)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_KEYUP()
	ON_UPDATE_COMMAND_UI(FD_VIEW_FORWARD, OnUpdateViewForward)
	ON_COMMAND(FD_VIEW_FORWARD, OnViewForward)
	ON_UPDATE_COMMAND_UI(FD_OPTIONS_MUSIC, OnUpdateOptionsMusic)
	ON_COMMAND(FD_OPTIONS_MUSIC, OnOptionsMusic)
	ON_UPDATE_COMMAND_UI(FD_OPTIONS_SOUND, OnUpdateOptionsSound)
	ON_COMMAND(FD_OPTIONS_SOUND, OnOptionsSound)
	ON_UPDATE_COMMAND_UI(FD_OPTIONS_AUTOMATIC, OnUpdateOptionsAutomatic)
	ON_COMMAND(FD_OPTIONS_AUTOMATIC, OnOptionsAutomatic)
	ON_UPDATE_COMMAND_UI(FD_OPTIONS_STATUS, OnUpdateOptionsStatus)
	ON_COMMAND(FD_OPTIONS_STATUS, OnOptionsStatus)
	ON_WM_SHOWWINDOW()
	ON_UPDATE_COMMAND_UI(FD_OPTIONS_CROSSHAIRS, OnUpdateOptionsCrosshairs)
	ON_COMMAND(FD_OPTIONS_CROSSHAIRS, OnOptionsCrosshairs)
	ON_COMMAND(FD_FILE_QUIT, OnFileQuit)
	ON_MESSAGE(MM_MOM_OPEN, OnMmMomOpen)
	ON_MESSAGE(MM_MOM_DONE, OnMmMomDone)
	ON_MESSAGE(MM_MOM_CLOSE, OnMmMomClose)
	ON_COMMAND(FD_OPTIONS_VOLUME, OnOptionsVolume)
	ON_COMMAND(FD_OPTIONS_MAP, OnOptionsMap)
	ON_UPDATE_COMMAND_UI(FD_OPTIONS_MAP, OnUpdateOptionsMap)
	ON_UPDATE_COMMAND_UI(FD_VIEW_LOW_RES, OnUpdateViewLowRes)
	ON_COMMAND(FD_VIEW_LOW_RES, OnViewLowRes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int	CFdView::ResizeWindow(int divisor)
{
HBITMAP		hbm;
 	//  Retrieve the current WinGBitmap, replace with the original
 	if (hbmMonoBuffer)
 	{
 		hbm = (HBITMAP)SelectObject(hdcBuffer, hbmMonoBuffer);
 		DeleteObject(hbm);	//  And delete the WinGBitmap
 	}

	ViewLowRes					= divisor - 1;
	ViewWidth						= DEF_VIEW_WIDTH / divisor;	//	Width
	ViewHeight					= DEF_VIEW_HEIGHT / divisor;//	Height
	ViewX								= DEF_VIEW_X;								//	First col of viewport
	ViewX1							= ViewWidth - 1;						//	Last col of viewport
	ViewXS							= ViewWidth;								//	Width
	ViewY								= DEF_VIEW_Y;								//	First row of viewport
	ViewY1							= ViewHeight - 1;						//	Last row of viewport
	ViewYS							= ViewHeight;								//	Height
	BytesPerRow					= ViewWidth;
	DWordsPerRow				= ViewWidth / 4;
	if (!ae->ObjList[PLAYER_OBJECT].Dead)
		ViewUpDown				= 32 << (8 - ViewLowRes);		//	Center of screen
	else
		ViewUpDown = (32 + ae->ObjList[PLAYER_OBJECT].Dead) << (8 - ViewLowRes);	//	if DEAD, lie down
	ViewUDCenter				= 32 << (8 - ViewLowRes);		//	ALWAYS center of screen

	ae->WinStartX				= ViewX;
	ae->WinStartY				= ViewY;		    /* Plug in the size we want */
	ae->WinEndX					= ViewX1;	    	/* for our viewport		*/
	ae->WinEndY					= ViewY1;
	ae->CenterRow				= ae->WinStartY + ((ae->WinEndY - ae->WinStartY) / 2);
	ae->WinStartOffset	= ae->WinStartY * BytesPerRow;
	ae->WinLength				= ((ae->WinEndY - ae->WinStartY) + 1) * DWordsPerRow;
	ae->WinWidth				= (ae->WinEndX - ae->WinStartX) + 1;
	ae->WinHeight				= (ae->WinEndY - ae->WinStartY) + 1;

	AckBuildHeightTables(ae);

	//	Create new size Bitmap
	hbmBuffer = BitmapCreate(&DibBufferInfo, &hdcBuffer, &pBufferBits);
  if (hbmBuffer == 0) return -1;
  //  Store the old hbitmap to select back in before deleting
  hbmMonoBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);
	ScreenBuffer = (unsigned char far *)pBufferBits;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//	CFdView::AppAckInitialize

int CFdView::AppAckInitialize(void)
{
int		i, result, ObjNum, num;
UCHAR	BitmapNumbers[MAX_VIEWS];
char	MovementFlags[MAX_VIEWS];
char	DisplayMax[MAX_VIEWS];

	ViewUpDown = 32 << (8 - ViewLowRes);

	ae->WinStartX				= ViewX;
	ae->WinStartY				= ViewY;		    /* Plug in the size we want */
	ae->WinEndX					= ViewX1;	    	/* for our viewport		*/
	ae->WinEndY					= ViewY1;
	ae->CenterRow				= ae->WinStartY + ((ae->WinEndY - ae->WinStartY) / 2);
	ae->WinStartOffset	= ae->WinStartY * BytesPerRow;
	ae->WinLength				= ((ae->WinEndY - ae->WinStartY) + 1) * DWordsPerRow;
	ae->WinWidth				= (ae->WinEndX - ae->WinStartX) + 1;
	ae->WinHeight				= (ae->WinEndY - ae->WinStartY) + 1;

	ae->DoorSpeed	= DOORSPEED;

	ae->LightFlag   = SHADING_ON;	    // Yes, we want light shading on
	ae->TopColor		= CEILING_COLOR;  // Setup our colors for the
	ae->BottomColor = FLOOR_COLOR;	  // background...

	ae->ElapsedTime = 0L;
	ae->NumberOfPaints = 0L;

	ae->ObjList[PLAYER_OBJECT].Dead = 0;	//	alive again

	ae->ObjList[PLAYER_OBJECT].x			= PLAYER_X;		/* Setup intial coordinates */
	ae->ObjList[PLAYER_OBJECT].y			= PLAYER_Y;		/* for the POV		*/
	ae->ObjList[PLAYER_OBJECT].mPos		= (PLAYER_Y & 0xFFC0) + (PLAYER_X >> 6);
	ae->ObjList[PLAYER_OBJECT].Dir		= PLAYER_ANGLE;
	ae->ObjList[PLAYER_OBJECT].Active	= 1;
	ae->ObjList[PLAYER_OBJECT].Health	= MAX_HEALTH;

	ObjList[PLAYER_OBJECT]->Weapon	= PISTOL;

 	TRACE("Opening View Map file %s\n",ae->MapFileName);
	result = AckReadMapFile(ae, ae->MapFileName);
	if (result)
	{
		TRACE("AckReadMapFile failed with code %d\n", result);
		AppAckWrapUp();
		return(result);
	}

	//	Initialize weapons info
	ae->WeaponsCarried[0].Possessed = 1;	//	Always have Fists
	ae->WeaponsCarried[0].Ammo = &ae->Fists;		//	Unlimited use of Fists
	*ae->WeaponsCarried[0].Ammo = -1;		//	Unlimited use of Fists
	ae->WeaponsCarried[1].Possessed = 1;	//	Always have Pistol
	ae->WeaponsCarried[1].Ammo = &ae->Bullets;
	*ae->WeaponsCarried[1].Ammo = 50;
	ae->WeaponsCarried[2].Possessed = 0;	//	Must find Shotgun
	ae->WeaponsCarried[2].Ammo = &ae->Shells;
	*ae->WeaponsCarried[2].Ammo = 0;
	ae->WeaponsCarried[3].Possessed = 0;	//	Must find Machine Gun
	ae->WeaponsCarried[3].Ammo = &ae->Bullets;
	*ae->WeaponsCarried[3].Ammo = 50;
	ae->WeaponsCarried[4].Possessed = 0;	//	Must find Rocket Laucher
	ae->WeaponsCarried[4].Ammo = &ae->Rockets;
	*ae->WeaponsCarried[4].Ammo = 0;
	ae->Keys = 0;

	//	This code will work for single sided Objects only, at this time.
	for (i=1 ; i < MAX_OBJECTS ; i++)
	{
		if ((ObjNum = ObjList[i]->bmNum[0]) != 0)
		{
			if (ObjNum == 1)	//	sman
			{
				ae->ObjList[i].Flags	= OF_ANIMATE;	// Object Flags (OF_xxxxxx)
				ae->ObjList[i].Health	= 1500;
				ae->ObjList[i].Enemy	= PLAYER_OBJECT;
				ae->ObjList[i].Ammo		= 1000;

				ObjList[i]->SksMax		= 12;					// Number of updates in one Dir before course correction
			  ObjList[i]->SkNum			= 0;					// Start after Player, immediately
			  ObjList[i]->Speed			= 6;					// Speed of Object movement
			  ObjList[i]->Weapon		= PISTOL;			//	Pistol
			  ObjList[i]->FireFreq	= 9;					//	Will average firing every 9 frames
			  ObjList[i]->Damage		= 100;				//	Contact damage
			  ObjList[i]->FieldOV		= INT_ANGLE_30;	//	+/- 30 degree FOV
			  ObjList[i]->IdlNum		= 0;
			  ObjList[i]->IdlMax		= 3;
			  ObjList[i]->MovNum		= 4;
			  ObjList[i]->MovMax		= 11;
			  ObjList[i]->CurNum		= i % 4;			//	Starting idle bitmap
			  ObjList[i]->CheckDist	= 32;
				BitmapNumbers[0]  = 1;							//	Idle bitmaps
				BitmapNumbers[1]  = 16;
				BitmapNumbers[2]  = 1;
				BitmapNumbers[3]  = 17;
				BitmapNumbers[4]  = 1;							//	Active bitmaps
				BitmapNumbers[5]  = 20;
				BitmapNumbers[6]  = 4;
				BitmapNumbers[7]  = 20;
				BitmapNumbers[8]  = 1;
				BitmapNumbers[9]  = 21;
				BitmapNumbers[10] = 5;
				BitmapNumbers[11] = 21;
				MovementFlags[0]  = 0;							//	Idle movement flag
				MovementFlags[1]  = 0;
				MovementFlags[2]  = 0;
				MovementFlags[3]  = 0;
				MovementFlags[4]  = 0;							//	Active movement flag
				MovementFlags[5]  = 0;
				MovementFlags[6]  = 1;
				MovementFlags[7]  = 1;
				MovementFlags[8]  = 0;
				MovementFlags[9]  = 0;
				MovementFlags[10] = 1;
				MovementFlags[11] = 1;
				DisplayMax[0]  = 3;									//	Display idle bitmap count
				DisplayMax[1]  = 2;
				DisplayMax[2]  = 3;
				DisplayMax[3]  = 2;
				DisplayMax[4]  = 0;									//	Display active bitmap count
				DisplayMax[5]  = 0;
				DisplayMax[6]  = 0;
				DisplayMax[7]  = 0;
				DisplayMax[8]  = 0;
				DisplayMax[9]  = 0;
				DisplayMax[10] = 0;
				DisplayMax[11] = 0;
				num = 12;
			}
			else
			if ((ObjNum == 2) || (ObjNum == 15))	//	can or column
			{
				ae->ObjList[i].Flags	= OF_STATIC;	// Object Flags (OF_xxxxxx)

			  ObjList[i]->Speed	= 0;							// Speed of Object movement
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0] = 0;
				num = 1;
			}
			else
			if (ObjNum == 6)	//	blue key
			{
				ae->ObjList[i].Flags	= OF_CAN_PICKUP|OF_KEY_BLUE;
				ae->ObjList[i].Health	= -1;					// Indestructable

			  ObjList[i]->Speed	= 0;							// Speed of Object movement
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 7)	//	green key
			{
				ae->ObjList[i].Flags	= OF_CAN_PICKUP|OF_KEY_GREEN;
				ae->ObjList[i].Health	= -1;					// Indestructable

			  ObjList[i]->Speed	= 0;							// Speed of Object movement
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 8)	//	red key
			{
				ae->ObjList[i].Flags	= OF_CAN_PICKUP|OF_KEY_RED;
				ae->ObjList[i].Health	= -1;					// Indestructable

			  ObjList[i]->Speed	= 0;							// Speed of Object movement
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 9)	//	gore (when something is killed)
			{
				ae->ObjList[i].Flags	= OF_PASSABLE;
				ae->ObjList[i].Health	= 0;

			  ObjList[i]->Speed	= 0;							// Speed of Object movement
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 10)	//	med kit
			{
				ae->ObjList[i].Flags	= OF_CAN_PICKUP|OF_HEALTH;
				ae->ObjList[i].Health	= 500;

			  ObjList[i]->Speed	= 0;							// Speed of Object movement
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 11)	//	ammo (Pistol)
			{
				ae->ObjList[i].Flags	= OF_CAN_PICKUP|OF_AMMUNITION;
				ae->ObjList[i].Ammo		= 15;

			  ObjList[i]->Speed		= 0;						// Speed of Object movement
			  ObjList[i]->Weapon	= PISTOL;
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 12)	//	ammo (Shotgun)
			{
				ae->ObjList[i].Flags	= OF_CAN_PICKUP|OF_AMMUNITION;
				ae->ObjList[i].Ammo		= 10;

			  ObjList[i]->Speed		= 0;						// Speed of Object movement
			  ObjList[i]->Weapon	= SHOTGUN;
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 13)	//	Rifle
			{
				ae->ObjList[i].Flags	= OF_CAN_PICKUP|OF_WEAPON;
				ae->ObjList[i].Ammo		= 25;

			  ObjList[i]->Speed		= 0;						// Speed of Object movement
			  ObjList[i]->Weapon	= MACHINEGUN;
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 14)	//	Shotgun
			{
				ae->ObjList[i].Flags	= OF_CAN_PICKUP|OF_WEAPON;
				ae->ObjList[i].Ammo		= 5;

			  ObjList[i]->Speed		= 0;						// Speed of Object movement
			  ObjList[i]->Weapon	= SHOTGUN;
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			else
			if (ObjNum == 18)	//	fireball
			{
				ae->ObjList[i].Flags	= OF_ANIMATE|OF_SEEN_PLAYER|OF_ONE_SHOT;	// Object Flags (OF_xxxxxx)
				ae->ObjList[i].Health	= Weapons[LAUNCHER].Damage;
				ae->ObjList[i].Enemy	= PLAYER_OBJECT;

			  ObjList[i]->SksMax	= 1;						// Number of updates in one Dir before course correction
			  ObjList[i]->SkNum		= 1;						// Start after Player, immediately
			  ObjList[i]->Damage	= Weapons[LAUNCHER].Damage;
			  ObjList[i]->Speed		= 64;						// Speed of Object movement
			  ObjList[i]->IdlNum	= 0;
			  ObjList[i]->IdlMax	= 0;
			  ObjList[i]->MovNum	= 0;
			  ObjList[i]->MovMax	= 0;
			  ObjList[i]->CurNum	= 0;						//	Starting idle bitmap
			  ObjList[i]->CheckDist	= 16;
				BitmapNumbers[0]	= ObjNum;					//	Idle bitmaps
				MovementFlags[0]	= 1;							//	Idle movement flag
				DisplayMax[0]	= 1;									//	Display idle bitmap count
				num = 1;
			}
			else
			if (ObjNum == 19)	//	bad guy
			{
				ae->ObjList[i].Flags	= OF_ANIMATE;	// Object Flags (OF_xxxxxx)
				ae->ObjList[i].Health	= 6000;
				ae->ObjList[i].Enemy	= PLAYER_OBJECT;
				ae->ObjList[i].Ammo		= 1000;

			  ObjList[i]->SksMax	= 32;						// Number of updates in one Dir before course correction
			  ObjList[i]->SkNum		= 1;						// Start after Player, immediately
			  ObjList[i]->Speed		= 8;						// Speed of Object movement
			  ObjList[i]->Weapon	= LAUNCHER;			//	Rocket Laucher
			  ObjList[i]->FireFreq	= 8;					//	Will average firing every 4 moves
			  ObjList[i]->Damage	= 200;					//	Contact damage
			  ObjList[i]->FieldOV	= INT_ANGLE_30;	//	+/- 30 degree FOV
			  ObjList[i]->IdlNum	= 0;
			  ObjList[i]->IdlMax	= 0;
			  ObjList[i]->MovNum	= 0;
			  ObjList[i]->MovMax	= 0;
			  ObjList[i]->CurNum	= 0;						//	Starting idle bitmap
			  ObjList[i]->CheckDist	= 32;
				BitmapNumbers[0]	= ObjNum;					//	bitmaps
				MovementFlags[0]	= 1;							//	movement flag
				DisplayMax[0]	= 2;									//	Display bitmap count
				num = 1;
			}
			else
			if (ObjNum == 22)	//	black hole
			{
				ae->ObjList[i].Flags	= OF_ANIMATE|OF_FORCE_FIELD;	// Object Flags (OF_xxxxxx)

			  ObjList[i]->SksMax	= 1;						// Number of updates in one Dir before course correction
			  ObjList[i]->SkNum		= 0;						// Start after Player, immediately
			  ObjList[i]->Speed		= 0;						// Speed of Object movement
			  ObjList[i]->Damage	= 100;					//	Contact damage
			  ObjList[i]->IdlNum	= 0;
			  ObjList[i]->IdlMax	= 2;
			  ObjList[i]->MovNum	= 0;
			  ObjList[i]->MovMax	= 2;
			  ObjList[i]->CurNum	= i % 3;				//	Starting idle bitmap
			  ObjList[i]->CheckDist	= 32;
				BitmapNumbers[0]  = 22;							//	Idle bitmaps
				BitmapNumbers[1]  = 23;
				BitmapNumbers[2]  = 24;
				MovementFlags[0]  = 0;							//	Idle movement flag
				MovementFlags[1]  = 0;
				MovementFlags[2]  = 0;
				DisplayMax[0]  = 0;									//	Display idle bitmap count
				DisplayMax[1]  = 0;
				DisplayMax[2]  = 0;
				num = 3;
			}
			else
			{		//	everything else
				ae->ObjList[i].Flags	= 0;					// Object Flags (OF_xxxxxx)

			  ObjList[i]->Speed		= 0;						// Speed of Object movement
			  ObjList[i]->SksMax	= 0;						// Number of updates in one Dir before course correction
			  ObjList[i]->SkNum		= 0;
			  ObjList[i]->CheckDist	= 32;
				BitmapNumbers[0]	= ObjNum;					// Bitmap to use with object
				MovementFlags[0]	= 0;							// Idle movement
				DisplayMax[0]	= 0;
				num = 1;
			}
			result = AckCreateObject(ae,i,num,BitmapNumbers,MovementFlags,DisplayMax);
			if (result)
   		{
				TRACE("AckCreateObject %d failed with code %d\n", ObjNum, result);
    		AppAckWrapUp();
    		return(result);
    	}
    	if (ObjNum == 18) ae->ObjList[ObjNum].Active = 0;	//	Keep deactivated
    }
  }

	//	the Gore object is stored at the top of ObjList
	//	there is NO corresponding entry in ae->ObjList
	ObjList[MAX_OBJECTS]->bmNum[0]	= 9;
	ObjList[MAX_OBJECTS]->mvFlg[0]	= 0;
	ObjList[MAX_OBJECTS]->bmDsp[0]	= 0;
	ObjList[MAX_OBJECTS]->IdlNum		= 0;
	ObjList[MAX_OBJECTS]->IdlMax		= 0;
	ObjList[MAX_OBJECTS]->MovNum		= 0;
	ObjList[MAX_OBJECTS]->MovMax		= 0;
	ObjList[MAX_OBJECTS]->CurNum		= 0;
	ObjList[MAX_OBJECTS]->Speed			= 0;
  ObjList[MAX_OBJECTS]->Speed			= 0;
  ObjList[MAX_OBJECTS]->SksMax		= 0;
  ObjList[MAX_OBJECTS]->SkNum			= 0;
  ObjList[MAX_OBJECTS]->CheckDist	= 32;
	GoreObject = MAX_OBJECTS;
	
	if (fds)
	{	//	Update status window
		fds->TextHealth();
		fds->TextAmmo();
		fds->TextMsg("");
		fds->DrawKeys();
	}
	
	if (fdm) fdm->InvalidateRect(NULL,FALSE);

	StartTime = CTime::GetCurrentTime();
	TRACE("Start time is %s\n", StartTime.Format("%H:%M:%S"));
	NumberOfPaints = 0;
	GetParentFrame()->SetWindowText("WinAdv");

	TRACE("AckInitialize completed successfully\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//	AppAckFree

void CFdView::AppAckWrapUp(void)
{
	AckWrapUp(ae);
	if (hOverlayPic)
	{
		GlobalUnlock(hOverlayPic);
		GlobalFree(hOverlayPic);
		hOverlayPic = NULL;
	}
	GlobalUnlock(hae);
	GlobalFree(hae);
	ae = NULL;

	GlobalUnlock(hao);
	GlobalFree(hao);

	TRACE("AckWrapUp completed\n");
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CFdView construction

CFdView::CFdView()
{
	// TODO: add construction code here
	TRACE("CFdView constructor (Begin...)\n");

	GlobalCompact(0xffffffff);	//	discard unlocked discardable objects

#ifdef	_DEBUG
afxMemDF = allocMemDF | checkAlwaysMemDF;
	fsr1 = GetFreeSystemResources(GFSR_GDIRESOURCES);	//	percent
	fgm1 = GetFreeSpace(0);	//	bytes
	fcm1 = GlobalCompact(0);	//	bytes
	TRACE("        Pre-allocation resources\n");
	TRACE("GDI resources available = %u%%\n", fsr1);
	TRACE("Free space available in global heap = %ld\n", fgm1);
	TRACE("Free contiguous space available = %ld\n", fcm1);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CFdView destruction

CFdView::~CFdView()
{
#ifdef	_DEBUG
	fsr2 = GetFreeSystemResources(GFSR_GDIRESOURCES);	//	percent
	fgm2 = GetFreeSpace(0);	//	bytes
	fcm2 = GlobalCompact(0);	//	bytes
	TRACE("        Post-allocation resources\n");
	TRACE("GDI resources available = %u%%\n", fsr2);
	TRACE("Free space available in global heap = %ld (%ld)\n", fgm2, fgm2-fgm1);
	TRACE("Free contiguous space available = %ld (%ld)\n", fcm2, fcm2-fcm1);
#endif

	TRACE("CFdView destructor (...End)\n");
}

/////////////////////////////////////////////////////////////////////////////
// ClearDesktop

void CFdView::ClearDesktop(void)
{
CDC		*pDC;
CWnd	*dt;
RECT	rect;

	dt = GetDesktopWindow();
	pDC = dt->GetDC();
	dt->GetWindowRect(&rect);
	pDC->PatBlt(rect.left,rect.top,rect.right,rect.bottom,BLACKNESS);
	dt->ReleaseDC(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnCreate

int CFdView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
#ifdef	_DEBUG
long	fgm1, fgm2;
	fgm1 = GetFreeSpace(0);	//	bytes
	TRACE("CFdView::OnCreate global heap available = %ld\n", fgm1);
#endif

int		i,j,k,result,Counter;
int		plotcolor;
FILE	*handle;
CString	tmp;
ANIMATION huge	*aptr;

	OurView = this;

	ClearDesktop();

	srand((int)GetTickCount());	//	Seed random number with "something"

	tmp = AfxGetApp()->GetProfileString("Preferences", "LowResolution", "No");
	if (!tmp.CompareNoCase("Yes"))
	{
		ViewLowRes = 1;
		ViewWidth = DEF_VIEW_WIDTH / 2;		//	Width
		ViewHeight = DEF_VIEW_HEIGHT / 2;	//	Height
		ViewX = DEF_VIEW_X;								//	First col of viewport
		ViewX1 = ViewWidth - 1;						//	Last col of viewport
		ViewXS = ViewWidth;								//	Width
		ViewY = DEF_VIEW_Y;								//	First row of viewport
		ViewY1 = ViewHeight - 1;					//	Last row of viewport
		ViewYS = ViewHeight;							//	Height
		BytesPerRow = ViewWidth;
		DWordsPerRow = ViewWidth / 4;
		ViewUpDown = 32 << (8 - ViewLowRes);	//	Center of screen
		ViewUDCenter = ViewUpDown;				//	ALWAYS center of screen
	}

	for (Counter=0 ; Counter < 21 ; Counter++)
		sysColors[Counter] = GetSysColor(sysNames[Counter]);

  ClearSystemPalette(this);
 	handle = fopen(PalFileName,"r+b");
 	if (handle == NULL)
	{
		AfxMessageBox("Could not open palette file", MB_OK|MB_ICONSTOP);
		return -1;
	}
	else
	{
 		fread((void *)LogicalPalette.palEntry,1024,1,handle);
 		fclose(handle);
 	}

  CPalApp.CreatePalette((LOGPALETTE *)&LogicalPalette);
  CPalPtr = &CPalApp;

	//	Create new size Bitmap
	hbmBuffer = BitmapCreate(&DibBufferInfo, &hdcBuffer, &pBufferBits);
  if (hbmBuffer == 0) return -1;
  //  Store the old hbitmap to select back in before deleting
  hbmMonoBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);
	ScreenBuffer = (unsigned char far *)pBufferBits;

	//	Allocate the 3D engine structure
	hae = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(ACKENG));
	if (hae == NULL)
	{
		TRACE("GlobalAlloc of ACKENG struct failed\n");
		return -1;
	}
	ae = (ACKENG far *)GlobalLock(hae);
	memset(ae, 0, sizeof(ACKENG));
	lstrcpy(ae->Descriptor,AE_DESCRIPTOR);
	ae->Version = AE_VERSION;

	//	Allocate the static Object structures
	hao = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, (long)sizeof(ANIMATION) * (long)MAX_OBJBMPS);
	if (hao == NULL)
	{
		TRACE("GlobalAlloc of ANIMATION struct failed\n");
		return -1;
	}
	aptr = (ANIMATION far *)GlobalLock(hao);
	for (i=0 ; i < MAX_OBJBMPS ; i++, aptr++)
	{
		memset(aptr, 0, sizeof(ANIMATION));
		ObjList[i] = aptr;
	}

	//	New Game Setup
	lstrcpy(ae->MapFileName,"FDMAP.L01");	//	Initial level
	result = AckInitialize(ae);
	if (result)
	{
		TRACE("AckInitialize failed with code %d\n", result);
   	AppAckWrapUp();
		return(-1);
	}

//	if (AppAckInitialize())
//	{
//		AfxMessageBox("AckInitialize failed", MB_OK|MB_ICONSTOP);
//		return -1;
//	}

/*	The background stuff... */
//	There are two types of background: 1) a static background that
//	is copied to the screen buffer directly and in total, and 2) a
//	dynamic scenic background that is copied to the screen buffer
//	based upon the direction the Player is facing.  If you use the
//	static background then use AckCopyBackground,  If you use the
//	dynamic scenic background then use AckCopyScene.  The present
//	implementation uses the scenic background; a 480 by 100 pixel
//	bitmap.
 	TRACE("Opening View Background file %s\n",BgdFileName);
	hBkgdBuffer = AckReadiff(BgdFileName);
	if (hBkgdBuffer == NULL)
  {
		TRACE("AckReadiff failed for background\n");
   	AppAckWrapUp();
   	return(-1);
  }
	BkgdBuffer = (unsigned char far *)GlobalLock(hBkgdBuffer);

/*	...or this stuff
	result = AckBuildBackground(ae);	    // Build the ceiling, floor
	if (result)
  {
		TRACE("AckBuildBackground failed with code %d\n", result);
   	AppAckWrapUp();
   	return(-1);
  }
*/

	//	Load all bitmaps into memory
	for (i=0 ; bmTable[i].Number != -1 ; i++)
  {
  	TRACE("Opening View LBM file %s\n",bmTable[i].Name);
   	result = AckLoadBitmap(ae, bmTable[i].Number,
   			 bmTable[i].Type, bmTable[i].Name);
   	if (result)
    {
			TRACE("AckLoadBitmaps failed with code %d\n", result);
   		AppAckWrapUp();
   		return(-1);
   	}
  }

	//	Setup Palette with 16 gradients for shading
	for (j=0 ; j < 256 ; j++)
		PalTable[j] = j;

	for (i=1 ; i < 16 ; i++)
  {
	  for (j=0 ; j < 256 ; j++)
	{
		plotcolor = (int)(PalTable[j + ((i - 1) * 256)]);
		k = plotcolor & 0xf0;
		plotcolor--;
		if (plotcolor < k) plotcolor = k;
		PalTable[j + (i * 256)] = (char)plotcolor;
		}
  }

	//	Load the Screen overlays into memory
	for (Counter=0 ; scrTable[Counter].Number != -1 ; Counter++)
	{
  	TRACE("Opening View Screen file %s\n",scrTable[Counter].Name);
		scrTable[Counter].pGridDib	= DibOpenFile(scrTable[Counter].Name);
		scrTable[Counter].pGridInfo	= (BITMAPINFO far *)DibInfo(scrTable[Counter].pGridDib);
		scrTable[Counter].pGridBits	= (char huge *)DibPtr(scrTable[Counter].pGridDib);
	}
  
	//	Initialize the sound and music
	SoundInit();

	tmp = AfxGetApp()->GetProfileString("Preferences", "Automatic", "No");
	if (!tmp.CompareNoCase("Yes")) rapid_fire = 1;

	tmp = AfxGetApp()->GetProfileString("Preferences", "Status", "No");
	if (!tmp.CompareNoCase("Yes"))
	{
		fdsActive = 1;
		fdsRect.left	= AfxGetApp()->GetProfileInt("Preferences", "Status_X", 10);
		fdsRect.top		= AfxGetApp()->GetProfileInt("Preferences", "Status_Y", 10);
		fdsRect.right	= fdsRect.left + (fdsWidth - 1) + GetSystemMetrics(SM_CXBORDER) * 2;
		fdsRect.bottom	= fdsRect.top + (fdsHeight - 1) + GetSystemMetrics(SM_CYBORDER) +
							GetSystemMetrics(SM_CYCAPTION);
	}

	tmp = AfxGetApp()->GetProfileString("Preferences", "MapWin", "No");
	if (!tmp.CompareNoCase("Yes"))
	{
		fdmActive = 1;
		fdmRect.left	= AfxGetApp()->GetProfileInt("Preferences", "Map_X", 10);
		fdmRect.top		= AfxGetApp()->GetProfileInt("Preferences", "Map_Y", 10);
		fdmRect.right	= fdmRect.left + (fdmWidth - 1) + GetSystemMetrics(SM_CXBORDER) * 2;
		fdmRect.bottom	= fdmRect.top + (fdmHeight - 1) + GetSystemMetrics(SM_CYBORDER) +
							GetSystemMetrics(SM_CYCAPTION);
	}

	tmp = AfxGetApp()->GetProfileString("Preferences", "CrossHairs", "Yes");
	if (!tmp.CompareNoCase("Yes")) CrossHairs = 1;

	Counter = AfxGetApp()->GetProfileInt("Preferences", "FramePeriod", 100);
	if (Counter < 1) Counter = 1;
	else
	if (Counter > 1000) Counter = 1000;
	if ((TimerOne = SetTimer(62749, Counter, NULL)) == 0)	//	100 ~= 9 fps
	{
		AfxMessageBox("No Timers available for timer one!", MB_OK|MB_ICONSTOP);
		AppAckWrapUp();
		return -1;
	}

#ifdef	_DEBUG
	fgm2 = GetFreeSpace(0);	//	bytes
	TRACE("CFdView::OnCreate global heap used = %ld\n", fgm1-fgm2);
#endif

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnDestroy

void CFdView::OnDestroy()
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	StopTime = CTime::GetCurrentTime();
	TRACE("Stop time is %s\n", StopTime.Format("%H:%M:%S"));

#ifdef	_DEBUG
long	fgm1, fgm2;
	fgm1 = GetFreeSpace(0);	//	bytes
	TRACE("CFdView::OnDelete global heap available = %ld\n", fgm1);
#endif

long	l=0, r=0;
char	tmp[32];
	ElapsedTime = StopTime - StartTime + ae->ElapsedTime;
	TRACE("Elapsed time was %s\n", ElapsedTime.Format("%H:%M:%S"));
	AfxGetApp()->WriteProfileString("Results", "PlayTime", ElapsedTime.Format("%H:%M:%S"));

	wsprintf(tmp,"%ld", NumberOfPaints + ae->NumberOfPaints);
	TRACE("Total number of screen paints was %s\n", tmp);
	AfxGetApp()->WriteProfileString("Results", "Paints", tmp);

	l = ElapsedTime.GetTotalSeconds();
	l = long_divide(NumberOfPaints * 100L,l);
	r = l % 100L;
	l = long_divide(l,100L);

	wsprintf(tmp,"%ld.%02ld", l, r);
	TRACE("Number of frames per second was %s\n", tmp);
	AfxGetApp()->WriteProfileString("Results", "F/S", tmp);

	if (GetCapture() == this) ReleaseCapture();

	KillTimer(TimerOne);

	SoundReset();
	MusicReset();

	AppAckWrapUp();
	if (fds)
	{
		fds->DestroyWindow();
		delete fds;
	}
	if (fdm)
	{
		fdm->DestroyWindow();
		delete fdm;
	}

HBITMAP		hbm;
	if (hdcBuffer)
	{
   	//  Retrieve the current WinGBitmap, replace with the original
   	if (hbmMonoBuffer)
   	{
   		hbm = (HBITMAP)SelectObject(hdcBuffer, hbmMonoBuffer);
   		DeleteObject(hbm);	//  And delete the WinGBitmap
   	}
   	DeleteDC(hdcBuffer);	//  And delete the WinGDC
	}

	//	Free the Screen overlay memory
int		Counter;
	for (Counter=0 ; scrTable[Counter].Number != -1 ; Counter++)
	{
		DibFree(scrTable[Counter].pGridDib);
	}

	CPalApp.DeleteObject();

#ifdef	_DEBUG
	fgm2 = GetFreeSpace(0);	//	bytes
	TRACE("CFdView::OnDelete global heap freed = %ld\n", fgm1-fgm2);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CFdView diagnostics

#ifdef _DEBUG
void CFdView::AssertValid() const
{
	CView::AssertValid();
}

void CFdView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFdDoc* CFdView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFdDoc)));
	return (CFdDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Miscellaneous Routines

/////////////////////////////////////////////////////////////////////////////
// CFdView::PaintScreen		Paint Buffer to Screen (may be a portion of buffer)

void CFdView::PaintScreen(int Left, int Top, int Width, int Height, int SrcLeft, int SrcTop, int SrcWidth, int SrcHeight)
{
UINT	sr, sc;
CDC		*pDC;
	if (ViewForward && !ae->ObjList[PLAYER_OBJECT].Dead)
	{
		switch (ObjList[PLAYER_OBJECT]->Weapon)
		{
		case FISTS:			//	Three
			switch (disp_shot)
			{
			default:
				disp_shot++;
				if (disp_shot >= Weapons[ObjList[PLAYER_OBJECT]->Weapon].Delay)
					disp_shot = 0;
				else
					break;
			case 0:
				break;
			case 1:
				disp_shot++;
			}
			break;
		case PISTOL:		//	Three
			if (ViewLowRes)
			{	sr = 15840; sc = 56; }
			else
			{	sr = 63680; sc = 112; }
			switch (disp_shot)
			{
			default:
				disp_shot++;
				if (disp_shot >= Weapons[ObjList[PLAYER_OBJECT]->Weapon].Delay)
					disp_shot = 0;
				else
					break;
			case 0:	//	Idle; picture is 96W by 64H
				TBitBlts((UCHAR huge *)pBufferBits,(UCHAR huge *)scrTable[1].pGridBits,
					sr,sc,96,64,BytesPerRow,ViewLowRes);
				break;
			case 1:	//	First fire; picture is 96W by 96H
				TBitBlts((UCHAR huge *)pBufferBits,(UCHAR huge *)scrTable[2].pGridBits,
					sr,sc,96,96,BytesPerRow,ViewLowRes);
				disp_shot++;
				break;
			}
			break;
		case SHOTGUN:		//	Six
			if (ViewLowRes)
			{	sr = 15840; sc = (disp_shot == 2 || disp_shot == 3) ? 48 : 56; }
			else
			{	sr = 63680; sc = (disp_shot == 2 || disp_shot == 3) ? 96 : 112; }
			switch (disp_shot)
			{
			default:
				disp_shot++;
				if (disp_shot >= Weapons[ObjList[PLAYER_OBJECT]->Weapon].Delay)
					disp_shot = 0;
				else
					break;
			case 0:	//	Idle; picture is 96W by 64H
				TBitBlts((UCHAR huge *)pBufferBits,(UCHAR huge *)scrTable[3].pGridBits,
					sr,sc,96,64,BytesPerRow,ViewLowRes);
				break;
			case 1:	//	First fire; picture is 96W by 96H
				TBitBlts((UCHAR huge *)pBufferBits,(UCHAR huge *)scrTable[4].pGridBits,
					sr,sc,96,96,BytesPerRow,ViewLowRes);
				disp_shot++;
				break;
			case 2:	//	Second fire; picture is 128W by 96H
			case 3:	//	Third fire; picture is 128W by 96H
				TBitBlts((UCHAR huge *)pBufferBits,(UCHAR huge *)scrTable[5].pGridBits,
					sr,sc,128,96,BytesPerRow,ViewLowRes);
				disp_shot++;
				break;
			case 4:	//	Fourth fire; picture is 96W by 64H
				TBitBlts((UCHAR huge *)pBufferBits,(UCHAR huge *)scrTable[6].pGridBits,
					sr,sc,96,64,BytesPerRow,ViewLowRes);
				disp_shot++;
				break;
			}
			break;
		case MACHINEGUN:	//	Three
			switch (disp_shot)
			{
			default:
				disp_shot++;
				if (disp_shot >= Weapons[ObjList[PLAYER_OBJECT]->Weapon].Delay)
					disp_shot = 0;
				else
					break;
			case 0:
				break;
			case 1:
				disp_shot++;
				break;
			}
			break;
		case LAUNCHER:		//	Six
			switch (disp_shot)
			{
			default:
				disp_shot++;
				if (disp_shot >= Weapons[ObjList[PLAYER_OBJECT]->Weapon].Delay)
					disp_shot = 0;
				else
					break;
			case 0:
				break;
			case 1:
			case 2:
			case 3:
			case 4:
				disp_shot++;
				break;
			}
			break;
		default:
			disp_shot = 0;
			break;
		}

		if (CrossHairs)	//	picture is 68W by 8H
		{
			if (ViewLowRes)
			{	sr = 8320; sc = 62; }
			else
			{	sr = 33280; sc = 126; }
			TBitBlts((UCHAR huge *)pBufferBits,(UCHAR huge *)scrTable[0].pGridBits,
				sr,sc,68,8,BytesPerRow,ViewLowRes);
		}
	}

	pDC = GetDC();
	pDC->SelectPalette(&CPalApp,FALSE);
	pDC->RealizePalette();
	WinGStretchBlt(pDC->m_hDC, Left, Top, Width, Height, hdcBuffer, SrcLeft, SrcTop, SrcWidth, SrcHeight);
	NumberOfPaints++;
	ReleaseDC(pDC);
}
/////////////////////////////////////////////////////////////////////////////
// CFdView::DibBufferInfoCreate

void CFdView::DibBufferInfoCreate(DibHeader far *DibBufferInfo, int BufferX, int BufferY)
{
int			Counter;
    //  set it up ourselves
    DibBufferInfo->Header.biSize = sizeof(BITMAPINFOHEADER);
    DibBufferInfo->Header.biPlanes = 1;
    DibBufferInfo->Header.biSizeImage = 0;
    DibBufferInfo->Header.biClrUsed = 0;
    DibBufferInfo->Header.biClrImportant = 0;
    DibBufferInfo->Header.biBitCount = 8;
    DibBufferInfo->Header.biCompression = BI_RGB;
    DibBufferInfo->Header.biWidth = BufferX;
    DibBufferInfo->Header.biHeight = BufferY;

    //	create an identity palette from the DIB's color table
    //	initialize the logical palette and DIB color table
    for(Counter = 0 ; Counter < 256 ; Counter++)
    {
        DibBufferInfo->aColors[Counter].rgbRed =
       		LogicalPalette.palEntry[Counter].peRed;
        DibBufferInfo->aColors[Counter].rgbGreen =
       		LogicalPalette.palEntry[Counter].peGreen;
        DibBufferInfo->aColors[Counter].rgbBlue =
       		LogicalPalette.palEntry[Counter].peBlue;
        DibBufferInfo->aColors[Counter].rgbReserved = 0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::BitmapCreate

HBITMAP CFdView::BitmapCreate(DibHeader *DibBufferInfo, HDC *hdcBuffer, void far **pBufferBits)
{
HBITMAP		hbm;
  if (*hdcBuffer)
  {
    //  Create a new 8-bit WinGBitmap with the new size
    DibBufferInfo->Header.biWidth = ViewXS;
    DibBufferInfo->Header.biHeight = ViewYS * Orientation;	//	BufferY * Orientation;
    hbm = WinGCreateBitmap(*hdcBuffer, (BITMAPINFO far *)DibBufferInfo, pBufferBits);
  }
  else
  {
		//  Fill the DibBufferInfo structure
  	if(WinGRecommendDIBFormat((BITMAPINFO far *)DibBufferInfo))
   	{
     	//  remember the orientation
     	Orientation = -1;	//	also Top-Down	//	DibBufferInfo->Header.biHeight;
     	TRACE("WinG recommended: %s bitmap blts\n", (Orientation == -1) ? "Top Down" : "Bottom Up");
   	}
		DibBufferInfoCreate(DibBufferInfo, ViewXS, ViewYS);	//	BufferX, BufferY);
    DibBufferInfo->Header.biHeight = BufferY * Orientation;
		//  Create a WinGDC
		*hdcBuffer = WinGCreateDC();
		//  Create the Bitmap
		hbm = WinGCreateBitmap(*hdcBuffer, (BITMAPINFO far *)DibBufferInfo, pBufferBits);
		//  Now, select away
  }
  return hbm;
}

/////////////////////////////////////////////////////////////////////////////
// CFdView message handlers

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnDraw

void CFdView::OnDraw(CDC* pDC)
{
	CFdDoc* pDoc = GetDocument();

	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnSize

void CFdView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if ((cx <= 0) || (cy <= 0)) return;
	TRACE("CFdView::OnSize: nType = %d, cx = %d, cy = %d\n", nType, cx, cy);
	GrowthX = long_divide((long)cx * 1000l + 500l,(long)BufferX);
	GrowthY = long_divide((long)cy * 1000l + 500l,(long)BufferY);
	BufferX = cx; BufferY = cy;
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnPaint

void CFdView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	// Do not call CView::OnPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnEraseBkgnd

BOOL CFdView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	
	return CView::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnKillFocus

void CFdView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	if (pNewWnd)
	if (pNewWnd->GetParentFrame() == GetParentFrame()) return;
CDC		*pDC=GetDC();
	SetSystemPaletteUse(pDC->m_hDC, SYSPAL_STATIC);
   	CPalApp.UnrealizeObject();
	SetSysColors(21, sysNames, sysColors);
	ReleaseDC(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnSetFocus

void CFdView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	if (fds == NULL)
	{	if (fdsActive)
			fds = new CFdStatus(this->m_hWnd, fdsRect);
	}
	if (fdm == NULL)
	{	if (fdmActive)
			fdm = new CFdMap(this->m_hWnd, fdmRect);
	}
	if (pOldWnd)
	if (pOldWnd->GetParentFrame() == GetParentFrame()) return;
int		nMapped;
CDC		*pDC=GetDC();
	SetSystemPaletteUse(pDC->m_hDC, SYSPAL_NOSTATIC);
  CPalApp.UnrealizeObject();
	pDC->SelectPalette(&CPalApp, FALSE);
	nMapped = pDC->RealizePalette();
	TRACE("Number of entries mapped at CFdView::OnSetFocus = %d\n", nMapped);
	//	replace each existing system color with the nearest matching
	//	color from our new palette; the result may look odd but should
	//	provide contrast between adjacent colors
//int		Counter;
//	for (Counter=0 ; Counter < 21 ; Counter++)
//	{
//		sysValues[Counter] = pDC->GetNearestColor(sysColors[Counter]);
//		TRACE("Color #%d (%08lX) = %08lX\n",Counter,sysColors[Counter],sysValues[Counter]);
//	}
	SetSysColors(21, sysNames, sysValues);
	ReleaseDC(pDC);
}

void CFdView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CView::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}

void CFdView::OnUpdateOptionsCrosshairs(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (CrossHairs) pCmdUI->SetCheck(TRUE); else pCmdUI->SetCheck(FALSE);
}

void CFdView::OnOptionsCrosshairs()
{
	// TODO: Add your command handler code here
CString		tmp;
	CrossHairs = 1 - CrossHairs;
	if (CrossHairs) tmp = "Yes"; else tmp = "No";
	AfxGetApp()->WriteProfileString("Preferences", "CrossHairs", tmp);
}

void CFdView::OnFileQuit()
{
	// TODO: Add your command handler code here
	GetDocument()->SetModifiedFlag(FALSE);
	GetParentFrame()->PostMessage(WM_CLOSE, 0, 0L);
}

long CFdView::OnMmMomOpen(WPARAM thmOut, LPARAM NotUsed)
{
	return 1;
}

long CFdView::OnMmMomDone(WPARAM thmOut, LPARAM tmhOut)
{

	return 1;
}

long CFdView::OnMmMomClose(WPARAM thmOut, LPARAM NotUsed)
{

	return 1;
}

void CFdView::OnOptionsVolume()
{
	// TODO: Add your command handler code here
	CVolDlg volDlg;
	volDlg.DoModal();

}
