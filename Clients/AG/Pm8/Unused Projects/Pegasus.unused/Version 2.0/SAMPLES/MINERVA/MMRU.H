/*
    mmru.h
*/



BOOL MruLoadList(HWND hwnd, LPCSTR pszIniFilename);
void MruSaveList(LPCSTR pszIniFilename);
void MruGetFilename(LPCSTR pszIniFilename, int nMruNum, LPSTR pszMruFilename, int nSize);
void MruDeleteFilename(HWND hwnd, LPCSTR pszIniFilename, LPCSTR pszFilename);
void MruSetFirstFilename(HWND hwnd, LPCSTR pszIniFilename, LPCSTR pszFilename);
