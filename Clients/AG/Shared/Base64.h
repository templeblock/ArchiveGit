#pragma once

class CByteArray
{
};

class CBase64
{
	private:
		// Don't allow canonical behavior (i.e. don't allow this class to be passed by value)
		CBase64(const CBase64&) {};
		CBase64& operator=(const CBase64&) { return *this; };

	public:
		CBase64();
		virtual ~CBase64();

		virtual BOOL Decode(const CByteArray& source, CByteArray& destination);
		virtual BOOL Decode(const CString&    source, CByteArray& destination);
		virtual BOOL Encode(const CByteArray& source, CByteArray& destination);
		virtual BOOL Encode(const CByteArray& source, CString&    destination);

	protected:
		BYTE m_DecoderTable[256];
		virtual void m_InitializeDecoderTable(void);
};
