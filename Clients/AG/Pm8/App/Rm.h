#ifndef _RM_INC_
#define _RM_INC_

class CResourceManager : public CObject
   {
   DECLARE_DYNAMIC(CResourceManager)
   public:
   // Construction
      CResourceManager();
      virtual ~CResourceManager();

   // Operations
      virtual UINT      GetResourceID(int enType) = 0;
      virtual LPCSTR    GetLibraryName() = 0;

      virtual HINSTANCE UseResources();
      // DoneWithResources() restores previous resource set that was in use
      // but doesn't release DLL
      virtual void      DoneWithResources();

      // ReleaseResources() restores previous resources and releases DLL
      virtual void      ReleaseResources();

      HINSTANCE         GetLibrary();

      BOOL              IsLoaded();

   protected:
   // Data Members
      HINSTANCE         m_hinstLibrary;
      HINSTANCE         m_hinstOldResources;
   };

// The Resource Loader provides an easy way to use resources associated with
// a Resource Manager object without having to worry about calling 
// UseResources and DoneWithResources.
//
// For Example, this class can be used to temporarily use resources from
// a long lived resource manager object. One of these objects could be 
// constructed within a method as a temporary object while attaching a 
// global resource manager object.  This object would automatically
// restore the default resource loader upon object destruction thus
// relieving the user to not have to worry about calling UseResources
// and DoneWithResources through the resource manager.
class CResourceLoader
   {
   public:
   // Construction
      CResourceLoader();
      CResourceLoader(CResourceManager *pResourceManager);
      virtual ~CResourceLoader();

   // Operations
      CResourceManager * Attach(CResourceManager *pResourceManager);
      CResourceManager * Detach(void);
      CResourceManager * GetManager();

      // The ResourceID parameter should be enumerated ID from a derived
      // CResourceManager Class, not an ID from the resource file
      // LoadBitmap makes a call to GetManager()->GetResourceID() 
      BOOL              LoadBitmap(CBitmap &refBitmap, UINT nIDResource, CPalette* pPalette);
      BOOL              LoadBitmap(CBitmap &refBitmap, LPCSTR szResource, CPalette* pPalette);
      HICON             LoadIcon(UINT nResourceID);

      protected:
      // Data Members
         CResourceManager *m_pResourceManager;
   };

// Inline Section
inline CResourceManager * CResourceLoader::GetManager()
   {
      return m_pResourceManager;
   }
#endif

