#define  USHT     unsigned short
typedef struct tagSPOINT 
{ // pt  
    USHT x; 
    USHT y; 
} SPOINT; 

#pragma pack(1)
struct ButtonSet
{
   USHT  idButton;               // Control ID sent to parent
   USHT  idBitmapUp;             // Bitmap ID for up state
   USHT  idBitmapDn;             // Bitmap ID for down state
   SPOINT Origin;                // Upper left corner, client coords
};

struct ScreenSet
{
   USHT              idScreen;   // Bitmap ID for screen background
   short             Width;      // Bitmap width
   short             Height;     // Bitmap Height
   short             ClipLimit;
   struct ButtonSet  *pButton;
};

struct ScreenInfo
{
   USHT              nNumScreen; 
   USHT              nNumButton;
   struct ScreenSet  *pScreen;
};
#pragma pack()

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
class CHubResourceManager
{
public:
   CHubResourceManager::CHubResourceManager(LPCSTR DLLName = NULL)
   {
      //m_ScreenInfo = NULL;
      
		m_InstDLL = NULL;
      SetDLLName(DLLName);
   }
   
   void
	SetDLLName(LPCSTR DLLName)
	{
		KillHubDLL();			// We're changing the name (maybe); get rid of any current DLL
		mDLLName = DLLName; 
	}

	virtual 
   CHubResourceManager::~CHubResourceManager()
   {
      KillResource();
   }
   
   ScreenInfo*
   GetTable(UINT ID);
   
   HINSTANCE
   GetDLLHandle()
   {
      if (m_InstDLL == NULL)
			m_InstDLL = LoadLibrary(mDLLName);
#ifdef  WIN32
      ASSERT(m_InstDLL != NULL);
#else
      ASSERT(m_InstDLL >= HINSTANCE_ERROR);
#endif
		return m_InstDLL;
   }

   int
   GetSmallestWindowSizeAndIndex(CPoint *p)const;

   void
   GetMinSize(CPoint *p)const
   {
      int i = GetSmallestWindowSizeAndIndex(p);
      
      p->x -= m_ScreenInfo->pScreen [i].ClipLimit;
      p->y -= m_ScreenInfo->pScreen [i].ClipLimit;
   }

   UINT
   GetScreenIndexByRect(CRect &Rect)const;

   BOOL
   IsSkippedButton(int id)const;
   
   BOOL
   SetSkipButton(int id);

	void
	KillHubDLL(void);

protected:
   HINSTANCE   m_InstDLL;
   CUIntArray	SkipButtonIDs;
	CString		mDLLName;
   static ScreenInfo  *m_ScreenInfo;
   
   
   void
   KillResource(void);
   
private:

   void
   SetTablePointers(ScreenInfo * const si);
};
