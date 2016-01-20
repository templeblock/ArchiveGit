#include "stdafx.h"
#include <string.h>
#include "Ctp.h"

#define IMPLEMENT_NpCtp
#include "NpCtp.h"

/////////////////////////////////////////////////////////////////////////////
static CCtp* GetApp(JRIEnv* env, struct NpCtp* self)
{
	NPP pInstance = NULL;
	try
	{
		pInstance = (NPP)netscape_plugin_Plugin_getPeer(env, self);
//j		pInstance = (NPP)self->getPeer(env);
	}
	catch(...)
	{
		::MessageBox(::GetActiveWindow(), "Java Run-time error.  Try opening another project or use the Internet Explorer browser.", "Create & Print", MB_OK);
	} 
	if (!pInstance)
		return NULL;

	return (CCtp*)pInstance->pdata;
}

/////////////////////////////////////////////////////////////////////////////
extern JRI_PUBLIC_API(void)
native_NpCtp_PutSrc(JRIEnv* env, struct NpCtp* self, struct java_lang_String* value)
{
	CCtp* pCtp = GetApp(env, self);
	if (!pCtp)
		return;

	CComBSTR bstrValue(JRI_GetStringLength(env, value), (LPCWSTR)JRI_GetStringChars(env, value));
	pCtp->PutSrc(bstrValue);
}

/////////////////////////////////////////////////////////////////////////////
extern JRI_PUBLIC_API(struct java_lang_String*)
native_NpCtp_GetSrc(JRIEnv* env, struct NpCtp* self)
{
	CCtp* pCtp = GetApp(env, self);
	if (!pCtp)
		return NULL;

	CComBSTR bstrValue;
	pCtp->GetSrc(&bstrValue);
	return (struct java_lang_String*)JRI_NewString(env, bstrValue, bstrValue.Length());
}

/////////////////////////////////////////////////////////////////////////////
extern JRI_PUBLIC_API(void)
native_NpCtp_PutContext(JRIEnv* env, struct NpCtp* self, struct java_lang_String* value)
{
	CCtp* pCtp = GetApp(env, self);
	if (!pCtp)
		return;

	CComBSTR bstrValue(JRI_GetStringLength(env, value), (LPCWSTR)JRI_GetStringChars(env, value));
	pCtp->PutContext(bstrValue);
}

/////////////////////////////////////////////////////////////////////////////
extern JRI_PUBLIC_API(struct java_lang_String*)
native_NpCtp_GetContext(JRIEnv* env, struct NpCtp* self)
{
	CCtp* pCtp = GetApp(env, self);
	if (!pCtp)
		return NULL;

	CComBSTR bstrValue;
	pCtp->GetContext(&bstrValue);
	return (struct java_lang_String*)JRI_NewString(env, bstrValue, bstrValue.Length());
}

/////////////////////////////////////////////////////////////////////////////
extern JRI_PUBLIC_API(jint)
native_NpCtp_DoCommand(JRIEnv* env, struct NpCtp* self, struct java_lang_String* value)
{
	CCtp* pCtp = GetApp(env, self);
	if (!pCtp)
		return 0;

	CComBSTR bstrValue(JRI_GetStringLength(env, value), (LPCWSTR)JRI_GetStringChars(env, value));
	jint lResult = 0;
	pCtp->DoCommand(bstrValue, &lResult);
	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
extern JRI_PUBLIC_API(jint)
native_NpCtp_GetCommand(JRIEnv* env, struct NpCtp* self, struct java_lang_String* value)
{
	CCtp* pCtp = GetApp(env, self);
	if (!pCtp)
		return 0;

	CComBSTR bstrValue(JRI_GetStringLength(env, value), (LPCWSTR)JRI_GetStringChars(env, value));
	jint lResult = 0;
	pCtp->GetCommand(bstrValue, &lResult);
	return lResult;
}

#ifdef NOTUSED //j How to return a string (BSTR)
/////////////////////////////////////////////////////////////////////////////
extern JRI_PUBLIC_API(struct java_lang_String*)
native_NpCtp_GetCommand(JRIEnv* env, struct NpCtp* self, struct java_lang_String* value)
{
	CCtp* pCtp = GetApp(env, self);
	if (!pCtp)
		return NULL;

	CComBSTR bstrValue(JRI_GetStringLength(env, value), (LPCWSTR)JRI_GetStringChars(env, value));
	CComBSTR bstrResult;
	pCtp->GetCommand(bstrValue, &bstrResult);
	return (struct java_lang_String*)JRI_NewString(env, bstrResult, bstrResult.Length());
}
#endif NOTUSED //j

/////////////////////////////////////////////////////////////////////////////
extern JRI_PUBLIC_API(void)
native_NpCtp_FileDownload(JRIEnv* env, struct NpCtp* self, struct java_lang_String* value)
{
	CCtp* pCtp = GetApp(env, self);
	if (!pCtp)
		return;

	CComBSTR bstrValue(JRI_GetStringLength(env, value), (LPCWSTR)JRI_GetStringChars(env, value));
	pCtp->FileDownload(bstrValue);
}

