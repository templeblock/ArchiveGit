#pragma once
#include "webmailbase.h"

static const DWORD YAHOO_VERSION_01	= 0x0001;
static const DWORD YAHOO_VERSION_02	= 0x0002;

class CYahooMail :
	public CWebMailBase
{
public:
	CYahooMail(CJMExplorerBand * pExplorerBar);
	virtual ~CYahooMail(void);
	virtual bool IsSendEvent(IHTMLDocument2 *pDoc);
	virtual bool IsComposeView();

protected:
	virtual BOOL WriteHtmlToDocument(BSTR bstrHtml, IHTMLDocument2Ptr spDoc=NULL);
	virtual bool GetComposeInterfaces(bool bSilent);
	virtual bool IsNewComposeMessage();
};
