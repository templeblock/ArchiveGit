#pragma once
#include "webmailbase.h"

class CJMExplorerBand;

class CHotmail :
	public CWebMailBase
{
public:
	CHotmail(CJMExplorerBand * pExplorerBar);
	virtual ~CHotmail(void);
	virtual bool IsSendEvent(IHTMLDocument2 *pDoc);
	

protected:
	virtual BOOL WriteHtmlToDocument(BSTR bstrHtml, IHTMLDocument2Ptr spDoc=NULL);
	virtual bool GetComposeInterfaces(bool bSilent);
	virtual bool IsNewComposeMessage();
	virtual bool IsComposeView(){return true;};
};
