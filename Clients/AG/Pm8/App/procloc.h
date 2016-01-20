/***************************************************************************
*  FILE:        PROCLOC.H                                                  *
*  SUMMARY:     Class for Finding a running process under Windows 95 & NT  *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*                                                                          *
***************************************************************************/

#ifndef _PROCLOC_INC
#define _PROCLOC_INC

class CProcessLocater
   {
   public:
   // Construction
      CProcessLocater();
      virtual ~CProcessLocater();

   // Operations

   // GetProcessFileName() returns name of process found upon successful
   // call to FindProcess()
      CString &   GetProcessFileName();
      HWND        GetWindowHandle();
      DWORD       GetProcessID();

      BOOL        Find(LPCSTR szProgramFileName, BOOL bBringWindowToTop=TRUE, WORD wUniqueEventID=1);
      BOOL        BringWindowToTop();

   // Close() attempt to terminate process
      BOOL        Close();

   // Static methods
      static BOOL CALLBACK enumProcessCallback(DWORD dwProcessID, WORD w16BitTask,
         LPSTR szFilename, LPARAM lUserParam);

   protected:
   // Helpers

   // FindProcessWindow() gets the window handle associated with the last found process
      BOOL        FindProcessWindow();
   protected:
   // Data members
      BOOL        m_bFoundProcess;              // State of FindProcess() call
      DWORD       m_dwProcessID;                // ID of process found
      WORD        m_wThisEventID;               // unique ID of this object
      CString     m_csProcessToFind;            // Copy of callers program name
      CString     m_csShortProcessFileName;     // Short FileName of process found
      CString     m_csLongProcessFileName;      // Long FileName of process found
      HWND        m_hwndProcess;                // Window handle of process

   // Static Data Members
      static CMapWordToPtr sm_thisList;          // List of this pointers
   };

// Inline Section
inline CString & CProcessLocater::GetProcessFileName()
   {
      return m_csShortProcessFileName;
   }

inline HWND CProcessLocater::GetWindowHandle()
   {
      return m_hwndProcess;
   }

inline DWORD CProcessLocater::GetProcessID()
   {
      return m_dwProcessID;
   }

inline BOOL CProcessLocater::Close()
   {
      return ::SendMessage(GetWindowHandle(), WM_CLOSE, 0, 0) == 0;
   }

#endif

