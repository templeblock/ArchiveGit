#include "stdafx.h"
#include "filemgr.h"

CFileManager::CFileManager()
   {
      m_bAutoRemove = FALSE;
   }

CFileManager::~CFileManager()
   {
      if(m_bAutoRemove)
         RemoveAll();
      Empty();
   }

ERRORCODE CFileManager::Add(LPCSTR lpszFileName)
   {
      m_csListFileNames.AddTail(lpszFileName);
      return ERRORCODE_None;
   }

// This method returns ERRORCODE_Fail if one or more files fail to be removed
ERRORCODE CFileManager::RemoveAll()
   {
      ERRORCODE   errorcode = ERRORCODE_None;
      CString     csFileName;

      // Run through object list and delete files
      for(POSITION pos = m_csListFileNames.GetHeadPosition();pos != NULL;)
         {
            POSITION posNext;

            m_csListFileNames.GetNext(posNext = pos);
            // Get data item associated with this element and release it
            csFileName = m_csListFileNames.GetAt(pos);
            TRY
               {
                  CFile::Remove(csFileName);
               }
            CATCH(CFileException, e)
               {
                  errorcode = ERRORCODE_Fail;
                  ASSERT(0);
                  #ifdef _DEBUG
                     afxDump << "File" << csFileName << "cannot be removed\n";
                  #endif
               }
            END_CATCH
				// Always remove CString object from list
				m_csListFileNames.RemoveAt(pos); // Remove from list
            pos = posNext;
         }
      return errorcode;
   }

void CFileManager::Empty()
   {
      // Empty list
      m_csListFileNames.RemoveAll();
   }

CTempFileManager::CTempFileManager()
   {
      // temp files should be automaticall deleted when object is destroyed
      SetAutoRemove(TRUE); 
   }


