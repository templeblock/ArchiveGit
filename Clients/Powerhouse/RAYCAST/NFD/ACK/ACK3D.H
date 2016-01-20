/* Header file for ACK3D engine interface */
/* Author: Lary Myers */
/* modified: John Lundy */

typedef unsigned long	ULONG;
typedef unsigned int	UINT;
typedef unsigned char	UCHAR;

/*** Error codes returned from Ack... functions ***/
#define ERR_BADFILE	    		100
#define ERR_BADCOMMAND	    101
#define ERR_BADOBJNUMBER    102
#define ERR_BADSYNTAX	    	103
#define ERR_LOADINGBITMAP   104
#define ERR_BADDIRECTION    105
#define ERR_BADSTARTX	    	106
#define ERR_BADSTARTY	    	107
#define ERR_BADANGLE	    	108
#define ERR_BADMAPFILE	    109
#define ERR_READINGMAP	    110
#define ERR_BADPICNAME	    111
#define ERR_INVALIDFORM			112
#define ERR_NOPBM	    			113
#define ERR_BADPICFILE	    114
#define ERR_NOMEMORY	    	115
#define ERR_BADPALFILE	    116
#define ERR_BADWINDOWSIZE   117
#define ERR_TOMANYVIEWS	    118

#define INT_ANGLE_1		5
#define INT_ANGLE_2		11
#define INT_ANGLE_3		16
#define INT_ANGLE_4		21
#define INT_ANGLE_5		27
#define INT_ANGLE_6		32
#define INT_ANGLE_7		37
#define INT_ANGLE_8		43
#define INT_ANGLE_9		48
#define INT_ANGLE_10	53
#define INT_ANGLE_15	80
#define INT_ANGLE_20	107
#define INT_ANGLE_28	149
#define INT_ANGLE_30	160
#define INT_ANGLE_45	240
#define INT_ANGLE_60	320
#define INT_ANGLE_90	480
#define INT_ANGLE_120	640
#define INT_ANGLE_135	720
#define INT_ANGLE_180	960
#define INT_ANGLE_225	1200
#define INT_ANGLE_270	1440
#define INT_ANGLE_315	1680
#define INT_ANGLE_360	1920

#define GRID_SIZE			64		    /* Size of each square */
#define GRID_WIDTH		64		    /* Map width */
#define GRID_HEIGHT		64		    /* Map height */
#define GRID_MAX			(GRID_WIDTH * GRID_HEIGHT)
#define GRID_XMAX			(GRID_WIDTH * GRID_SIZE)
#define GRID_YMAX			(GRID_HEIGHT * GRID_SIZE)
#define GRID_ARRAY		(GRID_WIDTH+1) * (GRID_HEIGHT+1)

#define MAX_WALLBMPS	256		/* Total wall bitmaps allowed */
#define MAX_OBJBMPS		256		/* Total object bitmaps allowed  */
#define MAX_OBJECTS		255		/* Total objects in map */
#define MAX_ZONES			8			/* Number of light zones */
#define MAX_VIEWS			64		/* Total sides to an object */
#define MAX_DOORS	    10    /* Max opening or closing at one time  */

#define PAL_SIZE			4096	/* Shading palette ranges */

#define DEFAULT_DOOR_SPEED  2

#define WALL_SECRET_DOOR    0x8000	//	Wall that opens like door
#define DOOR_TYPE_SLIDE	    0x4000	//	Set = SLIDE, Clear = SPLIT
#define DOOR_LOCKED_MASK		0x3C00	//	Mask for anything that locks door
#define DOOR_LOCKED					0x2000	//	Door is locked, may need key...
#define DOOR_KEY_MASK				0x1C00	//	Mask for key color to unlock door
#define DOOR_LOCKED_RED    	0x1000	//	Red key unlocks
#define DOOR_LOCKED_GREEN  	0x0800	//	Green key unlocks
#define DOOR_LOCKED_BLUE   	0x0400	//	Blue key unlocks
#define WALL_WAS_SEEN				0x0200	//	This wall has been viewed by Player

#define DOOR_NTYPES				12
#define DOOR_XSIDE_DELTA	DOOR_NTYPES
#define DOOR_YSIDE_DELTA	DOOR_NTYPES * 2
#define DOOR_SIDECODE	    220		/* Map codes for the various doors */
#define DOOR_XCODE	    	DOOR_SIDECODE + DOOR_NTYPES
#define DOOR_YCODE	    	DOOR_XCODE + DOOR_NTYPES

#define TYPE_WALL					0
#define TYPE_OBJECT				1

#define OF_PASSABLE				0x8000	/* Object can be walked thru */
#define OF_ANIMATE				0x4000	/* Object bitmaps are animated */
#define OF_SEEN_PLAYER		0x2000	//	Object has seen Player
#define OF_KEY_RED				0x1000	//	Unlocks RED lock
#define OF_KEY_GREEN			0x0800	//	Unlocks GREEN lock
#define OF_KEY_BLUE				0x0400	//	Unlocks BLUE lock
#define OF_CAN_PICKUP			0x0200	//	Object can be picked up
#define OF_TAKERS					0x01C0	//	Combination of Healtn, Ammo and Weapon
#define OF_HEALTH					0x0100	//	Health
#define OF_AMMUNITION			0x0080	//	Ammunition
#define OF_WEAPON					0x0040	//	Weapon
#define OF_STATIC					0x0020	//	Just sits there, can be ignored
#define OF_ONE_SHOT				0x0010	//	One shot Objects that deactivate on impact
#define OF_WAS_SEEN				0x0008	//	Object was seen by Player
#define OF_FORCE_FIELD		0x0004	//	Object is damaging force field

#define KEY_RED				0x01	//	RED key held
#define KEY_GREEN			0x02	//	GREEN key held
#define KEY_BLUE			0x04	//	BLUE key held

#define SHADING_ON			1		/* Used in LightFlag */
#define SHADING_OFF			0

#define OBJECT_ACTIVE		1		/* Active and may be moveable */
#define OBJECT_INACTIVE 0		/* Won't be considered moveable */

#define PLAYER_OBJECT		0		/* Object number reserved for Player */

#define POV_NOTHING			0		/* Nothing was hit */
#define POV_XWALL				1		/* An X wall was hit */
#define POV_YWALL				2		/* A Y wall was hit */
#define POV_OBJECT			3		/* An object was hit */
#define POV_PLAYER			4		/* The player was hit by object */
#define POV_DOORX_REG		5		/* An X door was approached */
#define POV_DOORY_REG		6		/* A Y door was approached */
#define POV_DOORX				7		/* An X door was hit */
#define POV_DOORY				8		/* A Y door was hit */

#define POV_NODOOR			0		/* No door was opened */
#define POV_XDOOR				1		/* An X door was opened */
#define POV_YDOOR				2		/* A Y door was opened */
#define POV_XSECRETDOOR 3		/* An X secret door was opened */
#define POV_YSECRETDOOR 4		/* A Y secret door was opened */

#define POV_DOORLOCKED	0x80	/* Bit set if door is locked */

#define MAX_HEALTH			5000
#define MAX_WEAPONS			16		//	All weapons, even Monsters
#define FISTS						0
#define PISTOL					1
#define SHOTGUN					2
#define MACHINEGUN			3
#define LAUNCHER				4

typedef struct
{
		char	Name[32];	//	Name, for status screen
		int		MaxAmmo;	//	Max ammunition it will hold
		int		Range;		//	How far will it shoot
		int		Spread;		//	How wide an area projectiles will Damage
		int		Rounds;		//	How many projectiles are released per firing
		int		Used;			//	Amount of ammo used per firing
		int		Damage;		//	How much damage it will cause on impact
		int		Delay;		//	Delay between firings
		int		Recoil;		//	How much power in weapon recoil
} WEAPON;

typedef struct
{
		int		Possessed;	//	Have this weapon
		int		*Ammo;			//	Have this much ammo for weapon
} ARSENAL;

typedef struct
{
    int		bmNum[MAX_VIEWS];	//	Bitmap numbers for this Object
    char	mvFlg[MAX_VIEWS];	//	Object can move during this bitmap
    char	bmDsp[MAX_VIEWS];	//	Display count max
    char	bmCnt;			//	Display count for this bitmap
    char	IdlNum;			//	Idle Object counter
    char	IdlMax;			//	Idle Object max bitmap
    char	MovNum;			//	Moving Object counter
    char	MovMax;			//	Moving Object max bitmap
    char	CurNum;			//	Current bitmap being displayed
    char	Sides;			//	Number of display sides for this Object
    UCHAR	SkNum;			//	Counter for Seeks
    UCHAR	SksMax;			//	Number of times an Object moves in one Dir
    char	Speed;			//	Object movement speed
    char	Weapon;			//	Weapon this object carries
    char	FireFreq;		//	How often the Object can fire weapon (somewhat random)
    char	FireDly;		//	How long after firing before next fire
    int		Damage;			//	Damage done if Object touches Player
    int		FieldOV;		//	Half the total field of view
    int		CheckDist;	//	How close to an Object can we get
} ANIMATION;

typedef struct
{
    char			Active;			//	Is this Object active
		int				bmNum;			//	Bitmap number of this Object
    UINT			Flags;			//	OF_xxxxx bit flags
    long			Health;			//	Total health of Object
    int				Dead;				//	Player is in various stages of death
    int				Ammo;				//	Unused ammo carried for weapon
    UCHAR			Enemy;			//	What object this Object is fighting
    int				Dir;				//	Direction Object is facing/moving
		union
		{   struct
			{	int		xlow;
	    	int		x;
	    };
	    long		xl;
		};
		union
		{   struct
			{	int		ylow;
	    	int		y;
	    };
	    long		yl;
		};
    int				mPos;
} OBJECTS;

#define DOOR_OPENING	0x80		/* On if door is currently opening */
#define DOOR_CLOSING	0x40		/* On if door is currently closing */

typedef struct
{
		int			mPos;
		int			mPos1;
		UINT		mCode;
		UINT		mCode1;
		UCHAR		ColOffset;
		char		Speed;
		UCHAR		Type;
		UCHAR		Flags;
} DOORS;

typedef struct
{
		int			Number;		//	Bitmap number of floor tile
		int			xPos;			//	X Position
		int			yPos;			//	Y Position
		int			Distance;	//	Distance to this column
		UINT		Offset;		//	Row to start drawing floor tile
} FLOORARRAY;

typedef struct
{
		int	    Number;		/* Bitmap number of this wall  */
		int	    Column;		/* Column of bitmap to display */
		int	    Distance;	/* Distance to this column     */
		UCHAR   LightAdj;	/* Light adjustment flag       */
} WALLARRAY;

typedef struct
{
	UCHAR		Number;			/* Number of Object */
	long		Distance;		/* Distance to Object */
	long		DeltaX;
	long		DeltaY;
	int			ColBeg;			//	Beginning Column
	int			ColEnd;			//	Ending Column
	UCHAR far *oMap;		//	Bitmap to draw
} ORDERARRAY;

typedef struct
{
	char far*		BS_Pointer;
	UINT				BS_Start;
	int					BS_Length;
} MAGICARRAY;

typedef struct
{
    int	    mPos;		/* Map Position */
    UINT    mCode;	/* Special code (start,up,down,goal) */
} SPECIALCODE;

#define		AE_VERSION		0x010B
#define		AE_DESCRIPTOR	"WA file"
typedef struct {

		char		Descriptor[8];

		union
		{	struct
			{	char	VersionLow;
				char	VersionHigh;
			};
			int		Version;
		};

		long		ElapsedTime;
		long		NumberOfPaints;

		char		MapFileName[13];

	  UINT  	xGrid[GRID_ARRAY];  /* Map for X walls */
	  UINT  	yGrid[GRID_ARRAY];  /* Map for Y walls */
		UCHAR		LightMap[GRID_MAX];	/* Independent darker areas */

		ARSENAL	WeaponsCarried[MAX_WEAPONS];
		int			Fists;
		int			Bullets;
		int			Shells;
		int			Rockets;

		UCHAR		Keys;					/* Keys held by Player */

	  int			DoorSpeed;	  /* Door open/close speed */

	  UCHAR		TopColor;	    /* Base color of ceiling */
	  UCHAR		BottomColor;	/* Base color of floor */

	  UCHAR		LightFlag;	    /* 0 = no light shading, 1 = ON */

	  int			WinStartX;	    /* Value of left side of viewport */
	  int			WinStartY;	    /* Value of top side of viewport */
	  int			WinEndX;	    	/* Value of right side */
	  int			WinEndY;	    	/* Value of bottom side */
	  int			CenterRow;	    /* Value of (WinEndY-WinStartY)/2 */
	  int			WinWidth;	    	/* Value of WinEndX - WinStartX */
	  int			WinHeight;	    /* Value of WinEndY - WinStartY */
	  UINT		WinLength;	    /* Number of dwords in window */
	  UINT		WinStartOffset;	/* Value of WinStartY * 320 */
	  DOORS		Door[MAX_DOORS];/* Doors moving at one time */
		int			MaxObjects;	    /* Total objects in map */
		OBJECTS	ObjList[MAX_OBJECTS];	/* Current objects in map */

} ACKENG;

/******* Prototypes *********/

int AckInitialize(ACKENG *ae);
/* Reads trig files, builds wall and object maps, general initialize */
/* Also sets up distance table */

int AckReadMapFile(ACKENG *ae,char *MapFileName);
/* Reads map file and setups map grids */

int AckLoadBitmap(ACKENG *ae,int BitmapNumber,int BitmapType,char *bmFileName);
/* Loads a bitmap and places in either bMaps or oMaps based on bmType */

HGLOBAL AckReadiff(char *FileName);
/* Reads in a .LBM or .BBM file and returns a buffer */

int AckLoadWall(ACKENG *ae,int WallNumber,char *bmFileName);
/* Loads a wall bitmap and places it into the wall array */

int AckLoadObject(ACKENG *ae,int BmpNumber,char *bmFileName);
/* Loads an object bitmap and places it into the object array */

int AckCreateObject(ACKENG *ae,int ObjNumber,int NumBitmaps,UCHAR *bmNums,char *mvFlgs, char *bmDsps);
/* Fills in ObjList structure with information passed */

int AckBuildBackground(ACKENG *ae);
/* Builds background buffer from TopColor, BottomColor, and LightFlag */

int AckBuildView(ACKENG *ae);
/* Draws the current view into ScreenBuffer */

void AckDrawOverlay(UCHAR far *Screen,UCHAR far *Overlay);
/* Places the overlay graphics on top of the screen */

int AckCheckHit(ACKENG *ae,int xPlayer,int yPlayer,int ViewAngle,int *Amount,int CheckDist);
/* Checks if a collision occurs */

int AckCheckDoorOpen(ACKENG *ae,int xPlayer,int yPlayer,int PlayerAngle);
/* Check if a door in front to open */

int AckCheckObjHit(ACKENG *ae,int ObjIndex,int xPlayer,int yPlayer,int PlayerAngle,int Spread,int *Distance);
/* Used to check for gunshot hit on objects */

int AckObjectAngle(ACKENG *ae,int ObjIndex,int xObject,int yObject);
/* Used to find the angle toward the Player */

int AckCheckObjView(ACKENG *ae,int Enemy,int ObjIndex,int xObject,int yObject,int ObjectAngle,int Spread);
/* Used to check for gunshot hit on Enemy */

int AckCheckObjPosn(ACKENG *ae,int xPlayer,int yPlayer,int PlayerAngle,int Amount);
/* Used by AckMovePOV() and AckMoveObjectPOV() to check collision with objects */

void AckCheckObjectMovement(ACKENG *ae);
/* Runs the list of objects to check movement */

int AckMovePOV(ACKENG *ae,int Angle,int Amount);
/* Moves the POV by the specified amount at the specified angle */

int AckMoveObjectPOV(ACKENG *ae,int ObjIndex,int Angle,int Amount);
/* Moves the object POV by the specified amount at the specified angle */

int AckGetObjectHit(void);
/* Returns the object index number of the last object hit */

int AckGetWallHit(void);
/* Returns the map location of the last wall hit */

int AckDeleteObject(ACKENG *ae,int ObjectIndex);
/* Removes the specified object from the map */

HGLOBAL AckCopyNewBitmap(UCHAR far *OldBitmap);
/* Creates a new bitmap buffer and copies OldBitmap into it */

UCHAR far *AckGetBitmapPtr(int BitmapNumber,UCHAR far **Maps);
/* Returns a real memory pointer to the specified bitmap */

int AckWrapUp(ACKENG *ae);
/* Frees memory buffers */

