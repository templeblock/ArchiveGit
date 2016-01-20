#ifndef GAMMA_TABLE
#define GAMMA_TABLE

#include <afxwin.h>
#include "colorconvert.h"
#include "gamma.h"

//  Define error type
#define GAMMA_TABLE_NOT_SUPPORTED 	(-1)

class GammaTable {
	protected:

	// Variables.
	WORD  m_orgGammaLut[256*3];
	WORD  m_calGammaLut[256*3];

	public:

	static BOOL isMillionAvailable(CDC* pDevice);
	
	// This function determines if theGDevice supports gamma tables.
    //	Returns TRUE if it does and FALSE if otherwise.
    // • Note: use this function to see if you can alter gamma tables
	// 	before any object instantiation. 
	static BOOL isGammaAvailable(CDC* pDevice);
	
	GammaTable(); 
	~GammaTable(); 		
	BOOL Init(CDC *pDevice);
	
	// Save a copy of the current gamma table.
	// When this is called the first time it saves to the originalTable
	// (this is done in initialize(.) ).
	// In subsequent calls it saves to the hackedTable.
	// So for all practical intents and purposes, the result will be in hackedTable.
	BOOL setCurrentTable(Gamma *gamma);

	// Restores original gamma table saved in originalTable.
	BOOL loadOriginalTable(CDC *pDevice);
	BOOL loadCalibratedTable(CDC *pDevice);

	
	// Create a gamma table and make it the current device gamma table.
	// ratio is the ratio  of the intensity of the R, G and B channels.
	// 	This triplet is used to balance the three guns if they are really
	//		off.  E.g. if the R gun is 2x too strong, you specify (0.5, 1.0, 1.0).
	//		these numbers have will be normalized so that only the ratios will be
	// 	significant.
	// target gamma is the desired nonlinearity of the monitor+gamma correction table.
	// monitor gamma is the nonlinearity of the monitor itself.
	// correction (e) is the desired nonlinearity in the gamma table. 
	// 	Since the monitor itself has a nonlinear function relating voltage
	// 	(v) to intensity (i) with an exponent of about 2.5: i ≈ v^2.5, the 
	//		function relating RGB code (c) to voltage must be: v ≈ c^(gamma/2.5) = c^e.  
	// 	For all intents and purposes this function is the gamma table.  
	// 	The default QuickDraw gamma table has an e≈1/1.4 ≈ 0.7, 
	// 	which makes gamma≈1.8.
	//	 chanCount is number of channels in the table.  If you want one table for
	// 	all three primaries, specify 1.  If you want separate tables for each
	// 	primary, specify 3.
//	OSErr createGammaTable(RGBNumber ratio, RGBNumber targetGamma, 
//		RGBNumber monitorGamma, short chanCount);
}; 
	
#endif // #ifndef GAMMA_TABLE
