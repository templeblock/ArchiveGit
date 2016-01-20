// 
// 	include file for step.c
//
// 	A step slider
//

/**********************************************************************/
// Step control styles
/**********************************************************************/
#define ST_HORZ 	1L

/**********************************************************************/
// Step control window words
/**********************************************************************/
#define GWW_STPOS		0
#define GWW_STTICKS		2
#define GWW_STMIN		4
#define GWW_STMAX		6
#define GWL_STCOLOR	8

/**********************************************************************/
// Public Routines
/**********************************************************************/
	// Call InitStepSlider before using the slider
BOOL InitStepSlider(HWND hWnd, int id, int nTicks, int minv, int maxv, BOOL bHorz);
	// must call SetStepSlider after recieving a command from the slider
void SetStepSlider(HWND hDlg, int id, int pos);
void SetStepSliderColor(HWND hDlg, int id, int R, int G, int B);
BOOL RegisterStepClass( HANDLE hInstance );
long pascal far StepSliderControl( HWND hWnd, unsigned msg, WORD wParam, long lParam );
void DrawStepSlider(HDC hDC, HBRUSH hFillBrush, LPRECT lpRect, int inl, int bwl,int nTicks, int pix, int oldpix, BOOL bHorz, BOOL bHasFocus);
