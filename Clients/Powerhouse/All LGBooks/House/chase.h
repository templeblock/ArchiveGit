#ifndef _CHASE_H_
#define _CHASE_H_

#include "house.h"
#include "chasedef.h"

#define MAX_LAUNDRY_SPRITES	15
#define CHASE_TIMER		2222

#define WALK_UP			0 
#define WALK_DOWN		1
#define WALK_LEFT		2
#define WALK_RIGHT		3

typedef struct _laundry
{
	LPSTR		lpGarmentName;
	int			nCells;
	BOOL		fGarmentInUse;
	LPSPRITE	lpSprite;

} LaundryList, FAR *LPLaundryList;


#define MAX_FURNITURE	10
typedef struct _Furniture
{
	FNAME		szImage;
	int 		x;
	int 		y;
	LPSPRITE	lpSprite;
	
} Furniture, FAR *LPFurnitureList;


typedef struct _ClothesSprites
{
	LPSPRITE	lpClothesSprite;
	BOOL 		fCaught;
	
} ClothingSprites;


class FAR CChaseScene : public CGBScene
{
public:
	// constructors/destructor
	CChaseScene(int nNextSceneNo);
	~CChaseScene();
	void OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	virtual void ToonInitDone();
	void SaveFurnitureBmp (LPSTR lpFilename, int nIndex);
	void SaveFurnitureX (int iX, int nIndex);
	void SaveFurnitureY (int iY, int nIndex);

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	BOOL OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
	UINT OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice );
	void OnTimer(HWND hWnd, UINT id);
	void PlayIntro();
	int GetLevel();
	void ChangeLevel(int iLevel);
	BOOL LoadTheLaundry(LPLaundryList lpLaundryList);
	BOOL LoadFurniture (LPFurnitureList lpFurnitureList);
	void AnnGotOne(unsigned int TheCaught);
	void MakeMove(short *XPos,short *YPos,short x,short y, LPSPRITE lpSprite=NULL);
	void MakeMove2(short *XPos,short *YPos,short x,short y);
	BOOL CheckCollision();
	BOOL IsAnnToDoor();
	int GetQuadrant(int x, int y);
	void GetNewLocation (short *NewX, short * NewY, int iAnnQuad, int iMyQuad, RECT rAnn, RECT rMe);
//j	BOOL MoveAwayFromAnn (short *iNewX, short *iNewY, RECT rAnnLoc, RECT rMyLoc);
	BOOL FurnitureCollision (LPSPRITE lpAnnSprite, RECT rLoc, UINT nKey);
	void RestartLevel();
	BOOL CreateAnnSprites();
	BOOL CreateDoorSprites();
	void PlayLevelSuccessWave();
	BOOL ClothesCaught (LPSPRITE lpSprite);
	void ClothingOutWindow();
	BOOL GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY);
	void AnnNoCarry();
	void PlayClickWave (BOOL fSync=TRUE);

public: // add variables to use with parser here

	int 		ANNS_MESS_SIZE;
	STRING 		ANN_WALK_R;
	STRING 		ANN_WALK_U;
	STRING 		ANN_WALK_L;
	STRING 		ANN_WALK_D;

	STRING 		ANN_GET_R;
	STRING 		ANN_GET_U;
	STRING 		ANN_GET_L;
	STRING 		ANN_GET_D;

	STRING 		ANN_BDOOR_OPEN;
	STRING 		ANN_BDOOR_SHUT;

	int 		DOOR_X_SHUT;
	int 		DOOR_Y_SHUT;
	
	int			DOOR_X_OPEN;
	int 		DOOR_Y_OPEN;

	int 		ANN_STEP;

	int 		ANN_X;
	int 		ANN_Y;
	int 		ANN_WALK_CELLS;
	int			ANN_CARRY_CELLS;

	int 		LEVEL_PIECES;
	
	FNAME 		m_szIntroWave;
	FNAME		m_szCatchWave;
	FNAME 		m_szSuccessPlay;						// Success wave for Play mode
	FNAME		m_szDoorCloseWave;
	FILE_SPEC	m_szSuccessLevel[MAX_SUCCESSWAVES];		// Success waves for level

	POINT		m_ptOutWindow;
	POINT		m_ptBasketLoc;							// Laundry basket coordinates
	POINT		m_ptLaundryStart;
	BOOL		m_fCatchByDoor;

private:
	HCURSOR		m_hHotCursor;
	int			m_iLevel;
	HMCI		m_hSound;

	LPSPRITE	m_lpAnnUpSprite;  
	LPSPRITE	m_lpAnnDownSprite; 
	LPSPRITE	m_lpAnnLeftSprite; 
	LPSPRITE	m_lpAnnRightSprite; 

	LPSPRITE	m_lpAnnGetUpSprite; 
	LPSPRITE	m_lpAnnGetDownSprite;
	LPSPRITE	m_lpAnnGetLeftSprite;
	LPSPRITE	m_lpAnnGetRightSprite;

	LPSPRITE	m_lpAnnBackDoorOpenSprite;
	LPSPRITE	m_lpAnnBackDoorShutSprite;

	LPSPRITE	m_lpLastAnnSprite;
	LPSPRITE	m_lpLastClothing;

	short 		LinenCleanedCount;
	unsigned int m_iLastCaught;
	int 		walkDir;
	short 		wLevelPiecesNeeded;
	BOOL		m_fCarrying;
	BOOL		m_fLevelComplete;
	Furniture	m_Furniture[MAX_FURNITURE];
	POINT		m_ptMouse;
	BOOL		m_fLButtonDown;
	BOOL		m_fInitializing;
	BOOL		m_fClothingOutWindow;
	int			m_nClothingOutWindowCnt;
	SoundPlaying m_SoundPlaying;
};

typedef CChaseScene FAR * LPCHASESCENE;
                            
#endif // _CHASE_H_
