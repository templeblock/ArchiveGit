#ifndef __browser
#define __browser

//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// for browser.c
// This .h file should provide all API 


// validation level flags
#define VL_NONE			0x0001
#define VL_EXIST		0x0002	// create if no thumb exits
#define VL_NOTIME		0x0004	// dont check original file timestamp (always update if one exists)
#define VL_CLEAN		0x0008	// delete if original can not be found (perminant media only)
#define VL_DATAONLY		0x0010	// update/check image data only (not image previews)
#define VL_CHECK		0x0020	// only checks if needs updating.  Does nothing.
#define VL_ESCAPABLE	0x0040	// Operation is escapable.

typedef enum
	{
	OM_NORMAL,
	OM_FASTBITS,
	OM_LOWRES
	} OPEN_MODE;

/***************************Types******************************/


//  Note VolLabel is used only if OurMark == -1.  
//	If both are -1 then matches anything.
//	If either is -2 then it has not yet been determined.
#define VOLUME_ANY	-1L
#define VOLUME_INVALID -2L
typedef struct _volumeid {
	LONG 	OurMark;	// Our Marker (-1 if there is none)
	LONG 	VolLabel;	// An id derived from the volume label 	 
	} VOLUMEID;
typedef VOLUMEID FAR *LPVOLUMEID;

// Callbacks:
//************************************************************************
// 		LPGETFILEPROC
// PARAMETERS:
//					LPTSTR lpFileName- String to return file name in.
//					LPTSTR lpExtName- The extended name to look up.
//					WORD wType- The type to llok up.
// DESCRIPTION:
//				This callback is used to get a full file name from the 
//				extended name.
// RETURNS:
//			FALSE if there is no such extended name, or if there is no file 
//			associated with it.
//************************************************************************
typedef BOOL (FAR *LPGETFILEPROC)(LPTSTR lpFileName, LPTSTR lpExtName, WORD wType);


typedef struct _previewinfo{
int RequestedWidth;			// desired (max) width of thumbnail. Filled by browser.
int RequestedHeight;		// desired (max) height of thumbnail. Filled by browser.
int idFileType;				// file type of original file
int idDataType;				// data type of original file
long fileXSize;				// size of original file
long fileYSize;				// size of original file
int fileRes;				// Resolution of original file
} PREVIEWINFO, FAR *LPPREVIEWINFO;

//************************************************************************
// 		LPGETPREVIEWPROC
// PARAMETERS:
//					LPTSTR lpFileName- The name of file to get. Sorry, no extended name.
//					WORD wType- The extended type.
//					LPPREVIEWINFO lpInfo - filled by callback with all available information.
//					LPINT bCompatible- callback fills with false if further default processing should occur.
// DESCRIPTION:
//			This callback is used to get a preview frame for the given 
//			extended name file.
// RETURNS:
//			A frame containing the preview, or NULL.
//************************************************************************
typedef LPFRAME (FAR *LPGETPREVIEWPROC)(LPTSTR lpFileName, WORD wType,  LPPREVIEWINFO lpInfo,  LPINT lpbCompatible);


//************************************************************************
// 		LPENUMALBUMPROC
// PARAMETERS:
//					LPTSTR lpAlbumName- The name of the album.
//					long lData - caller supplied data
// DESCRIPTION:
//			This callback is used when enumerating all albums.
// RETURNS:
//			TRUE to continue enumerating. FALSE to stop.
//************************************************************************
typedef BOOL (FAR *LPENUMALBUMPROC)(LPTSTR lpAlbumName, long lData);

// FBC => fill before calling

typedef struct _opendlgstate {
	TCHAR	szTitle[60];	// FBC, the dialog box title
	TCHAR	szOK[30];		// FBC, The text for the OK button
	BOOL	bSaving;		// FBC, is this a save dialog?
	FNAME 	szFileSpec;		// FBC, the initial entry of file box (if "\0" then idFileType used)
	FNAME	szAlbum;		// FBC, the current working album (can be "")
	FNAME	szPath;			// FBC, the initial path (include file)for browser (if "" then use saved)
	WORD 	wFileAttr;		// FBC, the param for DlgDirList for files (-1 for default)
	WORD 	idFileType;		// FBC, the file type (0 for default)(RETURN for SAVE only)
	WORD 	idDataType;		// FBC, the data type (RETURN for SAVE only)
	int 	frameDepth;		// FBC, the depth of the frame to save (FILL FOR SAVE ONLY) 
	FNAME far *lpFileList;	// RETURN array of file names of size numFiles (MUST be freed by caller!)
	LPVOLUMEID lpFileListVolumes; // RETURN array of volume id's for lpFileList (freed with lpFileList)
	int 	numFiles;		// RETURN number of files chosen (1 for save)
	BOOL	ViewThumbnails;	 // In Thumbnail Mode?
	BOOL 	ViewAlbums;		 // In Album Mode?
	BOOL    AutoThumbnails;	 // Auto Create Thumbnails?
	//	internal data
	BOOL 	bExtended;		// Is it an extened names dialog
	int 	idThumbDialog;	// the dlg id for the thumbnail version of the dialog
	int 	idFileDialog;	// the dlg id for the non thumbnail version of the dialog
	FNAME	szSearchSpec;	// what to use when filling file list	
	BOOL 	bNavigated;		// has user move around directories yet?
	BOOL 	bInitialized;	// set to TRUE when bsr has been initialized
	HANDLE 	hOldAccelTable;	// The Accelerator table for the main app.
	OPEN_MODE OpenMode;		// RETURN the mode to open image
	BOOL bMultiListBox;		// set by DoOpen Proc
	HWND hBrowser;			// set to the browser dialog window handle
	FNAME szNewFileSpec;	// used to fill szSearchSpec, filled when users types in list box.
	// for extended browser stuff
	WORD 	wExtType;				// the extended type
	LPGETFILEPROC lpGetFileProc;	// to get file name from ext name
	LPGETPREVIEWPROC lpGetPreview;	// to get preview frame from ext name
    BOOL bImported;
	CMSINFO	cmsInfo;
	void FAR *lpExtUserData;
	} OPENSTATE;
typedef OPENSTATE FAR *LPOPENSTATE;

	
/***************************Defines******************************/

/***************************Functions******************************/
BOOL DoOpenDlg( HWND hParent, LPOPENSTATE lpState, BOOL bUseSingleSelect = FALSE);
BOOL DoMiniOpenDlg( HWND hParent, LPOPENSTATE lpState);
void UpdateThumbnail(LPTSTR lpFile, LPFRAME lpFrame, int width, int height, LONG dataType, LONG fileType);
BOOL NonBSRAddToAlbum(LPTSTR lpFile, LPTSTR lpAlbum);
int NonBSREnumAlbum(LPENUMALBUMPROC lpCallbackProc, long lData);

BOOL NonBSRVolumeCheck(LPTSTR lpFile, LPVOLUMEID lpVolume, LPINT lpBoolRemovable); 

BOOL WINPROC EXPORT DlgOpenProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

DWORD CALLBACK EXPORT BrowserKeyHookProc(int code, UINT nVKey, LONG dwKeyData);

// bsrutil.c
WORD LocalFileType(ITEMID wAppType);
WORD AppFileType(ITEMID wLocalType);
BOOL CheckDir(LPTSTR lpPath);

#endif


