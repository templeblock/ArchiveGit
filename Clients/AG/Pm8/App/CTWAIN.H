/****************************************************************************
*  FILE:        CTWAIN.H                                                    *
*  SUMMARY:     Twain Class Wrapper.                                        *
*  AUTHOR:      Dennis Plotzke                                              *
*  -------------------------------------------------------------------------*
*  Initials Legend:                                                         *
*     DGP       Dennis Plotzke                                              *
*  -------------------------------------------------------------------------*
*  Revision History                                                         *
*                                                                           *
*  Date     Initials  Description of Change                                 *
*  -------------------------------------------------------------------------*
*  10/07/96 DGP       Added DisplayError().                                 *
*  09/25/95 DGP       Removed WriteImage().                                 *
*  08/22/96 DGP       Started History.                                      *
*                                                                           *
****************************************************************************/
#ifndef _CTWAIN_INC
#define _CTWAIN_INC

#include "twain.h"
#include "xerror.h"

// Messages sent to client
#define CTMSG_TRANSFER    1 // Message sent after image acquisition

// CTwain Return Codes
typedef enum
{
    CTRC_SUCCESS,           // No Error exists
    CTRC_FAIL,              // General error, check extended error 
    CTRC_SEQUENCE_ERR,      // Operation attempted out of sequence
    CTRC_FINDLIB_FAIL,      // Failed to find DLL
    CTRC_SMOPEN_FAIL,       // Source Manager request to open failed.
    CTRC_SMCLOSE_FAIL,      // Source Manager request to close failed.
    CTRC_SMLIB_NOTLOADED,   // Source Manager DLL not loaded.
	 CTRC_SMGETFIRST_FAIL,   // Source Manager get first source request failed.
    CTRC_DSOPEN_FAIL,       // Data Source request to open failed.
    CTRC_DSCLOSE_FAIL,      // Data Source request to close failed.
    CTRC_DSENABLE_FAIL,     // Data Source request to enable failed.
    CTRC_DSDISABLE_FAIL,    // Data Source request to disable failed.
    CTRC_DSSELECT_FAIL,     // Selection of Data Source selection failed.
    CTRC_DSXFERTYPE_FAIL,   // Selection of transfer type failed.
    CTRC_XFEREND_FAIL,      // End of transfer command failed.
    CTRC_XFER_CANCELED,     // User canceled scan.
    CTRC_XFER_FAILED,       // Image transfer failed scan.
}   CTRC_XERROR_LIST;

class CTwain
{
// Public Members
public:
  CTwain();
  virtual ~CTwain();
// Public Members
  CExtendedError  m_ErrorInfo;

// Operations
public:
  BOOL          Acquire(HWND hWnd);        // Start image acquisition.
  BOOL          ProcessMessage(MSG *pMsg);
  HANDLE        GetBitmap() { return m_hBitmap; }
  BOOL          HaveNewImage();

  // Lower Level Operations
  BOOL          SetHandler(HWND hWndToReceive, UINT nUserStartMessage=WM_USER+200);
  BOOL          IsManagerInstalled(); // Checks if Twain DLL is installed
  BOOL          IsSourceAvailable(HWND hWnd);  // Checks if any source device is available (does not open source).
  BOOL          IsSourceInstalled(HWND hWnd);  // Checks if a device exists.
  virtual BOOL          OpenManager(HWND hWnd); // Open  Data Source Manager (comm. layer)
  virtual BOOL          CloseManager();       // Close Data Source Manager
  virtual BOOL          OpenSource();         // Open     Data Source
  virtual BOOL          CloseSource();        // Close    Data Source
  virtual BOOL          EnableSource();       // Enable   Data Source
  virtual BOOL          DisableSource();      // Disable  Data Source
  virtual BOOL          SelectSource();       // Select   Data Source

// Helper functions
private:
  // Called for message handling
  void          Init();
  void          DeInit();
  BOOL          SelectTransferType(short nType=TWSX_NATIVE); // Select Transfer Type
  BOOL          TransferImage();
  BOOL          DoNativeTransfer();
  BOOL          DoFileTransfer(CString &strFilename);
  BOOL          DoMemTransfer();
  BOOL          FlipBitMap(TW_INT16 PixType);
  float         FIX32ToFloat(TW_FIX32 fix32);
  TW_UINT16     SourceManager(pTW_IDENTITY pSourceID, TW_UINT32 dwDataGroup, 
                    TW_UINT16 usDataType, TW_UINT16 usMessage, TW_MEMREF pData);
  void          MakeBMPHeader();

// Internal members
protected:
  BOOL          m_bSourceOpen, m_bManagerOpen, m_bSourceEnabled, m_bShow;
  BOOL          m_bAcquiring, m_bAcquiredImage;
  HWND          m_hWnd, m_hWndClient;
  HANDLE        m_hBitmap;
  BITMAPFILEHEADER    m_BitmapFileHeader;
  TW_IDENTITY   m_AppIdentity, m_SourceID;  // IDs used to communicate with manager
  HINSTANCE     m_hDll;                     // Handle of Source Manager
  DSMENTRYPROC  m_lpDSM_Entry;              // entry point to the Source Manager
  TW_STATUS     m_twStatus;                 // Status filled in Manager on errors
  CString       m_strSMFilename;            // DLL filename of manager
  UINT          m_nClientStartMessage;      // Clients user message start
};

class CModalTwain : public CTwain
{
	INHERIT(CModalTwain, CTwain)

public:
	CModalTwain();
	virtual ~CModalTwain();

	HANDLE        Acquire(HWND hWnd);        // Start image acquisition.

	// Our overrides.
	virtual BOOL          EnableSource();       // Enable   Data Source
	virtual BOOL          DisableSource();      // Disable  Data Source
  virtual void          DisplayError();       // Display Error Message
};

/////////////////////////////////////////////////////////////////////////////
// Inline Section
inline BOOL CTwain::HaveNewImage()
  {
    BOOL bRetVal = m_bAcquiredImage;
    m_bAcquiredImage = FALSE;
    return bRetVal;
  }
#endif	// End of _CTWAIN_INC
