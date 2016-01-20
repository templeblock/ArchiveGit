#include "stdafx.h"
#include "rm.h"
#include "util.h"

IMPLEMENT_DYNAMIC(CResourceManager, CObject)
   
CResourceManager::CResourceManager()
   {
      m_hinstLibrary = NULL;
      m_hinstOldResources = NULL;
   }

CResourceManager::~CResourceManager()
   {
      ReleaseResources();
   }

BOOL CResourceManager::IsLoaded()
   {
      return (m_hinstLibrary > (HINSTANCE)32);
   }

HINSTANCE CResourceManager::UseResources()
   {
      if(!IsLoaded())
         m_hinstLibrary = ::LoadLibrary(GetLibraryName());

      if(IsLoaded())
         {
            m_hinstOldResources = AfxGetResourceHandle();
            AfxSetResourceHandle(m_hinstLibrary);
         }

      return GetLibrary();
   }

void CResourceManager::ReleaseResources()
   {
      DoneWithResources();

      ::FreeLibrary(m_hinstLibrary);
      m_hinstLibrary = NULL;
   }

void CResourceManager::DoneWithResources()
   {
      if(!IsLoaded())
         return;

      // Set resource use back to original state
      AfxSetResourceHandle(m_hinstOldResources);
   }

HINSTANCE CResourceManager::GetLibrary()
   {
      return m_hinstLibrary;
   }

// CResourceLoader methods
CResourceLoader::CResourceLoader()
   {
      m_pResourceManager = NULL;
   }

CResourceLoader::CResourceLoader(CResourceManager *pResourceManager)
   {
      m_pResourceManager = NULL;
      Attach(pResourceManager);
   }

CResourceLoader::~CResourceLoader()
   {
      Detach();
   }

CResourceManager * CResourceLoader::Attach(CResourceManager *pResourceManager)
   {
      CResourceManager *pOldRM = Detach();
      m_pResourceManager = pResourceManager;
      ASSERT(pResourceManager);
      if(pResourceManager)
         pResourceManager->UseResources();
      return pOldRM;
   }

CResourceManager * CResourceLoader::Detach(void)
   {
      CResourceManager *pOldRM = m_pResourceManager;
      if(GetManager())
         {
            GetManager()->DoneWithResources();
         }
      m_pResourceManager = NULL;
      return pOldRM;
   }

BOOL CResourceLoader::LoadBitmap(CBitmap &refBitmap, UINT nIDResource, CPalette* pPalette)
   {
      if(GetManager() == NULL || !GetManager()->IsLoaded())
         return FALSE;

      return LoadBitmap(refBitmap, MAKEINTRESOURCE(GetManager()->GetResourceID(nIDResource)), pPalette);
   }

BOOL CResourceLoader::LoadBitmap(CBitmap &refBitmap, LPCSTR szResource, CPalette* pPalette)
   {
      return Util::LoadResourceBitmap(refBitmap,szResource,pPalette);
   }

HICON CResourceLoader::LoadIcon(UINT nResourceID)
   {
      HICON hIcon = NULL;

      if(GetManager() == NULL || !GetManager()->IsLoaded())
         return hIcon;

		hIcon = ::LoadIcon(
			GetManager()->GetLibrary(),
			MAKEINTRESOURCE(GetManager()->GetResourceID(nResourceID)));
      return hIcon;
   }

