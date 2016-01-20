/*
    merror.h
*/



#define STYLE_ERROR     ( MB_OK | MB_ICONEXCLAMATION )
#define STYLE_FATAL     ( MB_OK | MB_ICONSTOP )



void ErrorMessageInit(void);
void ErrorMessageTerm(void);
int  ErrorMessage(UINT fuStyle, int idsMsg, ...);
void ErrorFileMessage(int idsMsg, UINT nErrCode, LPCSTR pszFilename);
void ErrorPegasusMessage(long lStatus, int idsMsg, ...);
