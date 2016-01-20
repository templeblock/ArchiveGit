#include "stdafx.h"
#include <Exdispid.h>
#include "DWebBrowserEventsImpl.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Error codes returned by NavigateError().
//
// HTTP Status Codes
//
//	Status code string				C++ value Visual Basic value		Description 
//	HTTP_STATUS_BAD_REQUEST			400	400								Invalid syntax. 
//	HTTP_STATUS_DENIED				401 401								Access denied. 
//	HTTP_STATUS_PAYMENT_REQ			402 402								Payment required. 
//	HTTP_STATUS_FORBIDDEN			403 403								Request forbidden. 
//	HTTP_STATUS_NOT_FOUND			404 404								Object not found. 
//	HTTP_STATUS_BAD_METHOD			405 405								Method is not allowed. 
//	HTTP_STATUS_NONE_ACCEPTABLE		406 406								No response acceptable to client found. 
//	HTTP_STATUS_PROXY_AUTH_REQ		407 407								Proxy authentication required. 
//	HTTP_STATUS_REQUEST_TIMEOUT		408 408								Server timed out waiting for request. 
//	HTTP_STATUS_CONFLICT			409 409								User should resubmit with more info. 
//	HTTP_STATUS_GONE				410 410								Resource is no longer available. 
//	HTTP_STATUS_LENGTH_REQUIRED		411 411								Server refused to accept request without a length. 
//	HTTP_STATUS_PRECOND_FAILED		412 412								Precondition given in request failed. 
//	HTTP_STATUS_REQUEST_TOO_LARGE	413 413								Request entity was too large. 
//	HTTP_STATUS_URI_TOO_LONG		414 414								Request Uniform Resource Identifier (URI) too long. 
//	HTTP_STATUS_UNSUPPORTED_MEDIA	415 415								Unsupported media type. 
//	HTTP_STATUS_RETRY_WITH			449 449								Retry after doing the appropriate action. 
//	HTTP_STATUS_SERVER_ERROR		500 500								Internal server error. 
//	HTTP_STATUS_NOT_SUPPORTED		501 501								Server does not support the functionality required to fulfill the request. 
//	HTTP_STATUS_BAD_GATEWAY			502 502								Error response received from gateway. 
//	HTTP_STATUS_SERVICE_UNAVAIL		503 503								Temporarily overloaded. 
//	HTTP_STATUS_GATEWAY_TIMEOUT		504 504								Timed out waiting for gateway. 
//	HTTP_STATUS_VERSION_NOT_SUP		505 505								HTTP version not supported. 
//
// HRESULT Status Codes
//
//	Status code string				C++ value Visual Basic value		Description 
//	INET_E_INVALID_URL				(0x800C0002L) -2,146,697,214		URL string is not valid. 
//	INET_E_NO_SESSION				(0x800C0003L) -2,146,697,213		No session found. 
//	INET_E_CANNOT_CONNECT			(0x800C0004L) -2,146,697,212		Unable to connect to server. 
//	INET_E_RESOURCE_NOT_FOUND		(0x800C0005L) -2,146,697,211		Requested resource is not found. 
//	INET_E_OBJECT_NOT_FOUND			(0x800C0006L) -2,146,697,210		Requested object is not found. 
//	INET_E_DATA_NOT_AVAILABLE		(0x800C0007L) -2,146,697,209		Requested data is not available. 
//	INET_E_DOWNLOAD_FAILURE			(0x800C0008L) -2,146,697,208		Failure occurred during download. 
//	INET_E_AUTHENTICATION_REQUIRED	(0x800C0009L) -2,146,697,207		Requested navigation requires authentication. 
//	INET_E_NO_VALID_MEDIA			(0x800C000AL) -2,146,697,206		Required media not available or valid. 
//	INET_E_CONNECTION_TIMEOUT		(0x800C000BL) -2,146,697,205		Connection timed out. 
//	INET_E_INVALID_REQUEST			(0x800C000CL) -2,146,697,204		Request is invalid. 
//	INET_E_UNKNOWN_PROTOCOL			(0x800C000DL) -2,146,697,203		Protocol is not recognized. 
//	INET_E_SECURITY_PROBLEM			(0x800C000EL) -2,146,697,202		Navigation request has encountered a security issue. 
//	INET_E_CANNOT_LOAD_DATA			(0x800C000FL) -2,146,697,201		Unable to load data from the server. 
//	INET_E_CANNOT_INSTANTIATE_OBJECT(0x800C0010L) -2,146,697,200		Unable to create an instance of the object. 
//	INET_E_REDIRECT_FAILED			(0x800C0014L) -2,146,697,196		Attempt to redirect the navigation failed. 
//	INET_E_REDIRECT_TO_DIR			(0x800C0015L) -2,146,697,195		Navigation redirected to a directory. 
//	INET_E_CANNOT_LOCK_REQUEST		(0x800C0016L) -2,146,697,194		Unable to lock request with the server. 
//	INET_E_USE_EXTEND_BINDING		(0x800C0017L) -2,146,697,193		Reissue request with extended binding. 
//	INET_E_TERMINATED_BIND			(0x800C0018L) -2,146,697,192		Binding is terminated. 
//	INET_E_CODE_DOWNLOAD_DECLINED	(0x800C0100L) -2,146,697,960		Permission to download is declined. 
//	INET_E_RESULT_DISPATCHED		(0x800C0200L) -2,146,696,704		Result is dispatched. 
//	INET_E_CANNOT_REPLACE_SFP_FILE  (0x800C0300L) -2,146,696,448		Cannot replace a protected System File Protection (SFP) file. 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DWebBrowserEventsImpl::DWebBrowserEventsImpl()
{
	m_spWebBrowser = NULL;
	m_lpDocCompleteDisp = NULL;
	m_dwCookie = 0;

}
////////////////////////////////////////////////////////////////////////////////////////

DWebBrowserEventsImpl::~DWebBrowserEventsImpl(void)
{
}
////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP DWebBrowserEventsImpl::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult,
            EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	switch (dispIdMember)
		{
			/////////////////////////////////////////////////////////
			// The parameters for this DISPID are as follows:
			// [0]: Cancel flag  - VT_BYREF|VT_BOOL
			// [1]: IDispatch* - Pointer to an IDispatch interface. 
			//		You can set this parameter to the IDispatch of 
			//		a WebBrowser Control that you've created. When 
			//		you pass back an IDispatch like this, MSHTML will 
			//		use the control you've given it to open the link.
			//
		case DISPID_NEWWINDOW2:
			{
				NewWindow2(pDispParams->rgvarg[1].ppdispVal, pDispParams->rgvarg[0].pboolVal);
			}
			break;

			///////////////////////////////////////////////////////////
			// The parameters for this DISPID are as follows:
			// [0]: Cancel flag  - VT_BYREF|VT_BOOL
			// [1]: HTTP headers - VT_BYREF|VT_VARIANT
			// [2]: Address of HTTP POST data  - VT_BYREF|VT_VARIANT 
			// [3]: Target frame name - VT_BYREF|VT_VARIANT 
			// [4]: Option flags - VT_BYREF|VT_VARIANT
			// [5]: URL to navigate to - VT_BYREF|VT_VARIANT
			// [6]: An object that evaluates to the top-level or frame
			//      WebBrowser object corresponding to the event. 
			// 
			// User clicked a link or launched the browser.
			//
		case DISPID_BEFORENAVIGATE2:
			{
				BeforeNavigate2(pDispParams->rgvarg[6].pdispVal, pDispParams->rgvarg[5].pvarVal, pDispParams->rgvarg[4].pvarVal, 
					pDispParams->rgvarg[3].pvarVal, pDispParams->rgvarg[2].pvarVal, pDispParams->rgvarg[1].pvarVal, pDispParams->rgvarg[0].pboolVal);
			}
			break;
			
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: URL navigated to - VT_BYREF|VT_VARIANT
			// [1]: An object that evaluates to the top-level or frame
			//      WebBrowser object corresponding to the event. 
			//
			// Fires after a navigation to a link is completed on either 
			// a window or frameSet element.
			//
		case DISPID_NAVIGATECOMPLETE2:
			{
				NavigateComplete2(pDispParams->rgvarg[1].pdispVal, pDispParams->rgvarg[0].pvarVal);
				
				// Check if m_lpDocCompleteDisp is NULL. If NULL, that means it is
				// the top level NavigateComplete2. Save the LPDISPATCH
				if (!m_lpDocCompleteDisp)
				{
					VARIANTARG varDisp = pDispParams->rgvarg[1];
					m_lpDocCompleteDisp = varDisp.pdispVal;
				}

			}
			break;
			
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: New status bar text - VT_BSTR
			//
		case DISPID_STATUSTEXTCHANGE:
			{
				StatusTextChange(pDispParams->rgvarg[0].bstrVal);
			}
			break;
			
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: Maximum progress - VT_I4
			// [1]: Amount of total progress - VT_I4
			//
		case DISPID_PROGRESSCHANGE:
			{
				ProgressChange(pDispParams->rgvarg[1].intVal, pDispParams->rgvarg[0].intVal);
			}
			break;

			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: URL navigated to - VT_BYREF|VT_VARIANT
			// [1]: An object that evaluates to the top-level or frame
			//      WebBrowser object corresponding to the event. 
			//
			// Fires when a document has been completely loaded and initialized.
			// Unreliable -- currently, the DWebBrowserEvents2::DocumentComplete 
			// does not fire when the IWebBrowser2::Visible property of the 
			// WebBrowser Control is set to false (see Q259935).  Also, multiple 
			// DISPID_DOCUMENTCOMPLETE events can be fired before the final 
			// READYSTATE_COMPLETE (see Q180366).
			//
		case DISPID_DOCUMENTCOMPLETE:
			{
				DocumentComplete(pDispParams->rgvarg[1].pdispVal, pDispParams->rgvarg[0].pvarVal);
				
				VARIANTARG varDisp = pDispParams->rgvarg[1];
				if (m_lpDocCompleteDisp && m_lpDocCompleteDisp == varDisp.pdispVal)
				{
					// if the LPDISPATCH are same, that means
					// it is the final DocumentComplete. Reset m_lpDocCompleteDisp
					m_lpDocCompleteDisp = NULL;

					// Handle new doc.
					DocumentReallyComplete(pDispParams->rgvarg[1].pdispVal, pDispParams->rgvarg[0].pvarVal);
				}

			}
			break;
			
			///////////////////////////////////////////////////////////
			// No parameters
			//
			// Fires when a navigation operation is beginning.
			//
		case DISPID_DOWNLOADBEGIN:
			DownloadBegin();
			break;
			
			///////////////////////////////////////////////////////////
			// No parameters
			//
			// Fires when a navigation operation finishes, is halted, or fails.
			//
		case DISPID_DOWNLOADCOMPLETE:
			DownloadComplete();
			break;
			
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: Enabled state - VT_BOOL
			// [1]: Command identifier - VT_I4
			//
		case DISPID_COMMANDSTATECHANGE:
			{
				CommandStateChange(pDispParams->rgvarg[1].intVal, pDispParams->rgvarg[0].boolVal);
			}
			break;
			
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: Document title - VT_BSTR
			// [1]: An object that evaluates to the top-level or frame
			//      WebBrowser object corresponding to the event.
			//
		case DISPID_TITLECHANGE:
			{
				TitleChange(pDispParams->rgvarg[0].bstrVal);
			}
			break;
			
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: Name of property that changed - VT_BSTR
			//
		case DISPID_PROPERTYCHANGE:
			{
				PropertyChange(pDispParams->rgvarg[0].bstrVal);
			}
			break;
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: Cancel flag  - VT_BYREF|VT_BOOL
			// [1]: Child Window (created from script) - VT_BOOL
			//      
			//
		case DISPID_WINDOWCLOSING:
			{
				WindowClosing(pDispParams->rgvarg[1].boolVal, pDispParams->rgvarg[0].pboolVal);
			}
			break;

			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: Cancel flag  - VT_BYREF|VT_BOOL
			//      
			//
		case DISPID_FILEDOWNLOAD:
			{
				FileDownload(pDispParams->rgvarg[0].pboolVal);
			}
			break;
			
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID are as follows:
			// [0]: Cancel flag			- VT_BYREF|VT_BOOL
			// [1]: Error status code	- VT_BYREF|VT_VARIANT  
			// [2]: Target frame name	- VT_BYREF|VT_VARIANT 
			// [3]: URL where navigate failed - VT_BYREF|VT_VARIANT
			// [4]: An object that evaluates to the top-level or frame
			//      WebBrowser object corresponding to the event. 
			// 
			//
		case DISPID_NAVIGATEERROR:
			{
				NavigateError(pDispParams->rgvarg[4].pdispVal, pDispParams->rgvarg[3].pvarVal, 
					pDispParams->rgvarg[2].pvarVal, pDispParams->rgvarg[1].pvarVal, pDispParams->rgvarg[0].pboolVal);

				m_lpDocCompleteDisp = NULL;
			}
			break;
			///////////////////////////////////////////////////////////
			// The parameters for this DISPID:
			// [0]: Visible  - VT_BOOL
			//
		case DISPID_ONVISIBLE:
			{
				OnVisible(pDispParams->rgvarg[0].boolVal);
			}
			break;

			///////////////////////////////////////////////////////////
			// No parameters
			//
			// The BHO docs in MSDN say to use DISPID_QUIT, but this is an error.
			// The BHO never gets a DISPID_QUIT and thus the browser connection
			// never gets unadvised and so the BHO never gets the FinalRelease().
			// This is bad.  So use DISPID_ONQUIT instead and everything is cool --
			// EXCEPT when you exit the browser when viewing a page that launches
			// a popup in the onunload event!  In that case the BHO is already
			// unadvised so it does not intercept the popup.  So the trick is to
			// navigate to a known page (I used the about:blank page) that does 
			// not have a popup in the onunload event before unadvising.
			//
		case DISPID_ONQUIT:
			OnQuit();
			break;

		default:
			{
			}
			break;
	}

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT DWebBrowserEventsImpl::Connect(IWebBrowser2 * pWB)
{
	HRESULT hr = S_OK;
	m_spWebBrowser = pWB;
	if (!m_spWebBrowser)
		return hr;

	hr = AtlAdvise(m_spWebBrowser, (IDispatch*)this, __uuidof(DWebBrowserEvents2), &m_dwCookie);
	return hr; 
}
/////////////////////////////////////////////////////////////////////////////
HRESULT DWebBrowserEventsImpl::Disconnect()
{
	HRESULT hr = S_OK;
	if (!m_spWebBrowser)
		return hr;

	hr = AtlUnadvise(m_spWebBrowser, __uuidof(DWebBrowserEvents2), m_dwCookie);
	m_dwCookie = 0;
	return hr; 
}