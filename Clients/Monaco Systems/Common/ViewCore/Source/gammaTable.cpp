#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "gammaTable.h"

//check to see the system is running under millions of colors
BOOL GammaTable::isMillionAvailable(CDC* pDevice) 
{
	long hasmode;
	
	return TRUE;
}

//check to see the gamma table can be altered
BOOL GammaTable::isGammaAvailable(CDC* pDevice) 
{
	WORD GammaLut[256*3];

	BOOL bSupported = GetDeviceGammaRamp(pDevice->GetSafeHdc(), GammaLut);
	if (bSupported)
	{
		bSupported = SetDeviceGammaRamp(pDevice->GetSafeHdc(), GammaLut);
	}
		
	return bSupported;
}

GammaTable::GammaTable() 
{
}

GammaTable::~GammaTable() 
{
}

// Do initialization here.
// If not using parametered constructor, call this function first.
BOOL GammaTable::Init(CDC *pDevice) 
{
	BOOL bSupported = GetDeviceGammaRamp(pDevice->GetSafeHdc(), m_orgGammaLut);

	memcpy((char*)m_calGammaLut, (char*)m_orgGammaLut, sizeof(WORD)*256*3);
	return bSupported;
}

// Set gamma table.
BOOL GammaTable::setCurrentTable(Gamma *gamma)
{
	WORD	*pLut;
	int lutSize;

	pLut = (WORD*)gamma->GetGammaLut(&lutSize);
	if(!pLut)	return FALSE;

	memcpy((char*)m_calGammaLut, (char*)pLut, sizeof(WORD)*3*lutSize);
	return TRUE;
}

// load original gamma table.
BOOL GammaTable::loadOriginalTable(CDC *pDevice) 
{
	// In case the original gamma table wasn't saved
	return SetDeviceGammaRamp(pDevice->GetSafeHdc(), m_orgGammaLut);
}

// load calibrated gamma table 
BOOL GammaTable::loadCalibratedTable(CDC *pDevice) 
{
	// In case the original gamma table wasn't saved
	return SetDeviceGammaRamp(pDevice->GetSafeHdc(), m_calGammaLut);
}

/*
#define MAX(x,y) ((x)>(y)?(x):(y))

// Create a gamma table with the desired maximum level, exponent, 
// and number of channels.  See header for explanation of these values.

OSErr GammaTable::createGammaTable(RGBNumber ratio, RGBNumber targetGamma,
	RGBNumber monitorGamma, short chanCount) {
	short err=0;

	// If device can't alter gamma tables, return.
	if (!isGammaAvailable(theGDevice)) return(-1);
	
	// Hasn't been initialized yet.
	if (theGDevice == nil) return(-1);

	// Invalid number of channels.
	if ((chanCount!=1)&&(chanCount!=3)) return(-1);

	short size;
	GammaTblHandle theTable;
	
	// Calculate the required size of the new gamma table and get a handle to it.
	// Use the following fixed values.  Can we derive these values based on
	// hardware/driver config?  Can't find any docs on this.  But the default
	// system table uses these values.
	#define FORMULA_SIZE			0						// formula size in bytes
	#define DATA_COUNT			256					// number of entries per channel
	#define DATA_WIDTH			8                  	// data width in bits

	size = sizeof(GammaTbl)	+ FORMULA_SIZE
			+ (chanCount * DATA_COUNT * DATA_WIDTH / 8);

	theTable = (GammaTblHandle) NewHandle(size);
	if (theTable == nil) return(err = MemError());

#ifdef DEBUG
	ofstream outf("createGammaTable1.out",ios::out|ios::trunc);
#endif

	// Fill in  table.
	(*theTable)->gVersion 		= 0;
	(*theTable)->gType 				= 0;
	(*theTable)->gFormulaSize	= FORMULA_SIZE;
	(*theTable)->gChanCnt 		= chanCount;
	(*theTable)->gDataCnt 		= DATA_COUNT;
	(*theTable)->gDataWidth 	= DATA_WIDTH;
	
	unsigned char *dataPtr = (unsigned char *) (*theTable)->gFormulaData;
	short i,j;
	
	for (i=0; i<FORMULA_SIZE; i++) {
		// Put formula data here.  what the hell is the formula?
	}

	dataPtr += (*theTable)->gFormulaSize;

	RGBNumber exponent;
	exponent.red 	= targetGamma.red/monitorGamma.red;
	exponent.green = targetGamma.green/monitorGamma.green;
	exponent.blue 	= targetGamma.blue/monitorGamma.blue;

	double m;
	m = MAX(ratio.red, ratio.green);
	m = MAX(ratio.blue, m);
	ratio.red 		/= m;
	ratio.green 	/= m;
	ratio.blue 	/= m;
	
	for (i=0; i<DATA_COUNT; i++) {
		dataPtr[i] = (unsigned char) (pow(((double) i)/ ((double) DATA_COUNT), exponent.red) 
			* pow(ratio.red,1.0/monitorGamma.red) * DATA_COUNT);

		dataPtr[i + DATA_COUNT] = (unsigned char) (pow(((double) i)/ ((double) DATA_COUNT), 
			exponent.green) * pow(ratio.green,1.0/monitorGamma.green) * DATA_COUNT);
	
		dataPtr[i + (2*DATA_COUNT)] = (unsigned char) (pow(((double) i)/((double) DATA_COUNT), 
			exponent.blue) * pow(ratio.blue,1.0/monitorGamma.blue) * DATA_COUNT);

#ifdef DEBUG
		outf << "table[" << i << "].red = " << (short) dataPtr[i] << "\t";
		outf << "table[" << i << "].green = " << (short) dataPtr[i+DATA_COUNT] << "\t";
		outf << "table[" << i << "].blue = " << (short) dataPtr[i+(2*DATA_COUNT)] << "\n";
#endif
	}	
	// Done filling in table.

#ifdef DEBUG
	outf.close();
#endif

	// Set hackedTable to the new table.  If previously saved, get rid of it.
	if (hackedTable != nil) DisposeHandle((Handle) hackedTable);
	hackedTable = theTable;
	hackedSize = size;		

	// Set the device gamma table to this new table.
	return(setDevGammaTable(theGDevice, hackedTable));
}
*/