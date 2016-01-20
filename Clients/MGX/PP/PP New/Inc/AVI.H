// Source file: avi.cpp

LPFRAME avirdr( LPTSTR ifile, int outdepth, LPINT lpDataType, int frameNum );
LPFRAME AviReadFrame( LPTSTR ifile, int outdepth, LPINT lpDataType, int nFrame );
int aviwrt (LPTSTR ifile, LPOBJECT lpObject, LPFRAME lpFrame, LPRECT lpRect, int flag, LPTSTR lpBaseFile);
int DoAviSave(LPIMAGE lpImage, int idDataType, LPTSTR lpFileName );
