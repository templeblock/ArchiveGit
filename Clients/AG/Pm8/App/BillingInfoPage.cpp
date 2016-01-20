// BillingInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "BillingInfoPage.h"
#include "AddrInfoPage.h"
#include "blowfish.h"
#include "progbar.h"
#include "stdio.h"
#include "cdeflate.h"
#include "mainfrm.h"
#include "util.h"
#include "inifile.h"
#include "credit.h"
#include "pmwcfg.h"
#include "rmagcom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString	CBillingInfoPage::m_csCardHolderName;
int CBillingInfoPage::m_nCreditCardType;
CString	CBillingInfoPage::m_csPhoneNum;
CString	CBillingInfoPage::m_csCardNumber;

extern CPalette* pOurPal;  // PrintMaster Palette

/////////////////////////////////////////////////////////////////////////////
// CBillingInfoPage property page

IMPLEMENT_DYNCREATE(CBillingInfoPage, CPropertyPage)

CBillingInfoPage::CBillingInfoPage(CString csFilePath, PROJECT_TYPE CardType) : 
	CPropertyPage(CBillingInfoPage::IDD),
	m_csDocFilePath(csFilePath), 
	m_CardType(CardType)
{
	//{{AFX_DATA_INIT(CBillingInfoPage)
	m_csBillingName = _T("");
	m_csBillingAddr = _T("");
	m_csCardHolderName = _T("");
	m_nCreditCardType = 0;
	m_csPhoneNum = _T("");
	m_csCardNumber = _T("");
	m_dPostage = 0.0;
	m_dPrice = 0.0;
	m_dTax = 0.0;
	m_dTotal = 0.0;
	m_dShipping = 0.0;
	m_dHandling = 0.0;
	//}}AFX_DATA_INIT
}

CBillingInfoPage::CBillingInfoPage() : 
	CPropertyPage(CBillingInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CBillingInfoPage)
	m_csBillingName = _T("");
	m_csBillingAddr = _T("");
	m_csCardHolderName = _T("");
	m_nCreditCardType = 0;
	m_csPhoneNum = _T("");
	m_csCardNumber = _T("");
	m_dPostage = 0.0;
	m_dPrice = 0.0;
	m_dTax = 0.0;
	m_dTotal = 0.0;
	//}}AFX_DATA_INIT
}

CBillingInfoPage::~CBillingInfoPage()
{
}

void CBillingInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBillingInfoPage)
	DDX_Control(pDX, IDC_STATIC_CHARACTER, m_cImage);
	DDX_Control(pDX, IDC_CARD_NUM, m_cCardNum);
	DDX_Control(pDX, IDC_STATIC_TOTAL, m_cStaticTotal);
	DDX_Control(pDX, IDC_STATIC_TAX, m_cStaticTax);
	DDX_Control(pDX, IDC_STATIC_PRICE, m_cStaticPrice);
	DDX_Control(pDX, IDC_STATIC_POSTAGE, m_cStaticPostage);
	DDX_Control(pDX, IDC_EXP_DATE, m_cExpDate);
	DDX_Radio(pDX, IDC_CREDITCARD_TYPE, m_nCreditCardType);
	DDX_Price(pDX, IDC_STATIC_POSTAGE, m_dPostage);
	DDX_Price(pDX, IDC_STATIC_PRICE, m_dPrice);
	DDX_Price(pDX, IDC_STATIC_TAX, m_dTax);
	DDX_Price(pDX, IDC_STATIC_TOTAL, m_dTotal);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_BILLING_NAME, m_csBillingName);
	DDV_Empty(pDX, IDC_BILLING_NAME, m_csBillingName);

	DDX_Text(pDX, IDC_BILLING_ADDR, m_csBillingAddr);
	DDV_Empty(pDX, IDC_BILLING_ADDR, m_csBillingAddr);

	DDX_Text(pDX, IDC_CARD_NAME, m_csCardHolderName);
	DDV_Empty(pDX, IDC_CARD_NAME, m_csCardHolderName);

	DDX_Text(pDX, IDC_PHONE, m_csPhoneNum);
	DDV_Empty(pDX, IDC_PHONE, m_csPhoneNum);

	DDX_Text(pDX, IDC_CARD_NUM, m_csCardNumber);
	DDV_CardNum(pDX, m_nCreditCardType, m_csCardNumber);
	
	DDX_Date(pDX, IDC_EXP_DATE, m_csExpDate);
}


BEGIN_MESSAGE_MAP(CBillingInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBillingInfoPage)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBillingInfoPage message handlers

BOOL CBillingInfoPage::OnEraseBkgnd(CDC* pDC) 
{
	GetParent()->SendMessage(WM_ERASEBKGND, (WPARAM)pDC->GetSafeHdc());
	// title...
	if ((HBITMAP)m_bmpTitle)
	{
		BITMAP BitmapInfo;
		m_bmpTitle.GetBitmap(&BitmapInfo);
		CRect rcTitle(0,0, BitmapInfo.bmWidth, BitmapInfo.bmHeight);
		// get position from static control in dialog
		CWnd* pWnd = GetDlgItem(IDC_TITLE2);
		if (pWnd)
		{
			CRect rcClient;
			pWnd->GetClientRect(&rcClient);
			if(rcClient != rcTitle)
			{
				pWnd->SetWindowText("");
				pWnd->SetWindowPos(&wndTop, rcTitle.left, rcTitle.right, 
							rcTitle.Width(), rcTitle.Height(), SWP_NOZORDER); 
			}
		}
		// Use our global PrintMaster palette.
		CPalette* pOldPalette = pDC->SelectPalette(pOurPal, FALSE);
		pDC->RealizePalette();
		Util::TransparentBltBitmap(*pDC, rcTitle, m_bmpTitle, NULL, NULL, 0, 0);
		// Restore the previous DC settings.
		if (pOldPalette)
			pDC->SelectPalette(pOldPalette, FALSE);
	}
   	return TRUE;
}

HBRUSH CBillingInfoPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = NULL;
	
	if(nCtlColor == CTLCOLOR_STATIC || 
		(nCtlColor == CTLCOLOR_BTN && pWnd->GetStyle() & BS_RADIOBUTTON))
	{
        // set background color, text mode, and text font
        //
        pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
        pDC->SetBkMode(TRANSPARENT);


        hbr = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	}
	else
		hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

void CBillingInfoPage::DDX_Date(CDataExchange* pDX, int nIDC, CString& csExpDate)
{
	if(pDX->m_bSaveAndValidate )
	{
		CTimeStamp ts = m_cExpDate.GetSetTime();
		SYSTEMTIME sysTime;
		FILETIME ft = ts.GetTime();
		FileTimeToSystemTime(&ft, &sysTime);
		csExpDate.Format("%.2d%.4d", sysTime.wMonth, sysTime.wYear);	
	}
}

void CBillingInfoPage::DDX_Price(CDataExchange* pDX, int nIDC, double& dPrice)
{
	if(pDX->m_bSaveAndValidate)
	{
	
	}
	else
	{
		if(nIDC == IDC_STATIC_TAX)
		{
			
		}
		CString csWindowText;
		csWindowText.Format("$ %.2f", dPrice);
		GetDlgItem(nIDC)->SetWindowText(csWindowText);
	}
}

void CBillingInfoPage::DDV_CardNum(CDataExchange* pDX, int nCardType, CString csCardNumber)
{
	if(pDX->m_bSaveAndValidate )
	{
		BOOL bValid = CCreditCard::NumberValid(nCardType, csCardNumber);

		if(!bValid)
		{
			AfxMessageBox(IDS_INVALID_CC_NUMBER);
			pDX->Fail();
		}

	}
}
void CBillingInfoPage::DDV_Empty(CDataExchange* pDX, int nIDC, CString csText )
{
	if(pDX->m_bSaveAndValidate )
	{
		if(csText.GetLength() == 0)
		{	
			UINT nID;
			switch(nIDC)
			{
				case IDC_BILLING_NAME:
					nID = IDS_MISSING_BILL_NAME;
					break;
				case IDC_BILLING_ADDR:
					nID = IDS_MISSING_BILL_ADDR;
					break;
				case IDC_CARD_NAME:
					nID = IDS_MISSING_CARD_NAME;
					break;
				case IDC_PHONE:
					nID = IDS_MISSING_PHONE;
					break;
				default:
					nID = IDS_REQUIRED_FIELD;
					break;


			}

			AfxMessageBox(nID);
			pDX->Fail();	
		}
	}
	
}


CString CBillingInfoPage::GetFileEntry(FILE* stream, int nIndex, char cTag, CString csData)
{
	CString csResult;
	csResult.Empty();

	PriceFileEntry Entry;
	char line[81];
	fseek( stream, 0, SEEK_SET); 

	while(fgets( line, 80, stream ))
	{
		if(*line == cTag)
		{
			//separate the line into separate strings
			char* pToken = strtok(line, "\t");
			while(pToken != NULL)
			{
				Entry.Add(pToken);
				pToken = strtok(NULL, "\t");
			}

			if(Entry.GetSize() != 0)
			{
				//Is there a product number to check against
				if(csData.IsEmpty() || csData == Entry.GetAt(1))
					break;
				//not the right line so empty array
				Entry.RemoveAll();
			}
		}
	}
	if(nIndex < Entry.GetSize())
		csResult = Entry[nIndex];

	return csResult;
}

UINT CBillingInfoPage::UpdatePriceData(CString csPriceFilePath, BOOL bPrompt/* = FALSE*/)
{
	FILE *stream;
	stream = fopen( csPriceFilePath, "r" );
	if(stream == NULL)
	{
		ASSERT(0);
		AfxMessageBox(IDS_PRICE_FILE_ERR);
		return IDCANCEL;
	}

	CString csTime = GetFileEntry(stream, 1, 'T');
	//If the time is not empty(first time) and it is not the same(ie ftp file is diff
	//than local file) we want to put up a prompt otherwise the price is fine just return
	if(!m_csPriceFileTime.IsEmpty())
	{
		if( m_csPriceFileTime == csTime)
		{
			fclose( stream );
			return IDOK;
		}
	}

	m_csPriceFileTime = csTime;

	//get the price
	m_dPrice = atof(GetFileEntry(stream, 2, 'P', m_csProductNum));

	//get the shipping
	m_dShipping = atof(GetFileEntry(stream, 2, 'S', m_csProductNum));

	CPropertySheet* pPropSheet = (CPropertySheet*)GetParent();
	CAddrInfoPage* pAddressInfo = (CAddrInfoPage*)pPropSheet->GetPage(0);
	//get the tax
	CString csTax = GetFileEntry(stream, 2, 'X', pAddressInfo->m_csState);
	if(csTax.IsEmpty() == FALSE )
		m_dTax = atof(csTax) * m_dPrice;
	else
		m_dTax = 0.0;
	
	//get the handling
	if(pAddressInfo->m_bInternational)
	{
		m_dHandling = atof(GetFileEntry(stream, 1, 'H'));
	}
	m_dPostage = m_dShipping + m_dHandling;
	m_dTotal = m_dPrice + m_dPostage + m_dTax;

	fclose( stream );
	UpdateData(FALSE);
	if(bPrompt)
	{
		return AfxMessageBox(IDS_UPDATED_PRICE, MB_OKCANCEL);
	}
	
	return IDOK;

}

BOOL CBillingInfoPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if(m_csDocFilePath.GetLength() < 3)
		return FALSE;

	Util::LoadResourceBitmap(m_bmpTitle, MAKEINTRESOURCE(IDB_BILL_INFO_TITLE), pOurPal);

	//added brackets so the resource loader goes out of scope when done.
	{
		CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());
		CBitmap bmCharacter;
		if(rlProductSpecific.LoadBitmap(bmCharacter, CSharedIDResourceManager::riCharacterMed, pOurPal))
		{
			m_cImage.SetBitmap(bmCharacter);
			bmCharacter.Detach();
		}
		else
			m_cImage.ShowWindow(SW_HIDE);
	}

	m_cExpDate.SetWindowText("");
	m_cExpDate.Init(FALSE);

	//get the ini file path
	CString csIniPath = GetGlobalPathManager()->ExpandPath("AgRemote.ini");
	CIniFile iniFile(csIniPath);

	//get the product number
	if(m_CardType == PROJECT_TYPE_HalfCard)
	{
		m_csProductNum = iniFile.GetString("Product Number", "tabloid", "2005747");
	}
	else if(m_CardType == PROJECT_TYPE_Card)
	{
		m_csProductNum = iniFile.GetString("Product Number", "letter", "2005748");
	} 
	else 
		return FALSE;

	CString csPriceFile;
	csPriceFile.LoadString(IDS_PRICE_FILE);
	CString csPricePath = GetGlobalPathManager()->ExpandPath(csPriceFile);
	if(UpdatePriceData(csPricePath) == IDCANCEL)
		return FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CBillingInfoPage::OnKillActive() 
{
	//OnKillActive normally does an update data and returns that result, but
	//we don't want to do that because we don't want the validate to happen.
	return 1;
}

BOOL CBillingInfoPage::OnSetActive() 
{
	BOOL bResult = CPropertyPage::OnSetActive();
	if(bResult)
	{
		((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH );	
		CString csPriceFile;
		csPriceFile.LoadString(IDS_PRICE_FILE);
		CString csPricePath = GetGlobalPathManager()->ExpandPath(csPriceFile);
		//reset the price file time to 0 so UpdatePriceFile won't just return becasue
		//the time is changed
		m_csPriceFileTime = "";
		UpdatePriceData(csPricePath);
	}
		
	return bResult;
}

CString CBillingInfoPage::AddTagsToAddress(CString csAddress, UINT nTagStart, UINT nTagEnd)
{
	CString csResult;
	CString csTag;
	char delimit[] = "\n";
	char address[1024];
	strcpy(address, csAddress);

	char* token;
	token = strtok( address, delimit ); 
	while (token != NULL && nTagStart <= nTagEnd)
	{
		csTag.LoadString(nTagStart++);
		//each line is limited to 65 characters
		if(strlen(token) > 65)
			token[66] = '\0';

		csResult += csTag + token;
		csResult += "\r\n";
		token = strtok(NULL, delimit);
	}
	//get rid of the last newline character.
	csResult = csResult.Left(csResult.GetLength() - 1);
	return csResult;
}

BOOL CBillingInfoPage::OnWizardFinish() 
{
	if(!UpdateData(TRUE))
		return FALSE;

	BOOL bCreated = m_progressDialog.Create(IDD_PROGRESS_BAR,  this);
	if(!bCreated)
	{
		AfxMessageBox(IDS_PROGRESS_CREATE_ERROR);
		return FALSE;
	}


	if(m_csDocFilePath.IsEmpty())
	{
		ASSERT(0);
		AfxMessageBox(IDS_NO_FILE_NAME);
		m_progressDialog.DestroyWindow();
		return FALSE;
	}
	//get pointer to address info page
	CPropertySheet* pPropSheet = (CPropertySheet*)GetParent();
	CAddrInfoPage* pAddressInfo = (CAddrInfoPage*)pPropSheet->GetPage(0);

	//TODO: Get this number from somewhere
	CString csSoftwareRegNum;

	CString csVersion = GET_PMWAPP()->GetVersion();;
	//this is actually the encrypt key but I don't want to put it right 
	//next to where it is used;
	unsigned char szTag[] = "3F7CWT43";
	
	CString csChannel;
	csChannel.LoadString(IDS_SALES_CHANNEL);

	CString csBillingAddr = AddTagsToAddress(m_csBillingAddr, IDS_BILL_ADDR_TAG1, IDS_BILL_ADDR_TAG3);
	CString csDestAddress = AddTagsToAddress(pAddressInfo->m_csAddress, IDS_DEST_ADDR_TAG1, IDS_DEST_ADDR_TAG3);
	CString csReturnAddress = AddTagsToAddress(pAddressInfo->m_csReturnAddress, IDS_SENDER_ADDR_TAG1, IDS_SENDER_ADDR_TAG5);

	CString csCurDate;
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	csCurDate.Format("%.2d%.2d%.4d", sysTime.wMonth, sysTime.wDay, sysTime.wYear);	

	CString csState;
	CString csCountry;
	CString csCardType = GetCreditCardType(m_nCreditCardType);

	if(pAddressInfo->m_bInternational)
		csCountry = pAddressInfo->m_csCountry;
	else
		csState = pAddressInfo->m_csState;

	char csCombinedData[4096];
	sprintf(csCombinedData, "REGN %s\r\nSVER %s\r\nSCNL %s\r\nBNAM %s\r\n%s\r\nBPHO %s\r\nCTYP %s\r\nCNAM %s\r\nCNBR %s\r\nCEXP %s\r\nODAT %s\r\nPNBR %s\r\nPPRC %2f\r\nPTAX %2f\r\nPSHH %2f\r\nPPSG %2f\r\nPTDD %s\r\nRNAM %s\r\n%s\r\nRCTY %s\r\nRSTA %s\r\nRZIP %s\r\nRCOU%s\r\n%s\r\nEOOR", 
				csSoftwareRegNum,
				csVersion,
				csChannel,
				m_csBillingName, 
				csBillingAddr, 
				m_csPhoneNum, 
				csCardType, 
				m_csCardHolderName, 
				m_csCardNumber,
				m_csExpDate,
				csCurDate,
				m_csProductNum,
				m_dPrice,
				m_dTax,
				m_dHandling,
				m_dShipping,
				pAddressInfo->m_csDate, 
				pAddressInfo->m_csName, 
				csDestAddress, 
				pAddressInfo->m_csCity,
				csState,
				pAddressInfo->m_csZip, 
				csCountry, 
				csReturnAddress
				);

	CBlowfishEncrypt bf(szTag, strlen((char*)szTag));

	int nDataLength = strlen(csCombinedData);

	//create the combined order info and data file
	TRY
	{
		CString csCombinedFileName = GetGlobalPathManager()->ExpandPath("[[U]]\\temp.cmb");
		CFile combinedFile( csCombinedFileName, CFile::modeCreate | CFile::modeReadWrite );

		//write the size of the order info first;  Since this is encrypted it has to be 8 byte aligned
		int nEncryptPackets = nDataLength / 8;
		if(nDataLength % 8)
			nEncryptPackets++;


		int nBytesToWrite = nEncryptPackets * 8;

		combinedFile.Write(&nBytesToWrite, sizeof(int));

		//write the data
		char pszEncrypt[8];
		char* pTempData = csCombinedData;

		for(int i = 1; i <= nDataLength / 8; i ++)
		{
			memcpy(pszEncrypt, pTempData, 8);
			//encrypt the data and write it to the file
			bf.ProcessBlock((unsigned long*)(pszEncrypt + 4), (unsigned long*)pszEncrypt);
			combinedFile.Write(pszEncrypt, 8);
			pTempData += 8;
		}
		
		if(nDataLength % 8)
		{
			//read the remainder of the data
			strcpy(pszEncrypt, pTempData);
			//encrypt the data and write it to the file.  Even though there is less than 8 bytes 
			//of data the encryption requires 8 bytes to work so we are going to pad it with garbage
			bf.ProcessBlock((unsigned long*)(pszEncrypt + 4), (unsigned long*)pszEncrypt);
			combinedFile.Write(pszEncrypt, 8);
		}

		CFile dataFile( m_csDocFilePath, CFile::modeRead );
		//write out the file ext
		combinedFile.Write(m_csDocFilePath.Right(3), 3);

		int nFileLength = dataFile.GetLength();
		//write out the original file size
		combinedFile.Write(&nFileLength, sizeof(int));

		CString csProgText;
		csProgText.LoadString(IDS_COMPRESSING_DATA);
		m_progressDialog.SetWindowText(csProgText);
		//read in the file data
		BYTE* pData = new BYTE[nFileLength];
		int nBytesRead = dataFile.Read(pData, nFileLength);
		CDeflate deflate;
		BYTE* pCompressedData;
		DWORD compressedSize;
		deflate.Compress(pData, nBytesRead, &pCompressedData, &compressedSize);

		//write it to the combined file
		combinedFile.Write(pCompressedData, compressedSize);
		deflate.FreeData(pCompressedData);

		delete pData;
		//close the files
		CString csFilePath = combinedFile.GetFilePath();
		dataFile.Close();
		combinedFile.Close();

		UINT nResult = DoFtpExchange(csFilePath);
		CFile::Remove(csCombinedFileName);
		
		EndWaitCursor();
		if(m_progressDialog.GetSafeHwnd() != NULL)
			m_progressDialog.DestroyWindow();
 		
		if(nResult != SUCCESS )
		{
			AfxMessageBox(IDS_UNSUCCESSFUL_DELIVERY);
			return FALSE;
		}
		else
			AfxMessageBox(IDS_SUCCESSFUL_DELIVERY);
	}
	CATCH(CFileException,e)
	{
		CString csError;
		LoadConfigurationString(IDS_FILE_ERROR, csError);
		CString csErrorOut;
		csErrorOut.Format("%s %s.", csError, e->m_strFileName);
		AfxMessageBox(csErrorOut);
		m_progressDialog.DestroyWindow();
		return FALSE;
	}
	AND_CATCH (CMemoryException, e)
	{
		AfxMessageBox(IDS_UNSUCCESSFUL_DELIVERY);
		return FALSE;
	}
	END_CATCH

	return CPropertyPage::OnWizardFinish();
}


CString CBillingInfoPage::GetCreditCardType(UINT nCreditCardType)
{
	CString csResult;
	switch(nCreditCardType)
	{
		case CCreditCard::MASTERCARD:
		{
			csResult = "MASTERCARD";
			break;
		}
		case CCreditCard::VISA:
		{
			csResult = "VISA";
			break;
		}
		case CCreditCard::AMERICANEXPRESS:
		{
			csResult = "AMERICANEXPRESS";
			break;
		}
		case CCreditCard::DISCOVER:
		{
			csResult = "DISCOVER";
			break;
		}
		default:
		{
			ASSERT(0);
			break;
		}
	}
	return csResult;
}

UINT CBillingInfoPage::DoFtpExchange(CString csLocalFile)
{
#ifdef ARTSTORE
	return ERR_NO_CONNECTION;
#else
	ASSERT(m_progressDialog.GetSafeHwnd() != NULL);
	CString csProgText;
	csProgText.LoadString(IDS_CONNECT_WAITING);
	m_progressDialog.SetWindowText(csProgText);

	IConnectionManager* pConnectionManager = GetGlobalConnectionManager();
	if (pConnectionManager == NULL
		 || pConnectionManager->Connect() != CInternetConnectStatus::statusConnected)
	{
		return ERR_NO_CONNECTION;
	}

	CInternetSession InetSession(NULL, 1, PRE_CONFIG_INTERNET_ACCESS);
	CFtpConnection* pFtpConnection = NULL;	
	UINT nResult = OPERATION_INCOMPLETE;
	
	CString csFtpFile;
	CString csFtpSite;
	csFtpSite.LoadString(IDS_AG_FTP_SITE);
	//size of the chunks we read at write to/from the ftp file
	int nDataSize = 256;
	//pointer to data that will be read from the file
	BYTE* pData = new BYTE[nDataSize];

	try
	{
		pFtpConnection = GetFtpConnection(&InetSession, csFtpSite, "mindprice", "corky49");
		ASSERT(pFtpConnection);
		if(pFtpConnection == NULL)
		{
			delete pData;
			return URL_ERR;
		}

		CString csFtpDir;
		csFtpDir.LoadString(IDS_FTP_PRICE_DIR);
		//we have to set it to the root first
		pFtpConnection->SetCurrentDirectory(csFtpDir);

		//create the local price file name
		CString csPriceFileName;
		csPriceFileName.LoadString(IDS_AG_PRICE_FILE);
		char tempPath[MAX_PATH];
		GetModuleFileName(NULL, tempPath, MAX_PATH);
		CString csPriceFileLocal(tempPath);
		csPriceFileLocal = csPriceFileLocal.Left(csPriceFileLocal.ReverseFind('\\') + 1) + "TempPrice.txt";

		csProgText.LoadString(IDS_GETTING_PRICE_FILE);
		m_progressDialog.SetWindowText(csProgText);

		if(pFtpConnection->GetFile(csPriceFileName, csPriceFileLocal, FALSE))
		{
			if(UpdatePriceData(csPriceFileLocal, TRUE) == IDOK)
			{
				csProgText.LoadString(IDS_TRANSFERRING_DATA);
				m_progressDialog.SetWindowText(csProgText);
				//close the ftp connection.  The ftp site for the output file is
				//different
				pFtpConnection->Close();
				delete pFtpConnection;
				//get new connection for output file
				pFtpConnection = GetFtpConnection(&InetSession, csFtpSite, "mindorder", "dodie73");
				ASSERT(pFtpConnection);
				if(pFtpConnection == NULL)
				{
					delete pData;
					return URL_ERR;
				}
				csFtpDir.LoadString(IDS_FTP_ORDER_DIR);
				//we have to set it to the root first
				pFtpConnection->SetCurrentDirectory(csFtpDir);
				
				//create ftp file name from the time
				csFtpFile = GetFtpFileName();
				CFtpFileFind ftpFF(pFtpConnection);;
				//if the file exists keep trying until it doesn't
				while(TRUE)
				{
					BOOL bFailed = FALSE;
					if(ftpFF.FindFile(csFtpFile))
					{	
						ftpFF.FindNextFile();
						if(ftpFF.GetFileName() == csFtpFile)
							csFtpFile = GetFtpFileName();
						else
							break;
					}
					else
					{
						delete pData;
						AfxMessageBox(IDS_FTP_TRANSFER_ERR);
						pFtpConnection->Close();
						delete pFtpConnection;
						InetSession.Close();
						return URL_ERR;
					}
				}
				//create the file on the ftp site
				CInternetFile* pFile = pFtpConnection->OpenFile(csFtpFile, GENERIC_WRITE );
				ASSERT(pFile);
				if(!pFile)
				{
					delete pData;
					pFtpConnection->Close();
					delete pFtpConnection;
					InetSession.Close();
					AfxMessageBox(IDS_FTP_TRANSFER_ERR);
					return FILE_CREATE_FAILED;
				}

				//open the file to be transferred
				CFile dataFile( csLocalFile, CFile::modeRead );
				//get the file length
				DWORD nOriginalFileLength = dataFile.GetLength();
				DWORD nBytesLeft = nOriginalFileLength;

				// Create Progress Bar control
				//set the range for the progress bar
				DWORD nCycles = nBytesLeft / nDataSize;
				CProgressCtrl *pProgressBar = (CProgressCtrl *)m_progressDialog.GetDlgItem(IDC_PROGRESS_CONTROL);
				pProgressBar->SetRange(0, nCycles);

				//init the progress to position 0
				pProgressBar->SetStep(1);
				//start reading and writing the file
				BOOL bAborted = FALSE;
				while(nBytesLeft)
				{
					//if the user hit the cancel button abort the process
					if(m_progressDialog.CancelHit())
					{
						bAborted = TRUE;
						break;
					}
					//read the data
					int nBytesRead = dataFile.Read(pData, nDataSize);
					//write the data
					pFile->Write(pData, nBytesRead);

					nBytesLeft -= nBytesRead;
					pProgressBar->StepIt();

					MSG	msg;
               if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
               	// Pump the message through.
               	AfxGetApp()->PumpMessage();
				}
				if(bAborted)
				{
					pFile->Abort();
					pFtpConnection->Remove(csFtpFile);
					nResult = OPERATION_ABORTED;
				}
				else
				{
					pFile->Close();
					nResult = SUCCESS;
#if 0
					if(ftpFF.FindFile(csFtpFile))
					{
						ftpFF.FindNextFile();
						DWORD nLength = ftpFF.GetLength();
						if(nLength == nOriginalFileLength)
							nResult = SUCCESS;
						else
						{
							nResult = OPERATION_INCOMPLETE;
							pFtpConnection->Remove(csFtpFile);
						}
					}
					else
						nResult = OPERATION_INCOMPLETE;
#endif
				}
				delete pFile;
				m_progressDialog.DestroyWindow();
			}
			else
			{
				m_progressDialog.DestroyWindow();
			}
			CFile::Remove(csPriceFileLocal);
		}
		else
		{
			AfxMessageBox(IDS_FTP_TRANSFER_ERR);
			return FILE_ERROR;
		}
		if(nResult == SUCCESS)
		{
			CString csNewFtpFileName = csFtpFile.SpanExcluding(".") + ".msz";
			pFtpConnection->Rename(csFtpFile, csNewFtpFileName);
		}
		delete pData;
		pFtpConnection->Close();
		delete pFtpConnection;
		InetSession.Close();
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
		TCHAR szErr[1024];
		if (pEx->GetErrorMessage(szErr, 1024))
			AfxMessageBox(szErr, MB_OK);
		else
			AfxMessageBox(IDS_EXCEPTION, MB_OK);
		pEx->Delete();
		delete pData;
		pFtpConnection->Remove(csFtpFile);
		return INTERNET_EXCEPTION;
	}
	EndWaitCursor();

	return nResult;
#endif
}

#ifndef ARTSTORE

CFtpConnection* CBillingInfoPage::GetFtpConnection(CInternetSession* pSession, CString csFtpSite, CString csLogin, CString csPassWord)
{
	CFtpConnection* pFtpConnection = NULL;	
	CString csServerName;
	CString csObject;
	INTERNET_PORT nPort;
	DWORD dwServiceType;
	// check to see if this is a reasonable URL -- 
	// ie "ftp://servername/dirs" or just "servername/dirs"
	if (!AfxParseURL(csFtpSite, dwServiceType, csServerName, csObject, nPort))
	{
		// try adding the "ftp://" protocol
		CString csFtpURL = _T("ftp://");
		csFtpURL += csFtpSite;

		if (!AfxParseURL(csFtpURL, dwServiceType, csServerName, csObject, nPort))
		{
			AfxMessageBox(IDS_INVALID_URL, MB_OK);
			pSession->Close();
			return NULL;
		}
	}
	// Now open an annonymous FTP connection to the server
	if ((dwServiceType == INTERNET_SERVICE_FTP) && !csServerName.IsEmpty())
	{
		try
		{
			CWaitCursor;
			pFtpConnection = pSession->GetFtpConnection(csFtpSite, csLogin, csPassWord);
			ASSERT(pFtpConnection);
			if(pFtpConnection == NULL)
			{
				pSession->Close();
				AfxMessageBox(IDS_CONNECTION_ERR, MB_OK);
				return NULL;
			}
		}
		catch (CInternetException* pEx)
		{
			// catch errors from WinINet
			TCHAR szErr[1024];
			if (pEx->GetErrorMessage(szErr, 1024))
				AfxMessageBox(szErr, MB_OK);
			else
				AfxMessageBox(IDS_EXCEPTION, MB_OK);
			pEx->Delete();
			pSession->Close();
			return NULL;
		}
	}
	
	if(pFtpConnection == NULL)
	{
		pSession->Close();
		AfxMessageBox(IDS_CONNECTION_ERR, MB_OK);
	}
	return pFtpConnection;
	
}
#endif
CString CBillingInfoPage::GetFtpFileName()
{
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);

	CString csResult;
	csResult.Format("%.2d%.2d%.2d%.2d.mfw", sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	return csResult;


}
