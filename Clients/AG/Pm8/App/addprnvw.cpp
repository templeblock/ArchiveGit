//
// $Workfile: addprnvw.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/addprnvw.cpp $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 5     6/25/98 6:00p Johno
// FindFieldText no longer returns empty text strings
// 
// 4     2/06/98 1:23p Johno
// Now use CPrintInfo to get print range
// 
// 3     2/05/98 3:43p Johno
// Quick! Before the power goes out!
// 
// 2     1/30/98 11:13a Johno
// Compile update
// 
// 1     1/28/98 12:31p Johno
// 
// These is  a "special" view
//  for address book printing
//
#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "findtext.h"   // For CStoryTracker, must be after pmwdoc.h
#include "macrostd.h"   // For CStdMacroServer
#include "flatdb.h"     // For CFlatFileDatabase
#include "frameobj.h"   // For CFrameObject
#include "addprnvw.h"

//#define  JTRACE_ON
#include "jtrace.h"

/*
   ObjectListAddr: Special object list for address book printing
*/
void
ObjectListAddr::SaveDuplicate(PageObjectPtr org, PMGDatabase *pDataBase)
{
   PageObjectPtr pFrameObjectDup = org->duplicate(pDataBase);
   append(pFrameObjectDup);
   ((PMGPageObject*)pFrameObjectDup)->OnPage(FALSE);
   UnionBound (pFrameObjectDup);
}
// Consider this object's bound in the union of all object's bounds
void
ObjectListAddr::UnionBound (PageObjectPtr pop)
{
   if (pop != NULL)
   {
      PBOX     pb = ((CFrameObject*)pop)->get_unrotated_bound();
      if ((mBoxUnion.IsEmpty()) || (mBoxUnion.y0 > pb.y0))
         mBoxUnion.y0 = pb.y0;

      if (mBoxUnion.y1 < pb.y1)
         mBoxUnion.y1 = pb.y1;
   }
}
/*
   CPrintAddrView: Special view for address book printing
*/
// Try to get a pointer to the fill in list
ObjectListAddr* const
CPrintAddrView::GetListFillThrow(void)
{
   if(mpObjectsFill == NULL)
   {
      CPmwDoc  *pDoc = GetDocThrow();
      PMGDatabase *pDataBase = pDoc->get_database();
      mpObjectsFill = new ObjectListAddr(pDataBase);
   }

   return mpObjectsFill;
}
// Try to get a pointer to the non fill in list
ObjectListAddr* const
CPrintAddrView::GetListMiscThrow(void)
{
   if(mpObjectsMisc == NULL)
   {
      CPmwDoc  *pDoc = GetDocThrow();
      PMGDatabase *pDataBase = pDoc->get_database();
      mpObjectsMisc = new ObjectListAddr(pDataBase);
   }

   return mpObjectsMisc;
}
// Place duplicates from non fill in list in view's document
BOOL
CPrintAddrView::AddStaticObjects(void)
{
   TRY
   {
      CPmwDoc        *pDoc = GetDocThrow();
      ObjectListAddr *pMyObjectsMisc = GetListMiscThrow(); 
      
      PMGPageObjectPtr pObject;
      for(pObject = pMyObjectsMisc->FirstObject(); pObject != NULL; pObject = pMyObjectsMisc->NextObject(pObject))
         AddDuplicateDocObjectThrow(pObject, pDoc);
      
      return TRUE;
   }
   END_TRY
   
   return FALSE;
}
// Add a duplicate of specified object into specified document at specified position (maybe)
PMGPageObjectPtr
CPrintAddrView::AddDuplicateDocObjectThrow(PMGPageObjectPtr pObject, CPmwDoc *pDoc, PCOORD VerticalOffset)
{
   if(pDoc != NULL)
   {
      PMGPageObjectPtr pObjectdup;
      if((pObjectdup = (PMGPageObjectPtr)pObject->duplicate()) != NULL)
      {
         pDoc->append_object(pObjectdup);
         pObjectdup->OnPage(TRUE);
         if (VerticalOffset > 0)
         {
            PBOX  pb = ((CFrameObject*)pObjectdup)->get_unrotated_bound();
            pb.y0 += VerticalOffset;
            pb.y1 += VerticalOffset;
            ((CFrameObject*)pObjectdup)->set_unrotated_bound(pb);
            ((CFrameObject*)pObjectdup)->calc();
         }
         return pObjectdup;
      }
   }

   ThrowErrorcodeException(ERRORCODE_Fail);

   return NULL;
}
// The framework is telling us to print - I hope I got all my stuff...
SHORT 
CPrintAddrView::OnPrintDocument(PrintContext &pc, int32 aCurrentDocument, int32 aNumDocuments)
{
JTRACE ("\nCPrintAddrView::OnPrintDocument\n"); 
   SHORT ret = 0;

   AddStaticObjects();
   if(mpAddrDatabase != NULL)
   {
      TRY
      {
         PageObjectPtr     pObject;
         ERRORCODE         ec;
         
         CPmwDoc  *pDoc = GetDocThrow();
         PPNT DocDim = pDoc->get_dimensions();
   
         CStdMacroServer*  pServer = pDoc->GetMacroServer();

         CFlatFileDatabaseCursor Cursor;
         Cursor.Attach(mpAddrDatabase);
         Cursor.SetIndex(mIndexType);

         CString  FName;
         DWORD    FieldCount = Cursor.GetFieldCount();
         BOOL     DocHasObjects = FALSE;
         PCOORD   VerticalOffset = 0;

         ObjectListAddr *pMyObjectsFill = GetListFillThrow();

         int32 MyCurrentDocument = 1;
         //DWORD RecCount;
         //mpAddrDatabase->GetRecordCount(mIndexType, &RecCount);
         UINT  nFrom = pc.m_pInfo->GetFromPage();
         UINT  nTo = pc.m_pInfo->GetToPage();
         DWORD RecCount = (nTo - nFrom) + 1;
JTRACE1 ("Printing %d records\n", RecCount); 
         int   PrintSize = DocDim.y - pMyObjectsFill->RowTop();
JTRACE2 (" on %d of %d\n", PrintSize, DocDim.y);
         int32 MyNumDocuments = ((RecCount * pMyObjectsFill->RowHeight()) / PrintSize) + 1;
JTRACE1 ("Printing %d document(s)\n", MyNumDocuments); 
         
         UINT CurRec;
         for (Cursor.First(), CurRec = 1; !Cursor.AtEOF(); Cursor.Next(), CurRec ++)
         {
            if ((CurRec >= nFrom) && (CurRec <= nTo))
            {
               for(pObject = pMyObjectsFill->FirstObject(); pObject != NULL; pObject = pMyObjectsFill->NextObject(pObject))
               {
                  TRY
                  {
                     CFrameObject *fop =(CFrameObject*)AddDuplicateDocObjectThrow((PMGPageObjectPtr)pObject, pDoc, VerticalOffset);
                     DocHasObjects = TRUE;

                     CTextRecord *pText = fop->LockTextRecord(&ec);
                     if(pText != NULL)
                     {
                        CHARACTER   c;   
         
                        for(int i = 0; i < pText->NumberOfCharacters(); i ++)
                        {
                           c = *pText->GetCharacter(i); 
                           if(c >= MACRO_CHARACTER)
                           {
                              PMGDatabase *pDataBase = pDoc->get_database();
                              CTxp  tx(pDataBase);
                              tx.Init(fop, i);
                              tx.DeleteText(1);

                              LPCSTR FieldText;
                              CString MacroText = pServer->GetMacroFieldText(c);

                              if((FieldText = FindFieldText(&Cursor, MacroText)) != NULL)
                              {
                                 tx.CalcStyle();
                                 tx.InsertString(FieldText);
                              }
                              i = -1;
                           }
                        }
                        pText->release();
                     }
                  }
                  END_TRY
               }
               VerticalOffset += pMyObjectsFill->RowHeight();
               if (pMyObjectsFill->RowTop() + pMyObjectsFill->RowHeight() + VerticalOffset > DocDim.y)
               {
                  DocHasObjects = FALSE;
                  if ((ret = INHERITED::OnPrintDocument(pc, MyCurrentDocument, MyNumDocuments)) != 0)
                     break;
                  RemoveAllDocObjects();
                  AddStaticObjects();
                  VerticalOffset = 0;
                  MyCurrentDocument ++;
               }
            }
         }
         
         if (DocHasObjects == TRUE)
            ret = INHERITED::OnPrintDocument(pc, MyCurrentDocument, MyNumDocuments);
      }
      END_TRY
   }
   
JTRACE ("\n"); 
   return ret;
}
// Remove objects from specified document 
//  and save in this view's fill in, or non fill in, list
BOOL
CPrintAddrView::CollectnRemoveObjects(CPmwDoc* pDocTmp)
{
   BOOL  ret = FALSE;
   // Get rid of any existing lists
   KillLists();

   TRY
   {
      CPmwDoc        *pDoc = GetDocThrow();
      ObjectListAddr *pMyObjectsFill = GetListFillThrow(); 
      ObjectListAddr *pMyObjectsMisc = GetListMiscThrow(); 

      PMGDatabase *pDataBaseOut = pDoc->get_database();

      PMGDatabase    *pDataBaseTmp = pDocTmp->get_database();
      CStoryTracker  Tracker(pDocTmp);
JTRACE("\nCPrintAddrView::CollectnRemoveObjects searching for stuff\n");
      // Collect the page's fillins
      do
      {
         CHARACTER   c;
         BOOL        FillIn = FALSE;
         // Look through "stories" for macro character
         while((c = Tracker.GetNextCharacter()) != TrackerEOF)
         {
            if(c >= MACRO_CHARACTER)
            {
               FillIn = TRUE; // Found one
               break;         // That's all we need to know
            }
         }
         // Does this story have a macro?
         if(FillIn == TRUE)
         {
            DB_RECORD_NUMBER  Frame = Tracker.GetFrame();
            CFrameObject      *pFrameObject;

            if((pFrameObject = pDataBaseTmp->FindFrameObject(Frame)) != NULL)
            {
               if (!(pFrameObject->get_flags() & OBJECT_FLAG_grouped))  // Can't handle groups at this time...
               {
                  pMyObjectsFill->SaveDuplicate(pFrameObject, pDataBaseOut);
                  pDocTmp->detach_object(pFrameObject);
                  delete pFrameObject;
               }
               else 
               {
                  // We don't currently support grouped objects
                  JTRACE ("CPrintAddrView::CollectnRemoveObjects skipping group\n");
                  // It is OK to ignore the ASSERT
                  ASSERT(FALSE);
               }
            }
         }
      } while(Tracker.GetStoryNext() == TRUE);
      // Get all the rest of the objects into the other list
      PMGPageObjectPtr pObject;
      while((pObject = pDocTmp->objects()) != NULL)
      {
         pMyObjectsMisc->SaveDuplicate(pObject, pDataBaseOut);
         pDocTmp->detach_object(pObject);
         delete pObject;
      }
      ret = TRUE;
JTRACE("\n");
   }
   CATCH_ALL(e)
   {
      KillLists();
   }
   END_CATCH_ALL

   return ret;
}
// Empty view's document of all objects
void
CPrintAddrView::RemoveAllDocObjects()
{
   TRY
   {
      CPmwDoc  *pDoc = GetDocThrow();
      PMGPageObjectPtr pObject;
      while((pObject = pDoc->objects()) != NULL)
      {
         pDoc->detach_object(pObject);
         delete pObject;
      }
   }
   CATCH_ALL(e)
   {
   }
   END_CATCH_ALL
}
// Return field text for specified field name
LPCSTR
CPrintAddrView::FindFieldText(CFlatFileDatabaseCursor *pCursor, LPCSTR pMacroText)
{
   static CString RetText;
   DWORD          FieldCount = pCursor->GetFieldCount();
   
   for(DWORD d = 0; d < FieldCount; d ++)
   {
      CFlatFileDatabaseField* pField = pCursor->GetField(d);
      if(pField != NULL)
      {
         if(pField->GetType() == FFT_String)
         {
            CFlatFileFieldDescription* pDescription = pField->GetDescription();
            CString FName = pDescription->GetName();
            if(FName.Compare(pMacroText) == 0)
            {
               RetText = pCursor->GetField(FName)->GetAsString();
               if (RetText.IsEmpty() == 0)
						return RetText;
					break;
            }
         }
      }
   }

   return NULL;
}

