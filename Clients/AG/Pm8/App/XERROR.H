/****************************************************************************
*  FILE:        XERROR.H                                                    *
*  SUMMARY:     Extended Error Handling Class.                              *
*  AUTHOR:      Dennis Plotzke                                              *
*  -------------------------------------------------------------------------*
*  Initials Legend:                                                         *
*     DGP       Dennis Plotzke                                              *
*  -------------------------------------------------------------------------*
*  Revision History                                                         *
*                                                                           *
*  Date     Initials  Description of Change                                 *
*  -------------------------------------------------------------------------*
*  12/10/94 DGP       Started History.                                      *
*                                                                           *
****************************************************************************/
#ifndef _XERROR_INC
#define _XERROR_INC
#include <stdlib.h>       // For _MAX_PATH #define
#include <string.h>       // For strcpy()
#define EE_LOCATION    __FILE__, __LINE__
class CExtendedError
  {
  // Public data 
  public:
          CExtendedError()        { Clear(); }
          ~CExtendedError()       { }
    int   m_nCode;                // Error Code
    char  m_szFileName[_MAX_PATH];// Source FileName where error occurred
    int   m_nLineNum;             // Source Line Number where error occurred
    void *m_pCustomCode;          // Custom Error Code
    int   m_nCustomCode;          // Custom Error Code
    int   m_nCustomCodeType;      // Custom Error Code Type
  
  // Public member functions
    void SetError(char *FileName, int LineNum, int ErrorCode, void *pNewCustomCode, int CustomType);
    void SetError(char *FileName, int LineNum, int ErrorCode, void *pNewCustomCode);
    void SetError(char *FileName, int LineNum, int ErrorCode, int nNewCustomCode);
    void SetError(char *FileName, int LineNum, int ErrorCode);
    void Clear(void);
  };


// Inline Section
inline void CExtendedError::SetError(char *FileName, int LineNum, int ErrorCode, void *pNewCustomCode, int CustomType)
  {
    SetError(FileName, LineNum, ErrorCode, pNewCustomCode);
    m_nCustomCodeType = CustomType;
  }
inline void CExtendedError::SetError(char *FileName, int LineNum, int ErrorCode, void *pNewCustomCode)
  {
    SetError(FileName, LineNum, ErrorCode);
    m_pCustomCode = pNewCustomCode;
  }
inline void CExtendedError::SetError(char *FileName, int LineNum, int ErrorCode, int nNewCustomCode)
  {
    SetError(FileName, LineNum, ErrorCode);
    m_nCustomCode = nNewCustomCode;
  }
#endif // End of _XERROR_INC

