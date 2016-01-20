	// IPersistFile
	STDMETHOD(Load)(LPCOLESTR wszFile, DWORD dwMode);
	STDMETHOD(GetClassID)(LPCLSID clsid)
	{ return E_NOTIMPL; }
	STDMETHOD(IsDirty)(VOID)
	{ return E_NOTIMPL; }
	STDMETHOD(Save)(LPCOLESTR, BOOL)
	{ return E_NOTIMPL; }
	STDMETHOD(SaveCompleted)(LPCOLESTR)
	{ return E_NOTIMPL; }
	STDMETHOD(GetCurFile)(LPOLESTR FAR*)
	{ return E_NOTIMPL; }

	// IExtractImage2
	STDMETHOD(GetDateStamp)(FILETIME *pDateStamp);

	// IExtractImage
	STDMETHOD(GetLocation)(LPWSTR pszPathBuffer, DWORD cchMax, DWORD *pdwPriority, const SIZE *prgSize, DWORD dwRecClrDepth, DWORD *pdwFlags);
	STDMETHOD(Extract)(HBITMAP* phBmpThumbnail);

private:
	SIZE m_ExtractImageSize;
	CString m_strPersistFileName;
