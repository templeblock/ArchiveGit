class ObjectListAddr : public ObjectList
{
   INHERIT(ObjectListAddr, ObjectList)
public:
   ObjectListAddr(ObjectDatabasePtr owner)
   : ObjectList(owner)   
   {
      mBoxUnion.Empty();
   }

   PCOORD
   RowHeight (void)
   {
      return mBoxUnion.Height();
   }

   PCOORD
   RowTop (void)
   {
      return mBoxUnion.y0;
   }

   void
   SaveDuplicate(PageObjectPtr org, PMGDatabase *pDataBase);

   PMGPageObjectPtr
   FirstObject()
   {
      PMGPageObjectPtr  pop = (PMGPageObjectPtr)first_object();
      return pop;
   }

   PMGPageObjectPtr
   NextObject(PageObjectPtr pop)
   {
      PageObjectPtr  npop = pop->next_object();
      return (PMGPageObjectPtr)npop;
   }

protected:
   PBOX     mBoxUnion;

   void
   UnionBound (PageObjectPtr pop);
};

class CPrintAddrView : public CPmwView
{
   INHERIT(CPrintAddrView, CPmwView)
public:
   CPrintAddrView(void)
   {
      mpObjectsFill = mpObjectsMisc = NULL;
      mpAddrDatabase = NULL;
      mIndexType = 0;
   }

   ~CPrintAddrView(void)
   {
      KillLists();
   }

   void
   set_document(CPmwDoc *doc)
   { m_pDocument = doc; }

   void
   RemoveAllDocObjects(void);

   BOOL
   CollectnRemoveObjects(CPmwDoc* pDocTmp);
/*
   int 
   PrintProject(CPrintInfo* pInfo, CFlatFileDatabaseCursor* DBC, DWORD IndexType = 0)
   {
      mpAddrDatabase = DBC->GetDatabase();
      mIndexType = DBC->GetIndex();
      
      return INHERITED::PrintProject(pInfo);
   }
*/   
   int 
   PrintProject(CPmwDoc* pDocTmp, CPrintInfo* pInfo, CFlatFileDatabaseCursor* DBC)
   {
      CollectnRemoveObjects (pDocTmp);
      mpAddrDatabase = DBC->GetDatabase();
      mIndexType = DBC->GetIndex();
      
      return INHERITED::PrintProject(pInfo, 0, FALSE);
   }

   virtual SHORT 
   OnPrintDocument(PrintContext &pc, int32 aCurrentDocument, int32 aNumDocuments);

protected:
   DWORD             mIndexType;
   ObjectListAddr    *mpObjectsFill, *mpObjectsMisc;
   CFlatFileDatabase *mpAddrDatabase;

   ObjectListAddr* const
   GetListFillThrow(void);
   
   ObjectListAddr* const
   GetListMiscThrow(void);

   CPmwDoc*
   GetDocThrow(void)
   {
      CPmwDoc  *pDoc = INHERITED::GetDocument();
      if(pDoc == NULL)
         ThrowErrorcodeException(ERRORCODE_DoesNotExist);
      return pDoc;
   }

   void
   KillLists(void)
   {
      KillListFill();
      KillListMisc();
   }

   void
   KillListFill(void)
   {
      if(mpObjectsFill != NULL)
      {
         mpObjectsFill->destroy_objects();
         delete mpObjectsFill;
         mpObjectsFill = NULL;
      }
   }
    
   void
   KillListMisc(void) 
   {
      if(mpObjectsMisc != NULL) 
      {
         mpObjectsMisc->destroy_objects();
         delete mpObjectsMisc;
         mpObjectsMisc = NULL;
      }
   }
   
   BOOL
   AddStaticObjects(void);

   PMGPageObjectPtr 
   AddDuplicateDocObjectThrow(PMGPageObjectPtr pObject, CPmwDoc *pDoc, PCOORD VerticalOffset = 0);

   LPCSTR
   FindFieldText(CFlatFileDatabaseCursor *pCursor, LPCSTR pMacroText);

   PageObjectPtr
   GetMatchingFillIn(LPCSTR Text);
};
// This class uses Jay's evil method for getting at
// PmwDoc' private parts
class CAddrPrintDoc : public CPmwDoc
{
public:
   void
   add_hidden_view(CPrintAddrView *pView)
   {
      m_viewList.AddTail(pView);
      pView->set_document(this);
   }
};

