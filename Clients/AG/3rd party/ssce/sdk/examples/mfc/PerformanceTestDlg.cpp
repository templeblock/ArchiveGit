// Sentry Spelling Checker Engine
// CPerformanceTestDlg: Measure Sentry's spell-checking performance

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: PerformanceTestDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "PerformanceTestDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CPerformanceTestDlg, CDialog)
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

CPerformanceTestDlg::CPerformanceTestDlg(CWnd *parentWnd) :
  CDialog(IDD_PERFORMANCETEST, parentWnd) {
	// Do nothing.
}

BOOL CPerformanceTestDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("Alice was begining to get very tired of sittign by her sister on the "
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
	// should be ssce\sdk\examples\mfc\example
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

void CPerformanceTestDlg::OnCheckSpelling() {
	CWaitCursor busy;

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
	text.ReleaseBuffer();

	// The text will not be modified so we don't have to allocate
	// room for growth.
	SSCE_CHAR *str = (SSCE_CHAR *)text.GetBuffer(text.GetLength() + 1);

	// Check the spelling of the text. CheckString will return when it
	// finds a misspelled word or reaches the end of the text.
	SSCE_S16 rv;
	SSCE_CHAR word[SSCE_MAX_WORD_SZ];
	SSCE_CHAR otherWord[SSCE_MAX_WORD_SZ];
	int numMisspelledWords = 0;
	SSCE_S32 cursor = 0;
	DWORD startTime = GetTickCount();
	while ((rv = SSCE_CheckString(SSCE_GetSid(), str, &cursor, word, sizeof(word),
	  otherWord, sizeof(otherWord))) >= 0 && rv != SSCE_END_OF_BLOCK_RSLT) {
		if (rv & SSCE_MISSPELLED_WORD_RSLT) {
			++numMisspelledWords;
		}

		// Skip to the next word.
		cursor += lstrlen((const char *)word);
	}

	DWORD totalTime = GetTickCount() - startTime;
	if (totalTime == 0) {
		totalTime = 1;
	}

	// Count the number of words in the text.
	SSCE_S32 numWords = 0;
	cursor = 0;
	while ((cursor = SSCE_GetStringWord(SSCE_GetSid(), str, cursor, word, sizeof(word))) >= 0) {
		++numWords;
		cursor += lstrlen((const char *)word);
	}

	// Display the results.
	CString results;
	results.Format(_T("%d words. %d misspelled words. %d ms. %d words/second."),
	  numWords, numMisspelledWords, totalTime, numWords * 1000 / totalTime);
	CStatic *label = (CStatic *)GetDlgItem(IDC_RESULTS);
	label->SetWindowText(results);

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

