/*************************************************************
Module name: TimePerf.C
Notices: Written 1994 by Jeffrey Richter and Jim Harkins
*************************************************************/


#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "Resource.H"


//////////////////////////////////////////////////////////////


#define MYTIMERID 1           // Timer id for our timer

UINT g_uPeriod;               // Timer period in milliseconds
int  g_nSamplesMax;           // Number of samples to take
int  g_nTickSamples;          // Number of samples taken

// g_pdwTickSamples will be NULL when the timer is stopped
// and non-NULL when the timer it running.
PDWORD g_pdwTickSamples = NULL;  // Pointer to samples


//////////////////////////////////////////////////////////////


void Dlg_AppendEditText (HWND hwndEdit, LPCTSTR lpszLine) {
   int nLen;

   // Move caret to end of text
   nLen = Edit_GetTextLength(hwndEdit);
   Edit_SetSel(hwndEdit, nLen, nLen);

   // Insert text
   Edit_ReplaceSel(hwndEdit, lpszLine);
}


//////////////////////////////////////////////////////////////


// Calculate Interval Statistics
void Dlg_IntervalStat (PDWORD pdwSamples, int nSamples,
   DWORD *pdwMin, DWORD *pdwMax,
   float *prAvg, float *prStdDev) {

   DWORD dwIntervalCnt; // Number of intervals
   DWORD dwTotalTime;   // Total time in milliseconds
   float rVarSum;       // Running variance sum
   UINT ui;
   DWORD dwInterval;

   if (nSamples < 2) {
      *pdwMin = 0;
      *pdwMax = 0;
      *prAvg = 0.0f;
      *prStdDev = 0.0f;
   } else {
      dwIntervalCnt = nSamples - 1;
      dwTotalTime = pdwSamples[dwIntervalCnt] - pdwSamples[0];

      *prAvg = (float) dwTotalTime / dwIntervalCnt;

      // Initialize *pdwMin and *pdwMax
      *pdwMin = pdwSamples[1] - pdwSamples[0];
      *pdwMax = *pdwMin;

      rVarSum = 0.0f;

      // Find the min., max, and standard deviation.
      for (ui = 0; ui < (DWORD) (nSamples - 1); ui++) {
         dwInterval = pdwSamples[ui + 1] - pdwSamples[ui];

         *pdwMin = __min(*pdwMin, dwInterval);
         *pdwMax = __max(*pdwMax, dwInterval);

         rVarSum += (*prAvg - dwInterval) *
            (*prAvg - dwInterval);
      }

      // Standard deviation using biased (/n) method
      *prStdDev = (float) sqrt(rVarSum / dwIntervalCnt);
   }
}


//////////////////////////////////////////////////////////////


void Dlg_CreateReport (HWND hwndEdit) {
   TCHAR szBuf[500];    // Line buffer
   int   i, nVisLine;
   DWORD dwMin, dwMax;
   float rAvg, rStdDev;

   // Disable updates to prevent the edit box from scrolling.
   SetWindowRedraw(hwndEdit, FALSE);

   Dlg_IntervalStat(g_pdwTickSamples, g_nSamplesMax, &dwMin,
      &dwMax, &rAvg, &rStdDev);

   // Clear the report.
   Edit_SetText(hwndEdit, _TEXT(""));

   // Write the report.
   _stprintf(szBuf, 
      _TEXT("Period (mS):  %lu\r\n")
      _TEXT("Sample Size:  %lu\r\n")
      _TEXT("\r\n")
      _TEXT("Interval Statistics\r\n")
      _TEXT("   Avg. Period:  %.3f\r\n")
      _TEXT("   Min. Period:  %lu\r\n")
      _TEXT("   Max. Period:  %lu\r\n")
      _TEXT("   Standard Dev.:  %.3f\r\n")
      _TEXT("\r\n")
      _TEXT("Timer Samples:"),
      g_uPeriod,
      g_nSamplesMax,
      rAvg,
      dwMin,
      dwMax,
      (double) rStdDev);

   Dlg_AppendEditText(hwndEdit, szBuf);
   
   _tcscpy(szBuf, _TEXT("\r\n"));
   for (i = 0; i < g_nSamplesMax; i++) {
      _ultot(g_pdwTickSamples[i], szBuf + 2, 10);
      Dlg_AppendEditText(hwndEdit, szBuf);
   }

   // Move caret to start of text.
   Edit_SetSel(hwndEdit, 0, 0);

   // Make first report line first visible line.
   nVisLine = Edit_GetFirstVisibleLine(hwndEdit);
   Edit_Scroll(hwndEdit, -nVisLine, 0);

   // Enable updates, and force update.
   SetWindowRedraw(hwndEdit, TRUE);
   InvalidateRect(hwndEdit, NULL, TRUE);
}


//////////////////////////////////////////////////////////////


BOOL Dlg_StartTimer (HWND hwnd) {
   BOOL fRet = TRUE;
   UINT uTimerId;

   g_nTickSamples = 0;

   g_pdwTickSamples = malloc(g_nSamplesMax *
      sizeof(*g_pdwTickSamples));

   if (NULL == g_pdwTickSamples) {
      fRet = FALSE;
   } else {
      uTimerId = SetTimer(hwnd, MYTIMERID, g_uPeriod, NULL);
      if (0 == uTimerId) {
         fRet = FALSE;
         free(g_pdwTickSamples);
         g_pdwTickSamples = NULL;
      } else {
         // Now running, update button enabled states
         EnableWindow(GetDlgItem(hwnd, IDC_STOP), TRUE);
         EnableWindow(GetDlgItem(hwnd, IDC_START), FALSE);
      }
   }
   return(fRet);
}


//////////////////////////////////////////////////////////////


void Dlg_StopTimer (HWND hwnd) {

   // Stopped running, update the button enabled states.
   EnableWindow(GetDlgItem(hwnd, IDC_STOP), FALSE);
   EnableWindow(GetDlgItem(hwnd, IDC_START), TRUE);

   if (NULL != g_pdwTickSamples) {
      KillTimer(hwnd, MYTIMERID);
      free(g_pdwTickSamples);
      g_pdwTickSamples = NULL;
   }
}


//////////////////////////////////////////////////////////////


BOOL Dlg_OnInitDialog (HWND hwnd, HWND hwndFocus,
   LPARAM lParam) {

   //  We're not running, disable stop button.
   EnableWindow(GetDlgItem(hwnd, IDC_STOP), FALSE);

   // Limit inputs max. valid length.
   Edit_LimitText(GetDlgItem(hwnd, IDC_EDITPERIOD), 10);
   Edit_LimitText(GetDlgItem(hwnd, IDC_EDITSAMPLES), 5);

   // Allow output to be max. length.
   Edit_LimitText(GetDlgItem(hwnd, IDC_EDITREPORT), 0);

   return(TRUE);
}


//////////////////////////////////////////////////////////////


void Dlg_OnTimer (HWND hwnd, UINT id) {

   if (g_nTickSamples < g_nSamplesMax) {
      // Store the sample and keep going.
      g_pdwTickSamples[g_nTickSamples++] = GetTickCount();
   }

   if (g_nSamplesMax <= g_nTickSamples) {
      // We have all the samples.  Create the
      // report and stop the timer.
      Dlg_CreateReport(GetDlgItem(hwnd, IDC_EDITREPORT));
      Dlg_StopTimer(hwnd);
   }
}


//////////////////////////////////////////////////////////////


void Dlg_OnCommand (HWND hwnd, int id, HWND hwndCtl,
   UINT codeNotify) {

   switch (id) {
      case IDC_START:
         // Validate period and number of samples then 
         // start the timer.
         g_uPeriod = GetDlgItemInt(hwnd, IDC_EDITPERIOD,
            NULL, FALSE);
         if (g_uPeriod > INT_MAX) {
            MessageBox(NULL,
               __TEXT("Period must be <= 2147483647 ")
               __TEXT("or a flood of messages ensues"),
               NULL, MB_OK);
         }

         g_nSamplesMax = GetDlgItemInt(hwnd, 
            IDC_EDITSAMPLES, NULL, FALSE);
         if ((g_nSamplesMax < 2) || (g_nSamplesMax > 16384)) {
            MessageBox(NULL,
               __TEXT("Samples must be >= 2 and <= 16384"),
               NULL, MB_OK);
         } else {
            // Clear the report
            Edit_SetText(GetDlgItem(hwnd, IDC_EDITREPORT),
               _TEXT(""));

            if (!Dlg_StartTimer(hwnd)) {
               MessageBox(NULL,
                  __TEXT("Unable to start timer."),
                  NULL, MB_OK);
            }
         }
         break;

      case IDC_STOP:
         Dlg_StopTimer(hwnd);
         break;

      case IDCANCEL:
         Dlg_StopTimer(hwnd);
         EndDialog(hwnd, id);
         break;
   }
}


//////////////////////////////////////////////////////////////


BOOL WINAPI Dlg_Proc (HWND hwnd, UINT uMsg,
   WPARAM wParam, LPARAM lParam) {

   BOOL fProcessed = TRUE;

   switch (uMsg) {
      HANDLE_MSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
      HANDLE_MSG(hwnd, WM_COMMAND, Dlg_OnCommand);
      HANDLE_MSG(hwnd, WM_TIMER, Dlg_OnTimer);

      default:
         fProcessed = FALSE;
         break;
   }

   return(fProcessed);
}


//////////////////////////////////////////////////////////////


int WINAPI WinMain (HINSTANCE hinstExe,
   HINSTANCE hinstPrev, LPSTR lpszCmdLine, int nCmdShow) {

   DialogBox(hinstExe, MAKEINTRESOURCE(IDD_TIMEPERF),
      NULL, Dlg_Proc);

   return(0);
}


/////////////////////// End Of File //////////////////////////
