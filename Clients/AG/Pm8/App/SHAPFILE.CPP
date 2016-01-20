/*
// $Workfile: SHAPFILE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SHAPFILE.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     1/21/98 9:50a Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:25:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:24   Fred
// Initial revision.
// 
//    Rev 1.3   14 May 1997 10:16:48   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.2   28 Jan 1997 14:10:34   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.1   08 Nov 1996 11:40:44   johno
// Moved strings to resource DLL
// 
//    Rev 1.0   14 Mar 1996 13:34:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:54   FRED
// Initial revision.
// 
//    Rev 1.6   30 Aug 1995 14:13:54   JAY
// New FindShape method.
// 
//    Rev 1.5   07 Jul 1995 16:13:18   JAY
// Include rearranging; document saving with album.
// 
//    Rev 1.4   06 Jul 1995 17:26:42   JAY
// New binary caching technology
// 
//    Rev 1.3   18 May 1995 13:20:54   FRED
// Shape file/dialog stuff
// 
//    Rev 1.2   17 May 1995 16:57:06   FRED
// Draw object shadows
// 
//    Rev 1.1   17 May 1995 15:42:24   JAY
//  
// 
//    Rev 1.0   16 May 1995 12:00:22   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "timestmp.h"
#include "shapfile.h"

#include "file.h"       // For StorageFile, ReadOnlyFile, etc.

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShapeFile

CShapeFile::CShapeFile(void)
{
}

CShapeFile::~CShapeFile(void)
{
   FreeShapes();
}

BOOL CShapeFile::Init(LPCSTR lpszFileName, LPCSTR lpszBinaryFileName)
{
   BOOL fResult = FALSE;

   FreeShapes();

/*
// See if our bin file is up to date.
*/

   CTimeStamp BinaryTime;
   if (BinaryTime.SetFileModify(lpszBinaryFileName))
   {
		// The binary file exists and we have its time.
		// Get the modify time of the source to see if
		// the binary file is up-to-date. If the source
		// file exists and the modify time is less than
		// the binary file, the binary is up-to-date,
		// and we use it.
      CTimeStamp SourceTime;
      if ((!SourceTime.SetFileModify(lpszFileName))
       || (BinaryTime >= SourceTime))
      {
         if (ReadBinaryFile(lpszBinaryFileName) == ERRORCODE_None)
         {
            return TRUE;
         }
      }
   }

/*
// For one of a myriad reasons, we are unable to use the binary version
// of the shapes. Read the source to get our shapes (and generate the binary
// version while we're at it).
*/

/*
// We allow some extra ones here.
// This is a little ugly.
*/
#if 0
   CShapeEntry* pEntry;

   pEntry = new CShapeEntry;
   // JRO pEntry->m_csName = "Rectangle";
   pEntry->m_csName = GET_PMWAPP()->GetResourceStringPointer (IDS_RECTANGLE);
   pEntry->m_nType = CShapeEntry::RectangleObject;
   pEntry->m_Bounds.Left = MakeFixed(0);
   pEntry->m_Bounds.Top = MakeFixed(0);
   pEntry->m_Bounds.Right = MakeFixed(1);
   pEntry->m_Bounds.Bottom = MakeFixed(1);
   m_Shapes.Add(pEntry);

   pEntry = new CShapeEntry;
   // JRO pEntry->m_csName = "Circle";
   pEntry->m_csName = GET_PMWAPP()->GetResourceStringPointer (IDS_CIRCLE);
   pEntry->m_nType = CShapeEntry::EllipseObject;
   pEntry->m_Bounds.Left = MakeFixed(0);
   pEntry->m_Bounds.Top = MakeFixed(0);
   pEntry->m_Bounds.Right = MakeFixed(1);
   pEntry->m_Bounds.Bottom = MakeFixed(1);
   m_Shapes.Add(pEntry);

   pEntry = new CShapeEntry;
   // JRO pEntry->m_csName = "Line";
   pEntry->m_csName = GET_PMWAPP()->GetResourceStringPointer (IDS_LINE);
   pEntry->m_nType = CShapeEntry::LineObject;
   pEntry->m_Bounds.Left = MakeFixed(0);
   pEntry->m_Bounds.Top = MakeFixed(0);
   pEntry->m_Bounds.Right = MakeFixed(1);
   pEntry->m_Bounds.Bottom = MakeFixed(1);
   m_Shapes.Add(pEntry);
#endif

   ReadOnlyFile File(lpszFileName);
   File.set_read_buffer(2048);

   if (File.seek(0, ST_DEV_SEEK_SET) == ERRORCODE_None)
   {
      TRY
      {
         CShapeFileParser Parser(&File, &m_Shapes);
         Parser.Parse(CShapeFileParser::ParseStates, CShapeFileParser::TokenLookup);
         fResult = TRUE;
      }
      END_TRY

   /*
   // If we successfully parsed, try to write the binary file.
   // We don't pay attention to errors since it doesn't really matter.
   */

      if (fResult)
      {
         WriteBinaryFile(lpszBinaryFileName);
      }
   }
   
   return fResult;
}

CShapeEntry* CShapeFile::FindShape(LPCSTR pName)
{
   int nShapes = ShapeCount();
   for (int nShape = 0; nShape < nShapes; nShape++)
   {
      CShapeEntry* pShape = Shape(nShape);
      if (pShape != NULL && pShape->m_csName == pName)
      {
         return pShape;
      }
   }
   return NULL;
}

void CShapeFile::FreeShapes(void)
{
   for (int i = 0; i < m_Shapes.GetSize(); i++)
   {
      delete (CShapeEntry*)m_Shapes[i];
      m_Shapes[i] = NULL;
   }

   m_Shapes.RemoveAll();
}

ERRORCODE CShapeFile::ReadBinaryFile(LPCSTR lpszFileName)
{
   ReadOnlyFile File(lpszFileName);

   File.set_read_buffer(8192);

/*
// Read the number of entries.
*/

   ERRORCODE error;
   CShapeFileHeader FileHeader;

   if ((error = File.read_record(&FileHeader, sizeof(FileHeader))) == ERRORCODE_None)
   {
      if (FileHeader.nVersion != Version)
      {
         return ERRORCODE_VersionMismatch;
      }

      for (int nShape = 0; nShape < FileHeader.nShapes; nShape++)
      {
         CShapeEntryHeader EntryHeader;

      /* Read the entry header. */

         if ((error = File.read_record(&EntryHeader, sizeof(EntryHeader))) != ERRORCODE_None)
         {
            break;
         }

         CShapeEntry* pEntry = new CShapeEntry;

      /* Plug the info in. */

         pEntry->m_nType = EntryHeader.m_nType;
         pEntry->m_Bounds = EntryHeader.m_Bounds;
         if ((error = File.read_cstring(pEntry->m_csName)) != ERRORCODE_None
             || (error = pEntry->m_Path.ReadData(&File)) != ERRORCODE_None)
         {
            delete pEntry;
            break;
         }

         m_Shapes.Add(pEntry);
      }

      if (error != ERRORCODE_None)
      {
         FreeShapes();
      }
   }

   return error;
}

ERRORCODE CShapeFile::WriteBinaryFile(LPCSTR lpszFileName)
{
   StorageFile File(lpszFileName);
   ERRORCODE error;

   if ((error = File.initialize()) == ERRORCODE_None)
   {
   /*
   // Read the number of entries.
   */

      CShapeFileHeader FileHeader;

      FileHeader.nVersion = Version;
      FileHeader.nShapes = m_Shapes.GetSize();

      if ((error = File.write_record(&FileHeader, sizeof(FileHeader))) == ERRORCODE_None)
      {
         for (int nShape = 0; nShape < FileHeader.nShapes; nShape++)
         {
            CShapeEntry* pEntry = (CShapeEntry*)m_Shapes.GetAt(nShape);

            CShapeEntryHeader EntryHeader;

            EntryHeader.m_nType = pEntry->m_nType;
            EntryHeader.m_Bounds = pEntry->m_Bounds;

         /* Write the entry header. */

            if ((error = File.write_record(&EntryHeader, sizeof(EntryHeader))) != ERRORCODE_None)
            {
               break;
            }

         /* Plug the info in. */

            if ((error = File.write_cstring(pEntry->m_csName)) != ERRORCODE_None
                || (error = pEntry->m_Path.WriteData(&File)) != ERRORCODE_None)
            {
               break;
            }
         }
      }

      if (error != ERRORCODE_None)
      {
      /* Get rid of the file. */
         File.zap();
      }
   }

   return error;
}

/////////////////////////////////////////////////////////////////////////////
// CShapeFileParser

CShapeFileParser::CShapeFileParser(StorageFile* pFile, CPtrArray* pArray) :
   CParser(TRUE, FALSE)
{
   m_pFile = pFile;
   m_pArray = pArray;
   m_pEntry = NULL;
}

CShapeFileParser::~CShapeFileParser()
{
   delete m_pEntry;
}

int CShapeFileParser::NextChar(void)
{
   int nChar = 0;

   TRY
   {
      for (;;)
      {
         if (m_pFile->read(&nChar, 1) != ERRORCODE_None)
         {
            nChar = cspEOF;
         }
         // Skip CRs.
         if (nChar != '\r')
         {
            break;
         }
      }
   }
   CATCH (CFileException, e)
   {
   /* We only worry about EOF. */
      if (e->m_cause == CFileException::endOfFile)
      {
         nChar = cspEOF;
      }
      else
      {
         THROW_LAST();
      }
   }
   END_CATCH

   return nChar;
}

void CShapeFileParser::UngetChar(int nChar)
{
   m_pFile->seek(-1, ST_DEV_SEEK_CUR);
}

CShapeFileParser::PARSE_STATE CShapeFileParser::ParseStates[] =
{
// Shape "Name"

/*  0 */    { TOKEN_BeginString, (STATE_PROCESSOR)HandleName, REL+2, REL+1 },
/*  1 */    { TOKEN_EOF, NULL, PARSE_ACCEPT, PARSE_REJECT },

// Begin path.

/*  2 */    { TOKEN_BeginPath, NULL, REL+1, PARSE_REJECT },

// Get the path operator token.

/*  3 */    { TOKEN_EndPath, (STATE_PROCESSOR)HandleEndPath, 0, REL+1 },
/*  4 */    { TOKEN_Number, (STATE_PROCESSOR)HandleNumber, 3, REL+1 },
/*  5 */    { TOKEN_MoveTo, (STATE_PROCESSOR)HandlePathOp, 3, REL+1 },
/*  6 */    { TOKEN_LineTo, (STATE_PROCESSOR)HandlePathOp, 3, REL+1 },
/*  7 */    { TOKEN_CurveTo, (STATE_PROCESSOR)HandlePathOp, 3, REL+1 },
/*  8 */    { TOKEN_CurveTo01, (STATE_PROCESSOR)HandlePathOp, 3, REL+1 },
/*  9 */    { TOKEN_CurveTo23, (STATE_PROCESSOR)HandlePathOp, 3, REL+1 },
/* 10 */    { TOKEN_SplineTo, (STATE_PROCESSOR)HandlePathOp, 3, REL+1 },
/* 11 */    { TOKEN_Close, (STATE_PROCESSOR)HandlePathOp, 3, PARSE_REJECT },
};

TOKEN_LOOKUP CShapeFileParser::TokenLookup[] =
{
   { TOKEN_BeginString, "\""  },
   { TOKEN_LineComment, "//"  },
   { TOKEN_BeginPath,   "{"   },
   { TOKEN_EndPath,     "}"   },
   { TOKEN_MoveTo,      "M"   },
   { TOKEN_LineTo,      "L"   },
   { TOKEN_SplineTo,    "S"   },
   { TOKEN_CurveTo,     "C"   },
   { TOKEN_CurveTo01,   "V"   },
   { TOKEN_CurveTo23,   "Y"   },
   { TOKEN_Close,       "H"   },

   { TOKEN_EndOfList          }
};

void CShapeFileParser::HandleString(void)
{
   int nChar;

/*
// Skip to end of the string. We currently don't care what this is.
*/

   int i = 0;

   while ((nChar = NextStreamChar()) != '"')
   {
   /* See if we hit end of file! */
      if (nChar == cspEOF)
      {
         ThrowParseException(CParseException::unexpected_end_of_file);
      }

      if (i == MAX_TOKEN_SIZE-1)
      {
         ThrowParseException(string_too_long);
      }

      m_cbString[i++] = nChar;
   }

   m_cbString[i] = '\0';

/* Pull in the next token. */

   Match();
}

void CShapeFileParser::HandleName(void)
{
/* Parse the name. This does a Match(). */

   ASSERT(m_pEntry == NULL);

   HandleString();

   m_pEntry = new CShapeEntry;
   ASSERT(m_pEntry != NULL);

   m_pEntry->m_nType = CShapeEntry::DrawingObject;
   m_pEntry->m_csName = m_cbString;
}

void CShapeFileParser::HandlePathOp(void)
{
   switch (m_tLookahead)
   {
      case TOKEN_MoveTo:
      {
         PopPoint(m_cp);

         m_pEntry->m_Path.MoveTo(m_cp.x, m_cp.y); 

         break;
      }
      case TOKEN_LineTo:
      {
         PopPoint(m_cp);

         m_pEntry->m_Path.LineTo(m_cp.x, m_cp.y); 

         break;
      }
      case TOKEN_SplineTo:
      {
         CFixedPoint p0;

         PopPoint(m_cp);
         PopPoint(p0);

         m_pEntry->m_Path.SplineTo(p0.x, p0.y, m_cp.x, m_cp.y); 

         break;
      }
      case TOKEN_CurveTo:
      {
         CFixedPoint p0;
         CFixedPoint p1;

         PopPoint(m_cp);
         PopPoint(p1);
         PopPoint(p0);

         m_pEntry->m_Path.BezierTo(p0.x, p0.y, p1.x, p1.y, m_cp.x, m_cp.y); 

         break;
      }
      case TOKEN_CurveTo01:
      {
         CFixedPoint p2;
         CFixedPoint p1;

         PopPoint(p2);
         PopPoint(p1);

         m_pEntry->m_Path.BezierTo(m_cp.x, m_cp.y, p1.x, p1.y, p2.x, p2.y); 

         m_cp = p2;

         break;
      }
      case TOKEN_CurveTo23:
      {
         CFixedPoint p0;

         PopPoint(m_cp);
         PopPoint(p0);

         m_pEntry->m_Path.BezierTo(p0.x, p0.y, m_cp.x, m_cp.y, m_cp.x, m_cp.y); 

         break;
      }
      case TOKEN_Close:
      {
         ASSERT(m_pEntry != NULL);

         m_pEntry->m_Path.Close();
         break;
      }
      default:
      {
         ThrowParseException(CParseException::internal_error);
         break;
      }
   }

   Match();
}

void CShapeFileParser::HandleNumber(void)
{
   PushNumber((CFixed)(m_dTokenNumber*65536.0));

   Match();
}

void CShapeFileParser::PushNumber(CFixed l)
{
   m_dwaStack.Add((DWORD)l);
}

CFixed CShapeFileParser::PopNumber(void)
{
   int nCount = m_dwaStack.GetSize();

   if (nCount == 0)
   {
      ThrowParseException(stack_underflow);
   }

   CFixed l = (CFixed)m_dwaStack.GetAt(nCount-1);
   m_dwaStack.RemoveAt(nCount-1);

   return l;
}

void CShapeFileParser::PopPoint(CFixedPoint& p)
{
   p.y = PopNumber();
   p.x = PopNumber();

// Adjust for the coordinate system.

   p.y = -p.y;
}

void CShapeFileParser::HandleEndPath(void)
{
   ASSERT(m_pEntry != NULL);

   m_pEntry->m_Path.End();

/* Normalize the path. */

   m_pEntry->m_Path.BoundingBox(&(m_pEntry->m_Bounds));

   if ((m_pEntry->m_Bounds.Width() != 0) && (m_pEntry->m_Bounds.Height() != 0))
   {
      COutlinePathMatrix Matrix;

      Matrix.Identity();
      Matrix.TranslateBy(-m_pEntry->m_Bounds.Left, -m_pEntry->m_Bounds.Top);
      Matrix.ScaleBy(DivFixed(MakeFixed(1), m_pEntry->m_Bounds.Width()),
                     DivFixed(MakeFixed(1), m_pEntry->m_Bounds.Height()));

      m_pEntry->m_Path.Transform(&Matrix);
      m_pArray->Add(m_pEntry);
   }
   else
   {
      delete m_pEntry;
   }

   m_pEntry = NULL;

   Match();
}

