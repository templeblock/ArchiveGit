#ifndef _SRCHART_INC
#define _SRCHART_INC

class CSearchArtCollectionItem
   {
   // Construction
   public:
      CSearchArtCollectionItem();

   // Operations
   public:
   // Get operations
      const CString &   GetBaseName() const;
      int               GetSubDirCount() const;

   // Set operations
      void SetItem(LPCSTR szBaseName, int nSubDirs=0);

   protected:
      CString        m_csBaseName;
      int            m_nSubDirs;
   };

class CSearchArtCollections
   {
   // Construction
   public:
      CSearchArtCollections(LPCSTR pszIniName);
      ~CSearchArtCollections();

   // Operations
      // Reset() will reset object back to default state
      void              Reset();
      ERRORCODE         Add(LPCSTR szBaseName, int nSubDirs=0);
      int               Find(LPCSTR szBaseName);
      ERRORCODE         Update(void);

   // Get operations
      int               GetCount();
      const CString &   GetBaseName(int nItem) const;
      int               GetSubDirCount(int nItem) const;
      const CSearchArtCollectionItem * GetItem(int nItem) const;

   protected:
   // Helpers
      void              RemoveAll();   // Empties object freeing up resources
   protected:
   // Data Members
      CTypedPtrArray <CPtrArray, CSearchArtCollectionItem *>  m_cpaCollItem;

		LPCSTR m_pszIniName;
   };

// Inline Section

// CSearchArtCollectionItem Methods
inline const CString & CSearchArtCollectionItem::GetBaseName() const
   {
      return m_csBaseName;
   }

inline int CSearchArtCollectionItem::GetSubDirCount() const
   {
      return m_nSubDirs;
   }

// CSearchArtCollections Methods
inline const CString & CSearchArtCollections::GetBaseName(int nItem) const
   {
      return m_cpaCollItem[nItem]->GetBaseName();
   }

inline int CSearchArtCollections::GetSubDirCount(int nItem) const
   {
      return m_cpaCollItem[nItem]->GetSubDirCount();
   }

inline int CSearchArtCollections::GetCount()
   {
      return m_cpaCollItem.GetSize();
   }

inline const CSearchArtCollectionItem * CSearchArtCollections::GetItem(int nItem) const
   {
      return m_cpaCollItem[nItem];
   }

#endif

