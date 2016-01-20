// Source file: saveload.c
HWND AstralImageLoad( ITEMID idFileType, LPTSTR lpFileName, int iDoZoom, BOOL bCheckDuplicates );
BOOL CreateLoadFileParms( ITEMID idFileType, LPTSTR lpFileName,
                        BOOL bCheckDuplicates, LPCMSINFO pcmsInfo,
						LPINT lpPhotoCDResOverride,
						LPLOADFILE_PARMS lpParms);
CDocument *AstralImageLoadBG( ITEMID idFileType, LPTSTR lpFileName, BOOL bCheckDuplicates,
	LPCMSINFO pcmsInfo = NULL );
BOOL LoadFile( LPIMAGE lpImage, LPLOADFILE_PARMS lpParms );
BOOL AstralImageSave( LPIMAGE lpImage, ITEMID idFileType, ITEMID idDataType, LPTSTR lpFileName );
BOOL AstralImageSaveBG( LPIMAGE lpImage, ITEMID idFileType, ITEMID idDataType, LPTSTR lpFileName, LPCMSINFO pCmsInfo = NULL );
BOOL SaveFile( LPIMAGE lpImage, LPSAVEFILE_PARMS lpParms);
BOOL SaveToAlbum( LPIMAGE lpImage, LPSAVETOALBUM_PARMS lpParms);
BOOL OKToOpen( LPTSTR lpFile, LPPSTR lppName );
void GetRecallFileName(LPTSTR lpRecallName);
void AddRecallImageName( LPTSTR lpFileName );
void NewAddRecallImageName( LPTSTR lpFileName );
BOOL LoadMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPMAP lpMap4, LPTSTR lpFileName );
BOOL LoadMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPTSTR lpFileName );
BOOL LoadCalMap( LPMAP lpMasterMap, LPMAP lpCMap, LPMAP lpMMap, LPMAP lpYMap, LPMAP lpKMap, LPTSTR lpFileName );
BOOL SaveMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPMAP lpMap4, LPTSTR lpFileName );
BOOL SaveMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPTSTR lpFileName );
ITEMID GetPPFileType(LPTSTR lpFileName);
ITEMID GetFrameFileDataType(LPFRAME lpFrame);
BOOL PromptForVolume(LPTSTR lpFileName, LPVOLUMEID lpVolume, BOOL bLoad);
void DelRecallImageName( LPTSTR lpFileName );
BOOL AlreadyOpen( LPTSTR lpFile );
BOOL SavingPP5CmdList(LPLIST lpPacketList, LPVOID lpOptions);
	
LPCPolyList LoadShape(LPTSTR lpFileName, int res);
BOOL SaveShape(LPCPolyList lpPolyList,  LPTSTR lpFileName, int res);
void point_to_strings(int x, int y, LPTSTR lpStrX, LPTSTR lpStrY, int res);
BOOL SetPPFileExt( LPTSTR lpFileName, ITEMID idFileType );

