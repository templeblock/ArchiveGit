#ifndef WDEBUG_P
#define WDEBUG_P

void WeaponDebugStart(HWND hWnd, CWeapon *_pWeapon, LPVIDEO lpVid);
void WeaponDebugEnd(void);

int GetWDPlayerZ( long lFrame );
void GetWDWalls(long lFrame, int *yGround, int *nNumWalls, WALL *pWalls);
void GetWDEnemy(long lFrame, int *nNumEnemy, ENEMY *pEnemy);
void GetWDCommand(long lFrame, int *nCommand);

#endif
