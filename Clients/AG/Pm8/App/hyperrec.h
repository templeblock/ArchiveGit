#ifndef __HYPERREC_H__
#define __HYPERREC_H__
/////////////////////////////////////////////////////////////////////////////

enum HyperStringType
{
   TYPE_Invalid = -1,
   TYPE_URL,            // "Complete" URL, i.e. "http://foo.bar"
   TYPE_PageURL,        // "Incomplete" URL, ie "foo.bar"
   TYPE_EmailAddress,   // As "mailto:foobar@foo.bar"
	TYPE_FilePath        // As c:\foo\bar.foo
};

#define	HyperlinkRecordDisabled	1

struct HyperlinkData
{
   CString           FilePathOrURL;
   short             StringType;
   WORD              Flags;
   DB_RECORD_NUMBER  PageRecordNumber; // In the case of TYPE_PageURL

   HyperlinkData()
   {
      Init();
   }

   HyperlinkData(HyperlinkData &p)
   {
      Copy(p);
   }

   void 
   Init(void);
   
   BOOL 
   IsValid(void)
   {
      if (StringType == TYPE_Invalid)
         return FALSE;
      
      if ((StringType != TYPE_PageURL) && (FilePathOrURL.IsEmpty() != FALSE))
         return FALSE;
      
      return TRUE;
   }

   HyperlinkData*
   Copy(HyperlinkData &p);

	void
	Disable(void)
	{
		Flags |= (WORD)HyperlinkRecordDisabled;
	}

	void
	Enable(void)
	{
		Flags &= ~(WORD)HyperlinkRecordDisabled;
	}

	BOOL
	IsEnabled(void)
	{
		return (Flags & HyperlinkRecordDisabled) == 0 ? TRUE : FALSE;
	}
};

FARCLASS HyperlinkRecord : public DatabaseRecord
{
public:

// This increments the reference count.

	ULONG inc(BOOL temp = FALSE);

// This decrements the reference count.

	ULONG dec(BOOL temp = FALSE);

// Return the reference count.

	ULONG get_reference_count(void)
		{ return record.reference_count + record.tmp_reference_count; }

// Reset the temporary reference count.

	void reset_tmp_count(void)
		{ record.tmp_reference_count = 0; modified(); }

protected:

/* This part is written to the file. */

	struct
	{
		WORD		         StringType;
      WORD              Flags;
      DB_RECORD_NUMBER  PageRecordNumber;
		
      ULONG	reference_count;
		ULONG tmp_reference_count;
	} record;

// This part is not.

	CString				m_csFilePathOrURL;

// The constructor for this record.
// This is private. Use the create method.

	HyperlinkRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

// Assign method.

	virtual ERRORCODE assign(DatabaseRecordRef record);

public:

	virtual
   ~HyperlinkRecord();

// The creator for this type of record.
// Note that this is static so that we can register it with the database.

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);
   // Set the URL
   void
   SetData(HyperlinkData* p)
   {
      m_csFilePathOrURL       = p->FilePathOrURL;
      record.StringType       = p->StringType;
      record.Flags            = p->Flags;
      record.PageRecordNumber = p->PageRecordNumber;
      modified();
   }
   // Get the URL
   void
   GetData(HyperlinkData* p)
   {
      p->FilePathOrURL     = m_csFilePathOrURL;
      p->StringType        = record.StringType;
      p->Flags             = record.Flags;
      p->PageRecordNumber  = record.PageRecordNumber;
   }

	void
	Disable(void)
	{
		if (IsEnabled())
		{
			record.Flags |= (WORD)HyperlinkRecordDisabled;
			modified();
		}
	}

	void
	Enable(void)
	{
		if (!IsEnabled())
		{
			record.Flags &= ~(WORD)HyperlinkRecordDisabled;
			modified();
		}
	}

	BOOL
	IsEnabled(void)
	{
		return (record.Flags & HyperlinkRecordDisabled) == 0 ? TRUE : FALSE;
	}
/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

// ReadData()
//
// Read the record.

	ERRORCODE	ReadData(StorageDevicePtr device);

// WriteData()
//
// Write the record.

	ERRORCODE	WriteData(StorageDevicePtr device);

// SizeofData()
//
// Return the size of the record.

	ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};
typedef HyperlinkRecord far &HyperlinkRecordRef;

#endif
