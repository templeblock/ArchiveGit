class CProgressStatusDlg;

class CBindStatusCallback : public IBindStatusCallback{
  public:
    // IUnknown methods
    STDMETHODIMP    QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef()    { return m_cRef++; }
    STDMETHODIMP_(ULONG)    Release()   { if (--m_cRef == 0) { delete this; return 0; } return m_cRef; }

    // IBindStatusCallback methods
    STDMETHODIMP    OnStartBinding(DWORD dwReserved, IBinding* pbinding);
    STDMETHODIMP    GetPriority(LONG* pnPriority);
    STDMETHODIMP    OnLowResource(DWORD dwReserved);
    STDMETHODIMP    OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode,
                        LPCWSTR pwzStatusText);
    STDMETHODIMP    OnStopBinding(HRESULT hrResult, LPCWSTR szError);
    STDMETHODIMP    GetBindInfo(DWORD* pgrfBINDF, BINDINFO* pbindinfo);
    STDMETHODIMP    OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pfmtetc,
                        STGMEDIUM* pstgmed);
    STDMETHODIMP    OnObjectAvailable(REFIID riid, IUnknown* punk);

			// constructors/destructors
    CBindStatusCallback();
    //CBindStatusCallback(HWND hwndStatus, HWND hwndProgress, HWND hwndText, HWND hwndProgressBar);
    ~CBindStatusCallback();

    void SetWndText(CWnd* pWnd, LPCWSTR szText);

	void SetProgress(LPCWSTR szProgress);
	void SetStatus(LPCWSTR szProgress);

	void SetProgressBar(ULONG cProgress, ULONG maxProgress); 
	void CreateProgressControl(CProgressStatusDlg *pProgressDialog);

    // data members
    DWORD           m_cRef;
    IBinding*       m_pbinding;
    IStream*        m_pstm;
    CWnd*           m_pWndStatus;
    HWND            m_hwndProgress;
    HWND            m_hwndText;
	HWND			m_hwndProgressBar;
    DWORD           m_cbOld;
	BOOL		    m_fProgressShown;
	BOOL			m_fShowProgress;
	DWORD           m_dwStartTime;
	ULONG			m_ulStartTime;
	ULONG			m_ulProgressLast;
	ULONG			m_ulTimeLast;
	ULONG			m_ulTimeLeftLast;

    CProgressCtrl * m_pProgressBar;
	CProgressStatusDlg* m_pProgressDialog;

};

