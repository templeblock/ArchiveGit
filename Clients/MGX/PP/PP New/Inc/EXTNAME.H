
// Used in extname.c
typedef int (FAR *EXTADDPROC)( LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType, long lData );

// Source file: extname.c
void InitExtName( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName, ITEMID wType );
void InitExtNameN( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName, ITEMID wType, int idEntry1, BOOL fCategorize );
int GetExtName( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName, ITEMID wType, UINT wMsg );
int GetExtNameN( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName, ITEMID wType, UINT wMsg, int idEntry1, BOOL fCategorize );
void ExtNameManager( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName, ITEMID wType, int idFunction, int idNormal );
BOOL PromptForExtName( HWND hWnd, LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType );
BOOL WINPROC DlgAddExtNameProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int GetExtNameCount(ITEMID wType);
BOOL GetExtNameString(ITEMID wType, int index, LPTSTR lpName);
BOOL LookupExtFile(LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType);
BOOL LookupExtFileN( LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType, BOOL bFileCheck );
BOOL AddExtFile(LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType);
BOOL DeleteExtFile( LPTSTR lpExtName, ITEMID wType );
BOOL CreateFileName( LPTSTR lpBaseName, ITEMID wType, LPTSTR lpFileName, LPTSTR lpPathName = NULL );
BOOL FormFileName( LPTSTR lpOutPathFileName, int wType,  LPTSTR lpFileName);
BOOL LoadAllExtNamePaths( BOOL bClose );
BOOL GetExtNamePath( LPTSTR lpPath, ITEMID wType );
BOOL IsCategory(LPTSTR lpString);
BOOL GetCategory(LPTSTR lpString, LPTSTR lpCategory);
BOOL GetCategoryName(LPTSTR lpString, LPTSTR lpName);
BOOL ListExtFiles(ITEMID wType,  EXTADDPROC lpAddFunction, long lData);
void FixExtName( LPTSTR lpExtName );
void FixExtName( CString *ExtName );

// callback for extended browser
BOOL GetExtFileName_Bsr(LPTSTR lpFileName, LPTSTR lpExtName, WORD wType);

