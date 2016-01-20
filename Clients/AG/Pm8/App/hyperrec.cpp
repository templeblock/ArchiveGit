// $Workfile: hyperrec.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/hyperrec.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 12    7/01/98 4:14p Johno
// Changed HyperlinkRecord.record.Unused to HyperlinkRecord.record.Flags  
// (used by HyperlinkData.Disable() and HyperlinkData.Enable()).
// 
// 11    4/06/98 11:43a Johno
// Removed HyperPageType from HyperlinkData 
// 
// 10    4/01/98 12:12p Johno
// Added IsValid
// 
// 9     3/31/98 11:44a Jayn
// Hyperlink record reference count fixes.
// 
// 8     3/31/98 10:13a Johno
// Compile update
// 
// 7     3/27/98 5:42p Johno
// Added Reference count stuff
// 
// 6     3/25/98 6:35p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 5     3/18/98 6:11p Johno
// New copy function, copy ctor.
// Moved inlines to .CPP file (outlined them?).
// 
// 4     3/11/98 5:49p Johno
// New HyperlinkRecord members, use HyperlinkData struct for IO
// 
// 3     3/05/98 6:26p Johno
// Did you know that SourceSafe can TRUNCATE A FILE TO 0 CONTENT SO THAT
// IT NO LONGER EXISTS IN THE KNOWN UNIVERSE, AND YOU HAVE TO STAY LATE TO
// RECREATE THE GOD DAMED THING? Ask me how...
#include "stdafx.h"

#include "hyperrec.h"
/////////////////////////////////////////////////////////////////////////////
void 
HyperlinkData::Init(void)
{
   FilePathOrURL.Empty();
   StringType = TYPE_Invalid;
   Flags = 0;   
   PageRecordNumber = 0;
}

HyperlinkData*
HyperlinkData::Copy(HyperlinkData &p)
{
   FilePathOrURL    = p.FilePathOrURL;
   StringType       = p.StringType;
   Flags            = p.Flags;
   PageRecordNumber = p.PageRecordNumber;
   return this;
}
/////////////////////////////////////////////////////////////////////////////
HyperlinkRecord::HyperlinkRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&record, 0, sizeof(record));
}

HyperlinkRecord::~HyperlinkRecord()
{
}

ERRORCODE HyperlinkRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new Hyperlink record. */

	if ((*record = (DatabaseRecordPtr)(new HyperlinkRecord(number, type, owner, where))) == NULL)
	{
		return ERRORCODE_Memory;
	}
	return ERRORCODE_None;
}

ERRORCODE HyperlinkRecord::assign(DatabaseRecordRef srecord)
{
	ERRORCODE error;
/* Assign the base record first. */

	if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
	{
		HyperlinkRecordRef precord = (HyperlinkRecordRef)srecord;
		record = precord.record;
		m_csFilePathOrURL = precord.m_csFilePathOrURL;
      record.StringType = precord.record.StringType;
	}
	return error;
}

ST_MAN_SIZE HyperlinkRecord::SizeofData(StorageDevicePtr device)
{
	ST_MAN_SIZE size = device->size_record(sizeof(record))
							+ device->size_cstring(m_csFilePathOrURL);

	return size;
}
// Increment a reference count.
ULONG HyperlinkRecord::inc(BOOL temp)
{
	modified();
	if (temp)
	{
		record.tmp_reference_count++;
	}
	else
	{
		record.reference_count++;
//TRACE("===>> Hyperlink reference count: %d\n", record.reference_count);
	}
	TRACE(" [ref: %d (temp: %d)]", record.reference_count, record.tmp_reference_count);
	return record.tmp_reference_count + record.reference_count;
}
// Decrement a reference count.
ULONG HyperlinkRecord::dec(BOOL temp)
{
	modified();
	if (temp)
	{
		if (record.tmp_reference_count != 0)
		{
			record.tmp_reference_count--;
		}
	}
	else
	{
		if (record.reference_count != 0)
		{
			record.reference_count--;
//TRACE("===>> Hyperlink reference count: %d\n", record.reference_count);
		}
	}
	TRACE(" [ref: %d (temp: %d)]", record.reference_count, record.tmp_reference_count);
	return record.tmp_reference_count + record.reference_count;
}

ERRORCODE HyperlinkRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None
		 && (error = device->read_cstring(m_csFilePathOrURL)) == ERRORCODE_None)
	{
	}
	return error;
}

ERRORCODE HyperlinkRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None
		 && (error = device->write_string(m_csFilePathOrURL)) == ERRORCODE_None)
	{
	}
	return error;
}
