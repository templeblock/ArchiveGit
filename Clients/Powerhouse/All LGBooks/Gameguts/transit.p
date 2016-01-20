
typedef void (CALLBACK* TRANSCALLBACK)( DWORD dwIteration, DWORD dwTotalIterations, DWORD dwCallbackData );

typedef struct _transparms
{
	UINT	Transition;
	HDC		hDstDC;
	int 	dstLeft;
	int		dstTop;
	int 	dstWidth;
	int		dstHeight;
	int		srcLeft;
	int		srcTop;
	int		srcWidth;
	int		srcHeight;
	PDIB	lpSrcDib;
	HDC		hSrcDC;
	int		nTicks;
	int		nStepSize;
	DWORD	dwTotalTime;
	TRANSCALLBACK lpCallback;
	DWORD	dwCallbackData;
} TRANSPARMS, FAR *LPTRANSPARMS;

void TransitionBlt( UINT Transition, HDC hDstDC, int dstLeft, int dstTop, int dstWidth, int dstHeight,
	int srcLeft, int srcTop, int srcWidth, int srcHeight, PDIB lpSrcDib, HDC hSrcDC,
	int nTicks = -1, int nStepSize = 0, DWORD dwTotalTime = 0 );
void TransitionBlt( LPTRANSPARMS lpTransParms );
void SetTransitionParms( LPTRANSPARMS lpTransParms, UINT Transition, HDC hDstDC, int dstLeft, int dstTop, int dstWidth, int dstHeight,
	int srcLeft, int srcTop, int srcWidth, int srcHeight, PDIB lpSrcDib, HDC hSrcDC);
void SetTransitionTiming( LPTRANSPARMS lpTransParms, int nTicks = -1, int nStepSize = 0, DWORD dwTotalTime = 0);
void SetTransitionCallback( LPTRANSPARMS lpTransParms, TRANSCALLBACK lpCallback = NULL, DWORD dwCallbackData = 0);
