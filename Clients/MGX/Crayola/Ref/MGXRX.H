/****************************************************************************
  
         (C) Copyright 1992 by Micrografx, Inc.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
            from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

 ****************************************************************************

                                   MGXRX.H

 ****************************************************************************/

#ifndef _MGXRX_H_
#define _MGXRX_H_

#ifdef CPLUSPLUS_API
#include "afxwin.h"
#include "basetype.h"
#else
#include "windows.h"
#include "basetype.h"
#endif

/******************************** Constants *********************************/

/* MGXRX Version */
 
#define MGXRX_VERSION       MAKELONG (0,1)   // revision/version

/* Commen Control Messages */

#define CTLM_GETSCHEME      (WM_USER + 100)  // Get ctl attribute scheme
#define CTLM_SETSCHEME      (WM_USER + 101)  // Set ctl attribute scheme
#define CTLM_GETCOLOR       (WM_USER + 102)  // Get ctl specific color value 
#define CTLM_SETCOLOR       (WM_USER + 103)  // Set ctl specific color value
#define CTLM_SETRXSTYLE     (WM_USER + 104)  // Get rx style dword 
#define CTLM_GETRXSTYLE     (WM_USER + 105)  // Set rx style dword
#define CTLM_SETSTYLE       (WM_USER + 106)  // Change loword of window style
#define CTLM_GETNOMINALSIZE (WM_USER + 107)  // Get min size of ctl
#define CTLM_SCHEMECHANGED  (WM_USER + 108)  // Scheme changed, need to update

/* Specific Control Messages */

#define BTNM_CLICK          (WM_USER + 99)   // Forces a click on the btn ctl
#define BTNM_GETCHECK       BM_GETCHECK      // determines if btn is checked
#define BTNM_SETCHECK       BM_SETCHECK      // Sets or removes check from btn
#define BTNM_GETSTATE       BM_GETSTATE      // Determines if btn is pressed
#define BTNM_SETSTATE       BM_SETSTATE      // Sets pressed state of btn
#define BTNM_SETSTYLE       BM_SETSTYLE      // Sets control style of btn

#define PBM_INIT            (WM_USER + 500)  // Initialize picture btn ctl

#define RM_DIVISIONSIZE     (WM_USER + 501)   // Number of coord between major divisions
#define RM_INCREMENT        (WM_USER + 502)   // Ruler numbering increment
#define RM_MAX              (WM_USER + 503)   // Maximum coordinate value for the ruler
#define RM_NMARKS           (WM_USER + 504)   // Number of subdivision marks/ruler div
#define RM_SHOWPOS          (WM_USER + 505)   // TRUE if the cursor position is shown
#define RM_HORZTYPE         (WM_USER + 506)   // TRUE if it is a horizontal ruler
#define RM_SETMAPPING       (WM_USER + 507)   // Used to measure logical dist
#define RM_GETMAPPING       (WM_USER + 508)   // Retrieve the mapping
#define RM_SETPOS           (WM_USER + 509)   // Set the current cursor pos
#define RM_POSTPOS          (WM_USER + 510)   // Display the current cursor pos

#define PCTM_SETSCALE       (WM_USER + 520)   // set scaler & divisor in lparam
#define PCTM_CHANGE         (WM_USER + 521)   // change picture, style 

#define SCM_GETPERCENT      (WM_USER + 530)   // change picture, style 
#define SCM_SETPERCENT      (WM_USER + 531)   // change picture, style 

#define SLM_BUILDRENDERINFO     (WM_USER + 540)
#define SLM_DESTROYRENDERINFO   (WM_USER + 541)
#define SLM_GETATTR             (WM_USER + 542)
#define SLM_GETVALUE            (WM_USER + 543)
#define SLM_SETATTR             (WM_USER + 544)
#define SLM_SETFILLCOLOR        (WM_USER + 545)
#define SLM_SETSTYLE            (WM_USER + 546)
#define SLM_SETVALUE            (WM_USER + 547)

#define DLM_GETCURVAL       (WM_USER + 600)     // get current setting of the dial ctl
#define DLM_SETCURVAL       (WM_USER + 601)     // sets current setting of the dial ctl
#define DLM_GETLORANGE      (WM_USER + 602)     // gets lower range of the dial 
#define DLM_SETLORANGE      (WM_USER + 603)     //  sets lower range of the dial
#define DLM_GETHIRANGE      (WM_USER + 604)     // gets higher range of the dial 
#define DLM_SETHIRANGE      (WM_USER + 605)     // sets higher range of the dial 
#define DLM_SETTICKS        (WM_USER + 606)     // sets the intervals for tickmarks

#define SM_GETRANGE         (WM_USER + 650)
#define SM_GETVALUE         (WM_USER + 651)
#define SM_GETSTEP          (WM_USER + 652)
#define SM_GETTHUMBSIZE     (WM_USER + 653)
#define SM_GETTICKS         (WM_USER + 654)
#define SM_SETDATA          (WM_USER + 655)
#define SM_SETDRAWPROC      (WM_USER + 656)    
#define SM_SETRANGE         (WM_USER + 657)
#define SM_SETVALUE         (WM_USER + 658)
#define SM_SETSTEP          (WM_USER + 659)
#define SM_SETTHUMBSIZE     (WM_USER + 660)     
#define SM_SETTICKS         (WM_USER + 661)

#define SM_GETXRANGE        (WM_USER + 671)
#define SM_GETYRANGE        (WM_USER + 672)
#define SM_SETXRANGE        (WM_USER + 673)
#define SM_SETYRANGE        (WM_USER + 674)

#define SPM_GETCOUNTS       (WM_USER + 701)  // Spinner Counts (at a speed) DWORD
#define SPM_SETCOUNTS       (WM_USER + 702)
#define SPM_GETSPEED        (WM_USER + 703)  // Spinner Speed (notify/sec) DWORD
#define SPM_SETSPEED        (WM_USER + 704)
#define SPM_GETMIN          (WM_USER + 705)  // Spinner current min value
#define SPM_SETMIN          (WM_USER + 706)
#define SPM_GETMAX          (WM_USER + 707)  // Spinner current max value
#define SPM_SETMAX          (WM_USER + 708)
#define SPM_GETVALUE        (WM_USER + 709)  // Spinner current value
#define SPM_SETVALUE        (WM_USER + 710)
#define SPM_GETINCR         (WM_USER + 711)  // Spinner increment value
#define SPM_SETINCR         (WM_USER + 712)
                                             
#define PBM_GETTEXTLEN      (WM_USER + 721)  // Get text length for a state
#define PBM_GETTEXT         (WM_USER + 722)  // Get text for a state
#define PBM_SETTEXT         (WM_USER + 723)  // Set text for a state
#define PBM_GETIMAGE        (WM_USER + 724)  // Get image offset for a state
#define PBM_SETIMAGE        (WM_USER + 725)  // Set image offset for a state
#define PBM_SETCOL          (WM_USER + 726)  // Set image col for all states
#define PBM_SETROW          (WM_USER + 727)  // Set image row for all states
#define PBM_SETROWCOL       (WM_USER + 728)  // Set image row&col - all states

#define CSM_SETCOLOR        (WM_USER + 801)  // set color -- Color Swatch
#define CSM_GETCOLOR        (WM_USER + 802)
#define CSM_SETCHECK        BM_SETCHECK      // get current check value
#define CSM_GETCHECK        BM_GETCHECK      // set current check value

#define CPM_GETCOLOR        (WM_USER + 820)    // Color Picker msgs
#define CPM_SETCOLOR        (WM_USER + 821)    
#define CPM_GETMODEL        (WM_USER + 822)
#define CPM_GETSLICEAXES    (WM_USER + 823)
#define CPM_SETCUBE         (WM_USER + 824)
#define CPM_SETSTEP         (WM_USER + 825)
#define CPM_GETSTEP         (WM_USER + 826)
#define CPM_DISPLAY         (WM_USER + 827)
#define CPM_BLACKCOMP       (WM_USER + 828)

/* TabList Class User-Defined Messages */

#define TLM_GETSELECTED		(WM_USER + 901)
#define TLM_SETSELECTED		(WM_USER + 902)
#define TLM_DELETESELECTED	(WM_USER + 903)
#define TLM_CLEARALL        (WM_USER + 904)
#define TLM_GETCOUNT        (WM_USER + 905)
#define TLM_GETRANGE		(WM_USER + 906)
#define TLM_SETRANGE		(WM_USER + 907)
#define TLM_SETTICKS		(WM_USER + 908)
#define TLM_GETTICKS        (WM_USER + 909)
#define TLM_GETALLVALUES    (WM_USER + 910)
#define TLM_GETMINMAXOFFSET (WM_USER + 911)
#define TLM_GETDELETED      (WM_USER + 912)
#define TLM_GETADDED        (WM_USER + 913)
#define TLM_GETCHANGED      (WM_USER + 914)
#define TLM_ADDTHUMB        (WM_USER + 915)

/* Picture List Class User-Defined Messages */

#define PLM_GETSEL         (WM_USER + 1000)
#define PLM_SETSEL         (WM_USER + 1001)
#define PLM_SETPICTURE     (WM_USER + 1002)
#define PLM_SETTEXT        (WM_USER + 1004)
#define PLM_SETOWNERDRAW   (WM_USER + 1005)
#define PLM_ADDCELL        (WM_USER + 1006)
#define PLM_REMOVECELL     (WM_USER + 1007)
#define PLM_REMOVEALL      (WM_USER + 1008)
#define PLM_GETCELLCOUNT   (WM_USER + 1009)
#define PLM_GETALLSELECTED (WM_USER + 1010)
#define PLM_SETCELLMINSIZE (WM_USER + 1011)
#define PLM_REDRAWCELL     (WM_USER + 1012)
#define PLM_GETITEMDATA    LB_GETITEMDATA
#define PLM_SETITEMDATA    LB_SETITEMDATA

/* Big Ruler Class User-Defined Messages */

#define BRM_SETOFFSET       (WM_USER + 1100)
#define BRM_SETSCALE        (WM_USER + 1101)
#define BRM_SETPOS          (WM_USER + 1102)
#define BRM_SETLOWBOUND     (WM_USER + 1103)
#define BRM_SETHIGHBOUND    (WM_USER + 1104)
#define BRM_SETMARKS        (WM_USER + 1105)
#define BRM_SETDIVISIONSIZE (WM_USER + 1106)
#define BRM_SETINCREMENT    (WM_USER + 1107)
#define BRM_SETMAXCOORD     (WM_USER + 1108)

#define BRM_GETPOS          (WM_USER + 1109)
#define BRM_GETOFFSET       (WM_USER + 1110)
#define BRM_GETLOWBOUND     (WM_USER + 1111)
#define BRM_GETHIGHBOUND    (WM_USER + 1112)
#define BRM_GETSCALE        (WM_USER + 1113)
#define BRM_GETMARKS        (WM_USER + 1114)
#define BRM_GETDIVISIONSIZE (WM_USER + 1115)
#define BRM_GETINCREMENT    (WM_USER + 1116)
#define BRM_GETMAXCOORD     (WM_USER + 1117)

/* Common Control Notifications */

#define CTLN_CLICKED        0           // matches BN_CLICKED
#define CTLN_DOUBLECLICKED  5           // matches BN_DOUBLECLICKED
#define CTLN_BROWSED        10
#define CTLN_UNBROWSED      11
#define CTLN_BUTTONDOWN     12

/* Specific Control Notifications */

#define DLN_VALCHANGED      1           // dial value changed
#define DLN_VALCHANGING     2           // dial value changing 

#define SN_VALUECHANGING    1          // dragging of slider thumb.
#define SN_VALUESELECTED    2          // clicked slider.

/* Spinner notifications.  If the spinner has a text field, then the only
   message received will be SPN_CHANGED.  Otherwise, the spinner will use
   the line, page, and top/bottom notifications. */

#define SPN_LINEUP          SB_LINEUP      // Spinner line-up notification
#define SPN_LINEDOWN        SB_LINEDOWN    // Spinner line-down notification
#define SPN_PAGEUP          SB_PAGEUP      // Spinner page-up notification
#define SPN_PAGEDOWN        SB_PAGEDOWN    // Spinner page-down notification
#define SPN_TOP             SB_TOP         // Spinner top notification
#define SPN_BOTTOM          SB_BOTTOM      // Spinner bottom notification
#define SPN_CHANGED         EN_CHANGE      // Spinner value changed notification
#define SPN_CHANGING        (EN_CHANGE+1)  // Spinner value changing notification
#define SPN_CHANGE          SPN_CHANGED    // Backwards flag compatibility

#define CSN_COLCHANGED	    1             // color swatch color changed

/* TabList Class Notifications */

#define TLN_VALCHANGED      1         // value of the selected thumb changed
#define TLN_VALCHANGING     2         // value of the selected thumb changing
#define TLN_SELCHANGED      3         // selection moved to another thumb
#define TLN_GOTFOCUS        4         // control received focus
#define TLN_LOSTFOCUS       5         // control lost focus
#define TLN_ADDED           6         // new thumb added
#define TLN_DELETED         7         // an existing thumb deleted

/* Picture List Class Notifications */

#define PLN_SELCHANGED       1

/* Color Picker Notifications */
                            
#define CPN_COLORCHANGING   1        
#define CPN_COLORSELECTED   2        
                            
/* Color Picker Models */

#define CP_RGB          1
#define CP_HLS          2
#define CP_CMY          3
#define CP_CMYK         4

/* Color Picker Cube Axes */

#define CP_RED          1
#define CP_GREEN        2
#define CP_BLUE         3
#define CP_CYAN         1
#define CP_YELLOW       2
#define CP_MAGENTA      3
#define CP_HUE          1
#define CP_LIGHTNESS    2
#define CP_SATURATION   3

/* Color Picker Display Resolution */

#define CP_COARSE       0
#define CP_AVERAGE      1
#define CP_FINE         2
#define CP_PURE         3


/* Color Types for CTLM_SET/GETCOLOR Message */

#define SC_CHECK1           0   // Color for checkbox check mark 
#define SC_CHECK2           1   // Color for checkbox second check mark (3STATE)
#define SC_TEXT             2   // Color of text for text class
#define SC_BTNFACE          3   // Color of button face PushBtn class
#define SC_BTNSHADOW        4   // Color of button shadow for PushBtn class
#define SC_BTNHILIGHT       5   // Color of button hilight for PushBtn class
#define SC_BORDER           6   // Color of border
#define SC_BACKGROUND       7   // Color of background
#define SC_BKSHADOW         8   // Color of background shadow (frames)
#define SC_BKHILIGHT        9   // Color of background hilight (frames)
#define SC_RULERFACE       10   // Color of ruler face (ruler)
#define SC_RULERHILIGHT    11   // Color of ruler hilight (ruler)
#define SC_RULERSHADOW     12   // Color of ruler shadow (ruler)
#define SC_RULERMARKER     13   // Color of ruler marker (ruler)
#define SC_RADIO           14   // Color of radiobtn "button" (radio)
#define SC_BKCOLOR1        15   // Color of background class foreground
#define SC_BKCOLOR2        16   // Color of background class background
#define SC_STATUSBAR       17   // Color of status percenet bar (status)
#define SC_PIE             18   // Color of status indicator in Dial
#define SC_TICKMARKS       19   // Color of tickmarks in dials and sliders
#define SC_UNFILLED        20   // Color of unfilled area of slider
#define SC_FILLED          21   // color of filled area of slider
#define SC_CROSSHAIR       22   // color of crosshair for 2D slider
#define SC_LISTBKGROUND    23   // color of background in a list (pictlist)
#define SC_RULERPOS        24   // color of position indicator (big ruler)
#define SC_RULERBOUNDS     25   // color of bounds marks (big ruler)
                            
/* CTLM_SET/GETCOLOR special color values */

#define CTLCOLOR_DEFAULT   -1L  // Pass into SETCOLOR msg as color. (use default color)
#define CTLCOLOR_ERROR     -2L  // Returned from color msgs if color not supportered

/* Check value attribute for Super Buttons */

#define PBF_INDETERMINATE   0x8000      // Check flag specifies indeterminate

/* SuperButton States */

#define PBST_UP              1      // Button is up and not selected.
#define PBST_PRESSED         2      // Button is depressed via mouse/keyboard.
#define PBST_DISABLED        3      // Disabled.
#define PBST_INDETERMINATE   4      // Special indeterminate (between) state.
#define PBST_SELECTED        PBST_INDETERMINATE // +n, where 1<=n<=wCheckMax...
                                    // ... identifies each selected state.

/* Standard Attributes Schemes IDs */
                                        
#define AS_DEFAULT              0       // Default gray color scheme
#define AS_WINDOWS              1       // Based on defined window colors

/* Attributes Schemes Indices used with ChangeAttribute () */

#define ATT_FONT                0       //  handle of font used for drawing text
#define ATT_TEXTCOLOR           1       //  text color     
#define ATT_DISABLEDTEXTCOLOR   2       //  disabled text color
#define ATT_CHECK1COLOR         3       //  check (non mutually exclusive) mark color 
#define ATT_CHECK2COLOR         4       //  check mark color for 3rd state of 3 state btns
#define ATT_RADIOCOLOR          5       //  radio (mutually exclusive) mark color
#define ATT_BTNHILIGHTCOLOR     6       //  button 3D hilight color
#define ATT_BTNFACECOLOR        7       //  button 3D face color
#define ATT_BTNSHADOWCOLOR      8       //  button 3D shadow color
#define ATT_BTNTEXTCOLOR        9       //  button text color
#define ATT_EDITCOLOR          10       //  edit background color
#define ATT_LISTCOLOR          11       //  list background color
#define ATT_BKHILIGHTCOLOR     12       //  control background 3D hilight color
#define ATT_BACKGROUNDCOLOR    13       //  control background color
#define ATT_BKSHADOWCOLOR      14       //  control background 3D shadow color
#define ATT_BORDERCOLOR        15       //  control border color 
#define ATT_STATUSBARCOLOR     16       //  status bar fill color 

#define RXA_LEFT                0x00    // RX alignment options
#define RXA_CENTER              0x10
#define RXA_RIGHT               0x20
#define RXA_TOP                 0x00
#define RXA_MIDDLE              0x01
#define RXA_BOTTOM              0x02

#define RXF_FIXWIDTH            0x0001  // RxSetControlSize flags
#define RXF_VARWIDTH            0x0002
#define RXF_FIXHEIGHT           0x0010
#define RXF_VARHEIGHT           0x0020

#define RXF_TOP                 1       // RxSetControlPos flags
#define RXF_BOTTOM              2
#define RXF_BEFORE              3
#define RXF_AFTER               4


/* Required Class/Extra WORDs For RXC_RESIZE Classes */

#define RXR_CLASSWORDBASE   0

#define RXR_DEFTABLE        0
#define RXR_EXTRAWORDBASE   (sizeof (void near *))

/* Required Class/Extra WORDs For RXC_DLGBOX Classes */

#define RXD_CLASSWORDBASE   RXR_CLASSWORDBASE

#define RXD_DLGSTATE        RXR_EXTRAWORDBASE
#define RXD_EXTRAWORDBASE   (RXD_DLGSTATE + sizeof (void far *))

/********************************** Macros **********************************/
/********************************** Types ***********************************/

typedef struct          // Picture btn bitmap reference structure (PBM_INIT)
{                       // -----------------------------------------------
    uint BitmapId;      // Id of BITMAP containing picturebtn renderings
    uint CellWidth;     // pixel width of cells defined within bitmap
    uint CellHeight;    // pixel height of cells defined within bitmap
    byte RowOffset;     // cell row (0-n) containing picturebtn renderings
    byte nBrowseFaces;  // number of browse renderings defined in bitmap
    uint State;         // initial PBS_* state of picturebtn 
}   BMP_REF, far *LPBMPREF;

typedef struct                  //  Attribute scheme data struct
{                               //  ---------------------------
    uint    wReserved;          //  reserved                                          
    handle  hFont;              //  handle of font used for drawing text
    ulong   rgbText;            //  text color     
    ulong   rgbDisabledText;    //  disabled text color
    ulong   rgbCheck1;          //  check (non mutually exclusive) mark color 
    ulong   rgbCheck2;          //  check mark color for 3rd state of 3 state btns
    ulong   rgbRadio;           //  radio (mutually exclusive) mark color
    ulong   rgbBtnHilight;      //  button 3D hilight color
    ulong   rgbBtnFace;         //  button 3D face color
    ulong   rgbBtnShadow;       //  button 3D shadow color
    ulong   rgbBtnText;         //  button text color
    ulong   rgbEdit;            //  edit background color
    ulong   rgbList;            //  list background color
    ulong   rgbBkHilight;       //  control background 3D hilight color
    ulong   rgbBackground;      //  control background color
    ulong   rgbBkShadow;        //  control background 3D shadow color
    ulong   rgbBorder;          //  control border color 
    ulong   rgbStatus;          //  status bar fill color 
}   ATTSCHEME, far *LPATTSCHEME;

typedef struct                  // Ruler control mapping structure
{                               // -------------------------------
    int   MapMode;              // Windows map mode.
    ulong WindowOrg;            // Window origin (logical)
    ulong WindowExt;            // Window extent (logical)
    ulong ViewportOrg;          // Viewport origin (device)
    ulong ViewportExt;          // Viewport extent (device)
}   RULER_MAPPING, far *LPRULERMAP;

typedef struct                  // Ruler Initilization structure
{                               // -------------------------------
    uint wDivisionSize;         // Number of coords between major divisions
    uint wIncrement;            // Ruler numbering increment
    uint wMax;                  // Maximum coordinate value for the ruler
    uint wNumMarks;             // Number of subdivision marks per division
    bool bShowPosition;         // TRUE if the cursor position is shown
    bool bHorizontal;           // TRUE if it is a horizontal ruler 
    int  iPosition;             // Current Position;
    RULER_MAPPING RulerMapping; // Used to measure logical distances
}   RULER_INIT, far *LPRULERINIT;  

enum RXCLASS
{
    RXC_NONE        = 0,
    RXC_NOMINALSIZE = 1,        // responds to CTLM_GETNOMINALSIZE
    RXC_RESIZE      = 2,
    RXC_DLGBOX      = 3
};

typedef struct          // Picture initializition structure
{                       // --------------------------------
    int ResourceId;     // Id of picture resource, Null if uses window text
    int Scaler;         // Scaling parameters for PCS_SCALED type 
    int Divisor;        // Scaling parameters for PCS_SCALED type 
}   PICTINIT, far * LPPICTINIT;

typedef struct          // Background initializition structure
{                       // -----------------------------------
    uint  StockId;      // Id of stock patttern or hatch brush,ignored for solid 
                        // and gradients. (BKP_* and BKH_*)
    ulong ForColor;     // Solid, gradient from, hatch/pattern forground color
    ulong BkColor;      // Gradient to, hatch/pattern background color
}   BKINIT, far * LPBKINIT;

typedef struct          // Slider create/data struct 
{                       // ------------------------------------------------
   int Value;           // Value of Slider
   int Min;             // Min Value
   int Max;             // Max Value
   int Step;            // Minimun increment value
   int BigTickInc;      // number of value units between big tick marks.
   int SmallTickInc;    // number of value units between small tick marks.
}  SSliderData;

typedef struct          // Two Dimensional Slider create/data struct 
{                       // ------------------------------------------------
   int xValue;          // Horz Value of Slider
   int yValue;          // Vert Value of Slider
   int xMin;            // Horz Min Value
   int yMin;            // Vert Min Value
   int xMax;            // Horz Max Value
   int yMax;            // Vert Max Value
   int xStep;           // Horz Minimun increment value
   int yStep;           // Vert Minimun increment value
   int xTickInc;        // number of horz value units between grid marks.
   int yTickInc;        // number of vert value units between grid marks.
}  SSliderData2D;

typedef struct          // Dial create/data struct 
{                       // ------------------------------------------------
   int Max;             // Max of dial range (default : 360)
   int Min;             // Min of dial range (default : 0)
   int Val;             // Initial Value of dial (default : 0)
   int MajorTicks;      // number of value units between big tick marks.
   int MinorTicks;      // number of value units between small tick marks.
}  SDialData;

typedef struct          // TabList create/data struct
{                       // -----------------------------------------------
   int Min;             // Min of range (default : 0)
   int Max;             // Max of range (default : 100)
   int Step;            // Step (default : 1)
   int BigTickInc;      // increments for big tickmarks (default : 0)
   int SmallTickInc;    // increments for small tickmarks (default : 0)
} STabListData;

typedef struct          // Checkbox create struct
{                       // _______________________________________________
    COLORREF rgbCheck1; // check color for 1st check state
    COLORREF rgbCheck2; // check color for 1st check state
} SCheckData;

typedef struct          // Spinner Create data structure
{                       // ------------------------------------------------
    int Min;            // Min permissable value
    int Max;            // Max permissible value
    int Value;          // Current value
    DWORD Counts;       // Together with Speed define acceleration...
    DWORD Speed;        // ...characteristics.
    int Increment;      // Amount to increment/decrement value on step
} SpinCreateData;

typedef struct          // Text Create data struct
{                       // ---------------------------------------------
    uint ResID;         // Resource ID
    uint ResTypeID;     // Resource Type ID
} STextData;


typedef struct SpinnerData_tag SpinnerData; // Spinner internal data struct

typedef struct      // Image description data structure for Super Button...
{                   // ...creation.
    WORD wType;     // SP_TBMP, SP_BMP, SP_BMPA, SP_TBMPA, SP_WMF, SP_ICO,...
                    // ...or SP_NONE.  Describes image format.

    WORD wResID;    // resource id of image (ignored for SP_NONE)

    union           // Additional image data for some image types.  Not...
    {               // ...required for SP_BMP (bitmap), SP_WMF (metafile),...
                    // or SP_ICO (icon) image types.
        struct
        {
            DWORD dwBkColor;   // background (screen) color in bitmap
        }   TBitmap;           // SP_TBMP (transparent bitmap) data.
        struct
        {
            WORD wCellWidth;   // pixel width of cells defined within bitmap
            WORD wCellHeight;  // pixel height of cells defined within bitmap
            BYTE RowOffset;    // cell row (0-n) containing picturebtn renderings
            BYTE ColOffset;    // cell row (0-n) containing picturebtn renderings
            BOOL bHasFrame;    // cells have 1 pell frame arround them for convenience.
        }   BitmapArray;       // SP_BMPA (bitmap array) data.
        struct
        {
            WORD wCellWidth;   // pixel width of cells defined within bitmap
            WORD wCellHeight;  // pixel height of cells defined within bitmap
            BYTE RowOffset;    // cell row (0-n) containing picturebtn renderings
            BYTE ColOffset;    // cell row (0-n) containing picturebtn renderings
            BOOL bHasFrame;    // cells have 1 pell frame arround them for convenience.
            DWORD dwBkColor;   // background (screen) color in bitmap
        }   TBitmapArray;      // SP_TBMPA (transparent bitmap array) data.
    }   Desc;
} SPict;

typedef struct          // Text Structure for PictList
{                       // ---------------------------
   char *pText;         // pointer to text
   COLORREF TextColor;  // color of text 
   UINT Flags;          // alignment flags: any two from DT_CENTER,
                        // DT_VCENTER, DT_LEFT, DT_RIGHT, DT_TOP, DT_BOTTOM   
} SText;

typedef struct          // Picture List CreateStruct
{                       // ------------------------------------
   int CellHeight;      // minimum height of cell (maybe increased by adding)
   int CellWidth;       // minimum width of cell
   int nRows;           // number of rows used to calculate nominal height
   int nCols;           // number of columns used to calculate nominal width
   BOOL CellSizeTotal;  // this flag determines whether the cell size 
                        // specified includes the border for the buttons
                        // or not. If TRUE, the cellsize is inclusive of
                        // the border. Otherwise, it is assumed to be the
                        // size of the picture inside the cell and the
                        // border required for the button is added.
}  SPictListCreate;


typedef struct          // Super Button create data structure
{                       // ------------------------------------------------
    WORD   wCheckMax;   // Maximum number of check states
    SPict  Images;      // Image array for button states, or NULL for none.
} SuperBtnCreateData;


typedef struct          // Color Selector data structure
{                       // ------------------------------------------------
   int Model;
   int xAxis;
   int yAxis;
   int Step;
}  CSDATA;

typedef struct          // Color Selector Color definition data structure
{                       // ------------------------------------------------
   int Type;
   union
   {
       struct
       {
           int Red;
           int Blue;
           int Green;
       }   RGB;
       struct
       {
           int Hue;
           int Lightness;
           int Saturation;
       }   HLS;
       struct
       {
           int Cyan;
           int Yellow;
           int Magenta;
       }   CMY;
       struct
       {
           int Cyan;
           int Yellow;
           int Magenta;
           int Black;
       }   CMYK;
   }   Comp;
}  CSCOLOR;

typedef struct SuperBtnData_tag SuperBtnData;  // Super Button internal struct

typedef bool (far pascal * RXCALLBACK) (handle,uint,uint,ulong);

typedef handle (far pascal * RXWNDCREATE) (const char far *,const char far *,
    ulong,int,int,int,int,handle,handle,void far *);

/***************************** Public Variables *****************************/
/***************************** Public Functions *****************************/
/**************************** Exported Functions ****************************/

#ifdef CPLUSPLUS_API
extern "C" {
#endif
					
HANDLE far pascal EXPORT RxCreateDialog (HANDLE hModule,HWND hParent,
    const char far * pResName,RECT far * pAlignArea,RXCALLBACK Callback,
    void far * pData);
HANDLE far pascal EXPORT RxCreateSubDialog (HANDLE hModule,HWND hDlg,int ID,
    const char far * pResName,RXCALLBACK Callback,void far * pData);
int far pascal EXPORT RxDestroySubDialog (HWND hDlg,int ID);
int far pascal EXPORT RxDialogBox (HANDLE hModule,HWND hParent,
    const char far * pResName,RECT far * pAlignArea,RXCALLBACK Callback,
    void far * pData);
void far pascal EXPORT RxEndDialog (HWND hDlg,int ReturnCode);
const char far * far pascal EXPORT RxIndexedClass (HANDLE hModule,int Idx);
int far pascal EXPORT RxIndexedSize (HWND hWnd,int SizeIdx);
bool far pascal EXPORT RxIsDialogMessage (HWND hWnd,MSG far *);
int far pascal EXPORT RxFontRefSizeX (HWND hWnd,int RefSize);
int far pascal EXPORT RxFontRefSizeY (HWND hWnd,int RefSize);
int far pascal EXPORT RxMenuDialog (HANDLE hModule,HWND hParent,
    const char far * pResName,uint Flags,RECT far * pAlignArea,
    RXCALLBACK Callback,void far * pData);
void far pascal EXPORT RxOnDestroyDialog (HWND hDlg);
bool far pascal EXPORT RxOnMoveDialog (HWND hDlg,int X,int Y);
bool far pascal EXPORT RxOnSizeDialog (HWND hDlg,int W,int H);
bool far pascal EXPORT RxRegisterClassIndex (HANDLE hModule,int Idx,
    const char far * pName,enum RXCLASS Type,RXWNDCREATE pCreate);
void far pascal EXPORT RxRemoveClassIndices (HANDLE hModule);
bool far pascal EXPORT RxResizeDialog (HWND hDlg,RECT far * pAlignArea);
bool far pascal EXPORT RxSetControlPos (HWND hDlg,int ID,uint Flags,
    int OtherID);
bool far pascal EXPORT RxSetControlSize (HWND hDlg,int ID,uint Flags,
    int W,int H);
ulong far pascal EXPORT RxGetHelpHint (HWND hDlg,int ID);
ulong far pascal EXPORT RxSetHelpHint (HWND hDlg,int ID,int Help,int Hint);

bool far pascal EXPORT RxCheckVersion (ulong);
ulong far pascal EXPORT RxVersionStr (char far *,short);

LPATTSCHEME far pascal EXPORT AccessAttributeScheme (uint);
bool far pascal EXPORT ChangeAttribute (uint,uint,ulong);
void far pascal EXPORT DeleteAttributeScheme (uint);
uint far pascal EXPORT RegisterAttributeScheme (LPATTSCHEME);
void far pascal EXPORT ReleaseAttributeScheme (uint);

#ifdef CPLUSPLUS_API
};
#endif

/* **************************** Inline Functions ****************************/

#ifdef CPLUSPLUS_API

inline CWnd * RxCreateDialog (
const CWnd * pParent,
const char far * pResName,
CRect & AlignArea,
RXCALLBACK Callback = NULL,
void far * pData = NULL)
{
    return CWnd::FromHandle (RxCreateDialog (AfxGetInstanceHandle (),
        pParent->GetSafeHwnd (),pResName,AlignArea,Callback,pData));
}


inline CWnd * RxCreateDialog (
const CWnd * pParent,
uint ResName,
CRect & AlignArea,
RXCALLBACK Callback = NULL,
void far * pData = NULL)
{
    return RxCreateDialog (pParent,MAKEINTRESOURCE (ResName),AlignArea,
        Callback,pData);
}


inline CWnd * RxCreateSubDialog (
const CWnd * pParent,
int ID,
const char far * pResName,
RXCALLBACK Callback = NULL,
void far * pData = NULL)
{
    return CWnd::FromHandle (RxCreateSubDialog (AfxGetInstanceHandle (),
        pParent->GetSafeHwnd (),ID,pResName,Callback,pData));
}


inline CWnd * RxCreateSubDialog (
const CWnd * pParent,
int ID,
uint ResName,
RXCALLBACK Callback = NULL,
void far * pData = NULL)
{
    return RxCreateSubDialog (pParent,ID,MAKEINTRESOURCE (ResName),Callback,
        pData);
}


inline int RxDestroySubDialog (
const CWnd * pParent,
int ID)
{
    return RxDestroySubDialog (pParent->GetSafeHwnd (),ID);
}


inline int RxDialogBox (
const CWnd * pParent,
const char far * pResName,
CRect & AlignArea,
RXCALLBACK Callback = NULL,
void far * pData = NULL)
{
    return RxDialogBox (AfxGetInstanceHandle (),pParent->GetSafeHwnd (),
        pResName,AlignArea,Callback,pData);
}


inline int RxDialogBox (
const CWnd * pParent,
uint ResName,
CRect & AlignArea,
RXCALLBACK Callback = NULL,
void far * pData = NULL)
{
    return RxDialogBox (pParent,MAKEINTRESOURCE (ResName),AlignArea,Callback,
        pData);
}


inline void RxEndDialog (
CWnd * pDlg,
int ReturnCode)
{
    RxEndDialog (pDlg->GetSafeHwnd (),ReturnCode);
}


inline const char far * RxIndexedClass (
int Idx)
{
    return RxIndexedClass (AfxGetInstanceHandle (),Idx);
}


inline int RxIndexedSize (
const CWnd * pWnd,
int SizeIdx)
{
    return RxIndexedSize (pWnd->GetSafeHwnd (),SizeIdx);
}


inline int RxIsDialogMessage (
CWnd * pWnd,
MSG * pMsg)
{
    return RxIsDialogMessage (pWnd->GetSafeHwnd (),pMsg);
}


inline int RxFontRefSizeX (
const CWnd * pWnd,
int RefSize)
{
    return RxFontRefSizeX (pWnd->GetSafeHwnd (),RefSize);
}


inline int RxFontRefSizeY (
const CWnd * pWnd,
int RefSize)
{
    return RxFontRefSizeY (pWnd->GetSafeHwnd (),RefSize);
}


inline int RxMenuDialog (
const CWnd * pParent,
const char far * pResName,
uint Flags,
CRect & AlignArea,
RXCALLBACK Callback = NULL,
void far * pData = NULL)
{
    return RxMenuDialog (AfxGetInstanceHandle (),pParent->GetSafeHwnd (),
        pResName,Flags,AlignArea,Callback,pData);
}


inline int RxMenuDialog (
const CWnd * pParent,
uint ResName,
uint Flags,
CRect & AlignArea,
RXCALLBACK Callback = NULL,
void far * pData = NULL)
{
    return RxMenuDialog (pParent,MAKEINTRESOURCE (ResName),Flags,AlignArea,
        Callback,pData);
}


inline void RxOnDestroyDialog (
CWnd * pDlg)
{
    RxOnDestroyDialog (pDlg->GetSafeHwnd ());
}


inline bool RxOnMoveDialog (
CWnd * pDlg,
int X,
int Y)
{
    return RxOnMoveDialog (pDlg->GetSafeHwnd (),X,Y);
}


inline bool RxOnSizeDialog (
CWnd * pDlg,
int W,
int H)
{
    return RxOnSizeDialog (pDlg->GetSafeHwnd (),W,H);
}


inline bool RxRegisterClassIndex (
int Idx,
const char far * pName,
RXCLASS Type = RXC_NONE,
RXWNDCREATE pCreate = NULL)
{
    return RxRegisterClassIndex (AfxGetInstanceHandle (),Idx,pName,Type,pCreate);
}


inline void RxRemoveClassIndices ()
{
    RxRemoveClassIndices (AfxGetInstanceHandle ());
}


inline bool RxResizeDialog (
CWnd * pDlg,
CRect & AlignArea)
{
    return RxResizeDialog (pDlg->GetSafeHwnd (),AlignArea);
}


inline bool RxSetControlPos (
CWnd * pDlg,
int ID,
uint Flags,
int OtherID = NULL)
{
    return RxSetControlPos (pDlg->GetSafeHwnd (),ID,Flags,OtherID);
}


inline bool RxSetControlSize (
CWnd * pDlg,
int ID,
uint Flags,
int W = 0,
int H = 0)
{
    return RxSetControlSize (pDlg->GetSafeHwnd (),ID,Flags,W,H);
}


inline ulong RxGetHelpHint (
CWnd * pDlg,
int ID)
{
    return RxGetHelpHint (pDlg->GetSafeHwnd (),ID);
}


inline ulong RxSetHelpHint (
CWnd * pDlg,
int ID,
int Help,
int Hint)
{
    return RxSetHelpHint (pDlg->GetSafeHwnd (),ID,Help,Hint);
}

#endif

#endif
