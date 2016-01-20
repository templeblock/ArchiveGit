/*
// Text import
//
// $Header: /PM8/App/TIMPORT.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TIMPORT.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     4/16/98 6:30p Hforman
// no more CVirtualFileDialog
// 
//    Rev 1.0   14 Aug 1997 15:26:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:50   Fred
// Initial revision.
// 
//    Rev 1.2   16 Apr 1997 12:12:30   Jay
// Fixes for text undo
// 
//    Rev 1.1   02 Jan 1997 11:02:14   Jay
// Undo/redo for import text
// 
//    Rev 1.0   24 Dec 1996 12:06:50   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwcfg.h"

#include "timport.h"
#include "file.h"
#include "txpdest.h"
#include "tcommand.h"

static int TypeOfTextFile(StorageFile* pFile);

void CPmwView::OnImportText() 
{
   CPmwDoc* pDoc = GetDocument();

	ASSERT(m_Txp.Valid());

	if (m_Txp.Valid())
	{
		CString csFilters;
		LPCSTR pFilters = NULL;

		TRY
		{
			LoadConfigurationString(IDS_TEXT_FILTERS, csFilters);
			pFilters = csFilters;
		}
		END_TRY

		CFileDialog Dialog(TRUE,
								 NULL,
								 NULL,			// in filename.
								 OFN_HIDEREADONLY,
								 pFilters,
								 this);

		if (Dialog.DoModal())
		{
			// Do something.
			TRACE("Selected name.\n");

			ReadOnlyFile File(Dialog.GetPathName());
			File.set_read_buffer(8192);

		/*
		// Create an undo/redo command.
		*/

			CTextCommand* pCommand = new CTextCommand(IDCmd_ImportText);

			if (!pCommand->Before(this))
			{
				delete pCommand;
				pCommand = NULL;
			}

			if (m_Txp.SomethingSelected())
			{
			/* Get rid of the text here first. */
				m_Txp.DeleteText(1);
			}

			CHARACTER_INDEX c_start = m_Txp.Index();
			m_Txp.WantFlowText(FALSE);

			switch (TypeOfTextFile(&File))
			{
				case TEXTTYPE_RTF:
				{
					// Parse as an RTF file.
					File.seek(0, ST_DEV_SEEK_SET);		// Start at front.

					CFileRTFSource Source(&File);
					CTxpRTFDestination Destination(&m_Txp);

					// Setup our parser.
					CRTFParser Parser(&Source, &Destination);
					BeginWaitCursor();

					TRY
					{
						Parser.Parse();
					}
					CATCH(CRTFException, e)
					{
						TRACE("Got parse exception: %d\n", e->m_nError);
					}
					AND_CATCH_ALL(e)
					{
						ASSERT(FALSE);
					}
					END_CATCH_ALL
					EndWaitCursor();

					break;
				}
//				case TEXTTYPE_Plain:
//				case TEXTTYPE_Unknown:
				default:
				{
					// Import as normal text file.
					CHARACTER* pBuffer = NULL;
					int nBufferSize = 4096;
					TRY
					{
						pBuffer = new CHARACTER[nBufferSize];
					}
					END_TRY
               if (pBuffer == NULL)
               {
                  break;
               }

					for (;;)
					{
						CHARACTER* p = pBuffer;
						int n;
						for (n = 0; n < nBufferSize; )
						{
							BYTE b;
							if (File.read(&b, sizeof(b)) != ERRORCODE_None)
							{
								break;
							}
                     if (b != '\r')
                     {
                        *p++ = (CHARACTER)b;
                        n++;
                     }
						}

						// If we have any text, insert it now.
						if (n > 0)
						{
							m_Txp.InsertText(pBuffer, n);
						}

						// If we didn't read a full buffer last time, we're done.
						if (n < nBufferSize)
						{
							break;
						}
					}

               delete [] pBuffer;
					break;
				}
			}

			m_Txp.WantFlowText(TRUE);
			CHARACTER_INDEX c_end = m_Txp.Index();
			m_Txp.SelectText(c_start, c_end);

			if (c_start != c_end)
			{
				m_Txp.FlowText(c_start, c_end);
				m_Txp.SetDesiredX();
			}

			// Finish the undo/redo command.
			if (pCommand != NULL)
			{
				if (pCommand->After())
				{
					pDoc->AddCommand(pCommand, FALSE);
				}
				else
				{
					delete pCommand;
				}
			}

			m_Txp.Deselect();
		}
	}
}

void CPmwView::OnUpdateImportText(CCmdUI* pCmdUI) 
{
	OnEditingText(pCmdUI);
}

static int TypeOfTextFile(StorageFile* pFile)
{
	int nType = TEXTTYPE_Unknown;
	if (pFile->seek(0, ST_DEV_SEEK_SET) == ERRORCODE_None)
	{
		char cbBuffer[10];
		switch (pFile->read(cbBuffer, 5))
		{
			case ERRORCODE_None:
			{
				if (strnicmp(cbBuffer, "{\\rtf", 5) == 0)
				{
					nType = TEXTTYPE_RTF;
					break;
				}
				// Fall through to...
			}
			case ERRORCODE_Read:
			{
				// Not enough characters. Mark as plain.
				nType = TEXTTYPE_Plain;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	return nType;
}

/////////////////////////////////////////////////////////////////////////////
// CFileRTFSource

CFileRTFSource::CFileRTFSource(StorageFile* pFile)
{
	ASSERT(pFile != NULL);
	m_pFile = pFile;
}

CFileRTFSource::~CFileRTFSource()
{
}

// Functions.

int CFileRTFSource::GetChar(void)
{
	BYTE b;
	if (m_pFile->read(&b, sizeof(b)) != ERRORCODE_None || b == 0)
	{
		return RTFEOF;
	}

	return b;
}

void CFileRTFSource::UnGetChar(int nChar)
{
	m_pFile->seek(-1L, ST_DEV_SEEK_CUR);
}
