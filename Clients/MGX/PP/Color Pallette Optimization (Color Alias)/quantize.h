#ifndef _QUANTIZE_H_
#define _QUANTIZE_H_

#define TOTAL_BITS 18
#define HISTO_ENTRIES 262144 // 2 ^^ 18
#define CHANNEL_BITS 6
#define DOUBLE_BITS 12
#define SHIFT_BITS 2
#define MAXVAL 63
#define MINI(v) ((v) >> SHIFT_BITS)
#define MINIINDEX(r,g,b) (((((UINT)(r) << CHANNEL_BITS) | (UINT)(g)) << CHANNEL_BITS) | (UINT)(b))
#define INDEX(r,g,b) MINIINDEX(MINI(r), MINI(g), MINI(b)) 

typedef RGBTRIPLE FAR *LPRGBTRIPLE;

typedef struct _cube
{
	DWORD dwCount;
	DWORD dwPixelCount;
	BYTE r1, r2;
	BYTE g1, g2;
	BYTE b1, b2;
} CUBE, far *LPCUBE;

class CQuantize
{
public:
	// Constructor and Destructor
	CQuantize();
	~CQuantize();

	// implementation
	BOOL Init();
	void Reset();
	void CountFrequencies(LPRGBTRIPLE lpRGB, int nPixels);
	int CountColors();
	BOOL CreatePalette(LPRGBQUAD lpRGBMap, int nEntries, LPRGBQUAD lpInitialMap = NULL, int nInitialEntries = 0);

private:
	void InitCubes();
	BOOL SplitCube();
	LPCUBE MedianCut( LPCUBE lpLargestCube, BOOL fByPixels );
	void SumCube( LPCUBE lpCube );
	LPCUBE FindCubeWithMostPixels();
	LPCUBE FindCubeWithMostColors();
	LPCUBE SplitR(LPCUBE lpLargestCube, DWORD dwTargetCount, BOOL fByPixels);
	LPCUBE SplitG(LPCUBE lpLargestCube, DWORD dwTargetCount, BOOL fByPixels);
	LPCUBE SplitB(LPCUBE lpLargestCube, DWORD dwTargetCount, BOOL fByPixels);
	DWORD SumPlaneRByPixels(int r, LPCUBE lpCube);
	DWORD SumPlaneGByPixels(int g, LPCUBE lpCube);
	DWORD SumPlaneBByPixels(int b, LPCUBE lpCube);
	DWORD SumPlaneRByColors(int r, LPCUBE lpCube);
	DWORD SumPlaneGByColors(int g, LPCUBE lpCube);
	DWORD SumPlaneBByColors(int b, LPCUBE lpCube);
	void CalculatePalette(LPRGBQUAD lpMap, LPRGBQUAD lpInitialMap, int nInitialEntries);

	// data
private:
	LPDWORD	m_lpHisto;
	LPCUBE  m_lpCubes;
	int		m_nCubes;
};

#endif // _QUANTIZE_H_
