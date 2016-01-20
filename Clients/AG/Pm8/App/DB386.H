typedef struct
{
	PBOX Pbox;
	DWORD scaled_source_x0;
	DWORD scaled_source_y0;
	DWORD scaled_page_res_denominator;
	USHORT x_resolution;
	USHORT y_resolution;
	SHORT destination_x0;
	SHORT destination_y0;
	USHORT scale_numerator;
	USHORT scale_denominator;
	BOOL fExpandIfEmpty;
} PboxToScreen32Data;

typedef struct
{
	PCOORD page_x;
	DWORD scaled_source_x0;
	DWORD scaled_page_res_denominator;
	USHORT x_resolution;
	SHORT destination_x0;
	USHORT scale_numerator;
	USHORT scale_denominator;
} PageXToScreen32Data;


extern "C" void pascal PboxToScreen32(PboxToScreen32Data far *pData);
extern "C" void pascal PageXToScreen32(PageXToScreen32Data far *pData);
extern BOOL g_fProcessorIs386Compatible;

// These are mirrored in CFIXED.H.
extern "C" long pascal MulFixed(long A, long B);
extern "C" long pascal DivFixed(long A, long B);
extern "C" long pascal ModFixed(long A, long B);
extern "C" long pascal MulDivFixed(long A, long B, long C);
// Compute (2*A+B)/3.
extern "C" long pascal OneThirdFixed(long A, long B);
extern "C" long pascal MidPointFixed(long A, long B);

// C versions defined in CFIXED.CPP, but use this .H file.
extern "C" long pascal MulDivLong(long A, long B, long C);
extern "C" long pascal SafeMulDivLong(long A, long B, long C);
