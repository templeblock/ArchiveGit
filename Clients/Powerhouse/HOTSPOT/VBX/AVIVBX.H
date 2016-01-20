// avivbx.h --

#ifndef VB_VERSION
 #include <vbapi.h>
#endif 

extern PCTLINFO pCtlInfoList;

// this could also have been named avihotCtl...
#define pCtlInfoDEREF(hctl)     ((PCTLINFO)VBDerefControl(hctl))

//---------------------------------------------------------------------------
// Resource Information
//---------------------------------------------------------------------------
// Toolbox bitmap resource IDs numbers.
//---------------------------------------------------------------------------
#define IDBMP_AVI            8000
#define IDBMP_AVIDOWN        8001
#define IDBMP_AVIMONO        8003
#define IDBMP_AVIEGA         8006


//---------------------------------------------------------------------------
// Change these for each new VBX file
//---------------------------------------------------------------------------
#define VBX_COMPANYNAME        "Microsoft Corporation\0"
#define VBX_FILEDESCRIPTION    "Visual Basic AVI Hotspot Custom Control\0"
#define VBX_INTERNALNAME       "AVIHOT\0"
#define VBX_LEGALCOPYRIGHT     "Copyright \251 Microsoft Corp. 1993\0"
#define VBX_LEGALTRADEMARKS    "Microsoft\256 is a registered trademark of Microsoft Corporation. Visual Basic\231 is a trademark of Microsoft Corporation. Windows\231 is a trademark of Microsoft Corporation.\0"
#define VBX_ORIGINALFILENAME       "AVIHOT.VBX\0"
#define VBX_PRODUCTNAME        "Microsoft\256 Visual Basic\231 for Windows\231\0"

//---------------------------------------------------------------------------
// Update these fields for each build.
//---------------------------------------------------------------------------
#define VBX_VERSION         3,00,0,00
#define VBX_VERSION_STR        "3.00.000\0"


#ifndef RC_INVOKED  

//---------------------------------------------------------------------------
// Macro for referencing member of structure
//---------------------------------------------------------------------------
#define OFFSETIN(struc, field)      ((USHORT)&(((struc *)0)->field))

//---------------------------------------------------------------------------
// Control Procedure
//---------------------------------------------------------------------------
LONG FAR PASCAL _export AVICtlProc(HCTL, HWND, USHORT, USHORT, LONG);


//---------------------------------------------------------------------------
// Property list
//---------------------------------------------------------------------------
// Define the consecutive indicies for the properties
//---------------------------------------------------------------------------
#define IPROP_AVI_CTLNAME              0
#define IPROP_AVI_INDEX                1
#define IPROP_AVI_BACKCOLOR            2
#define IPROP_AVI_LEFT                 3
#define IPROP_AVI_TOP                  4
#define IPROP_AVI_WIDTH                5
#define IPROP_AVI_HEIGHT               6
#define IPROP_AVI_VISIBLE              7
#define IPROP_AVI_PARENT               8
#define IPROP_AVI_DRAGMODE             9
#define IPROP_AVI_DRAGICON            10
#define IPROP_AVI_TAG                 11
#define IPROP_AVI_HWND                12

#define IPROP_AVI_LASTSTD             13

#define IPROP_AVI_AVIFILE             IPROP_AVI_LASTSTD + 0
#define IPROP_AVI_HOTFILE             IPROP_AVI_LASTSTD + 1
#define IPROP_AVI_PLAY                IPROP_AVI_LASTSTD + 2
#define IPROP_AVI_FRAME               IPROP_AVI_LASTSTD + 3
#define IPROP_AVI_LENGTH              IPROP_AVI_LASTSTD + 4
#define IPROP_AVI_SHOWHOT             IPROP_AVI_LASTSTD + 5

PROPINFO Property_Play =
{
    "Play",
    DT_SHORT | PF_fNoShow | PF_fSetMsg | PF_fSetData | PF_fGetData,
    OFFSETIN(CTLINFO, fPlay),
    0, 0, NULL, 0    
};

PROPINFO Property_Length =
{
    "Length",
    DT_LONG | PF_fSetData | PF_fSetMsg | PF_fGetData,
    OFFSETIN(CTLINFO, lLength),
    0, 0, NULL, 0    
};


PROPINFO Property_Frame =
{
    "Frame",
    DT_LONG | PF_fSetMsg | PF_fSetData | PF_fGetData,
    OFFSETIN(CTLINFO, lFrame),
    0, 0, NULL, 0    
};

PROPINFO Property_ShowHot =
{
    "ShowHot",
    DT_ENUM | PF_fSetMsg | PF_fSetData | PF_fGetData,
    OFFSETIN(CTLINFO, nShowHot),
    0, 0, "0 - Invisible\0" "1 - Visible", 1    
};

PROPINFO Property_AVIFile = 
{
    "AVIFile",
    DT_HSZ | PF_fGetData | PF_fSetData | PF_fSetMsg | PF_fSaveData,
    OFFSETIN(CTLINFO, hszAVIFile),
    0, 0, NULL, 0
};

PROPINFO Property_HOTFile = 
{
    "HOTFile",
    DT_HSZ | PF_fGetData | PF_fSetData | PF_fSetMsg | PF_fSaveData,
    OFFSETIN(CTLINFO, hszHotFile),
    0, 0, NULL, 0
};

PPROPINFO AVI_Properties[] =
    {
    PPROPINFO_STD_CTLNAME,
    PPROPINFO_STD_INDEX,
    PPROPINFO_STD_BACKCOLOR,
    PPROPINFO_STD_LEFT,
    PPROPINFO_STD_TOP,
    PPROPINFO_STD_WIDTH,
    PPROPINFO_STD_HEIGHT,
    PPROPINFO_STD_VISIBLE,
    PPROPINFO_STD_PARENT,
    PPROPINFO_STD_DRAGMODE,
    PPROPINFO_STD_DRAGICON,
    PPROPINFO_STD_TAG,
    PPROPINFO_STD_HWND,
    &Property_AVIFile,
    &Property_HOTFile,
    &Property_Play,
    &Property_Frame,
    &Property_Length,
    &Property_ShowHot,
    NULL
    };

//---------------------------------------------------------------------------
// Event list
//---------------------------------------------------------------------------
// Define the consecutive indicies for the events
//---------------------------------------------------------------------------
#define IEVENT_AVI_HSPCLICK            0
#define PLACE_HOLDER                   1
#define IEVENT_AVI_DRAGDROP            2
#define IEVENT_AVI_DRAGOVER            3
#define IEVENT_AVI_SIZEADJUST          4
#define IEVENT_AVI_AVIDONE			   5

typedef struct tagHSPCLICKPARAMS
{
    HLSTR hlszHotspotID;          // appear in reverse order
    LPVOID Index;
} HSPCLICKPARAMS;

typedef struct tagSIZEADJUSTPARAMS
{
    int far *CX;           // must be a far ptr to data
    int far *CY;
    LPVOID Index;
} SIZEADJUSTPARAMS;

typedef struct tagAVIDONEPARAMS
{
	LPVOID Index;
} AVIDONEPARAMS;

WORD Paramtypes_HspClick[] = {ET_HLSTR};
WORD Paramtypes_SizeAdjust[] = {ET_I2, ET_I2};

EVENTINFO Event_SizeAdjust =
    {
    "SizeAdjust",
    2,
    4,      // always twice the number in the second field
    Paramtypes_SizeAdjust,
    "CX As Single, CY As Single"        // width & height
    };

EVENTINFO Event_HspClick =
    {
    "HotspotClick",
    1,
    2,      // always twice the number in the second field
    Paramtypes_HspClick,
    "HotspotID As String"
    };

EVENTINFO Event_AviDone =
    {
    "AVIDone",
    0,
    0,      // always twice the number in the second field
    Paramtypes_HspClick,
    ""
    };

PEVENTINFO AVI_Events[] =
    {
    &Event_HspClick,
    PEVENTINFO_STD_NONE,    
    PEVENTINFO_STD_DRAGDROP,
    PEVENTINFO_STD_DRAGOVER,
    &Event_SizeAdjust,
    &Event_AviDone,
    NULL
    };

//---------------------------------------------------------------------------
// Model struct
//---------------------------------------------------------------------------
// Define the control model (using the event and property structures).
//---------------------------------------------------------------------------
MODEL modelAvihot =
    {
    VB_VERSION,             // VB version being used
    MODEL_fInitMsg | MODEL_fLoadMsg,                  // MODEL flags
    (PCTLPROC)AVICtlProc,   // Control procedure
    CS_VREDRAW | CS_HREDRAW,// Class style
    WS_BORDER,              // Default Windows style
    sizeof(CTLINFO),        // for passing info from VB to VBX
    IDBMP_AVI,              // Palette bitmap ID
    "AVIHOT",               // Default control name
    "AVIHOT",               // Visual Basic class name
    NULL,                   // Parent class name
    AVI_Properties,         // Property information table
    AVI_Events,             // Event information table
    IPROP_AVI_BACKCOLOR,    // Default property
    IEVENT_AVI_HSPCLICK,    // Default event
    -1                      // Property representing value of ctl
    };

#endif  // RC_INVOKED

//---------------------------------------------------------------------------
