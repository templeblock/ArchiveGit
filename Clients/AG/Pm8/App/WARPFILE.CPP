/*
// $Workfile: WARPFILE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPFILE.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     3/17/98 2:43p Fredf
// New workspace toolbars.
// 
//    Rev 1.0   14 Aug 1997 15:27:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:58   Fred
// Initial revision.
// 
//    Rev 1.1   14 May 1997 10:16:48   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.0   14 Mar 1996 13:35:24   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:32   FRED
// Initial revision.
// 
//    Rev 1.3   15 May 1995 17:19:46   JAY
// No longer uses sharing mode since it's a CStdioFile.
// 
//    Rev 1.2   15 May 1995 09:55:06   JAY
// Now uses CStdioFile which has some buffering.
// 
//    Rev 1.1   28 Apr 1995 17:36:42   FRED
//  
// 
//    Rev 1.0   28 Apr 1995 13:51:28   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "warpfile.h"
#include "warpfld.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarpFile

CWarpFile::CWarpFile(void)
{
}

CWarpFile::~CWarpFile(void)
{
   FreeWarpFields();
}

BOOL CWarpFile::Init(LPCSTR lpszFileName)
{
   BOOL fResult = FALSE;

   FreeWarpFields();

   ReadOnlyFile File(lpszFileName);
   File.set_read_buffer(2048);

   if (File.seek(0, ST_DEV_SEEK_SET) == ERRORCODE_None)
   {
      TRY
      {
         CWarpFileParser Parser(&File, &m_WarpFields);
         Parser.Parse(CWarpFileParser::ParseStates, CWarpFileParser::TokenLookup);
         fResult = TRUE;
      }
      END_TRY
   }
   
   return fResult;
}

void CWarpFile::FreeWarpFields(void)
{
   for (int i = 0; i < m_WarpFields.GetSize(); i++)
   {
      delete (CWarpField*)m_WarpFields[i];
      m_WarpFields[i] = NULL;
   }
   
   m_WarpFields.RemoveAll();
};

/////////////////////////////////////////////////////////////////////////////
// CWarpFileParser

CWarpFileParser::CWarpFileParser(StorageFile* pWarpFile, CPtrArray* pArray) :
   CParser(TRUE, FALSE)
{
   m_pFile = pWarpFile;
   m_pArray = pArray;
}

CWarpFileParser::~CWarpFileParser()
{
}

int CWarpFileParser::NextChar(void)
{
   int nChar = 0;

   TRY
   {
      if (m_pFile->read(&nChar, 1) != ERRORCODE_None)
      {
         nChar = cspEOF;
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

void CWarpFileParser::UngetChar(int nChar)
{
   m_pFile->seek(-1, ST_DEV_SEEK_CUR);
}

CWarpFileParser::PARSE_STATE CWarpFileParser::ParseStates[] =
{

// Category "Name"

/*  0 */    { TOKEN_BeginString, (STATE_PROCESSOR)HandleName,        REL+2,         REL+1          },
/*  1 */    { TOKEN_EOF,         NULL,                               PARSE_ACCEPT,  PARSE_REJECT   },

// Type

/*  2 */    { TOKEN_X,           (STATE_PROCESSOR)HandleType,        REL+3,         REL+1          },
/*  3 */    { TOKEN_Y,           (STATE_PROCESSOR)HandleType,        REL+2,         REL+1          },
/*  4 */    { TOKEN_XY,          (STATE_PROCESSOR)HandleType,        REL+1,         PARSE_REJECT   },

// Begin edge

/*  5 */    { TOKEN_BeginEdge,   (STATE_PROCESSOR)HandleBeginEdge,   REL+1,         PARSE_REJECT   },

// Edge type

/*  6 */    { TOKEN_Line,        (STATE_PROCESSOR)HandleEdgeType,    REL+3,         REL+1          },
/*  7 */    { TOKEN_Spline,      (STATE_PROCESSOR)HandleEdgeType,    REL+2,         REL+1          },
/*  8 */    { TOKEN_Bezier,      (STATE_PROCESSOR)HandleEdgeType,    REL+1,         PARSE_REJECT   },

// Edge coordinates

/*  9 */    { TOKEN_Number,      (STATE_PROCESSOR)HandleEdgeNumber,  REL+0,         REL+1          },

// End edge

/* 10 */    { TOKEN_EndEdge,     (STATE_PROCESSOR)HandleEndEdge,     REL+1,         PARSE_REJECT   },

// End entry

/* 11 */    { TOKEN_EndEntry,    (STATE_PROCESSOR)HandleEndEntry,    0,             5              }

};

TOKEN_LOOKUP CWarpFileParser::TokenLookup[] =
{
   { TOKEN_BeginString, "\""  },
   { TOKEN_LineComment, "//"  },
   { TOKEN_X,           "X"   },
   { TOKEN_Y,           "Y"   },
   { TOKEN_XY,          "XY"  },
   { TOKEN_BeginEdge,   "("   },
   { TOKEN_Line,        "L"   },
   { TOKEN_Spline,      "S"   },
   { TOKEN_Bezier,      "B"   },
   { TOKEN_EndEdge,     ")"   },
   { TOKEN_EndEntry,    ";"   },

   { TOKEN_EndOfList          }
};

void CWarpFileParser::HandleString(void)
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

void CWarpFileParser::HandleName(void)
{
/* Parse the name. This does a Match(). */

   HandleString();

   m_csName = m_cbString;
}

void CWarpFileParser::HandleType(void)
{
   switch (m_tLookahead)
   {
      case TOKEN_X:
      {
         m_nType = CWarpFieldBounds::X;
         break;
      }
      case TOKEN_Y:
      {
         m_nType = CWarpFieldBounds::Y;
         break;
      }
      case TOKEN_XY:
      {
         m_nType = CWarpFieldBounds::XY;
         break;
      }
      default:
      {
         ThrowParseException(CParseException::internal_error);
         break;
      }
   }
   
   m_nEdgesParsed = 0;
   
   Match();
}

void CWarpFileParser::HandleBeginEdge(void)
{
   if (m_nEdgesParsed >= 4)
   {
      ThrowParseException(too_many_edges);
   }

   m_nCoordinatesParsed = 0;
   
   if (m_nEdgesParsed > 0)
   {
      // Fill in the first point of the current edge with the last point
      // of the previous edge.
      
      m_Edge[m_nEdgesParsed].m_lCoordinate[0] = m_LastX;
      m_Edge[m_nEdgesParsed].m_lCoordinate[1] = m_LastY;
   }
   
   Match();
}

void CWarpFileParser::HandleEdgeType(void)
{
   switch (m_tLookahead)
   {
      case TOKEN_Line:
      {
         m_Edge[m_nEdgesParsed].m_nType = CWarpFieldEdge::Line;
         m_nCoordinatesNeeded = 2;
         break;
      }
      case TOKEN_Spline:
      {
         m_Edge[m_nEdgesParsed].m_nType = CWarpFieldEdge::Spline;
         m_nCoordinatesNeeded = 4;
         break;
      }
      case TOKEN_Bezier:
      {
         m_Edge[m_nEdgesParsed].m_nType = CWarpFieldEdge::Bezier;
         m_nCoordinatesNeeded = 6;
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

void CWarpFileParser::HandleEdgeNumber(void)
{
   if (m_nCoordinatesParsed >= m_nCoordinatesNeeded)
   {
      ThrowParseException(too_many_coordinates);
   }
   
   long lNumber = (long)(m_dTokenNumber*65536.0);
   
   if ((m_nCoordinatesParsed & 1) == 0)
   {
      m_LastX = lNumber;
   }
   else
   {
      m_LastY = lNumber;
   }

   m_Edge[m_nEdgesParsed].m_lCoordinate[2+(m_nCoordinatesParsed++)] = lNumber;
   
   Match();
}

void CWarpFileParser::HandleEndEdge(void)
{
   if (m_nCoordinatesParsed != m_nCoordinatesNeeded)
   {
      ThrowParseException(too_few_coordinates); 
   }
   
   m_nEdgesParsed++;

   Match();
}

void CWarpFileParser::HandleEndEntry(void)
{
   if (m_nEdgesParsed != 4)
   {
      ThrowParseException(too_few_edges); 
   }

   // Fill in the first coordinate of the first edge with the last
   // point of the last edge.
   
   m_Edge[0].m_lCoordinate[0] = m_LastX;
   m_Edge[0].m_lCoordinate[1] = m_LastY;
   
   // Create a new warp field object and initialize it.
   
   CWarpField* pWarpField = new CWarpField;

   pWarpField->Name(m_csName);
   pWarpField->OriginalBounds()->Type(m_nType);
   
   for (int i = 0; i < 4; i++)
   {
      CWarpFieldEdge* pEdge = pWarpField->OriginalBounds()->Edge(i);
      
      switch (m_Edge[i].m_nType)
      {
         case CWarpFieldEdge::Line:
         {
            pEdge->InitLine(
               m_Edge[i].m_lCoordinate[0], m_Edge[i].m_lCoordinate[1],
               m_Edge[i].m_lCoordinate[2], m_Edge[i].m_lCoordinate[3]);
            break;
         }
   
         case CWarpFieldEdge::Spline:
         {
            pEdge->InitSpline(
               m_Edge[i].m_lCoordinate[0], m_Edge[i].m_lCoordinate[1],
               m_Edge[i].m_lCoordinate[2], m_Edge[i].m_lCoordinate[3],
               m_Edge[i].m_lCoordinate[4], m_Edge[i].m_lCoordinate[5]);
            break;
         }
                              
         case CWarpFieldEdge::Bezier:
         {
            pEdge->InitBezier(
               m_Edge[i].m_lCoordinate[0], m_Edge[i].m_lCoordinate[1],
               m_Edge[i].m_lCoordinate[2], m_Edge[i].m_lCoordinate[3],
               m_Edge[i].m_lCoordinate[4], m_Edge[i].m_lCoordinate[5],
               m_Edge[i].m_lCoordinate[6], m_Edge[i].m_lCoordinate[7]);
            break;
         }
         
         default:
         {
            ThrowParseException(CParseException::internal_error);
            break;
         }
      }
   }
   
   m_pArray->Add(pWarpField);
      
   Match();
}

