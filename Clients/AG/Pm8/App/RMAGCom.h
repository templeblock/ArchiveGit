#ifndef _RMCOM_INC_
#define _RMCOM_INC_

#include "rm.h"

class CSharedIDResourceManager : public CResourceManager
   {
   DECLARE_DYNAMIC(CSharedIDResourceManager)
   public:
      CSharedIDResourceManager();
      ~CSharedIDResourceManager();

      enum ResourceItems
         {
            riAppIcon,
            riCharacterSmall, 
            riCharacterMed, 
            riCharacterLarge, 
            riToolbarBackground, 
            riToolbarEdit1, 
            riToolbarEdit2, 
            riToolbarFormat, 
            riToolbarFile,
            riArtGalleryTitle, 
            riArtGallerySidebar, 
            riArtGallerySidebarBorder, 
            riProjectGalleryTitle, 
            riProjectGallerySidebar, 
            riProjectGallerySidebarBorder, 
            riSentimentGalleryTitle, 
            riSentimentGallerySidebar, 
            riSentimentGallerySidebarBorder,
            riDialogCharacter, 
            riLast
         };
      virtual UINT      GetResourceID(int enItem);
      virtual LPCSTR    GetLibraryName()
         {
            return "";
         }
   };
#endif

