#include "stdafx.h"
#include "rmagcom.h"

IMPLEMENT_DYNAMIC(CSharedIDResourceManager, CResourceManager)

CSharedIDResourceManager::CSharedIDResourceManager()
{
}

CSharedIDResourceManager::~CSharedIDResourceManager()
{
}

UINT CSharedIDResourceManager::GetResourceID(int enItem)
   {
      return enItem;
   }
