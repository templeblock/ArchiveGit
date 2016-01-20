#ifndef _FILEMANAGER_INC
#define _FILEMANAGER_INC

#include "error.h"

// This class manages a list of files 
// Currently only deletes files, but could be extended to copy, move files
class CFileManager
   {
   public:
      // Construction
      CFileManager();
      virtual ~CFileManager();

      // Operations

      // Add a filename to this object
      ERRORCODE   Add(LPCSTR lpszFileName);

      // GetFileNames returns the current list of files 
      const CStringList & GetFileNames()
         { return m_csListFileNames; }

      // RemoveAll() deletes all files that have been added to this object
      // Successfully removed files are removed from this object
      ERRORCODE   RemoveAll();

      // Passing TRUE to SetAutoRemove() will turn on automatically removing
      // files when object is destroyed.  
      // Default functionality for object without calling this method 
      // is to NOT automatically remove files
      // If you want objects to always have removal functionality, 
      // derive your class from CTempFileManager.
      void        SetAutoRemove(BOOL bOn=TRUE)
         { m_bAutoRemove = bOn; }

      BOOL        IsAutoRemoving()
         { return m_bAutoRemove; }

      // Empty() resets this object back to an empty state
      void        Empty();
   protected:
      // Data Members
      BOOL        m_bAutoRemove;
      CStringList m_csListFileNames;
   };

// This class provides the same functionality as CFileManager, but
// will default to automatically removing files when the object is destroyed.
class CTempFileManager : public CFileManager
   {
   public:
      // Construction
      CTempFileManager();
   };

#endif   // End of file
