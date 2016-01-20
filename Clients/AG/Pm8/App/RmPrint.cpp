#include "stdafx.h"
#include "rmprint.h"
#include "..\Resource\AG\Print\resource.h"

IMPLEMENT_DYNAMIC(CPrintResourceManager, CSharedIDResourceManager)

UINT CPrintResourceManager::GetResourceID(int enItem)
   {
      UINT  uiResourceID;

      switch(enItem)
         {
            case CSharedIDResourceManager::riAppIcon:
               uiResourceID = IDI_APP_ICON;
               break;
            case CSharedIDResourceManager::riCharacterSmall:
               uiResourceID = IDB_CHARACTER_SMALL;
               break;
            case CSharedIDResourceManager::riCharacterMed:
               uiResourceID = IDB_CHARACTER_MED;
               break;
            case CSharedIDResourceManager::riCharacterLarge:
               uiResourceID = IDB_CHARACTER_LARGE;
               break;
            case CSharedIDResourceManager::riToolbarBackground:
               uiResourceID = IDB_TOOLBAR_BACKGROUND;
               break;
            case CSharedIDResourceManager::riToolbarEdit1:
               uiResourceID = IDB_TOOLBAR_EDIT1;
               break;
            case CSharedIDResourceManager::riToolbarEdit2:
               uiResourceID = IDB_TOOLBAR_EDIT2;
               break;
            case CSharedIDResourceManager::riToolbarFile:
               uiResourceID = IDB_TOOLBAR_FILE;
               break;
            case CSharedIDResourceManager::riToolbarFormat:
               uiResourceID = IDB_TOOLBAR_FORMAT;
               break;
            case CSharedIDResourceManager::riArtGalleryTitle:
               uiResourceID = IDB_ART_GALLERY_TITLE;
               break;
            case CSharedIDResourceManager::riArtGallerySidebar:
               uiResourceID = IDB_ART_GALLERY_SIDEBAR;
               break;
            case CSharedIDResourceManager::riArtGallerySidebarBorder:
               uiResourceID = IDB_ART_GALLERY_SIDEBAR_BORDER;
               break;
            case CSharedIDResourceManager::riProjectGalleryTitle:
               uiResourceID = IDB_PROJECT_GALLERY_TITLE;
               break;
            case CSharedIDResourceManager::riProjectGallerySidebar:
               uiResourceID = IDB_PROJECT_GALLERY_SIDEBAR;
               break;
            case CSharedIDResourceManager::riProjectGallerySidebarBorder:
               uiResourceID = IDB_PROJECT_GALLERY_SIDEBAR_BORDER;
               break;
            case CSharedIDResourceManager::riDialogCharacter:
               uiResourceID = IDB_DIALOG_CHARACTER;
               break;
            default:
               uiResourceID = 0;
               break;
         }

      return uiResourceID;
   }
