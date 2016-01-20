#include "phtypes.h"
 
#define IDC_VRECO_MSG WM_USER+1

BOOL VoiceRecoInit(HWND hWnd);
void VoiceRecoClose(void);
long ProcessICSSReplyMsg(HWND hWnd, UINT wFuncId, LONG lData);
BOOL VoiceRecoOn(HWND hWnd);
void VoiceRecoOff(void);
HWND FindClassDescendent(HWND hParent, LPCTSTR lpClassName);
