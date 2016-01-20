#ifndef _RM_CRAFT_INC
#define _RM_CRAFT_INC

#include "rmagcom.h"

class CCraftResourceManager : public CSharedIDResourceManager
   {
   DECLARE_DYNAMIC(CCraftResourceManager) 
   public:
   // Construction
      CCraftResourceManager() {}
      virtual ~CCraftResourceManager() {}

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
inline LPCSTR CCraftResourceManager::GetLibraryName()
   {
      return "craftres.dll";
   }

#endif

