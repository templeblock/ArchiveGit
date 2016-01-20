#ifndef _RM_PRNT_INC
#define _RM_PRNT_INC

#include "rmagcom.h"

class CPrintResourceManager : public CSharedIDResourceManager
   {
   DECLARE_DYNAMIC(CPrintResourceManager) 
   public:
   // Construction
      CPrintResourceManager() {}
      virtual ~CPrintResourceManager() {}

   // Operations
      enum ResourceItems
         {
            riFirst = CSharedIDResourceManager::riLast, 
         };
      virtual UINT GetResourceID(int enItem);
      virtual LPCSTR GetLibraryName();

   protected:
   // Data Members
   };

// Inline Section
inline LPCSTR CPrintResourceManager::GetLibraryName()
   {
      return "printres.dll";
   }

#endif

