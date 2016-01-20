/*		   ACK-3D ( Animation Construction Kit 3D )		      */
/* External Reference */
/* Author: Lary Myers */
/* Modified: John Lundy */

extern	int 	    	LastX1;
extern	int 	    	LastY1;
extern	int		    	iLastX;
extern	int		    	iLastY;
extern	int					ViewWidth;
extern	int					ViewHeight;
extern	int					ViewX;
extern	int					ViewX1;
extern	int					ViewXS;
extern	int					ViewY;
extern	int					ViewY1;
extern	int					ViewYS;
extern	int					BytesPerRow;
extern	int					DWordsPerRow;
extern	int	    		MaxDistance;
extern	int	    		ErrorCode;
extern	int	    		xMapPosn;
extern	int	    		yMapPosn;
extern	HGLOBAL			hGrid;
extern	HGLOBAL			hObjGrid;
extern	UINT  far   *Grid;
extern	UINT  far   *ObjGrid;
extern	SPECIALCODE far SpecialCodes[];
extern	int	    		TotalSpecial;
extern	int far 		DistanceTable[];
extern	HGLOBAL			hAdjustTable;
extern	long  far   *AdjustTable;
extern	WALLARRAY 	far   Walls[];
extern	FLOORARRAY	far		xFloors[];
extern	FLOORARRAY	far		yFloors[];
extern	FLOORARRAY	far		Floors[];
extern	int					xTiles;
extern	int					yTiles;
extern	int					Tiles;
extern	int	    		TotalSecret;
extern	int	    		ViewColumn;
extern	HGLOBAL			hSinTable;
extern	HGLOBAL			hCosTable;
extern	HGLOBAL			hLongTanTable;
extern	HGLOBAL			hLongInvTanTable;
extern	HGLOBAL			hInvCosTable;
extern	HGLOBAL			hInvSinTable;
extern	HGLOBAL			hArcCosTable;
extern	HGLOBAL			hArcSinTable;
extern	HGLOBAL			hViewCosTable;
extern	HGLOBAL			hxNextTable;
extern	HGLOBAL			hyNextTable;
extern	HGLOBAL			hDeflectTable;
extern	long far    *SinTable;
extern	long far    *CosTable;
extern	long far    *LongTanTable;
extern	long far    *LongInvTanTable;
extern	long far    *InvCosTable;
extern	long far    *InvSinTable;
extern	int  far    *ArcCosTable;
extern	int  far    *ArcSinTable;
extern	long far    *ViewCosTable;
extern	long far    *xNextTable;
extern	long far    *yNextTable;
extern	int far			*DeflectTable;

extern	int	    		TotalObjects;
extern	int					GoreObject;
extern	UCHAR far		PalTable[];

extern	int	    		LastObjectHit;
extern	int					LastWallHit;
extern	int					LastObjectDistance;
extern	int	    		LastMapPosn;
extern	int					LastDoorDetail;

extern	ORDERARRAY	far ObjectOrder[MAX_OBJECTS];

extern	HGLOBAL far	hBmScrn;
extern	MAGICARRAY	far pBmScrn[MAX_DISTANCE];
extern	int far			BS_Floor[MAX_DISTANCE];
extern	int far			BS_Ceiling[MAX_DISTANCE];

extern	WEAPON far	Weapons[MAX_WEAPONS];
extern	int					MaxWeaponsInUse;

extern	HGLOBAL far	hbMaps[MAX_WALLBMPS];
extern	UCHAR far*	far bMaps[MAX_WALLBMPS];
extern	HGLOBAL far	hoMaps[MAX_OBJBMPS];
extern	UCHAR far*	far oMaps[MAX_OBJBMPS];

extern	UCHAR far		*ScreenBuffer;
extern	HGLOBAL			hBkgdBuffer;
extern	UCHAR far		*BkgdBuffer;

extern	ANIMATION far*	far ObjList[MAX_OBJBMPS];
