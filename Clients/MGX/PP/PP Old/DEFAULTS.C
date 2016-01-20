// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"
#include "scan.h"

extern WORD wDrawShapeStyle;

#define PRIVATE_INI
#define GetInt(N,INT) N=GetDefaultInt(#N,INT)
#define GetStr(N,STR) GetDefaultString(#N,STR,N,sizeof(N))
#define GetTmp(N,STR) GetDefaultString(#N,STR,tmp,sizeof(tmp))
#define PutStr(N,STR) PutDefaultString(#N,STR)

#define GetFix(N,INT) {	GetTmp(N,itoa(INT,tmp,10)); N=AsciiFixed(tmp); }
#define GetHex(N,INT) {	GetTmp(N,itoh(INT)); N=htoi(tmp); }
#define GetRGB(N,RGB) {	GetTmp(N,RGB); AsciiRGB(tmp,&N); }
#define PutInt(N,INT) PutStr(N,itoa(INT,tmp,10))
#define PutHex(N,INT) PutStr(N,itoh(INT))
#define PutFix(N,FIX) PutStr(N,FixedAscii(FIX,tmp))
#define PutRGB(N,RGB) PutStr(N,RGBAscii(&RGB,tmp))

static int fpPreferences;
static STRING szPreferenceFile;
static LPTR lpPreferences;

/************************************************************************/
WORD GetDefaultInt( LPTR lpKeyName, int nDefault )
/************************************************************************/
{
LPTR lp;
STRING tmp;

#ifdef PRIVATE_INI
if ( !(lp = FindPreference( stripstruct(lpKeyName) )) )
	return( nDefault );
else	{
	lstrcpy( tmp, lp );
	return( atoi( tmp ) );
	}

//return( GetPrivateProfileInt( ID_APPL, stripstruct(lpKeyName), nDefault,
//	szPreferenceFile ) );

#else
return( GetProfileInt( ID_APPL, stripstruct(lpKeyName), nDefault ) );
#endif
}


/************************************************************************/
WORD GetDefaultString( LPTR lpKeyName, LPTR lpDefault, LPTR lpReturnedString,
			int nSize)
/************************************************************************/
{
LPTR lp;
int length;

#ifdef PRIVATE_INI
if ( !(lp = FindPreference( stripstruct(lpKeyName) )) )
	lp = lpDefault;
length = min( lstrlen(lp), nSize-1 );
copy( lp, lpReturnedString, length );
*(lpReturnedString+length) = '\0';
return( length+1 );

//return( GetPrivateProfileString( ID_APPL, stripstruct(lpKeyName),
//	lpDefault, lpReturnedString, nSize, szPreferenceFile ) );

#else
return( GetProfileString( ID_APPL, stripstruct(lpKeyName), lpDefault,
	lpReturnedString, nSize ) );
#endif
}


/************************************************************************/
BOOL PutDefaultString( LPTR lpKeyName, LPTR lpString)
/************************************************************************/
{
STRING szBuffer;

#ifdef PRIVATE_INI
if ( fpPreferences )
	{
	wsprintf( szBuffer, "%ls=%ls\r\n", stripstruct(lpKeyName), lpString );
	_lwrite( fpPreferences, szBuffer, lstrlen(szBuffer) );
	}
else	{
	return( WritePrivateProfileString( ID_APPL, stripstruct(lpKeyName),
		lpString, szPreferenceFile ));
	}
#else
return( WriteProfileString( ID_APPL, stripstruct(lpKeyName), lpString ));
#endif
}


/********************************************************************** 
 Default palette settings
 **********************************************************************/
static PALETTE defPalette = {
{
	{  0,  0,  0},	/* RGB[0] */
	{  0,  0, 96},	/* RGB[1] */
	{  0, 96,  0},	/* RGB[2] */
	{  0, 96, 96},	/* RGB[3] */
	{ 96,  0,  0},	/* RGB[4] */
	{ 96,  0, 96},	/* RGB[5] */
	{ 96, 96,  0},	/* RGB[6] */
	{ 96, 96, 96},	/* RGB[7] */
	{  0,  0,192},	/* RGB[8] */
	{  0, 96,  0},	/* RGB[9] */
	{  0, 96,192},	/* RGB[10] */
	{ 96,  0,  0},	/* RGB[11] */
	{ 96,  0,192},	/* RGB[12] */
	{ 96, 96,  0},	/* RGB[13] */
	{ 96, 96,192},	/* RGB[14] */
	{  0,  0, 96},	/* RGB[15] */
	{  0,192,  0},	/* RGB[16] */
	{  0,192, 96},	/* RGB[17] */
	{ 96,  0,  0},	/* RGB[18] */
	{ 96,  0, 96},	/* RGB[19] */
	{ 96,192,  0},	/* RGB[20] */
	{ 96,192, 96},	/* RGB[21] */
	{  0,  0, 96},	/* RGB[22] */
	{  0, 96,  0},	/* RGB[23] */
	{  0, 96, 96},	/* RGB[24] */
	{192,  0,  0},	/* RGB[25] */
	{192,  0, 96},	/* RGB[26] */
	{192, 96,  0},	/* RGB[27] */
	{192, 96, 96},	/* RGB[28] */
	{  0,  0,192},	/* RGB[29] */
	{  0,192,  0},	/* RGB[30] */
	{  0,192,192},	/* RGB[31] */
	{ 96,  0,  0},	/* RGB[32] */
	{ 96,  0,192},	/* RGB[33] */
	{ 96,192,  0},	/* RGB[34] */
	{ 96,192,192},	/* RGB[35] */
	{  0,  0,192},	/* RGB[36] */
	{  0, 96,  0},	/* RGB[37] */
	{  0, 96,192},	/* RGB[38] */
	{192,  0,  0},	/* RGB[39] */
	{192,  0,192},	/* RGB[40] */
	{192, 96,  0},	/* RGB[41] */
	{192, 96,192},	/* RGB[42] */
	{  0,  0, 96},	/* RGB[43] */
	{  0,192,  0},	/* RGB[44] */
	{  0,192, 96},	/* RGB[45] */
	{192,  0,  0},	/* RGB[46] */
	{192,  0, 96},	/* RGB[47] */
	{192,192,  0},	/* RGB[48] */
	{192,192, 96},	/* RGB[49] */
	{  0,  0,255},	/* RGB[50] */
	{  0, 96,  0},	/* RGB[51] */
	{  0, 96,255},	/* RGB[52] */
	{ 96,  0,  0},	/* RGB[53] */
	{ 96,  0,255},	/* RGB[54] */
	{ 96, 96,  0},	/* RGB[55] */
	{ 96, 96,255},	/* RGB[56] */
	{  0,  0, 96},	/* RGB[57] */
	{  0,255,  0},	/* RGB[58] */
	{  0,255, 96},	/* RGB[59] */
	{ 96,  0,  0},	/* RGB[60] */
	{ 96,  0, 96},	/* RGB[61] */
	{ 96,255,  0},	/* RGB[62] */
	{ 96,255, 96},	/* RGB[63] */
	{  0,  0, 96},	/* RGB[64] */
	{  0, 96,  0},	/* RGB[65] */
	{  0, 96, 96},	/* RGB[66] */
	{255,  0,  0},	/* RGB[67] */
	{255,  0, 96},	/* RGB[68] */
	{255, 96,  0},	/* RGB[69] */
	{255, 96, 96},	/* RGB[70] */
	{  0,  0,255},	/* RGB[71] */
	{  0,255,  0},	/* RGB[72] */
	{  0,255,255},	/* RGB[73] */
	{ 96,  0,  0},	/* RGB[74] */
	{ 96,  0,255},	/* RGB[75] */
	{ 96,255,  0},	/* RGB[76] */
	{ 96,255,255},	/* RGB[77] */
	{  0,  0,255},	/* RGB[78] */
	{  0, 96,  0},	/* RGB[79] */
	{  0, 96,255},	/* RGB[80] */
	{255,  0,  0},	/* RGB[81] */
	{255,  0,255},	/* RGB[82] */
	{255, 96,  0},	/* RGB[83] */
	{255, 96,255},	/* RGB[84] */
	{  0,  0, 96},	/* RGB[85] */
	{  0,255,  0},	/* RGB[86] */
	{  0,255, 96},	/* RGB[87] */
	{255,  0,  0},	/* RGB[88] */
	{255,  0, 96},	/* RGB[89] */
	{255,255,  0},	/* RGB[90] */
	{255,255, 96},	/* RGB[91] */
	{  0,  0,255},	/* RGB[92] */
	{  0,255,  0},	/* RGB[93] */
	{  0,255,255},	/* RGB[94] */
	{255,  0,  0},	/* RGB[95] */
	{255,  0,255},	/* RGB[96] */
	{255,255,  0},	/* RGB[97] */
	{255,255,255},	/* RGB[98] */
	{  0,  0,255},	/* RGB[99] */
	{  0,192,  0},	/* RGB[100] */
	{  0,192,255},	/* RGB[101] */
	{ 96,  0,  0},	/* RGB[102] */
	{ 96,  0,255},	/* RGB[103] */
	{ 96,192,  0},	/* RGB[104] */
	{ 96,192,255},	/* RGB[105] */
	{  0,  0,255},	/* RGB[106] */
	{  0, 96,  0},	/* RGB[107] */
	{  0, 96,255},	/* RGB[108] */
	{192,  0,  0},	/* RGB[109] */
	{192,  0,255},	/* RGB[110] */
	{192, 96,  0},	/* RGB[111] */
	{192, 96,255},	/* RGB[112] */
	{  0,  0, 96},	/* RGB[113] */
	{  0,255,  0},	/* RGB[115] */
	{  0,255, 96},	/* RGB[114] */
	{192,  0,  0},	/* RGB[116] */
	{192,  0, 96},	/* RGB[117] */
	{192,255,  0},	/* RGB[118] */
	{192,255, 96},	/* RGB[119] */
	{  0,  0,192},	/* RGB[120] */
	{  0,255,  0},	/* RGB[121] */
	{  0,255,192},	/* RGB[122] */
	{ 96,  0,  0},	/* RGB[123] */
	{ 96,  0,192},	/* RGB[124] */
	{ 96,255,  0},	/* RGB[125] */
	{ 96,255,192},	/* RGB[126] */
	{255,255,255},	/* RGB[127] */
	},

{  0,  0,  0},	/* ActiveRGB */
{255,255,255},	/* AlternateRGB */
};


/************************************************************************/
void LoadAllPreferences( lpProgHome )
/************************************************************************/
LPTR lpProgHome;
{
int i;
STRING tmp, szPaletteFile;
FNAME szFileName;

AstralCursor( IDC_WAIT );
lstrcpy( szPreferenceFile, lpProgHome );
lstrcat( szPreferenceFile, "PP.INI" );
lpPreferences = LoadPreferences( szPreferenceFile );

GetInt( Control.RecallNext, 0 );
if ( Control.RecallNext > MAX_RECALL )
	Control.RecallNext = 0;
Control.RecallCount = 0;
for ( i=0; i<MAX_RECALL; i++ )
	{
	GetDefaultString( itoa(i,tmp,10), "",
		Control.RecallImage[i], sizeof(Control.RecallImage[i]) );
	if ( *Control.RecallImage[i] )
		Control.RecallCount++;
	}

lstrcpy( szPaletteFile, lpProgHome );
lstrcat( szPaletteFile, "DEFAULT.PAL" );
if ( !paletteRead( szPaletteFile, &Palette ) )
	{ // Palette not found, using default
	copy( (LPTR)&defPalette, (LPTR)&Palette, sizeof(PALETTE) );
	paletteWrite( szPaletteFile, &Palette );
	}

lstrcpy( tmp, lpProgHome );
lstrcat( tmp, "POUCH" );
GetStr( Control.PouchPath, tmp );
GetProfileString( "Micrografx", "Pouch 3.0", tmp, Control.PouchPath,
	sizeof(Control.PouchPath) );

GetStr( Control.RamDisk, Control.ProgHome );
GetStr( Control.EPSSplitDir, "" );
GetInt( Control.MainMemMin, 150 );
GetInt( Control.MainMemFactor, 50 );
if (  Control.MainMemFactor < 10 )
	Control.MainMemFactor = 10;
if (  Control.MainMemFactor > 100 )
	Control.MainMemFactor = 100;
GetInt( Control.Units, 0 ); Control.Units += IDC_PREF_UNITINCHES;
GetFix( Control.ScreenWidth, 9 );
GetInt( Control.EMSdisable, NO );
GetHex( Control.FGPort, 0xFFF );
GetInt( Control.SmartPaste, NO );
GetInt( Control.ColorEnabled, YES );
GetInt( Control.ImageWhere.x, 50 );
GetInt( Control.ImageWhere.y, 4 );
GetInt( Control.UseApply, NO );
GetInt( Control.ImageType, 0 ); Control.ImageType += IDC_SAVETIFF;
GetInt( Control.ScratchpadWidth, 100 );
GetInt( Control.ScratchpadHeight, 200 );
GetInt( Control.IsScratchpadColor, YES );
GetInt( Control.ZoomOut, NO );
GetInt( Control.ZoomWindow, NO );
GetStr( Names.Map, "" );
GetStr( Names.Smooth, "" );
GetStr( Names.Sharp, "" );
GetStr( Names.Edge, "" );
GetStr( Names.Special, "" );
GetStr( Names.Mask, "" );
GetStr( Names.Scanner, "" );
GetStr( Names.Grabber, "" );
GetStr( Names.ScanMap, "" );
GetStr( Names.Printer, "" );
GetStr( Names.PrintStyle, "" );
GetStr( Names.Clipboard, "" );
GetInt( Page.Type, 0 ); Page.Type += IDC_PRINT_GRAY;
GetInt( Page.ScatterPrint, NO );
GetInt( Page.Centered, YES );
GetFix( Page.OffsetX, 0 );
GetFix( Page.OffsetY, 0 );
GetInt( Page.SepFlags, 0 );
GetInt( Printer.DotShape, 0 ); Printer.DotShape += IDC_CIRCLEDOT;
GetFix( Printer.ScreenRuling[0], 53 );
GetFix( Printer.ScreenRuling[1], 53 );
GetFix( Printer.ScreenRuling[2], 53 );
GetFix( Printer.ScreenRuling[3], 53 );
GetFix( Printer.ScreenAngle[0], 15 );
GetFix( Printer.ScreenAngle[1], 75 );
GetFix( Printer.ScreenAngle[2], 0 );
GetFix( Printer.ScreenAngle[3], 45 );
GetInt( Printer.Type, 0 ); Printer.Type += IDC_PRINTER_IS_CMYK;
GetInt( Printer.DoHalftoning, YES );
GetInt( Printer.DoToneRepro, YES );
GetInt( Printer.DoCalibration, YES );
GetInt( Printer.DoInkCorrect, YES );
GetInt( Printer.DoBlackAndUCR, YES );
GetInt( Printer.TrimMarks, NO );
GetInt( Printer.RegMarks, NO );
GetInt( Printer.Labels, NO );
GetInt( Printer.StepScale, NO );
GetInt( Printer.Negative, NO );
GetInt( Printer.EmulsionDown, NO );
GetInt( Printer.MfromR, 0 );
GetInt( Printer.YfromG, 0 );
GetInt( Printer.CfromB, 0 );
GetInt( Printer.UCR, 100 );
GetInt( Printer.Hilight, 0 );
GetInt( Printer.Shadow, 100 );
GetStr( Printer.CalName, "" );
ResetMap( &Printer.CalMap[0], MAPPOINTS );
ResetMap( &Printer.CalMap[1], MAPPOINTS );
ResetMap( &Printer.CalMap[2], MAPPOINTS );
ResetMap( &Printer.CalMap[3], MAPPOINTS );
GetStr( Printer.BGName, "" );
ResetMap( &Printer.BGMap, MAPPOINTS );
if ( LookupExtFile( Names.PrintStyle, szFileName, IDN_STYLE ) )
	LoadPrintStyle( &Printer, szFileName );
GetInt( Scans.DoCalibration, NO );
ResetMap( &Scans.CalMap[0], MAPPOINTS );
ResetMap( &Scans.CalMap[1], MAPPOINTS );
ResetMap( &Scans.CalMap[2], MAPPOINTS );
ResetMap( &Scans.CalMap[3], MAPPOINTS );
if ( LookupExtFile( Names.ScanMap, szFileName, IDN_SCANMAP ) )
	LoadMap( &Scans.CalMap[0], &Scans.CalMap[1],
		 &Scans.CalMap[2], &Scans.CalMap[3], szFileName );
GetHex( SCinit.PortAddr, 0x278 );
GetInt( SCatt.BitsPerPixel, 8 );
GetInt( SCatt.Resolution, 150 );
GetInt( SCatt.ScanSpeed,  5 );
GetInt( SCatt.UseDMA, YES );
GetInt( SCatt.Frame.left, 1000 );
GetInt( SCatt.Frame.top, 1000 );
GetInt( SCatt.Frame.right, 7000 );
GetInt( SCatt.Frame.bottom, 9000 );
GetInt( Save.Compressed, NO );
GetInt( Mask.AutoMask, NO );
GetInt( Mask.AutoSearchDepth, 3 );
GetInt( Mask.AutoMinEdge, 3 );
GetInt( Mask.PasteBlend, NO );
GetInt( Mask.ConstrainRectAspect, NO );
GetFix( Mask.ConstrainRectWidth, 1 );
GetFix( Mask.ConstrainRectHeight, 1 )
GetInt( Mask.ConstrainCircleAspect, NO );
GetFix( Mask.ConstrainCircleWidth, 1 );
GetFix( Mask.ConstrainCircleHeight, 1 );
GetInt( Mask.BlendPressure, 0 ); Mask.BlendPressure += IDC_BLENDLIGHT;
GetInt( Mask.VignetteType, 0 ); Mask.VignetteType += IDC_VIGLINEAR;
GetInt( Mask.VigRepeatCount, 1 );
GetInt( Mask.TransformImage, NO );
GetInt( Mask.RemoveImage, NO );
//GetInt( Mask.PasteQuality, ??? );
GetInt( Mask.Opacity, 255 ); Mask.Opacity = bound( Mask.Opacity, 0, 255 );
GetInt( Mask.WandRange, 15);
GetInt( Mask.PointerFunction, IDC_MOVEPOINTS );
GetInt( Mask.Rotate, NO );
GetInt( Edit.SmartSize, NO );
GetInt( ColorMask.MaskShield, 0 ); // Which is selected for the tool
GetInt( ColorMask.Mask, 0 ); // What's active
GetInt( ColorMask.Include, YES );
GetInt( ColorMask.Off, YES );
GetInt( ColorMask.ProbeMethod, 0 ); ColorMask.ProbeMethod += IDC_PROBEPOINT;
GetInt( ColorMask.ShieldMethod, 0 ); ColorMask.ShieldMethod += IDC_PROBERECT;

// do from 0 to NUM_COLORMASKS
GetInt( ColorMask.MaskRange[0], 10 );
GetInt( ColorMask.MaskRange[1], 10 );
GetInt( ColorMask.MaskRange[2], 10 );
GetInt( ColorMask.MaskRange[3], 10 );
GetInt( ColorMask.MaskRange[4], 10 );
GetInt( ColorMask.MaskRange[5], 10 );
GetInt( ColorMask.MaskRange[6], 10 );
GetInt( ColorMask.MaskRange[7], 10 );

// do from 0 to NUM_COLORMASKS
GetRGB( ColorMask.MaskColor[0], "255,255,255" );
GetRGB( ColorMask.MaskColor[1], "255,255,255" );
GetRGB( ColorMask.MaskColor[2], "255,255,255" );
GetRGB( ColorMask.MaskColor[3], "255,255,255" );
GetRGB( ColorMask.MaskColor[4], "255,255,255" );
GetRGB( ColorMask.MaskColor[5], "255,255,255" );
GetRGB( ColorMask.MaskColor[6], "255,255,255" );
GetRGB( ColorMask.MaskColor[7], "255,255,255" );

GetInt( Text.Size, 12 );
GetInt( Text.Italic, NO );
GetInt( Text.Underline, NO );
GetInt( Text.Strikeout, NO );
GetInt( Text.Weight, NO );
GetStr( Text.TextString, "" );
GetInt( wDrawShapeStyle, 0 ); wDrawShapeStyle += IDC_CIRDRAWEDGE;

/* Texture settings */
GetStr( Texture.Name, "");
GetInt( Texture.fHorzFlip, NO);
GetInt( Texture.fVertFlip, NO);

/* old brush setting */
GetStr( Retouch.CustomBrushName, "" );
GetInt( Retouch.BrushSize, 9 );
GetInt( Retouch.FloodRange, 2 );
GetInt( Retouch.Opacity, 255 );
GetInt( Retouch.FuzzyPixels, 0 );
GetFix( Retouch.Fadeout, 0 );
GetInt( Retouch.Pressure, 1 );   Retouch.Pressure += IDC_PRESSLIGHT;
GetInt( Retouch.BrushShape, 0 ); Retouch.BrushShape += IDC_BRUSHCIRCLE;
GetInt( Retouch.BrushStyle, 0 ); Retouch.BrushStyle += IDC_BRUSHSOLID;

/* new brushes */
RetouchBrushCurrent = NO_BRUSH;
GetRetouchBrush( "Paint", PAINT_BRUSH );
GetRetouchBrush( "Spray", SPRAY_BRUSH );
GetRetouchBrush( "Smear", SMEAR_BRUSH );
GetRetouchBrush( "Clone", CLONE_BRUSH );
GetRetouchBrush( "Lighten", LIGHTEN_BRUSH );
GetRetouchBrush( "Darken", DARKEN_BRUSH );
GetRetouchBrush( "Sharpen", SHARPEN_BRUSH );
GetRetouchBrush( "Smooth", SMOOTH_BRUSH );
GetRetouchBrush( "Erase", ERASER_BRUSH );
GetRetouchBrush( "Texture", TEXTURE_BRUSH );
GetRetouchBrush( "DrawLine", DRAW_LINE_BRUSH );
GetRetouchBrush( "DrawRect", DRAW_RECT_BRUSH );
GetRetouchBrush( "DrawEllipse", DRAW_ELLIPSE_BRUSH );
GetRetouchBrush( "DrawFree", DRAW_FREE_BRUSH );

SetupMask();
if ( lpPreferences )
	FreeUp( lpPreferences );
else	StoreAllPreferences( lpProgHome );
lpPreferences = NULL;
AstralCursor( NULL );
}


/************************************************************************/
void StoreAllPreferences( lpProgHome )
/************************************************************************/
LPTR lpProgHome;
{
int i;
STRING tmp, szPaletteFile;

AstralCursor( IDC_WAIT );
lstrcpy( szPreferenceFile, lpProgHome );
lstrcat( szPreferenceFile, "PP.INI" );
if ( ( fpPreferences = _lcreat( szPreferenceFile, 0 ) ) < 0 )
	{
	Message( IDS_ECREATE, Lowercase(szPreferenceFile) );
	return;
	}

_lwrite( fpPreferences, "\r\n[", 3 );
_lwrite( fpPreferences, ID_APPL, lstrlen(ID_APPL) );
_lwrite( fpPreferences, "]\r\n", 3 );

lstrcpy( szPaletteFile, lpProgHome );
lstrcat( szPaletteFile, "DEFAULT.PAL" );
/* write the palette to the palette file */
if ( !paletteWrite( szPaletteFile, &Palette ) )
	Print( "Error writing palette");

PutInt( Control.RecallNext, Control.RecallNext );
for (  i=0; i<MAX_RECALL; i++ )
    if ( *Control.RecallImage[i] )
	PutDefaultString( itoa(i,tmp,10), Control.RecallImage[i] );

WriteProfileString( "Micrografx", "Pouch 3.0", Control.PouchPath );

PutStr( Control.RamDisk, Control.RamDisk );
//PutStr( Control.EPSSplitDir, Control.EPSSplitDir );
PutInt( Control.MainMemMin, Control.MainMemMin );
PutInt( Control.MainMemFactor, Control.MainMemFactor );
PutInt( Control.Units, Control.Units - IDC_PREF_UNITINCHES );
PutFix( Control.ScreenWidth, Control.ScreenWidth );
PutInt( Control.EMSdisable, Control.EMSdisable );
PutHex( Control.FGPort, Control.FGPort );
PutInt( Control.SmartPaste, Control.SmartPaste );
//PutInt( Control.ColorEnabled, Control.ColorEnabled );
PutInt( Control.ImageWhere.x, Control.ImageWhere.x );
PutInt( Control.ImageWhere.y, Control.ImageWhere.y );
PutInt( Control.UseApply, Control.UseApply );
PutInt( Control.ImageType, Control.ImageType - IDC_SAVETIFF );
PutInt( Control.ScratchpadWidth, Control.ScratchpadWidth );
PutInt( Control.Scratchpadheight, Control.ScratchpadHeight );
PutInt( Control.IsScratchpadColor, Control.IsScratchpadColor );
PutInt( Control.ZoomOut, Control.ZoomOut );
PutInt( Control.ZoomWindow, Control.ZoomWindow );
PutStr( Names.Map, Names.Map );
PutStr( Names.Smooth, Names.Smooth );
PutStr( Names.Sharp, Names.Sharp );
PutStr( Names.Edge, Names.Edge );
PutStr( Names.Special, Names.Special );
PutStr( Names.Mask, Names.Mask );
PutStr( Names.Scanner, Names. Scanner );
PutStr( Names.Grabber, Names.Grabber );
PutStr( Names.ScanMap, Names.ScanMap );
PutStr( Names.Printer, Names.Printer );
PutStr( Names.PrintStyle, Names.PrintStyle );
PutStr( Names.Clipboard, Names.Clipboard );
PutInt( Page.Type, Page.Type - IDC_PRINT_GRAY );
PutInt( Page.ScatterPrint, Page.ScatterPrint );
PutInt( Page.Centered, Page.Centered );
PutFix( Page.OffsetX, Page.OffsetX );
PutFix( Page.OffsetY, Page.OffsetY );
PutInt( Page.SepFlags, Page.SepFlags );
PutInt( Printer.DotShape, Printer.DotShape - IDC_CIRCLEDOT );
PutFix( Printer.ScreenRuling[0], Printer.ScreenRuling[0] );
PutFix( Printer.ScreenRuling[1], Printer.ScreenRuling[1] );
PutFix( Printer.ScreenRuling[2], Printer.ScreenRuling[2] );
PutFix( Printer.ScreenRuling[3], Printer.ScreenRuling[3] );
PutFix( Printer.ScreenAngle[0], Printer.ScreenAngle[0] );
PutFix( Printer.ScreenAngle[1], Printer.ScreenAngle[1] );
PutFix( Printer.ScreenAngle[2], Printer.ScreenAngle[2] );
PutFix( Printer.ScreenAngle[3], Printer.ScreenAngle[3] );
PutInt( Printer.Type, Printer.Type - IDC_PRINTER_IS_CMYK );
PutInt( Printer.DoHalftoning, Printer.DoHalftoning );
PutInt( Printer.DoToneRepro, Printer.DoToneRepro );
PutInt( Printer.DoCalibration, Printer.DoCalibration );
PutInt( Printer.DoInkCorrect, Printer.DoInkCorrect );
PutInt( Printer.DoBlackAndUCR, Printer.DoBlackAndUCR );
PutInt( Printer.TrimMarks, Printer.TrimMarks );
PutInt( Printer.RegMarks, Printer.RegMarks );
PutInt( Printer.Labels, Printer.Labels );
PutInt( Printer.StepScale, Printer.StepScale );
PutInt( Printer.Negative, Printer.Negative );
PutInt( Printer.EmulsionDown, Printer.EmulsionDown );
PutInt( Printer.MfromR, Printer.MfromR );
PutInt( Printer.YfromG, Printer.YfromG );
PutInt( Printer.CfromB, Printer.CfromB );
PutInt( Printer.UCR, Printer.UCR );
PutInt( Printer.Hilight, Printer.Hilight );
PutInt( Printer.Shadow, Printer.Shadow );
PutInt( Scans.DoCalibration, Scans.DoCalibration );
PutHex( SCinit.PortAddr, SCinit.PortAddr );
PutInt( SCatt.BitsPerPixel, SCatt.BitsPerPixel );
PutInt( SCatt.Resolution, SCatt.Resolution );
PutInt( SCatt.ScanSpeed, SCatt.ScanSpeed );
PutInt( SCatt.UseDMA, SCatt.UseDMA );
PutInt( SCatt.Frame.left, SCatt.Frame.left );
PutInt( SCatt.Frame.top, SCatt.Frame.top );
PutInt( SCatt.Frame.right, SCatt.Frame.right );
PutInt( SCatt.Frame.bottom, SCatt.Frame.bottom );
PutInt( Save.Compressed, Save.Compressed );
PutInt( Mask.AutoMask, Mask.AutoMask );
PutInt( Mask.AutoSearchDepth, Mask.AutoSearchDepth );
PutInt( Mask.AutoMinEdge, Mask.AutoMinEdge );
PutInt( Mask.PasteBlend, Mask.PasteBlend );
PutInt( Mask.ConstrainRectAspect, Mask.ConstrainRectAspect );
PutFix( Mask.ConstrainRectWidth, Mask.ConstrainRectWidth );
PutFix( Mask.ConstrainRectHeight, Mask.ConstrainRectHeight );
PutInt( Mask.ConstrainCircleAspect, Mask.ConstrainCircleAspect );
PutFix( Mask.ConstrainCircleWidth, Mask.ConstrainCircleWidth );
PutFix( Mask.ConstrainCircleHeight, Mask.ConstrainCircleHeight);
PutInt( Mask.BlendPressure, Mask.BlendPressure - IDC_BLENDLIGHT );
PutInt( Mask.VignetteType, Mask.VignetteType - IDC_VIGLINEAR );
PutInt( Mask.VigRepeatCount, Mask.VigRepeatCount );
PutInt( Mask.TransformImage, Mask.TransformImage );
PutInt( Mask.RemoveImage, Mask.RemoveImage );
//PutInt( Mask.PasteQuality, Mask.PasteQuality );
PutInt( Mask.Opacity, Mask.Opacity );
PutInt( Mask.WandRange, Mask.WandRange);
PutInt( Mask.PointerFunction, Mask.PointerFunction );
PutInt( Mask.Rotate, Mask.Rotate );
PutInt( Edit.SmartSize, Edit.SmartSize );
PutInt( ColorMask.MaskShield, ColorMask.MaskShield );
PutInt( ColorMask.Mask, ColorMask.Mask );
PutInt( ColorMask.Include, ColorMask.Include );
PutInt( ColorMask.Off, ColorMask.Off );
PutInt( ColorMask.ProbeMethod, ColorMask.ProbeMethod - IDC_PROBEPOINT );
PutInt( ColorMask.ShieldMethod, ColorMask.ShieldMethod - IDC_PROBERECT );

// do from 0 to NUM_COLORMASKS
PutInt( ColorMask.MaskRange[0], ColorMask.MaskRange[0] );
PutInt( ColorMask.MaskRange[1], ColorMask.MaskRange[1] );
PutInt( ColorMask.MaskRange[2], ColorMask.MaskRange[2] );
PutInt( ColorMask.MaskRange[3], ColorMask.MaskRange[3] );
PutInt( ColorMask.MaskRange[4], ColorMask.MaskRange[4] );
PutInt( ColorMask.MaskRange[5], ColorMask.MaskRange[5] );
PutInt( ColorMask.MaskRange[6], ColorMask.MaskRange[6] );
PutInt( ColorMask.MaskRange[7], ColorMask.MaskRange[7] );

// do from 0 to NUM_COLORMASKS
PutRGB( ColorMask.MaskColor[0], ColorMask.MaskColor[0] );
PutRGB( ColorMask.MaskColor[1], ColorMask.MaskColor[1] );
PutRGB( ColorMask.MaskColor[2], ColorMask.MaskColor[2] );
PutRGB( ColorMask.MaskColor[3], ColorMask.MaskColor[3] );
PutRGB( ColorMask.MaskColor[4], ColorMask.MaskColor[4] );
PutRGB( ColorMask.MaskColor[5], ColorMask.MaskColor[5] );
PutRGB( ColorMask.MaskColor[6], ColorMask.MaskColor[6] );
PutRGB( ColorMask.MaskColor[7], ColorMask.MaskColor[7] );

PutInt( Text.Size, Text.Size );
PutInt( Text.Italic, Text.Italic );
PutInt( Text.Underline, Text.Underline );
PutInt( Text.Strikeout, Text.Strikeout );
PutInt( Text.Weight, Text.Weight );
PutStr( Text.TextString, Text.TextString );
PutInt( wDrawShapeStyle, wDrawShapeStyle - IDC_CIRDRAWEDGE );

/* Texture settings */
PutStr( Texture.Name, Texture.Name );
PutInt( Texture.fHorzFlip, Texture.fHorzFlip );
PutInt( Texture.fVertFlip, Texture.fVertFlip );

PutStr( Retouch.CustomBrushName, Retouch.CustomBrushName );
PutInt( Retouch.BrushSize, Retouch.BrushSize );
PutInt( Retouch.FloodRange, Retouch.FloodRange );
PutInt( Retouch.Opacity, Retouch.Opacity );
PutInt( Retouch.FuzzyPixels, Retouch.FuzzyPixels );
PutFix( Retouch.Fadeout, Retouch.Fadeout );
PutInt( Retouch.Pressure, Retouch.Pressure - IDC_PRESSLIGHT );
PutInt( Retouch.BrushShape, Retouch.BrushShape - IDC_BRUSHCIRCLE );
PutInt( Retouch.BrushStyle, Retouch.BrushStyle - IDC_BRUSHSOLID );

SaveRetouchBrush( RetouchBrushCurrent );
PutRetouchBrush( "Paint", PAINT_BRUSH );
PutRetouchBrush( "Spray", SPRAY_BRUSH );
PutRetouchBrush( "Smear", SMEAR_BRUSH );
PutRetouchBrush( "Clone", CLONE_BRUSH );
PutRetouchBrush( "Lighten", LIGHTEN_BRUSH );
PutRetouchBrush( "Darken", DARKEN_BRUSH );
PutRetouchBrush( "Sharpen", SHARPEN_BRUSH );
PutRetouchBrush( "Smooth", SMOOTH_BRUSH );
PutRetouchBrush( "Erase", ERASER_BRUSH );
PutRetouchBrush( "Texture", TEXTURE_BRUSH );
PutRetouchBrush( "DrawLine", DRAW_LINE_BRUSH );
PutRetouchBrush( "DrawRect", DRAW_RECT_BRUSH );
PutRetouchBrush( "DrawEllipse", DRAW_ELLIPSE_BRUSH );
PutRetouchBrush( "DrawFree", DRAW_FREE_BRUSH );

_lwrite( fpPreferences, "\r\n", 2 );
_lclose( fpPreferences );
fpPreferences = NULL;

AstralCursor( NULL );
}


/************************************************************************/
static BOOL paletteWrite( LPTR lpFileName, PALETTE *pal)
/************************************************************************/
{
int		ofh;		/* file handle( unbuffered) */
FILEBUF		ofd;		/* file descriptor( buffered) */
int		i;

/* open the output file */
if ( ( ofh = _lcreat( lpFileName, 0)) < 0 )
	{
	Message( IDS_EOPEN, lpFileName);
	return( NO);
	}

/* create a buffered stream to speed access */
FileFDOpenWrt( &ofd, ofh, LineBuffer[0], 16*1024);

/*********************/
/* write the palette */
/*********************/

/* write palette version number */
intelWriteWord( &ofd, 1);

/* write number of entries in palette */
intelWriteWord( &ofd, MAX_USER_COLORS);

/* write the active color value */
paletteWriteRGB( &ofd, &pal->ActiveRGB);

/* write the alternate color value */
paletteWriteRGB( &ofd, &pal->AlternateRGB);

/* write the palette entries */
for ( i = 0; i < MAX_USER_COLORS; i++)
	paletteWriteRGB( &ofd, &pal->RGB [i]);

/* flush the stream */
(void) FileFlush( &ofd);

/* close the file */
_lclose( ofh);

if ( ofd.err)
	{
	Message( IDS_EWRITE, lpFileName);
	return( NO);
	}
return( YES);
}


/************************************************************************/
static void paletteWriteRGB( LPFILEBUF ofd, LPRGB rgb)
/************************************************************************/
{
BYTE b[3];

b[0] = rgb->red;
b[1] = rgb->green;
b[2] = rgb->blue;
FileWrite( ofd, b, 3 );
}


/************************************************************************/
static BOOL paletteRead( LPTR lpFileName, PALETTE *pal)
/************************************************************************/
{
int		ifh;		/* file handle( unbuffered) */
FILEBUF		ifd;		/* file descriptor( buffered) */
int		i;
WORD	entries;
WORD	version;

/* open the palette file */
if ( ( ifh = _lopen( lpFileName, OF_READ)) < 0 )
	{
//	Message( IDS_EOPEN, lpFileName);
	return( NO);
	}

/* create a buffered stream to speed access */
FileFDOpenRdr( &ifd, ifh, LineBuffer[0], 16*1024);

/*********************/
/* read the palette */
/*********************/

/* read palette version number */
intelReadWord( &ifd, &version);

if ( version != 1)
	{
	Print( "Incorrect palette version");
	_lclose( ifh);
	return( NO);
	}

/* read number of entries in palette */
intelReadWord( &ifd, &entries);

/* write the active color value */
paletteReadRGB( &ifd, &pal->ActiveRGB);

/* write the alternate color value */
paletteReadRGB( &ifd, &pal->AlternateRGB);

/* make sure we don't read more entries than we have room for */
if ( entries > MAX_USER_COLORS)
	entries = MAX_USER_COLORS;

/* read the palette entries */
for ( i = 0; i < entries; i++)
	paletteReadRGB( &ifd, &pal->RGB [i]);

/* initialize any remaining palette entries */
for (  ; i < MAX_USER_COLORS; i++)
	{
	pal->RGB [i].red = i;
	pal->RGB [i].green = i;
	pal->RGB [i].blue = i;
	}

/* close the file */
_lclose( ifh);

if ( ifd.err)
	{
	Message( IDS_EREAD, lpFileName);
	return( NO);
	}

return( YES);
}


/************************************************************************/
static void paletteReadRGB( LPFILEBUF ifd, LPRGB rgb)
/************************************************************************/
{
BYTE b[3];

FileRead( ifd, b, 3 );
rgb->red   = b[0];
rgb->green = b[1];
rgb->blue  = b[2];
}


/************************************************************************/
static void GetRetouchBrush( name, brush )
/************************************************************************/
char	*name;
int	brush;
{
LPTR lp;
BRUSHSETTINGS FAR *ThisBrush;
STRING tmp, str;

/* set basic portion of property name */
lstrcpy( str, name);
lp = str + lstrlen( str);

ThisBrush = &RetouchBrushes[brush];

lstrcpy( lp, "BrushSize");
ThisBrush->BrushSize = GetDefaultInt(str, Retouch.BrushSize);

lstrcpy( lp, "Opacity");
ThisBrush->Opacity = GetDefaultInt(str, Retouch.Opacity);

lstrcpy( lp, "FuzzyPixels");
ThisBrush->FuzzyPixels = GetDefaultInt(str, Retouch.FuzzyPixels);

lstrcpy( lp, "Fadeout");
GetDefaultString(str, itoa( Retouch.Fadeout, tmp, 10), tmp, sizeof(tmp));
ThisBrush->Fadeout = AsciiFixed(tmp);

lstrcpy( lp, "Pressure");
ThisBrush->Pressure = IDC_PRESSLIGHT +
	GetDefaultInt(str, Retouch.Pressure - IDC_PRESSLIGHT);

lstrcpy( lp, "BrushShape");
ThisBrush->BrushShape = IDC_BRUSHCIRCLE +
	GetDefaultInt(str, Retouch.BrushShape - IDC_BRUSHCIRCLE);

lstrcpy( lp, "BrushStyle");
ThisBrush->BrushStyle = IDC_BRUSHSOLID +
	GetDefaultInt(str, Retouch.BrushStyle - IDC_BRUSHSOLID);
}


/************************************************************************/
static void PutRetouchBrush( name, brush )
/************************************************************************/
char	*name;
int	brush;
{
LPTR lp;
BRUSHSETTINGS FAR *ThisBrush;
STRING tmp, str;

/* set basic portion of property name */
lstrcpy( str, name); 
lp = str + lstrlen( str);

ThisBrush = &RetouchBrushes[brush];

lstrcpy( lp, "BrushSize");
PutDefaultString( str, itoa( ThisBrush->BrushSize, tmp, 10));

lstrcpy( lp, "Opacity");
PutDefaultString( str, itoa( ThisBrush->Opacity, tmp, 10));

lstrcpy( lp, "FuzzyPixels");
PutDefaultString( str, itoa( ThisBrush->FuzzyPixels, tmp, 10));

lstrcpy( lp, "Fadeout");
PutDefaultString( str, FixedAscii( ThisBrush->Fadeout, tmp));

lstrcpy( lp, "Pressure");
PutDefaultString( str, itoa( ThisBrush->Pressure - IDC_PRESSLIGHT, tmp, 10));

lstrcpy( lp, "BrushShape");
PutDefaultString( str, itoa( ThisBrush->BrushShape - IDC_BRUSHCIRCLE, tmp, 10));

lstrcpy( lp, "BrushStyle");
PutDefaultString( str, itoa( ThisBrush->BrushStyle - IDC_BRUSHSOLID, tmp, 10));
}


/************************************************************************/
LPTR stripstruct( lpString )
/************************************************************************/
LPTR lpString;
{
LPTR lp;

lp = lpString;
while ( *lp )
	{
	if ( *lp++ == '.' )
		return( lp );
	}
return( lpString );
}


/************************************************************************/
LPTR LoadPreferences( lpFileName )
/************************************************************************/
LPTR lpFileName;
{
int ifh;
LPTR lp, lpDefaults, lpNew;
BYTE c;
long lCount, lBufferSize;
BOOL fCopyingName;

// Load the defaults file into a block of memory
// comprised of pairs of strings - keyname and value

/* open the defaults file */
if ( ( ifh = _lopen( lpFileName, OF_READ)) < 0 )
	return( NULL );

lCount = 8192;
if ( !(lpDefaults = Alloc( lCount )) )
	return( NULL );

// Read the entire file into the buffer lpDefaults
lCount = _lread( ifh, lpDefaults, (WORD)lCount );

lp = lpDefaults;
lpNew = lp;
fCopyingName = YES;
while ( --lCount >= 0 )
	{ // Break the memory block up into pairs of strings
	c = *lp++;
	if ( fCopyingName )
		{
		if ( c == '[' )
			{ // Skip over any [Names] in brackets
			while ( --lCount >= 0 )
				{
				if ( *lp++ == ']' )
					break;
				}
			continue;
			}
		if ( c == '=' )
			{ // When we hit an equal sign, we change modes
			fCopyingName = NO;
			c = '\0';
			}
		else
		if ( c < ' ' || c > 128 ) // Skip over any NULLs and non-ascii
			continue;
		else
		if ( c == ' ' ) // Skip over any spaces
			continue;
		c = tolower(c);
		}
	else
	if ( !fCopyingName )
		{
		if ( c == '\r' || c == '\n' )
			{ // When we hit a "return", we change modes
			fCopyingName = YES;
			c = '\0';
			}
		else
		if ( c < ' ' || c > 128 ) // Skip over any NULLs and non-ascii
			continue;
		}
	*lpNew++ = c;
	}

*lpNew++ = '\0'; // Add a few nulls to terminate
*lpNew++ = '\0';
*lpNew++ = '\0';

_lclose( ifh );
return( lpDefaults );
}


/************************************************************************/
LPTR FindPreference( lpKeyName )
/************************************************************************/
LPTR lpKeyName;
{
LPTR lp;

if ( !(lp = lpPreferences) )
	return( NULL );
while ( *lp )
	{
	if ( StringsEqual( lp, lpKeyName ) )
		return( lp + lstrlen(lp) + 1 );
	lp = lp + lstrlen(lp) + 1;
	lp = lp + lstrlen(lp) + 1;
	}

return( NULL );
}


/************************************************************************/
void SaveToolPreferences( idTool )
/************************************************************************/
WORD idTool;
{
LPTR lp;
STRING szString;

if ( AstralStr( idTool, &lp ) )
	lstrcpy( szString, lp );
else	szString[0] = '\0';
if ( AstralOKCancel( IDS_OKTOSAVEPREF, (LPTR)szString ) == IDCANCEL )
	return;
}
