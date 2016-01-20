/* $Header: /PM8/ATM/Windows/ATM.H 1     3/03/99 6:14p Gbeddow $ */
/*
** ATM.h
**
** Version 3.02 -- 09-24-95
** Adobe Type Manager is a trademark of Adobe Systems Incorporated.
** Copyright 1983-1995 Adobe Systems Incorporated
** All Rights Reserved
*/

#ifndef _H_ATM
#define _H_ATM

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ATMAPI
#define ATMAPI       __stdcall
#define ATMCALLBACK  __stdcall
#endif

#define BD_VERSION_260  3  /* Backdoor version number. */
#define BD_VERSION_300  3
#define BD_VERSION_301  3
#define BD_VERSION_400  4
#define BD_VERSION      BD_VERSION_400

/* Define return values for non-BOOL functions. */
#define ATM_NOERR               (0)    /* Normal return. */
#define ATM_INVALIDFONT         (-1)   /* Font not consistent. */
#define ATM_CANTHAPPEN          (-2)   /* Some kind of internal error. */
#define ATM_BADMATRIX           (-3)   /* Matrix inverse undefined. */
#define ATM_MEMORY              (-4)   /* Out of memory. */
#define ATM_NOTSUPPORTED        (-5)   /* Running ATM doesn't support call. */
#define ATM_NOTRUNNING          (-6)   /* ATM is not running. */
#define ATM_CANCELLED           (-8)   /* Client halted operation. */
#define ATM_NOCHAR              (-9)   /* No outline for char code in font. */
#define ATM_BADPROC             (-100) /* Bad callback proc address. */
#define ATM_CANTDRAW            (-101) /* Error in imaging; a bad rop? */
#define ATM_BADPARM             (-102) /* Bad parameter passed in. */
#define ATM_SELECTED            (-200) /* For compatibility with old headers.
                                       ** See ATMSelectObject.
                                       */
#define ATM_NEWFONTSELECTED     (-200) /* See ATMSelectObject. */
#define ATM_OLDFONTSELECTED     (-201) /* See ATMSelectObject. */
#define ATM_FOREIGNFONTSELECTED (-202) /* See ATMSelectObject. */
#define ATM_SELECTERROR         (-204) /* See ATMSelectObject. */

#define ATM_ERROR               (-219) /* General error, such as out of memory,
                                       ** or input string too long. 
                                       */

#define ATM_NOTFOUND            (-223) /* Font not found. */
#define ATM_NOFONTS             (-224) /* No fonts were enumerated. */
#define ATM_SYSTEM              (-226) /* Operating system call error. */
#define ATM_BUSY                (-227) /* This reentry of the library requires
                                       ** a resource that is still in use.
                                       */

#define ATM_PATHTOOLONG         (-400) /* At least one path is too long. */
#define ATM_BADFONTTYPE         (-401) /* The font is uncompressed. */
#define ATM_BADSTYLE            (-402) /* A style with bits other than 
                                       ** ATM_ITALIC or ATM_BOLD.
                                       */
#define ATM_NOROOM              (-403) /* Can't add to internal tables. */
#define ATM_NOFONT              (-404) /* At least one path doesn't point to
                                       ** the correct file.
                                       */
#define ATM_BADMENUNAME         (-405) /* Missing/invalid menu name given. */
#define ATM_FONTINUSE           (-406) /* The paths and menu name specify an
                                       ** existing font that is in use.
                                       */
#define ATM_FONTPRESENT         (-407) /* The paths and menu name specify an
                                       ** existing font which is NOT in use.
                                       */
#define ATM_FONTDIFFERENT       (-408) /* The font, as specified by its menu
                                       ** name and styles, exists but its data
                                       ** files are different from those
                                       ** specified by lpMetricsFile and
                                       ** lpFontFile parameters.
                                       */
#define ATM_FONTABSENT          (-409) /* There is no such font installed. */
#define ATM_ADDERROR            (-410) /* General ATMAddFont error. */
#define ATM_MMMISSING           (-418) /* Add and/or FontStatus could not
                                       ** process instance because base font
                                       ** not installed.
                                       */
#define ATM_NOMMMFORMM          (-419) /* An attempt was made to add an MM Base
                                       ** font with a pfm/pfb pair:  an mmm is
                                       ** required for this.
                                       */
#define ATM_ALREADYINSTALLED    (-2000)/* Font was already installed. */
#define ATM_NOFONTFILE          (-2001)/* Bad or missing .pfb file. */
#define ATM_NOMETRICSFILE       (-2002)/* Bad or missing .pfm or .mmm file. */


/* Define additional flags for the wOption parameter of the ...XYShow... */
/* routines.  These flags are NOT supported in all versions of the ATM   */
/* backdoor.  Check the version numbers for each option flag below.      */
#define ATM_USELOGCOLOR         (0x0100) /* In ATM backdoor since version 2.01
                                         ** This flag causes ATM to use the
                                         ** logical color set for text
                                         ** foreground in the DC even when
                                         ** it would normally switch to the
                                         ** nearest pure color based on the
                                         ** settings in ATM.INI.
                                         ** This is mutually exclusive with
                                         ** ATM_USEPURECOLOR.  Incorrect usage
                                         ** will cause an error.
                                         */
#define ATM_USEPURECOLOR        (0x0200) /* In ATM backdoor since version 2.01
                                         ** This flag causes ATM to use the
                                         ** nearest pure color instead of the
                                         ** actual logical color set for text
                                         ** foreground in the DC.
                                         ** This is mutually exclusive with
                                         ** ATM_USELOGCOLOR.  Incorrect usage
                                         ** will cause an error.
                                         */
#define ATM_MAKEVISIBLE         (0x0400) /* In ATM backdoor since version 2.01
                                         ** This flag is used for B/W devices
                                         ** and causes ATM to use either white
                                         ** or black depending on the color of
                                         ** background in order to make the
                                         ** text visible.
                                         ** If this flag is specified,
                                         ** ATM_USELOGCOLOR & ATM_USEPURECOLOR
                                         ** are ignored.
                                         */


/* Define flags for the 'options' parameter of the ATMSelectObject routine */
/* These flags are NOT supported in all versions of ATM.  Check the        */
/* version numbers for each option flag below.                             */
#define ATM_DEFER               (0x0001) /* In ATM backdoor since version 2.01
                                         ** This flag causes ATM to defer to a
                                         ** device font when selecting a font
                                         ** handle into a DC the first time.
                                         ** This is mutually exclusive with
                                         ** ATM_SELECT.  Incorrect usage will
                                         ** cause an error.
                                         */
#define ATM_SELECT              (0x0002) /* In ATM backdoor since version 2.01
                                         ** This flag forces ATM to render a
                                         ** font even when normally it would
                                         ** be deferred to the device.  This
                                         ** can happen only when selecting a
                                         ** handle into a DC the first time.
                                         ** This is mutually exclusive with
                                         ** ATM_DEFER.  Incorrect usage will
                                         ** cause an error.
                                         */
#define ATM_USEEXACTWIDTH       (0x0004) /* In ATM backdoor since version 2.01
                                         ** This flag causes ATM to create the
                                         ** font being selected into a screen
                                         ** DC without adjusting the width
                                         ** data.  This can happen only when
                                         ** selecting a handle into a DC for
                                         ** the first time.
                                         */


#define ATM_ITALIC              (0x0001) /* In ATM backdoor since version 2.6.
                                         ** This flag is used in calls to and
                                         ** return values from ATM to indicate
                                         ** the style of a font's face name.
                                         ** If set, it indicates that the style
                                         ** associated with the font is ITALIC.
                                         */
#define ATM_BOLD                (0x0002) /* In ATM backdoor since version 2.6.
                                         ** This flag is used in calls to and
                                         ** return values from ATM to indicate
                                         ** the style of a font's face name.
                                         ** If set, it indicates that the style
                                         ** associated with the font is BOLD.
                                         */
#define ATM_BOLDITALIC          (ATM_BOLD | ATM_ITALIC)


/* Type bits for ATMEnumFonts() */
#define ATM_TYPE1               (0x0100) /* Normal Type 1 pfb file. */
#define ATM_MMTYPE1             (0x0200) /* Multiple Master Type 1 pfb file. */
#define ATM_MMINSTANCE          (0x0400) /* Instance of Multiple Master font.*/
#define ATM_DATABASE            (0x0800) /* Database substitution font. */
#define ATM_TEMPORARY           (0x1000) /* Can be ORed with the other bits. */

#define ATM_PSNAMESIZE          (64)     /* max length PostScript font name */
#define ATM_MAXPATHLEN          (260)    /* max length for .pfb & .pfm paths */
                            

/* mmFlags for ATMMMMetricsHeader structure */
#define MM_SUBSTOK              (0x01)   /* Font usable for substitution. */
#define MM_TEXTINSTOK           (0x02)   /* Font usable for text instances. */
#define MM_IS_PI                (0x04)   /* Font is decorative. */
#define MM_IS_FIXED_PITCH       (0x08)   /* Font is fixed pitch. */

/* mmAxisAttrs for ATMMMMetricsHeader structure */
#define MM_IS_ITALIC            (0x001)
#define MM_IS_BOLD              (0x002)
#define MM_IS_SERIF             (0x004)
#define MM_ITALIC_AXIS          (0x100)
#define MM_WEIGHT_AXIS          (0x200)
#define MM_SERIF_AXIS           (0x400)


#define MI_FACESIZE LF_FACESIZE


typedef long int ATMFixed;  /* 16.16 signed number.  Integer in hi word. */

typedef struct
  {
  ATMFixed a, b, c, d, tx, ty;
  } ATMFixedMatrix, *LPATMFixedMatrix;

typedef struct
  {
  ATMFixed x, y;
  } ATMFixedPoint, *LPATMFixedPoint;

typedef struct
  {
  short int x, y;
  } ATMShortPoint, *LPATMShortPoint;

typedef struct
  {
  ATMShortPoint ll, ur;
  } ATMBBox, *LPATMBBox;

typedef struct
  {
  char faceName[LF_FACESIZE];
  WORD styles;
  } ATMFontSpec, *LPATMFontSpec;

typedef struct
  {
  WORD  mmVersion;
  WORD  mmFlags;
  char  mmCopyright[72];
  char  mmFontVersion[8];
  char  mmPSFontName[80];
  WORD  mmAxisAttrs;
  BYTE  mmFamily;   // MS family designations.
  BYTE  mmEncoding;
  WORD  mmCharSet;
  WORD  mmFirstChar;
  WORD  mmLastChar;
  WORD  mmDefaultChar;
  WORD  mmBreakChar;
  WORD  mmNumKernPairs;
  WORD  mmNumKernTracks;
  WORD  mmNumUnencodedChars;
  WORD  mmSizeUnencodedCharNames; // In Bytes
  WORD  mmNumUnencodedKernPairs;
  short mmMasterUnits;
  short mmMasterHeight;
  DWORD mmSizeDesignPoint; // Size of one MMDesignPoint for this font.
  WORD  mmNumAxes;
  WORD  mmNumBaseDesigns;
  WORD  mmNumPrimaryDesigns;
  char  mmFileNamePrefix[6];
  char  mmMenuName[MI_FACESIZE];  // Has to be < MI_FACESIZE!
  DWORD mmReserved[20];
  } ATMMMMetricsHeader, *LPATMMMMetricsHeader;


typedef struct
  {
  LPSTR lpszInstName;
  WORD  styles;
  } ATMEnumMMInstInfo, *LPATMEnumMMInstInfo;

// lpEnumData is type LPMMMetricsHeader when ATMEnumMMFonts is called with NULL
// lpEnumData is type LPEnumMMInstInfo when ATMEnumMMFonts is called with name 

typedef struct
  {
  char pfbPath[ATM_MAXPATHLEN]; 
  char pfmPath[ATM_MAXPATHLEN];
  char mmmPath[ATM_MAXPATHLEN];
  char pssPath[ATM_MAXPATHLEN];
  } ATMFontPathInfo, *LPATMFontPathInfo;


#define ATMINTTOFIXED(n)        ((ATMFixed) (((long int) (n)) << 16))

#ifndef ATMBD_IMPL

/* 
** NOTE: Unless otherwise noted, the following functions that return ints
** use ATM_NOERR to indicate success. In addition to the specific values 
** noted with each function, these functions can all return the following 
** error values:
**
**      ATM_ERROR
**      ATM_CANTHAPPEN
**      ATM_NOTSUPPORTED
**      ATM_NOTRUNNING
**      ATM_BADPARM
**      ATM_BADSTYLE
*/


extern BOOL ATMAPI ATMProperlyLoaded (void);
        /*
        ** In ATM backdoor since version 1.0
        **
        ** This routine initializes the backdoor and returns true if all
        ** goes well and if ATM is loaded and running.  It will return false
        ** if ATM is not installed or loaded.
        **
        ** NOTE: You must have a call to 'ATMProperlyLoaded',
        **       or all other calls will fail.
        */

extern int ATMAPI ATMFinish (void);
        /* 
        ** In ATM backdoor since version 3.02 (Win32 only)
        **
        ** This routine unloads the backdoor and cleans things up.
        **
        ** NOTE: You must call ATMFinish when your program exits
        **       or you will have a memory leak!
        **
        ** Failure return values are:
        **     ATM_NOTRUNNING   (Library is not initialized)
        **     ATM_BUSY         (A callback has not returned)
        */

extern int ATMAPI ATMBeginFontChange (void);
        /* 
        ** In ATM backdoor since version 2.5
        **
        ** Starts a series of font change operations consisting of calls to
        ** the routines ATMAddFont and ATMRemoveFont.  Must be called before
        ** calling ATMAddFont or ATMRemoveFont.
        */


// backdoor API
#define ATMFontStatus(lpMenuName, styleAndType, lpMetricsFile, lpFontFile) \
    libATMFontStatus(BD_VERSION, lpMenuName, styleAndType, lpMetricsFile, \
                      lpFontFile)

// libATMFontStatus is for internal use only.  Applications should call
// ATMFontStatus.
extern int ATMAPI libATMFontStatus (
                                WORD  wBdVersion,
                                LPSTR lpMenuName,
                                WORD  styleAndType,
                                LPSTR lpMetricsFile,
                                LPSTR lpFontFile);
        /* 
        ** extern int ATMAPI ATMFontStatus (
        **                      LPSTR lpMenuName,
        **                      WORD  styleAndType,
        **                      LPSTR lpMetricsFile,
        **                      LPSTR lpFontFile);
        **
        ** In ATM backdoor since version 2.5
        **
        ** This routine determines whether or not a given font is currently
        ** installed and/or in use by ATM.  It should be called prior to
        ** installing a new font.
        ** If the return value is ATM_FONTINUSE, then no attempt should be made
        ** to install the font.
        ** 
        ** The lpMenuName contains the Windows name of the font. 
        **
        ** The styleAndType parameter contains none, one or more of:
        **     ATM_BOLD            -- a bold font
        **     ATM_ITALIC          -- an italic font
        **     ATM_TYPE1           -- a normal Type 1 font
        **     ATM_MMTYPE1         -- a Multiple Master Type 1 font
        **     ATM_MMINSTANCE      -- an instance of a Multiple Master font
        **     ATM_DATABASE        -- an instance from substitution database.
        ** Note that the 2.5 version of this routine allowed only the two style
        ** flags (ATM_BOLD and ATM_ITALIC) to be specified.
        **
        ** The lpMetricsFile contains the full pathname to the metrics file
        ** for the font.  This is a .pfm file for an ATM_TYPE1 or an .mmm file
        ** for an ATM_MMTYPE1. (FIX - Document what pathnames are needed for 
        ** other font types.) 
        **
        ** The lpFontFile contains the full pathname to the font file for
        ** the font.  This is a .pfb file for ATM_TYPE1 or ATM_MMTYPE1.  For an
        ** ATM_MMINSTANCE, this is the .pfb file for the Mutiple Master font.
        ** It is ignored for other types.
        **
        ** Successful return values are:
        **     ATM_FONTINUSE            
        **     ATM_FONTPRESENT          
        **     ATM_FONTDIFFERENT        
        **     ATM_FONTABSENT           
        **
        ** Failure return values are:
        **     ATM_PATHTOOLONG          
        **     ATM_BADFONTTYPE          
        **     ATM_NOFONT               
        **     ATM_BADMENUNAME          
        */


// backdoor API
#define ATMAddFont(lpMenuName, styleAndType, lpMetricsFile, lpFontFile) \
    libATMAddFont(BD_VERSION, lpMenuName, styleAndType, lpMetricsFile,  \
                   lpFontFile)

// libATMAddFont is for internal use only.  Applications should call
// ATMAddFont.
extern int ATMAPI libATMAddFont (
                                WORD  wBdVersion,
                                LPSTR lpMenuName,
                                WORD  styleAndType,
                                LPSTR lpMetricsFile,
                                LPSTR lpFontFile);
        /* 
        ** extern int ATMAPI ATMAddFont (
        **                      LPSTR lpMenuName,
        **                      WORD  styleAndType,
        **                      LPSTR lpMetricsFile,
        **                      LPSTR lpFontFile);
        **
        ** In ATM backdoor since version 2.5
        **
        ** This routine makes a newly installed font available to ATM.
        ** 
        ** The lpMenuName contains the Windows name of the font. 
        **
        ** The styleAndType parameter contains none, one or more of:
        **     ATM_BOLD            -- a bold font
        **     ATM_ITALIC          -- an italic font
        **     ATM_TYPE1           -- a normal Type 1 font
        **     ATM_MMTYPE1         -- a Multiple Master Type 1 font
        **     ATM_MMINSTANCE      -- an instance of a Multiple Master font
        **     ATM_DATABASE        -- an instance from substitution database.
        **
        ** Note that the 2.5 version of this routine allowed only the two style
        ** flags (ATM_BOLD and ATM_ITALIC) to be specified.
        **
        ** The lpMetricsFile contains the full pathname to the metrics file
        ** for the font.  This is a .pfm file for an ATM_TYPE1, ATM_MMINSTANCE,
        ** or ATM_DATABASE.  This is an .mmm file for an ATM_MMTYPE1.
        **
        ** The lpFontFile contains the full pathname to the font file for
        ** the font.  This is a .pfb file for ATM_TYPE1 or ATM_MMTYPE1.  For an
        ** ATM_MMINSTANCE, this is the .pfb file for the Mutiple Master font.
        ** It is ignored for other types.
        **
        ** Failure return values:
        **     ATM_INVALIDFONT          
        **     ATM_PATHTOOLONG          
        **     ATM_BADFONTTYPE          
        **     ATM_NOROOM               
        **     ATM_NOFONT               
        **     ATM_BADMENUNAME          
        **     ATM_FONTINUSE
        **     ATM_MMMVERSION
        **                              
        */


// backdoor API
#define ATMRemoveFont(lpMenuName, style) \
    libATMRemoveFont(BD_VERSION, lpMenuName, style)

// libATMRemoveFont is for internal use only.  Applications should call
// ATMRemoveFont.
extern int ATMAPI libATMRemoveFont (
                                WORD  wBdVersion,
                                LPSTR lpMenuName,
                                WORD  style);
        /* 
        ** extern int ATMAPI ATMRemoveFont (
        **                      LPSTR lpMenuName,
        **                      WORD  style);
        ** 
        ** In ATM backdoor since version 2.5
        ** 
        ** This call makes a font inaccessible to ATM. Any subsequent
        ** requests to ATM for this font will fail.
        ** 
        ** The lpMenuName contains the Windows name of the font. 
        **
        ** The style parameter contains none, one or both of the flags
        ** ATM_ITALIC and ATM_BOLD.
        **
        ** Failure return values include:
        **     ATM_NOTFOUND
        **     ATM_BADMENUNAME          
        */


extern int ATMAPI ATMForceFontChange (void);
        /* 
        ** In ATM backdoor since version 3.0
        ** 
        ** Forces a WM_FONTCHANGE message to be sent when ATMEndFontChange()
        ** is called regardless of whether a font was added or not.
        */


extern int ATMAPI ATMEndFontChange (void);
        /* 
        ** In ATM backdoor since version 2.5
        ** 
        ** Finishes a series of font change operations consisting of calls to
        ** the rotuines ATMAddFont and ATMRemoveFont.
        ** Call this function after font changes are completed, or your font
        ** changes may not take effect.
        */

// types for ATMEnumFonts callback
typedef BOOL (ATMCALLBACK ATMEnumFontProc)(LPLOGFONT  lpLogFont,
                                           LPSTR      lpPostScriptName,
                                           WORD       flags,
                                           DWORD      dwUserData);

typedef ATMEnumFontProc *LPATMEnumFontProc;

// backdoor API
#define ATMEnumFonts(lpProc, dwUserData) \
    libATMEnumFonts(BD_VERSION, lpProc, dwUserData)

// libATMEnumFonts is for internal use only.  Applications should call
// ATMEnumFonts.
extern int ATMAPI libATMEnumFonts (
                                WORD              wBdVersion,
                                LPATMEnumFontProc lpProc,
                                DWORD             dwUserData);
        /*
        ** extern int ATMAPI ATMEnumFonts(
        **                      LPATMEnumFontProc lpProc,
        **                      DWORD             dwUserData);
        **
        ** In ATM backdoor since version 2.6
        **
        ** This routine enumerates all of ATM's fonts. These fonts may
        ** be Type 1 fonts, Multiple Master Type 1 fonts, 
        ** Multiple Master instances, or database fonts.
        **
        ** The callback procedure lpProc is called once for each 
        ** font. The callback is called with a LOGFONT for the font,
        ** a pointer to the PostScript name of the font, and a flag 
        ** that indicates the type of the font.
        **
        ** Font types are: ATM_TYPE1, ATM_MMTYPE1, ATM_MMINSTANCE,
        ** and ATM_DATABASE.
        **
        ** If the callback returns zero, the enumeration halts.
        ** 
        ** Returns ATM_CANCELLED if the callback returns zero.
        **
        ** Returns ATM_BUSY if another callback has not returned.
        **
        ** NOTE: If no fonts are installed, the callback will not be 
        ** called, and this function will return ATM_NOFONTS.
        */


// types for ATMEnumMMFonts callback
typedef BOOL (ATMCALLBACK ATMEnumMMFontProc)(LPVOID   lpEnumData,
                                             DWORD    dwUserData);

typedef ATMEnumMMFontProc *LPATMEnumMMFontProc;

// backdoor API
#define ATMEnumMMFonts(lpszMMMenuName, lpProc, dwUserData) \
    libATMEnumMMFonts(BD_VERSION, lpszMMMenuName, lpProc, dwUserData)

// libATMEnumMMFonts is for internal use only.  Applications should call
// ATMEnumMMFonts.
extern int ATMAPI libATMEnumMMFonts (
                                WORD                wBdVersion,
                                LPSTR               lpszMMMenuName,
                                LPATMEnumMMFontProc lpProc,
                                DWORD               dwUserData);
        /* 
        ** extern int ATMAPI ATMEnumMMFonts (
        **                      LPSTR               lpszMMMenuName,
        **                      LPATMEnumMMFontProc lpProc,
        **                      DWORD               dwUserData);
        **
        ** In ATM backdoor since version 2.6
        ** 
        ** This function enumerates installed multiple master base fonts and 
        ** instances.  To enumerate base fonts, the caller passes in a NULL
        ** menu name, and the callback procedure receives a pointer to an
        ** ATMMMMetricsHeader structure.  To enumerate instances, the caller
        ** passes in a multiple master base font menu name, and the callback
        ** procedure receives a pointer to an ATMEnumMMInstInfo structure.
        **
        ** Returns ATM_BUSY if another callback has not returned.
        **
        */

extern BOOL ATMAPI ATMFontAvailable (
                                LPSTR    lpFacename,
                                int      nWeight,
                                BYTE     cItalic,
                                BYTE     cUnderline,
                                BYTE     cStrikeOut,
                                int *    lpFromOutline);
        /*
        ** In ATM backdoor since version 1.0
        **
        ** Returns TRUE if ATM can render the specified font.
        ** lpFromOutline is non-zero if there is an actual outline available
        ** to ATM for rendering the font.  It will be 0 if ATM would
        ** synthesize the font from a base outline.
        */


extern BOOL ATMAPI ATMFontSelected (HDC hDC);
        /* 
        ** In ATM backdoor since version 1.0
        **
        ** Returns TRUE if the currently selected font in the hDC is an
        ** ATM font.
        */


extern int ATMAPI ATMGetFontBBox (
                                HDC       hDC,
                                LPATMBBox lpFontBBox);
        /* 
        ** In ATM backdoor since version 2.01
        **
        ** This routine obtains the font bounding box for the font currently
        ** selected into 'hDC'.
        **
        ** lpFontBBox is expressed in unscaled character units.  If NULL,
        ** ATM_BADPARM is returned.
        */
          

// backdoor API
#define ATMGetFontPaths(lpFontSpec, lpFontPaths) \
    libATMGetFontPaths(BD_VERSION, lpFontSpec, lpFontPaths)

// libATMGetFontPaths is for internal use only.  Applications should call
// ATMGetFontPaths.
extern int ATMAPI libATMGetFontPaths (
                                WORD              wBdVersion,
                                LPATMFontSpec     lpFontSpec,
                                LPATMFontPathInfo lpFontPaths);
        /* 
        ** extern int ATMAPI ATMGetFontPaths (
        **                      LPATMFontSpec     lpFontSpec,
        **                      LPATMFontPathInfo lpFontPaths);
        **
        ** In ATM backdoor since version 2.6
        **
        ** Given a font name and style via the lpFontSpec, this routine
        ** returns the font path and file names in the structure pointed
        ** to by lpFontPaths.
        **
        ** The following table indicates which paths will be nonempty
        ** for particular types of fonts. An 'x indicates that this
        ** path will be set in all cases. An 'y' indicates that this
        ** path will be set in some cases.
        **                      
        **                      PFB     PFM     MMM     PSS
        ** ATM_TYPE1             x       x
        ** ATM_MMTYPE1           x               x
        ** ATM_MMINSTANCE        x               x        
        **
        ** Failure return values include:
        **     ATM_NOFONT
        **     ATM_NOFONTFILE
        **     
        */


// backdoor API
#define ATMGetMenuName(lpPostScriptName, lpFont) \
    libATMGetMenuName(BD_VERSION, lpPostScriptName, lpFont)

// libATMGetMenuName is for internal use only.  Applications should call
// ATMGetMenuName.
extern int ATMAPI libATMGetMenuName(
                                WORD            wBdVersion,
                                LPSTR           lpPostScriptName,
                                LPATMFontSpec   lpFont);
       /* 
       ** extern int ATMAPI ATMGetMenuName(
       **                       LPSTR           lpPostScriptName,
       **                       LPATMFontSpec   lpFont);
       **
       ** In ATM backdoor since version 2.6
       **
       ** Takes a PostScript name and looks up the Windows name.
       ** This function looks at the list of currently installed
       ** fonts and potentially at the font database.
       **
       ** The lpPostScriptName is the input argument, which is a pointer 
       ** to a string containing the PostScript name of the font in question. 
       ** The Windows name and style bits are returned in the LPATMFontSpec 
       ** argument. The ATM_ITALIC and ATM_BOLD bits indicate the styles.
       **
       ** Possible return values include:
       **    ATM_NOTFOUND
       **    ATM_NODATABASE
       **    ATM_DBERROR
       **    ATM_BADPSNAME
       */


// types for ATMGetOutline callbacks
// ClosePath proc types
typedef BOOL (ATMCALLBACK ATMClosePathProc)(
								DWORD 			dwUserData);
typedef ATMClosePathProc *LPATMClosePathProc;

// MoveTo proc types
typedef BOOL (ATMCALLBACK ATMMoveToProc)(
								LPATMFixedPoint lpFixPnt,
								DWORD 			dwUserData);
typedef ATMMoveToProc *LPATMMoveToProc;

// LineTo proc types
typedef BOOL (ATMCALLBACK ATMLineToProc)(
								LPATMFixedPoint lpFixPnt,
								DWORD 			dwUserData);
typedef ATMLineToProc *LPATMLineToProc;

// CurveTo proc types
typedef BOOL (ATMCALLBACK ATMCurveToProc)(
								LPATMFixedPoint lpFixPnt1,
                                LPATMFixedPoint lpFixPnt2,
                                LPATMFixedPoint lpFixPnt3,
                                DWORD 			dwUserData);
typedef ATMCurveToProc *LPATMCurveToProc;

// backdoor API
extern int ATMAPI ATMGetOutline (
                                HDC              	hDC,
                                char             	c,
                                LPATMFixedMatrix 	lpMatrix,
                                LPATMMoveToProc     lpProcMoveTo,
                                LPATMLineToProc     lpProcLineTo,
                                LPATMCurveToProc    lpProcCurveTo,
                                LPATMClosePathProc  lpProcClosePath,
                                DWORD            	dwUserData);
        /* 
        ** In ATM backdoor since version 1.0
        **
        ** Reports the outline of the specified character.
        **
        ** Returns ATM_BUSY if another callback has not returned.
        **
        */


// backdoor API
#define ATMGetPostScriptName(lpFontSpec, lpPostScriptName) \
    libATMGetPostScriptName(BD_VERSION, lpFontSpec, lpPostScriptName)

// libATMGetPostScriptName is for internal use only.  Applications should
// call ATMGetPostScriptName.
extern int ATMAPI libATMGetPostScriptName(
                                WORD            wBdVersion,
                                LPATMFontSpec   lpFontSpec,
                                LPSTR           lpPostScriptName);
        /* 
        ** extern int ATMAPI ATMGetPostScriptName(
        **                     LPATMFontSpec   lpFontSpec,
        **                     LPSTR           lpPostScriptName);
        **
        ** In ATM backdoor since version 2.6
        **
        ** Given a font name and style via the lpFontSpec, this routine
        ** returns the postscript name in the buffer pointer to by 
        ** lpPostScriptName. This buffer must be ATM_PSNAMESIZE bytes in
        ** size.
        **
        ** Possible return values include:
        **   ATM_NOTFOUND
        */


extern WORD ATMAPI ATMGetVersion (void);
        /* 
        ** In ATM backdoor since version 1.0
        **
        ** Returns the version number of the currently running ATM.  Minor
        ** number in high byte and major number in low byte.
        */


extern int ATMAPI ATMSelectObject (
                                HDC      hDC,
                                HANDLE   hNewObject,
                                WORD     options,
                                LPHANDLE lphOldObject);
        /* 
        ** In ATM backdoor since version 2.01
        **
        ** The handle being selected into the DC is passed in 'hNewObject'.
        ** The handle to the old object is returned in 'lphOldObject'.  If
        ** this parameter is NULL, the old object is not returned.
        **
        ** Successful return values are:
        **      ATM_NEWFONTSELECTED     hNewObject selected for the 1st time.
        **      ATM_OLDFONTSELECTED     hNewObject had previously been selected
        **                              into the same DC.  'options' will NOT
        **                              have any effect in this case.
        **      ATM_FOREIGNFONTSELECTED hNewObject represents a non-ATM font
        **                              (such as when ATM_DEFER flag is given)
        **
        **   NOTE that versions 2.01 and 2.02 of ATM had another successful
        **        return value for this routine: ATM_NONFONTSELECTED.  This
        **        is no longer supported.
        **
        ** Failure return values are:
        **      ATM_NOTRUNNING          ATM is not running.
        **      ATM_BADPARM             Conflicting 'options' flags.
        **      ATM_SELECTERROR         'hDC' or 'hNewObject' is bad/invalid.
        */
                              

extern BOOL ATMAPI ATMXYShowText (
                                HDC              hDC,
                                int              x,
                                int              y,
                                WORD             wOptions,
                                LPRECT           lpRect,
                                LPSTR            lpString,
                                int              nCount,
                                LPATMFixedPoint  lpPoints,
                                LPATMFixedMatrix lpMatrix);
        /* 
        ** In ATM backdoor since version 1.0
        **
        ** Renders text given an arbitrary transformation matrix (lpMatrix)
        ** and specific widths (lpPoints) for precise positioning of each
        ** character in the string.  All but the last two parameters
        ** correspond to and are interpreted the same as the parameters of
        ** GDI's ExtTextOut with the following exceptions:
        **
        **    1) the wOptions parameter may include additional ATM-specific
        **       values ATM_USELOGCOLOR, ATM_USEPURECOLOR, or ATM_MAKEVISIBLE.
        **    2) Opaquing text is not supported.  That is, if wOptions includes
        **       the ETO_OPAQUE option, it will be ignored.
        **
        ** Returns TRUE if the call is successful, FALSE otherwise.
        **
        ** lpPoints is expressed in DEVICE units.  If NULL, default character
        ** widths are used.
        ** lpMatrix is expressed in DEVICE units.  If NULL, the font is used
        ** unmodified.
        **
        ** NOTE: x and y are the coordinates of the upper left corner of the
        **       text box without taking the matrix into consideration.  In
        **       other words, x,y defines the origin of a normal TextOut or
        **       ExtTextOut call which would use the font unmodified (i.e.
        **       with default matrix).  This means that the baseline position
        **       is determined relative to the unmodified font.  Also note
        **       that x and y are expressed in LOGICAL units (just as the x
        **       and y parameters of the TextOut and ExtTextOut routines).
        **       However, the character widths passed in 'lpPoints' are
        **       expressed in DEVICE units and are not subject to the current
        **       mapping mode in effect for 'hDC'.  This means that for
        **       devices that are capable of rendering graphics at lower than
        **       normal resolutions (such as the PCL driver's ability to
        **       switch to 150 or 75 dpi), these widths are expressed at the
        **       graphics resolution in effect.
        **       lpRect is expressed in LOGICAL units.
        */


extern BOOL ATMAPI ATMBBoxBaseXYShowText (
                                HDC              hDC,
                                int              x,
                                int              y,
                                WORD             wOptions,
                                LPRECT           lpRect,
                                LPSTR            lpString,
                                int              nCount,
                                LPATMFixedPoint  lpPoints,
                                LPATMFixedMatrix lpMatrix,
                                BOOL             bDoOutput,
                                LPATMBBox        lpBBox,
                                LPATMFixedPoint  lpDelta);
        /* 
        ** In ATM backdoor since version 1.15
        **
        ** This routine is quite similar to the ATMXYShowText routine with
        ** some slight differences.  First, x and y specify the coordinates
        ** of the start of the baseline (rather than the upper left corner).
        ** Similar to ATMXYShowText, x and y are interpreted as LOGICAL units,
        ** lpRect is expressed in LOGICAL units, and lpPoints are given in
        ** DEVICE units.
        ** If bDoOutput is TRUE then actual output is produced in the same
        ** manner as ATMXYShowText.
        ** lpBBox will receive the bounding box (or extent), in LOGICAL units,
        ** of the string.
        ** lpDelta will receive the deltas that should be applied to x,y in
        ** order to make a following call to this routine continue at the
        ** correct point on the baseline.  lpDelta is given in DEVICE units.
        **
        ** Returns TRUE if the call is successful, FALSE otherwise.
        **
        ** lpBBox is expressed in LOGICAL units.  If NULL, bbox is not
        ** returned.
        ** lpDelta is expressed in DEVICE units.  If NULL, delta is not
        ** returned.
        */

#endif /* ATMBD_IMPL */

#ifdef __cplusplus
}
#endif

#endif /* _H_ATM */
