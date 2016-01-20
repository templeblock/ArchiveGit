//////////////////////////////////////////////////////////////////////
// TutInfo.cpp
//
// Searches for "*.tut" files in subdirectories and builds a list with
// tutorial info to be read by PrintMaster at startup.
//////////////////////////////////////////////////////////////////////

#include <afx.h>			// MFC core and standard components
#include <ostream.h>
#include <conio.h>
#include "TutRes.h"
#include "TutInfo.h"

main()
{
   CTutInfo info;
   info.BuildList();

   CStdioFile File(TUTORIAL_INFO_FILENAME, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
   info.Write(File);

   cout << "\n*** Done creating " << TUTORIAL_INFO_FILENAME << "! ***\n";
   cout << "\nPress a key to exit\n";
   cout.flush();
   _getch();

   return 1;
}

IMPLEMENT_SERIAL(CTutInfo, CObject, 1)

CTutInfo::CTutInfo()
{
}

CTutInfo::~CTutInfo()
{
}

BOOL CTutInfo::BuildList()
{
   // find all the "tut" DLLs and load their resource strings

   CFileFind find;
   if (find.FindFile("*.*"))
   {
      CString filename;
      DWORD num = 0;
      BOOL more = TRUE;
      do
      {
         more = find.FindNextFile();
         CString tutDir = find.GetFileTitle();

         if (!find.IsDirectory() || find.IsDots() || tutDir == "")
            continue;

			CString filename = tutDir;
			if (filename.GetLength() > 8)
			{
				filename = filename.Left(8);
			}
         filename += ".tut";

         CString filepath = tutDir + "\\Release\\" + filename;
         HINSTANCE handle = LoadLibrary(filepath);

         if (handle)
         {
            cout << "Reading " << filepath << '\n';
            cout.flush();

            TUTINFO* pTut = new TUTINFO;

            pTut->csFilename = filename;

            // Get ID and Title from resource string
            char buf[512];
            LoadString(handle, IDS_TUTORIAL_INFO, buf, sizeof(buf));
            pTut->csInfo = buf;
            m_TutList.Add(pTut);
            FreeLibrary(handle);
         }
      } while (more);
   }
   return TRUE;
}

void CTutInfo::Write(CStdioFile& File)
{
   int count = m_TutList.GetSize();
   for (int i = 0; i < count; i++)
   {
      TUTINFO* pInfo = (TUTINFO*)m_TutList[i];
		File.WriteString(pInfo->csFilename);
		File.WriteString("|");
		File.WriteString(pInfo->csInfo);
		File.WriteString("\n");

      cout << "Writing " << pInfo->csFilename << '\n';
      cout.flush();

      delete pInfo;
   }
}
