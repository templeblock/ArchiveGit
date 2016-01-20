//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static void HandleBrushSettings( int idTool, BOOL fPut );

static STRING szPreferenceFile;

#define PRIVATE_INI "CRAYOLA.INI"
#define ID_APPL	"Crayola"

#define GetDefMergeMode(N,INT) (N=(MERGE_MODE)GetDefaultInt(#N,INT))
#define GetDefDitherType(N,INT) (N=(DITHER_TYPE)GetDefaultInt(#N,INT))

/************************************************************************/
BOOL GetIniPath( LPSTR lpIniFile )
/************************************************************************/
{
FNAME szFileName;
BOOL bRet;

#ifdef _MAC
	// The macintosh ALWAYS uses the "Root:System:Preferences:" path when
	// acessing preferences - ALWAYS.
	// Do not use path information in the INI filename.
	return TRUE;
#else

if ( GetProfileString( "Micrografx", PRIVATE_INI, "",
	szFileName, sizeof(FNAME) ) > 2 )
	{
	FixPath( szFileName );
	lstrcat( szFileName, lpIniFile );
	if ( bRet = FileExists( szFileName ) )
		goto Found;
	}

#ifdef CDBUILD
if ( GetWindowsDirectory( szFileName, sizeof(FNAME) ) > 2 )
	{
	FixPath( szFileName );
	lstrcat( szFileName, lpIniFile );
	if ( bRet = FileExists( szFileName ) )
		goto Found;
	}
#endif

lstrcpy( szFileName, Control.ProgHome );
lstrcat( szFileName, lpIniFile );
if ( !(bRet = FileExists( szFileName )) )
	Message( IDS_INI_NOEXIST, Lowercase(lpIniFile) );

Found:
lstrcpy( lpIniFile, szFileName );

#ifndef CDBUILD
if ( bRet && !FilePermission( lpIniFile, 2 /*WRITE*/ ) )
	Message( IDS_INI_NOWRITE, Lowercase(lpIniFile) );
#endif

// Set the network variable if the INI and EXE are not in the same place
lstrcpy( szFileName, lpIniFile );
stripfile( szFileName );
FixPath( szFileName );
Control.bNetworkInstall = !StringsEqual( Control.ProgHome, szFileName );
return( bRet );
#endif // #ifdef _MAC
}

/************************************************************************/
void ResetIniPath( void )
/************************************************************************/
{
LPSTR lp;

if ( !AstralStr( IDS_INIFILE, &lp ) ) lp = PRIVATE_INI;
lstrcpy( szPreferenceFile, lp );
GetIniPath( szPreferenceFile );
}

/************************************************************************/
int GetDefaultInt( LPSTR lpKeyName, int nDefault )
/************************************************************************/
{
LPSTR lp;

#ifdef PRIVATE_INI
if ( !szPreferenceFile[0] )
	{
	if ( !AstralStr( IDS_INIFILE, &lp ) ) lp = PRIVATE_INI;
	lstrcpy( szPreferenceFile, lp );
	GetIniPath( szPreferenceFile );
	}
return( GetPrivateProfileInt( ID_APPL, stripstruct(lpKeyName),
	nDefault, szPreferenceFile ) );

#else
return( GetProfileInt( ID_APPL, stripstruct(lpKeyName), nDefault ) );
#endif
}


/************************************************************************/
WORD GetDefaultString( LPSTR lpKeyName, LPSTR lpDefault, LPSTR lpReturnedString,
			int nSize)
/************************************************************************/
{
LPSTR lp;

#ifdef PRIVATE_INI
if ( !szPreferenceFile[0] )
	{
	if ( !AstralStr( IDS_INIFILE, &lp ) ) lp = PRIVATE_INI;
	lstrcpy( szPreferenceFile, lp );
	GetIniPath( szPreferenceFile );
	}
return( GetPrivateProfileString( ID_APPL, stripstruct(lpKeyName),
	lpDefault, lpReturnedString, nSize, szPreferenceFile ) );

#else
return( GetProfileString( ID_APPL, stripstruct(lpKeyName), lpDefault,
	lpReturnedString, nSize ) );
#endif
}


/************************************************************************/
BOOL PutDefaultInt( LPSTR lpKeyName, int nDefault )
/************************************************************************/
{
char tmp[40];

return( PutDefaultString( lpKeyName, itoa(nDefault,tmp,10) ) );
}


/************************************************************************/
BOOL PutDefaultString( LPSTR lpKeyName, LPSTR lpString)
/************************************************************************/
{
LPSTR lp;

#ifdef PRIVATE_INI
if ( !szPreferenceFile[0] )
	{
	if ( !AstralStr( IDS_INIFILE, &lp ) ) lp = PRIVATE_INI;
	lstrcpy( szPreferenceFile, lp );
	GetIniPath( szPreferenceFile );
	}
return( WritePrivateProfileString( ID_APPL, stripstruct(lpKeyName),
	lpString, szPreferenceFile ));
#else
return( WriteProfileString( ID_APPL, stripstruct(lpKeyName), lpString ));
#endif
}

/************************************************************************/
void LoadAllPreferences(BOOL fMemOnly)
/************************************************************************/
{
LPSTR lp;
int i, English, Didot;
STRING tmp;

if (fMemOnly) // memory related info only
	{
	// Memory Group Preferences
	GetDefStr( Control.RamDisk, Control.ProgHome );
	FixPath( Lowercase( Control.RamDisk ) );
	GetDefInt( Control.MainMemMin, 1024 );
	GetDefInt( Control.MainMemFactor, 100 );
	if (  Control.MainMemFactor < 10 )
		Control.MainMemFactor = 10;
	if (  Control.MainMemFactor > 100 )
		Control.MainMemFactor = 100;
	GetDefInt( Control.LineArtAsGray, NO );
	FrameSettings( Control.RamDisk, Control.MainMemMin, Control.MainMemFactor);

	// Get Gamma settings
	GetDefFix( BltSettings.RGamma, 1 );
	GetDefFix( BltSettings.GGamma, 1 );
	GetDefFix( BltSettings.BGamma, 1 );
	return;
	}

AstralCursor( IDC_WAIT );
if ( !szPreferenceFile[0] )
	{
	if ( !AstralStr( IDS_INIFILE, &lp ) ) lp = PRIVATE_INI;
	lstrcpy( szPreferenceFile, lp );
	GetIniPath( szPreferenceFile );
	}

// Recall List
Control.RecallCount = 0;
for ( i=0; i<MAX_RECALL; i++ )
	{
	GetDefaultString( itoa(i,tmp,10), "",
		Control.RecallImage[i], sizeof(Control.RecallImage[i]) );
	if ( *Control.RecallImage[i] )
		Control.RecallCount++;
	}

// Undo Group Preferences
GetDefInt( Control.NoUndo, FALSE );
GetDefInt( Control.UseApply, NO );
GetDefInt( Control.UndoObjects, NO );
GetDefInt( Control.UndoMasks, NO );

// Miscellaneous Group Preferences
#ifdef _MAC
lstrcpy(Control.PouchPath, ":files:");
#else
GetDefStr( Control.PouchPath, Control.ProgHome );
if ( Control.PouchPath[1] != ':' )
	{
	lstrcpy( tmp, Control.ProgHome );
	lstrcat( tmp, Control.PouchPath );
	lstrcpy( Control.PouchPath, tmp );
	}
FixPath( Lowercase( Control.PouchPath ) );
#endif // _MAC	
GetDefInt( Control.UseWacom, 0 );
GetDefInt( Control.MaskTint, 0 ); // Red
GetDefInt( Control.IndependentSettings, YES );
GetDefInt( Control.bNoTheme, NO );

#ifdef USEWAVEMIX
GetDefInt( Control.bUseWaveMix, YES );
#endif // USEWAVEMIX

GetPrivateProfileString( "Drivers", "Wave", "none", tmp, sizeof(STRING),
  "system.ini");

// if they have no wave driver or the speaker.drv installed, disable
// using the wavemix.dll
#ifdef USEWAVEMIX
if ( (! lstrcmpi( tmp, "none")) || (! lstrcmpi( tmp, "Speaker.drv")) ||
  (! lstrcmpi( tmp, "")) )
	Control.bUseWaveMix = NO;
#endif // USEWAVEMIX

GetDefInt( Control.iAdventBrush, BRUSH_SIZE_MEDIUM );
GetDefInt( Control.iMaxAnimations, 10 );
GetDefInt( Control.iScrnSaverTime, 2 );
IntermissionSetTimeout ((DWORD)Control.iScrnSaverTime * 60000);

// Units Group Preferences
English = GetProfileInt( "intl", "iMeasure", 1 ); // 1 = English
if ( GetDefInt( Didot, -1 ) >= 0 ) // If Didot is overridden...
	Control.Points = ( !Didot ? 723 : 676 );
else	Control.Points = ( English ? 723 : 676 );
GetDefInt( Control.Units, !English ); Control.Units += IDC_PREF_UNITINCHES;
GetDefFix( Control.ScreenWidth, 9 );
GetDefInt( View.UseRulers, NO );
GetDefInt( Control.DisplayPixelValue, NO );

// Object Group Preferences
GetDefInt( Control.MultipleObjects, YES );
GetDefInt( Control.UseMaskAndObjects, YES );

// New Dialog
GetDefInt( Control.NewDepth, 3 );
GetDefInt( Control.NewWidth, 480 );
GetDefInt( Control.NewHeight, 360 );
GetDefInt( Control.NewResolution, 50 );

// Monitor Gamma Dialog
GetDefInt( Control.GammaLock, NO );

// Save file options
GetDefInt( Save.bSaveMask, NO );
GetDefInt( Save.OKtoSavePath, NO );
GetDefInt( Save.Compressed, YES );

// Clipboard settings
GetDefInt( Control.DoPicture, NO );
GetDefInt( Control.DoBitmap, YES );

// Miscellaneous
GetDefInt( Control.ScreenColor, NO );
GetDefRGB( Control.rgbBackground, "0,0,0" );
GetDefInt( Control.xImage, 4 );
GetDefInt( Control.yImage, 4 );
GetDefInt( Control.Hints, YES );
GetDefInt( Control.Save24BitWallpaper, FALSE);
GetDefInt( Control.UseObjectMarquee, NO );

// Extended Names
GetDefStr( Names.ColorMap, "" );
GetDefStr( Names.Special, "" );
GetDefStr( Names.Mask, "" );
GetDefStr( Names.Printer, "" );
GetDefStr( Names.PrintStyle, "" );
GetDefStr( Names.Clipboard, "" );
GetDefStr( Names.CustomBrush, "" );

// Print Style
LoadPrintStyle( Names.PrintStyle );

// Print Dialog
GetDefInt( Page.Type, 1 ); Page.Type += IDC_PRINTER_IS_MONO;
GetDefInt( Page.PrintNumeric, NO );
GetDefInt( Page.OutputType, 0 ); Page.OutputType += IDC_PRINT_GRAY;
GetDefInt( Page.ScatterPrint, NO );
GetDefInt( Page.Centered, YES );
GetDefFix( Page.OffsetX, 0 );
GetDefFix( Page.OffsetY, 0 );
GetDefInt( Page.SepFlags, 0 );
GetDefInt( Page.TrimMarks, NO );
GetDefInt( Page.RegMarks, NO );
GetDefInt( Page.Labels, NO );
GetDefInt( Page.StepScale, NO );
GetDefInt( Page.Negative, NO );
GetDefInt( Page.EmulsionDown, NO );
GetDefInt( Page.BinaryPS, NO ); // Binary Postscript
GetDefInt( Page.BinaryEPS, NO ); // Binary Encapsulated Postscript
GetDefInt( Page.UsePrinterScreening, YES );

// Size Dialog
GetDefInt( Edit.SmartSize, NO );

// Convert settings
GetDefInt( Convert.MiniType, 0); Convert.MiniType += IDC_MINI256;
GetDefInt( Convert.fOptimize, YES );
GetDefInt( Convert.fScatter, NO );
GetDefInt( Convert.fDither, NO );

// Selector Tool

// Mask Transformer Tool
GetDefInt( Mask.TransformImage, 1 );
Mask.TransformImage = Mask.TransformImage + IDC_COPYIMAGE;

// Object Transformer Tool
GetDefInt( Mask.iTransformModes, SHAPE_DEFAULT );
GetDefInt( Mask.PasteQuality, NO );
GetDefInt( Mask.TransMode, 0 ); Mask.TransMode += IDC_TRANSMODEFIRST;
GetDefInt( Mask.TransformOpacity, 255 );
Mask.TransformOpacity = bound( Mask.TransformOpacity, 0, 255 );
GetDefMergeMode( Mask.TransformMergeMode, (int)MM_NORMAL );

// Paint Tools
HandleBrushSettings( IDC_PAINT, NO );
HandleBrushSettings( IDC_CRAYON, NO );
HandleBrushSettings( IDC_MARKER, NO );
HandleBrushSettings( IDC_LINES, NO );
HandleBrushSettings( IDC_SHAPES, NO );
HandleBrushSettings( IDC_ERASER, NO );

// Gradient Tool
GetDefInt( Vignette.Gradient, 0 ); Vignette.Gradient += IDC_VIGLINEAR;
GetDefInt( Vignette.RepeatCount, 1 );
GetDefInt( Vignette.SoftTransition, YES );
GetDefInt( Vignette.VigColorModel, 0);
GetDefInt( Vignette.VigOpacity, 255 );
GetDefMergeMode( Vignette.VigMergeMode, (int)MM_NORMAL );
GetDefInt( Vignette.Midpoint, 128 );

// Texture Fill Tool
GetDefStr( Texture.TextureName, "" );
GetDefInt( Texture.fHorzFlip, NO );
GetDefInt( Texture.fVertFlip, NO );
GetDefInt( Texture.TextureOpacity, 255 );
GetDefMergeMode( Texture.TextureMergeMode, (int)MM_NORMAL );

// Tint Fill Tool
GetDefInt( Fill.FillOpacity, 255 );
GetDefMergeMode( Fill.FillMergeMode, (int)MM_NORMAL );

// Magic Fill Tool
GetDefInt( Fill.FloodRange, 2 );
GetDefInt( Fill.idFillColorModel, 0);
GetDefInt( Fill.FloodOpacity, 255 );
GetDefMergeMode( Fill.FloodMergeMode, (int)MM_NORMAL );

// Custom View Tool
GetDefInt( View.ZoomOut, NO );
GetDefInt( View.ZoomWindow, NO );
GetDefInt( View.ZoomDefaultSize, 0x6000);
GetDefInt( View.FullScreen, NO );

// Text Tool
GetDefInt( Text.nFont, 0 );
GetDefInt( Text.Size, 48 );
GetDefInt( Text.Italic, NO );
GetDefInt( Text.Underline, NO );
GetDefInt( Text.Strikeout, NO );
GetDefInt( Text.Weight, NO );
GetDefInt( Text.AntiAlias, NO );
GetDefInt( Text.AutoFill, YES );
GetDefMergeMode( Text.TextMergeMode, (int)MM_NORMAL );
GetDefInt( Text.TextOpacity, 255 );

// Load Common Tool Settings
GetDefInt( Common.Opacity, 255 );
GetDefMergeMode( Common.MergeMode, (int)MM_NORMAL );
GetDefInt( Common.BrushSize, 15 );

SetCommonSettings();
Shields( ON );

LoadFileLocations();
AstralCursor( NULL );
}


/************************************************************************/
void SavePreferences()
/************************************************************************/
{
PutDefInt( Control.NoUndo, Control.NoUndo );
PutDefInt( Control.Hints, Control.Hints );
PutDefInt( Control.bNoTheme, Control.bNoTheme );
#ifdef USEWAVEMIX
PutDefInt( Control.bUseWaveMix, Control.bUseWaveMix );
#endif // USEWAVEMIX
PutDefInt( Control.iAdventBrush, Control.iAdventBrush );
PutDefInt( Control.iMaxAnimations, Control.iMaxAnimations );
PutDefInt( Control.iScrnSaverTime, Control.iScrnSaverTime );

#ifdef UNUSED
AstralCursor( IDC_WAIT );

// Memory Group Preferences
PutDefStr( Control.RamDisk, Control.RamDisk );
PutDefInt( Control.MainMemMin, Control.MainMemMin );
PutDefInt( Control.MainMemFactor, Control.MainMemFactor );
PutDefInt( Control.LineArtAsGray, Control.LineArtAsGray );

// Undo Group Preferences
PutDefInt( Control.NoUndo, Control.NoUndo );
PutDefInt( Control.UseApply, Control.UseApply );
PutDefInt( Control.UndoObjects, Control.UndoObjects );
PutDefInt( Control.UndoMasks, Control.UndoMasks );

// Miscellaneous Group Preferences
PutDefStr( Control.PouchPath, Control.PouchPath );
PutDefInt( Control.UseWacom, Control.UseWacom );
PutDefInt( Control.MaskTint, Control.MaskTint );
PutDefInt( Control.IndependentSettings, Control.IndependentSettings );

// Units Group Preferences
PutDefInt( Control.Units, Control.Units - IDC_PREF_UNITINCHES );
PutDefInt( View.UseRulers, View.UseRulers );
PutDefInt( Control.DisplayPixelValue, Control.DisplayPixelValue );
PutDefFix( Control.ScreenWidth, Control.ScreenWidth );

// Object Group Preferences
PutDefInt( Control.MultipleObjects, Control.MultipleObjects );
PutDefInt( Control.UseMaskAndObjects, Control.UseMaskAndObjects );

AstralCursor( NULL );
#endif
}


/************************************************************************/
void SaveMiscPreferences()
/************************************************************************/
{
//LPSTR lp;

PutDefInt( Page.Type, Page.Type - IDC_PRINTER_IS_MONO );
PutDefInt( Page.BinaryPS, Page.BinaryPS );

#ifdef UNUSED
AstralCursor( IDC_WAIT );
if ( !szPreferenceFile[0] )
	{
	if ( !AstralStr( IDS_INIFILE, &lp ) ) lp = PRIVATE_INI;
	lstrcpy( szPreferenceFile, lp );
	GetIniPath( szPreferenceFile );
	}

// Recall List
SaveRecallList();

// Save file options
PutDefInt( Save.bSaveMask, Save.bSaveMask );
PutDefInt( Save.OKtoSavePath, Save.OKtoSavePath );
PutDefInt( Save.Compressed, Save.Compressed );

// Clipboard
PutDefInt( Control.DoPicture, Control.DoPicture );
PutDefInt( Control.DoBitmap, Control.DoBitmap );

// New Dialog
PutDefInt( Control.NewDepth, Control.NewDepth );
PutDefInt( Control.NewWidth, Control.NewWidth );
PutDefInt( Control.NewHeight, Control.NewHeight );
PutDefInt( Control.NewResolution, Control.NewResolution );

// Miscellaneous
PutDefRGB( Control.rgbBackground, Control.rgbBackground );
PutDefInt( Control.Save24BitWallpaper, Control.Save24BitWallpaper );

// Monitor Gamma Dialog
PutDefFix( BltSettings.RGamma, BltSettings.RGamma );
PutDefFix( BltSettings.GGamma, BltSettings.GGamma );
PutDefFix( BltSettings.BGamma, BltSettings.BGamma );
PutDefInt( Control.GammaLock, Control.GammaLock );

// Extended Names
PutDefStr( Names.ColorMap, Names.ColorMap );
PutDefStr( Names.Mask, Names.Mask );
PutDefStr( Names.Printer, Names.Printer );
PutDefStr( Names.PrintStyle, Names.PrintStyle );
PutDefStr( Names.Clipboard, Names.Clipboard );
PutDefStr( Names.CustomBrush, Names.CustomBrush );
PutDefStr( Names.Special, Names.Special );

// Print Dialog
PutDefInt( Page.Type, Page.Type - IDC_PRINTER_IS_MONO );
PutDefInt( Page.PrintNumeric, Page.PrintNumeric );
PutDefInt( Page.OutputType, Page.OutputType - IDC_PRINT_GRAY );
PutDefInt( Page.ScatterPrint, Page.ScatterPrint );
PutDefInt( Page.Centered, Page.Centered );
PutDefFix( Page.OffsetX, Page.OffsetX );
PutDefFix( Page.OffsetY, Page.OffsetY );
PutDefInt( Page.SepFlags, Page.SepFlags );
PutDefInt( Page.TrimMarks, Page.TrimMarks );
PutDefInt( Page.RegMarks, Page.RegMarks );
PutDefInt( Page.Labels, Page.Labels );
PutDefInt( Page.StepScale, Page.StepScale );
PutDefInt( Page.Negative, Page.Negative );
PutDefInt( Page.EmulsionDown, Page.EmulsionDown );
PutDefInt( Page.BinaryPS, Page.BinaryPS );
//PutDefInt( Page.BinaryEPS, Page.BinaryEPS );
PutDefInt( Page.UsePrinterScreening, Page.UsePrinterScreening );

// Size Dialog
PutDefInt( Edit.SmartSize, Edit.SmartSize );

// Convert settings
PutDefInt( Convert.MiniType, Convert.MiniType-IDC_MINI256);
PutDefInt( Convert.fOptimize, Convert.fOptimize );
PutDefInt( Convert.fScatter, Convert.fScatter );
PutDefInt( Convert.fDither, Convert.fDither );

// Save Common Tool Settings
PutDefInt( Common.Opacity, Common.Opacity );
PutDefInt( Common.MergeMode, Common.MergeMode );
PutDefInt( Common.BrushSize, Common.BrushSize );
AstralCursor( NULL );
#endif
}


/***********************************************************************/
void SaveRecallList()
/***********************************************************************/
{
BYTE tmp[10];
int i;

for (i = 0; i < MAX_RECALL; ++i)
	{
	if (i >= Control.RecallCount)
			PutDefaultString( itoa(i,(char*)tmp,10), "" );
	else	PutDefaultString( itoa(i,(char*)tmp,10), Control.RecallImage[i] );
	}
}


/************************************************************************/
LPSTR stripstruct( LPSTR lpString)
{
LPSTR lp;

lp = lpString;
while ( *lp )
	{
	if ( *lp++ == '.' )
		return( lp );
	}
return( lpString );
}


/************************************************************************/
BOOL SaveToolPreferences( int idTool)
{
LPSTR lp;
STRING szString;

#ifndef NOTCRAYOLA
	return(YES); // DON'T WRITE INTO THE INI JUST YET
#endif

if ( AstralStr( IDC_TOOLNAMESTART+idTool, &lp ) )
	lstrcpy( szString, lp );
else	szString[0] = '\0';
if ( AstralOKCancel( IDS_OKTOSAVEPREF, (LPSTR)szString ) == IDCANCEL )
	return( NO );

switch ( idTool )
   {
   case 0:
	break;
   case IDC_TRANSFORMER:
	PutDefInt( Mask.TransformImage, Mask.TransformImage-IDC_COPYIMAGE);
	PutDefInt(Mask.iTransformModes, Mask.iTransformModes ); 
	PutDefInt( Mask.PasteQuality, Mask.PasteQuality );
	PutDefInt( Mask.TransMode, Mask.TransMode-IDC_TRANSMODEFIRST );
	PutDefInt( Mask.TransformOpacity, Mask.TransformOpacity );
	PutDefInt( Mask.TransformMergeMode, Mask.TransformMergeMode );
	break;
   case IDC_PAINT:
   case IDC_CRAYON:
   case IDC_MARKER:
   case IDC_LINES:
   case IDC_SHAPES:
   case IDC_ERASER:
	HandleBrushSettings(idTool, YES);
	break;
   case IDC_FILL:
	PutDefInt( Fill.FloodRange, Fill.FloodRange );
	PutDefInt( Fill.idFillColorModel, Fill.idFillColorModel);
	PutDefInt( Fill.FloodOpacity, Fill.FloodOpacity );
	PutDefInt( Fill.FloodMergeMode, Fill.FloodMergeMode );
	PutDefInt( Fill.FillOpacity, Fill.FillOpacity );
	PutDefInt( Fill.FillMergeMode, Fill.FillMergeMode );
	PutDefInt( Vignette.Gradient, Vignette.Gradient - IDC_VIGLINEAR );
	PutDefInt( Vignette.RepeatCount, Vignette.RepeatCount );
	PutDefInt( Vignette.SoftTransition, Vignette.SoftTransition );
	PutDefInt( Vignette.VigColorModel, Vignette.VigColorModel );
	PutDefInt( Vignette.VigOpacity, Vignette.VigOpacity );
	PutDefInt( Vignette.VigMergeMode, Vignette.VigMergeMode );
	PutDefInt( Vignette.Midpoint, Vignette.Midpoint );
	PutDefStr( Texture.TextureName, Texture.TextureName );
	PutDefInt( Texture.fHorzFlip, Texture.fHorzFlip );
	PutDefInt( Texture.fVertFlip, Texture.fVertFlip );
	PutDefInt( Texture.TextureOpacity, Texture.TextureOpacity );
	PutDefInt( Texture.TextureMergeMode, Texture.TextureMergeMode );
	break;
   case IDC_CUSTOMVIEW:
	PutDefInt( View.ZoomOut, View.ZoomOut );
	PutDefInt( View.ZoomWindow, View.ZoomWindow );
	break;
   case IDC_TEXT:
	PutDefInt( Text.nFont, Text.nFont );
	PutDefInt( Text.Size, Text.Size );
	PutDefInt( Text.Italic, Text.Italic );
	PutDefInt( Text.Underline, Text.Underline );
	PutDefInt( Text.Strikeout, Text.Strikeout );
	PutDefInt( Text.Weight, Text.Weight );
	PutDefInt( Text.AntiAlias, Text.AntiAlias );
	PutDefInt( Text.AutoFill, Text.AutoFill );
	PutDefInt( Text.TextMergeMode, Text.TextMergeMode );
	PutDefInt( Text.TextOpacity, Text.TextOpacity );
	break;
   }
return( YES );
}


/************************************************************************/
static void HandleBrushSettings( int idTool, BOOL fPut )
/************************************************************************/
{
LPSTR lp;
BRUSHSETTINGS FAR *Br;
STRING str;
STRING tmp;
LPSTR pName;
int brush;

if ( fPut )
	return; // DON'T WRITE INTO THE INI JUST YET

switch ( idTool )
    {
    case IDC_PAINT:
	pName = "Paint";
	break;
    case IDC_CRAYON:
	pName = "Crayon";
	break;
    case IDC_MARKER:
	pName = "Marker";
	break;
    case IDC_LINES:
	pName = "Lines";
	break;
    case IDC_SHAPES:
	pName = "Shapes";
	break;
    case IDC_ERASER:
	pName = "Eraser";
	break;
    default:
	return;
    }

/* set basic portion of property name */
lstrcpy( str, pName );
lp = str + lstrlen( str);

if ( fPut )
	SaveRetouchBrush( idTool );

/* set basic portion of property name */
lstrcpy( str, pName );
lp = str + lstrlen( str);
brush = GetBrushIndex( idTool );
Br = &lpBrushSettings[brush];

lstrcpy( lp, "BrushShape");
if (fPut)
	PutDefaultString( str, itoa( Br->BrushShape - IDC_BRUSHCIRCLE, tmp, 10));
else
	Br->BrushShape = GetDefaultInt( str, 0 ) + IDC_BRUSHCIRCLE;
lstrcpy( lp, "BrushSize");
if (fPut)
	PutDefaultString( str, itoa( Br->BrushSize, tmp, 10));
else
	Br->BrushSize = GetDefaultInt( str, 15 );
lstrcpy( lp, "Opacity");
if (fPut)
	PutDefaultString( str, itoa( Br->Opacity, tmp, 10));
else
	Br->Opacity = GetDefaultInt( str, 255 );
lstrcpy( lp, "Pressure");
if (fPut)
	PutDefaultString( str, itoa( Br->Pressure, tmp, 10));
else
	Br->Pressure = GetDefaultInt( str, 64 );
lstrcpy( lp, "MergeMode");
if (fPut)
	PutDefaultString( str, itoa( Br->MergeMode, tmp, 10));
else
	Br->MergeMode = (MERGE_MODE)GetDefaultInt( str, (int)MM_NORMAL );
lstrcpy( lp, "Feather");
if (fPut)
	PutDefaultString( str, itoa( Br->Feather, tmp, 10));
else
	Br->Feather = GetDefaultInt( str, 100 );
lstrcpy( lp, "StyleName" );
if ( fPut )
	PutDefaultString( str, Retouch.StyleName );
else
	GetDefaultString( str, "", Retouch.StyleName, sizeof(FNAME) );
}

/************************************************************************/
void SetCommonSettings()
/************************************************************************/
{
SetCommonOpacity(Common.Opacity);
SetCommonMergeMode(Common.MergeMode);
SetCommonBrushSize(Common.BrushSize);
}

/************************************************************************/
void SetCommonOpacity(int Opacity)
/************************************************************************/
{
int i;

Common.Opacity = Opacity;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].Opacity = Opacity;
	}
Vignette.VigOpacity = Opacity;
Fill.FillOpacity = Opacity;
Fill.FloodOpacity = Opacity;
Mask.TransformOpacity = Opacity;
Text.TextOpacity = Opacity;
Texture.TextureOpacity = Opacity;
}

/************************************************************************/
void SetCommonMergeMode(MERGE_MODE MergeMode)
/************************************************************************/
{
int i;

Common.MergeMode = MergeMode;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].MergeMode = MergeMode;
	}
Vignette.VigMergeMode = MergeMode;
Fill.FillMergeMode = MergeMode;
Fill.FloodMergeMode = MergeMode;
Mask.TransformMergeMode = MergeMode;
Text.TextMergeMode = MergeMode;
Texture.TextureMergeMode = MergeMode;
}

/************************************************************************/
void SetCommonBrushSize(int BrushSize)
/************************************************************************/
{
int i;

Common.BrushSize = BrushSize;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].BrushSize = BrushSize;
	}
}


