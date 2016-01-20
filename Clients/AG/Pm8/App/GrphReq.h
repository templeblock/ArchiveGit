class CBasicRequestInfo;
class CDownloadStatusDialog;
class StorageFile;
class CCompressInfo;
class CBindStatusCallback;
class CPGSSession;

/////////////////////////////////////////////////////////////////////////////
// CGetGraphicRequest command target

class CGetGraphicRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetGraphicRequest)

	CGetGraphicRequest();           // protected constructor used by dynamic creation
	CGetGraphicRequest(LPCSTR strFile, LPCSTR strLoginID, LPCSTR strPasswordID, LPCSTR strOutBuffer);   

// Attributes
public:

protected:
	CString m_strFile;
	CString m_strPasswordID;
	CString m_strLoginID;
	CString m_strOutBuffer;

	BOOL    m_fLastAttempt;
	BOOL		m_fGraphicIsAdvertisement;

	// Operations
public:
	LPWSTR m_url;

	 enum
	 {
		COMPRESSION_None			= 0,
		COMPRESSION_JPEG			= 1,
		COMPRESSION_Deflate			= 2,
		COMPRESSION_ReducedDeflate	= 3		// Deflate with 256-color palette.
	 };

// Implementation
public:
	virtual ~CGetGraphicRequest();
	virtual void ProcessRequest(CString& pString);
	virtual UINT DoRequest(void);

	BOOL DecompressFile(CCompressInfo*, void* pFile, void* bufOut);
	BOOL CheckForBadHeader(CCompressInfo* pCompressInfo);

	void CreateStatusDialog(void);
	void DestroyStatusDialog(void);
	void ShowStatusDialog(void);
	void SetStatusProgress(int nCurrent, int nTotal);

	HRESULT DoDownload(CPGSSession* pSession, CHttpFile* pFile);

	const BOOL GetLastAttempt (void)
	{  return m_fLastAttempt; }
	void SetLastAttempt (const BOOL fAttempt)
	{  m_fLastAttempt = fAttempt; }

	const BOOL GetGraphicIsAdvertisement (void)
	{  return m_fGraphicIsAdvertisement; }
	void SetGraphicIsAdvertisement (const BOOL fAd)
	{  m_fGraphicIsAdvertisement = fAd; }

	//DECLARE_MESSAGE_MAP()
	CDownloadStatusDialog* m_pStatusDialog;
};

/////////////////////////////////////////////////////////////////////////////

