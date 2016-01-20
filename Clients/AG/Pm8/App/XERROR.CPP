/****************************************************************************
*  FILE:        XERROR.CPP                                                  *
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
#include "stdafx.h"
#include "xerror.h"
void CExtendedError::SetError(char *FileName, int LineNum, int ErrorCode)
  {
    Clear();
    m_nCode = ErrorCode;
    strcpy(m_szFileName, FileName);
    m_nLineNum = LineNum;
  }

void CExtendedError::Clear()
  {
    m_nCode = m_nLineNum = m_nCustomCode = m_nCustomCodeType = 0;
    m_pCustomCode = NULL;
    m_szFileName[0] = 0;
  }
