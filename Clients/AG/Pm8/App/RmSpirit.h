#ifndef _RM_SPIRIT_INC
#define _RM_SPIRIT_INC

#include "rmagcom.h"

class CSpiritResourceManager : public CSharedIDResourceManager
   {
   DECLARE_DYNAMIC(CSpiritResourceManager) 
   public:
   // Construction
      CSpiritResourceManager() {}
      virtual ~CSpiritResourceManager() {}

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
inline LPCSTR CSpiritResourceManager::GetLibraryName()
   {
      return "SpiritRe.dll";
   }

#endif

