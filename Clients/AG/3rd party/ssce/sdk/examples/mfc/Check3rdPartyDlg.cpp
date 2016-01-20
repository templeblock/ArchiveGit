// Sentry Spelling Checker Engine
// CCheck3rdPartyDlg: Example showing how to spell-check text
//	in a 3rd-party text control that doesn't respond to
//	standard messages.

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: Check3rdPartyDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "Check3rdPartyDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CCheck3rdPartyDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECKSPELLING, OnCheckSpelling)
END_MESSAGE_MAP()

// Buffer information used when streaming text out of the control.
struct StreamBuffer {
	TCHAR *bfr;
	size_t bfrSz;
	size_t n;
};

static DWORD CALLBACK streamInCb(DWORD cookie, BYTE *bfr, LONG nBytes,
  LONG *nBytesCopied);
static DWORD CALLBACK streamOutCb(DWORD cookie, BYTE *bfr, LONG nBytes,
  LONG *nBytesCopied);

CCheck3rdPartyDlg::CCheck3rdPartyDlg(CWnd *parentWnd) :
  CDialog(IDD_CHECK3RDPARTY, parentWnd) {
	// Do nothing.
}

BOOL CCheck3rdPartyDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("Alice was begining to get very tired of siting by her sister on the "
	  "bank, and of having nothing to do: once or twice she had peeped into the "
	  "book her sister was reading, but it had no pitures or conversatoins in "
	  "it, \"and what is the use of a book,\" thought Alice \"without pictures or "
	  "conversation?\"\r\n\r\n"
	  "So she was considerring in her own mind (as well as she could, for the "
	  "hot day made her feel very sleepy and stuppid), whether the plesure of "
	  "making a daisy-chain would be worth the trouble of getting up and "
	  "picking the daisys, when sudenly a White Rabbit with pink eyes ran "
	  "close by her."));

	// Load the contents of the rich-text control from a file.
	CRichEditCtrl *richEditCtrl = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT1);
	richEditCtrl->LimitText(0xfffffff);
	CFile inFile;
	// alice.rtf is located in ssce\sdk\examples; current directory
	// should be ssce\sdk\examples\mfc\mfcexample
	if (inFile.Open(_T("..\\alice.rtf"), CFile::modeRead)) {
		EDITSTREAM es;
		es.dwCookie = (DWORD)&inFile;
		es.dwError = 0;
		es.pfnCallback = streamInCb;
		richEditCtrl->StreamIn(SF_RTF, es);
		inFile.Close();
	}
	else {
		richEditCtrl->SetWindowText(sampleText);
	}

	return (TRUE);
}

void CCheck3rdPartyDlg::OnCheckSpelling() {
	// Load the contents of the rich-edit control into a CString. GetWindowText
	// won't return all of the text in large rich edit controls, so
	// the text is extracted as a stream.
	CRichEditCtrl *richEditCtrl = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT1);
	CString text;
	int textLen = richEditCtrl->GetWindowTextLength() + 1;
	EDITSTREAM es;
	StreamBuffer sb;
	sb.bfrSz = textLen;
	sb.bfr = text.GetBuffer(textLen);
	sb.n = 0;
	es.dwCookie = (DWORD)&sb;
	es.dwError = 0;
	es.pfnCallback = streamOutCb;
	richEditCtrl->StreamOut(SF_TEXT, es);
	sb.bfr[sb.n] = _T('\0');
	text.ReleaseBuffer();

	// Check the spelling of the CString. The CString's buffer is increased
	// to allow room for growth.
	int strSz = text.GetLength();
	int growth = strSz / 5;
	growth = max(growth, 256);
	strSz += growth;
	SSCE_CHAR *str = (SSCE_CHAR *)text.GetBuffer(strSz);

	// Check the spelling of the CString's contents. CheckString will return when it
	// finds a misspelled word or reaches the end of the text.
	SSCE_S16 rv;
	SSCE_CHAR word[SSCE_MAX_WORD_SZ];
	SSCE_CHAR otherWord[SSCE_MAX_WORD_SZ];
	SSCE_S32 cursor = 0;
	while ((rv = SSCE_CheckString(SSCE_GetSid(), str, &cursor, word, sizeof(word),
	  otherWord, sizeof(otherWord))) >= 0 && rv != SSCE_END_OF_BLOCK_RSLT) {
        // A problem was encountered with a word in the text. 
        // Highlight the misspelled word in the rich-edit control. Use
		// whatever properties and methods are appropriate to the 3rd-party
		// control you are using.
		richEditCtrl->SetSel(cursor, cursor + lstrlen((TCHAR *)word));
        
        // Check the current word using CheckBlockDlg. This will give the user
        // a chance to correct the misspelling. It may also be corrected
        // automatically.
		CString curWord((TCHAR *)word);
        rv = (SSCE_S16)SSCE_CheckBlockDlg(m_hWnd,
		  (SSCE_CHAR *)curWord.GetBuffer(SSCE_MAX_WORD_SZ),
		  lstrlen((TCHAR *)word), SSCE_MAX_WORD_SZ, FALSE);
		curWord.ReleaseBuffer();
        if (rv == SSCE_CANCEL_ERR) {
            // User canceled.
			break;
        }
        
        // See if the word was changed.
        if (curWord != (TCHAR *)word) {
            // The word was changed. Update the CString.
			if (SSCE_ReplaceStringWord(SSCE_GetSid(), str, strSz,
			  cursor, (SSCE_CHAR *)(const TCHAR *)curWord) >= 0) {
				// Update the rich-edit control.
				// The current word is selected, so all we have to do is
				// replace the selected text.
				// Use whatever properties or methods are appropriate to the
				// 3rd-party control you are using.
				richEditCtrl->ReplaceSel(curWord);
			}
        }
        
        // Advance to the next word.
		cursor += curWord.GetLength();
	}

	text.ReleaseBuffer();
}

// Callback function used to load the contents of the rich-edit control
// from a file.
static DWORD CALLBACK streamInCb(DWORD cookie, BYTE *bfr, LONG nBytes,
  LONG *nBytesCopied) {
   CFile *inFile = (CFile *)cookie;

   *nBytesCopied = inFile->Read(bfr, nBytes);

   return 0;
}

// Callback function used to store the contents of the rich-edit control
// to a buffer.
static DWORD CALLBACK streamOutCb(DWORD cookie, BYTE *bfr, LONG nBytes,
  LONG *nBytesCopied) {
	// Note that we have to leave room in the buffer for a terminating null.
	StreamBuffer *sb = (StreamBuffer *)cookie;
	if (sb->n + nBytes >= sb->bfrSz) {
		nBytes = (sb->bfrSz - 1) - sb->n;
	}
	memcpy(sb->bfr + sb->n, bfr, (size_t)nBytes);
	*nBytesCopied = nBytes;
	sb->n += nBytes;
	return (sb->n < sb->bfrSz - 1 ? 0 : 1);
}
