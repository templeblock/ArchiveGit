/*®TP5¯®BT5¯®RFA®FC¯Page ®PN¯ of ®FP¯ -- ppv11a ®VA$FI¯ -- 1:53 PM , May 31, 1989¯®PL1¯®FD1¯®TP0¯®BT0¯*/
#ifndef TYPES
    #include "types.h"
#endif

#define	SCANMSG		0 // Get Error Message Text
#define SCANINIT	1 // Initialize Scanner
#define SCANENV		2 // Get Scanner Environment
#define SCANSETATT	3 // Set Scan Attributes
#define SCANGETATT	4 // Get Scan Attributes
#define SCANSTART	5 // Start Scan
#define SCANIMAGE	6 // Get Image
#define SCANABORT	7 // Abort Scan

/************************************************************************/
/* Scanner initialization definition					*/
/************************************************************************/
typedef struct _Initialize {
	WORD PortAddr;		// Scanner I/O port or SCSI id set
	LPTR DOSMemory;		// A 1024 byte DOS memory buffer so the DLL
				// can safely pass parameters to a DOS driver
	BYTE Reserved[10];	// by the user in Scan Setup dialog box
	} SCINIT;
typedef SCINIT far *LPSCINIT;

/************************************************************************/
/* Scanner environment definition					*/
/************************************************************************/
typedef struct _Environment {
	BYTE Company[48];	// Scanner company name and version tag line
	WORD PortAddr;		// Scanner I/O port or SCSI id
				// Pass back 0 if port or SCSI id is not
				// settable by the user
	BYTE ScanBits;		// 1 = Line art (bilevel), 1 bit/pixel
				// 2 = Grayscale (256 levels), 8 bits/pixel
				// 4 = 24 bits/pixel "Wide" Color, 3 x nPixels
				// 8 = 24 bits/pixel "Long" Color, 3 x nLines
	WORD MinScanLength;	// Minimum page length in inches x 1000
	WORD MaxScanLength;	// Maximum page length in inches x 1000
	WORD MaxScanWidth;	// Maximum page width in inches x 1000
	BYTE DataChannel;	// Image data transmission methods available
				// 1 = ordinary I/O
				// 2 = DMA channel
	BYTE NumSpeeds;		// Number of scanning speeds (up to 5)
	BYTE Reserved[10];
	} SCENV;
typedef SCENV far *LPSCENV;

/************************************************************************/
/* Scanner attributes definition					*/
/************************************************************************/
typedef struct _Attributes {
	BYTE BitsPerPixel;	// Number of bits per pixel; 1 or 8 only
	WORD Resolution;	// Scanning resolution in pixels per inch
	RECT Frame;		// Scanning frame coordinates inches x 1000
	BYTE UseDMA;		// Image transmission method being used
				// 0 = ordinary I/O - no DMA
				// 1 = use the DMA channel
	BYTE ScanSpeed;		// 1 = lowest speed, 5 = highest speed;
				// lowest speed makes the best image quality
	LPTR Lut;		// The image lookup table for grayscale scans
				// If Lut is not NULL, all image data
				// should be passed through it before
				// passing it back to the caller
	BOOL Invert;		// The invert data option: TRUE or FALSE
	BOOL Mirror;		// The mirror option: TRUE or FALSE
	int Contrast;		// 0 = low, 50 = normal, 100 = high
	int Brightness;		// 0 = low, 50 = normal, 100 = high
	BYTE ColorOption;	// The color scan option: TRUE or FALSE
	LPTR RLut;		// The 3 image lookup tables for color scans
	LPTR GLut;		// If the luts are not NULL, all image data
	LPTR BLut;		// should be passed through them before
				// passing it back to the caller
	BYTE Reserved[10];
	} SCATT;
typedef SCATT far *LPSCATT;

/************************************************************************/
/* Scanner start definition						*/
/************************************************************************/
typedef struct _StartScan {
	WORD ScanLength;	// Number of bytes per scan line (returned)
	WORD ScanLines;		// Number of scan lines (returned)
	WORD ScanPixels;	// Number of pixels per scan line (returned)
	BYTE Reserved[10];
	} SCSTART;
typedef SCSTART far *LPSCSTART;

/************************************************************************/
/* Scanner get image definition						*/
/************************************************************************/
typedef struct _GetImage {
	LPTR ImageAddress;	// Storage location for scanned image data
	int ScanLines;		// Number of scan lines requested
				// The actual scan lines should be returned
	int LinesLeft;		// Lines remaining after this call (returned)
	LPTR DOSImageAddress;	// Same storage location as ImageAddress,
				// except that the DLL may need to pass this
				// one to the DOS driver
	BYTE Reserved[10];
} SCGET;
typedef SCGET far *LPSCGET;

/************************************************************************/
/* Scanner message definition						*/
/************************************************************************/
typedef struct _GetMessage {
	BYTE Message[100]; // ASCII message buffer (returned NULL terminated)
} SCMSG;
typedef SCMSG far *LPSCMSG;

// If this is the main routine, declare the space for the structures
// Any other file will automatically have them declared as externals

#ifdef MAIN
	#define DECLARE ;
#else
	#define DECLARE extern
#endif

DECLARE SCINIT	SCinit;
DECLARE SCENV	SCenv;
DECLARE SCATT	SCatt;

// the scanner DLL calls in PPSCAN.LIB
extern  int ScanOpen(void);
extern  int ScanClose(void);
extern  int ScanCall(int,LPTR);
