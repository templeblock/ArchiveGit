/*
// $Workfile: INSTICFG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/INSTICFG.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:34:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:56   FRED
// Initial revision.
// 
//    Rev 1.2   15 Nov 1995 14:40:20   FRED
//  
// 
//    Rev 1.1   17 Oct 1995 16:57:10   JAY
// Noe delete's the Rec Array.
// 
//    Rev 1.0   16 Oct 1995 15:06:10   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "insticfg.h"
#include "util.h"

InstallIdapiConfiguration ConfigureIdapi;

const LPCSTR IDAPIKEY_DLLPATH			= "DLLPATH";
const LPCSTR IDAPIKEY_CONFIGFILE01	= "CONFIGFILE01";
const LPCSTR PXNETDIRECTORY			= "PXNET";
const LPCSTR PXNETDIR					= "NET DIR";

//////////////////////////////////////////////////////////////////////////
//
// Class CExceptionWithMessage
//

class CExceptionWithMessage : public CException
{
	DECLARE_DYNAMIC(CExceptionWithMessage);
public:
	CExceptionWithMessage(LPCSTR pszMessage);
	~CExceptionWithMessage();
public:
	const CString& Message(void)
		{ return m_csMessage; }
protected:
	CString m_csMessage;
};

IMPLEMENT_DYNAMIC(CExceptionWithMessage, CException)

CExceptionWithMessage::CExceptionWithMessage(LPCSTR pszMessage)
{
	m_csMessage = pszMessage;
}

CExceptionWithMessage::~CExceptionWithMessage()
{
}

//////////////////////////////////////////////////////////////////////////
//
// Class tIdapiRec
//

InstallIdapiConfiguration::tIdapiRec::tIdapiRec()
{
	irType = irTypeMarker2;
	irSubType = irSubTypeHidden0;
	irpStr1.Empty();
	irArgType = irArgType1;
	irpStr2.Empty();
}

InstallIdapiConfiguration::tIdapiRec::~tIdapiRec()
{
}

BOOL InstallIdapiConfiguration::tIdapiRec::IsRec(tIRTypes aType, tIRSubTypes aSubType, LPCSTR s)
{
	if (aType != irType)
	{
		return FALSE;
	}

	if (aType == irTypeMarker2)
	{
		return TRUE;
	}

	if (aType == irTypeAllOthers3)
	{
		if (aSubType != irSubType)
		{
			return FALSE;
		}

		return stricmp(irpStr1, s) == 0;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//
// Class tIdapiRecArray
//

InstallIdapiConfiguration::tIdapiRecArray::tIdapiRecArray()
{
}

InstallIdapiConfiguration::tIdapiRecArray::~tIdapiRecArray()
{
	RemoveAll();
}

void InstallIdapiConfiguration::tIdapiRecArray::RemoveAll()
{
	int i;
	
	for (i = 0; i < GetSize(); i++)
	{
		tIdapiRec* pIdapiRec = GetAt(i);
		
		ASSERT(pIdapiRec != NULL);
		
		delete pIdapiRec;
		SetAt(i, NULL);
	}
	
	CObArray::RemoveAll();
}

void InstallIdapiConfiguration::tIdapiRecArray::RemoveAt(int nIndex, int nCount /*=1*/)
{
	for (int i = 0; i < nCount; i++)
	{
		if (((nIndex+i) >= 0) && ((nIndex+i) < GetSize()))
		{
			tIdapiRec* pIdapiRec = GetAt(nIndex+i);

			ASSERT(pIdapiRec != NULL);

			delete pIdapiRec;

			SetAt(nIndex+i, NULL);
		}
	}
	
	CObArray::RemoveAt(nIndex, nCount);
}

//////////////////////////////////////////////////////////////////////////
//
// Class InstallIdapiConfiguration
//

InstallIdapiConfiguration::InstallIdapiConfiguration()
{
	Initialized = FALSE;

	// Read the Idapi keys from WIN.INI

	::GetProfileString("IDAPI", IDAPIKEY_DLLPATH, "", KeyDllPath.GetBuffer(256), 256-1);
	KeyDllPath.ReleaseBuffer();

	::GetProfileString("IDAPI", IDAPIKEY_CONFIGFILE01, "", KeyConfigFile01.GetBuffer(256), 256-1);
	KeyConfigFile01.ReleaseBuffer();
}

InstallIdapiConfiguration::~InstallIdapiConfiguration()
{
#if 0
var
	i	: Integer;
begin
	if aList <> NIL then for i := 0 to aList.Count-1 do TObject(aList.Items[i]).Free;
end;
#endif
}

BOOL InstallIdapiConfiguration::Hello(LPCSTR ApplicationVolume, LPSTR IdapiDirectory, LPSTR IdapiCfgFile, BOOL& MinimalCopy, LPSTR ErrorMessage)
{
	BOOL Result = FALSE;

	// (input)	ApplicationDirectory is the target for the application, for example, if
	//				installing PrintMaster into C:\PMW then set ApplicationDirectory="C:\".
	//
	// (output)	IdapiDirectory is the full path to the target Idapi files.
	//
	// (output)	IdapiCfgFile is the full file spec to copy the primordial IDAPI.CFG
	//				file to. If this is blank then do not copy IDAPI.CFG.
	//
	// (output)	MinimalCopy indicates that the smallest number of IDAPI files required to
	//				access Paradox tables is all that is necessary to install.
	//
	// (output)	RetMsg is an optional message to display.
	//
	// RETURNS TRUE for success.

	TRY
	{
		// Clear out the error message.
		strcpy(ErrorMessage, "");

		// Save the application directory.
		DefaultVolume = ApplicationVolume;
		Util::AppendBackslashToPath(DefaultVolume);

		// Do the necessary preparation.
		PrepareForIdapi();
	 	
		// Return installation information.
		strcpy(IdapiDirectory, FirstSpecFromKey(KeyDllPath));

		if (CopyCfgFile)
		{
			strcpy(IdapiCfgFile, FirstSpecFromKey(KeyConfigFile01));
		}
		else
		{
			strcpy(IdapiCfgFile, "");
		}

		MinimalCopy	= DoMinimalCopy;

		// Everything worked.
		Result = TRUE;
	}
	CATCH(CExceptionWithMessage, e)
	{
		strcpy(ErrorMessage, e->Message());
	}
	AND_CATCH(CException, e)
	{
		strcpy(ErrorMessage, "Unknown Exception");
	}
	END_CATCH

	Initialized = Result;

	return Result;

#if 0
Function Hello(
	ApplicationDirectory: pChar;
	IdapiDirectory: pChar;
	IdapiCfgFile: pChar;
	Var MinimalCopy: Boolean;
	RetMsg: pChar): Integer;
begin
	{ Save the application directory. }
	DefaultVolume	:= aDefaultVolume;
	if DefaultVolume[length(DefaultVolume)] <> '\' then AppendStr(DefaultVolume, '\');

	Try

		{ Create the Idapi installation object. }
		InstIdapi	:= NIL;
		InstIdapi	:= TInstIdapi.Create(StrPas(ApplicationDirectory));

		{ Do the necessary preparation. }
		InstIdapi.PrepareForIdapi;

		{ Return installation information. }
		StrPCopy(IdapiDirectory, InstIdapi.FirstSpecFromKey(InstIdapi.KeyDllPath));

		with InstIdapi do
			if CopyCfgFile then
				StrPCopy(IdapiCfgFile, FirstSpecFromKey(KeyConfigFile01))
			else
				StrPCopy(IdapiCfgFile, '');

		MinimalCopy	:= InstIdapi.MinimalCopy;

		StrPCopy(RetMsg, '');

		{ Everything worked. }
		Result		:= ord(TRUE);

	Except
		On E: Exception do begin
			InstIdapi.Free;
			InstIdapi	:= NIL;		{ Allow GoodBye() to know we failed. }
			StrPCopy(RetMsg, E.Message);
			Result := ord(FALSE);
		end;
	end;
end;
#endif

}

BOOL InstallIdapiConfiguration::Goodbye(LPSTR ErrorMessage)
{
	// Assume GoodBye() failed.
	BOOL Result = FALSE;

	TRY
	{
		// Clear out the error message.
		strcpy(ErrorMessage, "");

		if (!Initialized)
		{
			THROW(new CExceptionWithMessage("Previous fatal error, unable to continue."));
		}

		ConfigureIdapi();

		Result = TRUE;
	}
	CATCH(CExceptionWithMessage, e)
	{
		strcpy(ErrorMessage, e->Message());
	}
	AND_CATCH(CException, e)
	{
		strcpy(ErrorMessage, "Unknown Exception");
	}
	END_CATCH

	return Result;

#if 0
Function GoodBye(RetMsg: pChar): Integer;
begin

	{ Assume GoodBye() failed. }
	Result	:= ord(FALSE);
	StrPCopy(RetMsg, '');
	if NOT Assigned(InstIdapi) then begin
		StrPCopy(RetMsg, 'Previous fatal error, unable to continue.');
		EXIT;
	end;

	Try
		Try
			InstIdapi.ConfigureIdapi;
			Result	:= ord(TRUE);
		Finally
			InstIdapi.Free;
		end;
	Except
		On E: Exception do begin
			StrPCopy(RetMsg, E.Message);
			Result	:= ord(FALSE);
		end;
	end;
end;
#endif
}

void ReadIdapiArgument(CFile& f, CString& s)
{
	s.Empty();

	BOOL fDone = FALSE;

	while (!fDone)
	{
		BYTE b;

		if (f.Read(&b, 1) == 0)
		{
			THROW(new CExceptionWithMessage("Idapi: Argument not read."));
		}

		if (b == 0)
		{
			fDone = TRUE;
		}
		else
		{
			s += b;
		}
	}

#if 0
	begin
		s	:= '';
		repeat
			blockread(f, b, 1, i);
			if i <> 1 then Raise Exception.Create('Idapi: Argument not read.');
			if b <> 0 then s := s + chr(b);
		until b = 0;
	end;
#endif
}

InstallIdapiConfiguration::tIdapiRecArray* InstallIdapiConfiguration::ReadIdapiCfg(LPCSTR cfgFileName)
{
	tIdapiRecArray* Result = NULL;
	tIdapiRec* ir = NULL;
	CFile f;

	TRY
	{
		if (!f.Open(cfgFileName, CFile::modeRead|CFile::shareDenyNone))
		{
			char szMessage[128];
			wsprintf(szMessage, "Unable to open: %s", cfgFileName);
			THROW(new CExceptionWithMessage(szMessage));
		}

		Result = new tIdapiRecArray;

		DWORD dwLength = f.GetLength();

		while (f.GetPosition() < dwLength)
		{
			ir = new tIdapiRec;

			WORD w;

			if (f.Read(&w, 2) != 2)
			{
				THROW(new CExceptionWithMessage("Idapi: Record type not read."));
			}

			switch (w)
			{
				case 2:
				{
					ir->irType = irTypeMarker2;
					break;
				}

				case 3:
				{
					ir->irType = irTypeAllOthers3;

					WORD w;

					if (f.Read(&w, 2) != 2)
					{
						THROW(new CExceptionWithMessage("Idapi: Subtype not read."));
					}

					switch (w)
					{
						case 0:
						{
							ir->irSubType = irSubTypeHidden0;
							break;
						}

						case 1:
						{
							ir->irSubType = irSubTypeDisplay1;
							break;
						}

						default:
						{
							THROW(new CExceptionWithMessage("Idapi: Subtype not recognized."));
							break;
						}
					}

					ReadIdapiArgument(f, ir->irpStr1);

					if (f.Read(&w, 2) != 2)
					{
						THROW(new CExceptionWithMessage("Idapi: Argument type not read."));
					}

					switch (w)
					{
						case 1:
						{
							ir->irArgType = irArgType1;
							break;
						}

						case 4:
						{
							ir->irArgType = irArgType4;
							ReadIdapiArgument(f, ir->irpStr2);
							break;
						}

						default:
						{
							THROW(new CExceptionWithMessage("Idapi: Argument type not recognized."));
							break;
						}
					}

					break;
				}

				default:
				{
					THROW(new CExceptionWithMessage("Idapi: Record type not recognized."));
					break;
				}
			}

			Result->Add(ir);
			ir = NULL;
		}
	}
	CATCH_ALL(e)
	{
		TRY
		{
			f.Close();
		}
		END_TRY

		delete Result;
		delete ir;

		THROW_LAST();
	}
	END_CATCH_ALL

	TRY
	{
		f.Close();
	}
	END_TRY;

	return Result;

#if 0
Function TInstIdapi.ReadIdapiCfg(cfgFilename: string): TList;
var
	f	: File;
	w	: Word;
	i	: Integer;
	s	: String;
	b	: Byte;
	ir	: TIdapiRec;

begin

	Result	:= NIL;

	Try

		FileMode	:= 0; 
		AssignFile(f, cfgFilename);
		{$i-} Reset(f, 1); {$i+}
		if IoResult <> 0 then Raise Exception.Create('Unable to open: '+cfgFilename);

		Result	:= TList.Create;

		while not eof(f) do begin

			ir	:= TIdapiRec.Create;

			Try
				Result.Add(ir);
			Except
				ir.Free;
				raise;
			end;

			blockread(f, w, 2, i);
			if i <> 2 then Raise Exception.Create('Idapi: Record type not read.');

			case w of

				2	: begin
					ir.irType	:= irTypeMarker2;
				end;

				3	: begin
					ir.irType	:= irTypeAllOthers3;

					blockread(f, w, 2, i);
					if i <> 2 then Raise Exception.Create('Idapi: Subtype not read.');
					case w of
						0	: ir.irSubType := irSubTypeHidden0;
						1	: ir.irSubType	:= irSubTypeDisplay1;
						else Raise Exception.Create('Idapi: Subtype not recognized.');
					end;

					ReadArgument;
					ir.irpStr1	:= NewStr(s);

					blockread(f, w, 2, i);
					if i <> 2 then Raise Exception.Create('Idapi: Argument type not read.');

					case w of

						1	: begin
							ir.irArgType:= irArgType1;
						end;

						4	: begin
							ir.irArgType:= irArgType4;
							ReadArgument;
							ir.irpStr2	:= NewStr(s);
						end;

						else Raise Exception.Create('Idapi: Argument type not recognized.');

					end;

				end;

				else Raise Exception.Create('Idapi: Record type not recognized.');

			end;

		end;

	Except
		{$i-} CloseFile(f); {$i+}
		i	:= IOResult;
		Result.Free;
		Raise;
	end;

	{$i-} CloseFile(f); {$i+}
	i	:= IOResult;

end;
#endif
}

BOOL WriteIdapiArgument(CFile& f, const CString& s)
{
	BOOL fResult = FALSE;

	TRY
	{
		f.Write(s, s.GetLength()+1);
		fResult = TRUE;
	}
	END_TRY

	return fResult;
}
		
BOOL WriteIdapiWord(CFile& f, WORD w)
{
	BOOL fResult = FALSE;

	TRY
	{
		f.Write(&w, sizeof(w));
		fResult = TRUE;
	}
	END_TRY

	return fResult;
}
		
void InstallIdapiConfiguration::WriteIdapiCfg(LPCSTR cfgFileName, tIdapiRecArray* iFile, BOOL CreateBackup)
{
	CFile f;

	TRY
	{
		if (CreateBackup)
		{
			// Rename the original file as a backup.

			CString csExpanded = cfgFileName;

			if (!Util::ExpandDirectory(csExpanded, FALSE))
			{
				THROW(new CExceptionWithMessage(""));
			}

			CString csDrive;
			CString csDirectory;
			CString csFile;

			Util::SplitPath(csExpanded, &csDrive, &csDirectory, &csFile, NULL);
			Util::ConstructPath(csExpanded, csDrive, csDirectory, csFile, ".BAK");

			TRY
			{
				CFile::Remove(csExpanded);
			}
			END_TRY

			TRY
			{
				CFile::Rename(cfgFileName, csExpanded);
			}
			END_TRY
		}

		if (!f.Open(cfgFileName, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
		{
			char szMessage[128];
			wsprintf(szMessage, "Unable to create: %s", cfgFileName);
			THROW(new CExceptionWithMessage(szMessage));
		}

		for (int i = 0; i <= iFile->GetUpperBound(); i++)
		{
			tIdapiRec* t = (tIdapiRec*)(iFile->GetAt(i));

			switch (t->irType)
			{
				case irTypeMarker2:
				{
					if (!WriteIdapiWord(f, 2))
					{
						THROW(new CExceptionWithMessage(""));
					}

					break;
				}

				case irTypeAllOthers3:
				{
					if (!WriteIdapiWord(f, 3))
					{
						THROW(new CExceptionWithMessage(""));
					}

					WORD w;

					switch (t->irSubType)
					{
						case irSubTypeHidden0:
						{
							w = 0;
							break;
						}

						case irSubTypeDisplay1:
						{
							w = 1;
							break;
						}
					}

					if (!WriteIdapiWord(f, w))
					{
						THROW(new CExceptionWithMessage(""));
					}

					if (!WriteIdapiArgument(f, t->irpStr1))
					{
						THROW(new CExceptionWithMessage(""));
					}

					switch (t->irArgType)
					{
						case irArgType1:
						{
							if (!WriteIdapiWord(f, 1))
							{
								THROW(new CExceptionWithMessage(""));
							}

							break;
						}

						case irArgType4:
						{
							if (!WriteIdapiWord(f, 4))
							{
								THROW(new CExceptionWithMessage(""));
							}

							if (!WriteIdapiArgument(f, t->irpStr2))
							{
								THROW(new CExceptionWithMessage(""));
							}

							break;
						}
					}

					break;
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		TRY
		{
			f.Close();
		}
		END_TRY

		CString csMessage;

		TRY
		{
			csMessage = "Unable to write: ";
			csMessage += cfgFileName;

			if (e->IsKindOf(RUNTIME_CLASS(CExceptionWithMessage)))
			{
				csMessage += "("+((CExceptionWithMessage*)e)->Message()+")";
			}
		}
		END_TRY

		if (csMessage.GetLength() == 0)
		{
			THROW_LAST();
		}
		else
		{
			THROW(new CExceptionWithMessage(csMessage));
		}
	}
	END_CATCH_ALL

	TRY
	{
		f.Close();
	}
	END_TRY

#if 0
procedure TInstIdapi.WriteIdapiCfg(cfgFileName: String; iFile: TList; CreateBackup: Boolean);
var
	f	: File;
	w	: Word;
	i	: Integer;
	s	: String;
	b	: Byte;
	ir	: TIdapiRec;

begin

	if CreateBackup then begin

		{ Rename the original file as a backup. }
		s	:= ChangeFileExt(cfgFilename, '.BAK');
		DeleteFile(s);
		RenameFile(cfgFilename, s);

	end;

	Try

		AssignFile(f, cfgFilename);
		{$i-} Rewrite(f, 1); {$i+}
		if IoResult <> 0 then Raise Exception.Create('Unable to create: '+cfgFilename);

		for i := 0 to iFile.Count-1 do begin

			ir	:= iFile.Items[i];

			case ir.irType of

				irTypeMarker2 : begin
					w	:= 2;
					blockwrite(f, w, 2, w);
					if w <> 2 then Raise Exception.Create('');
				end;

				irTypeAllOthers3 : begin
					w	:= 3;
					blockwrite(f, w, 2, w);
					if w <> 2 then Raise Exception.Create('');

					case ir.irSubType of
						irSubTypeHidden0	: w := 0;
						irSubTypeDisplay1	: w := 1;
					end;
					blockwrite(f, w, 2, w);
					if w <> 2 then Raise Exception.Create('');

					s	:= ir.irpStr1^+#0;
					blockwrite(f, s[1], length(s), w);
					if w <> length(s) then Raise Exception.Create('');

					case ir.irArgType of

						irArgType1	: begin
							w	:= 1;
							blockwrite(f, w, 2, w);
							if w <> 2 then Raise Exception.Create('');
						end;

						irArgType4	: begin
							w	:= 4;
							blockwrite(f, w, 2, w);
							if w <> 2 then Raise Exception.Create('');

							s	:= ir.irpStr2^+#0;
							blockwrite(f, s[1], length(s), w);
							if w <> length(s) then Raise Exception.Create('');
						end;

					end;

				end;

			end;

		end;

	Except
		on E: Exception do begin
			{$i-} CloseFile(f); {$i+}
			i	:= IOResult;
			if E.Message = '' then E.Message := 'Unable to write: '+cfgFilename;
			Raise;
		end;
	end;

	{$i-} CloseFile(f); {$i+}
	i	:= IOResult;

end;
#endif
}

void InstallIdapiConfiguration::PrepareForIdapi(void)
{
	// Validate the [IDAPI] keys.
	// If both keys changed, then we are the only ones using IDAPI and we
	// can do a minimal installation.

	ValidateKeys(DoMinimalCopy);

	// If a configuration file exists then do not copy the primordial one.
	CopyCfgFile = !Util::FileExists(FirstSpecFromKey(KeyConfigFile01));

#if 0
var
	BothKeysChanged	: Boolean;
begin

	{ Validate the [IDAPI] keys. }
	ValidateKeys(BothKeysChanged);

	{ If both keys changed, then we are the only ones using IDAPI and we
	can do a minimal installation. }
	if BothKeysChanged then MinimalCopy := TRUE else MinimalCopy := FALSE;

	{ If a configuration file exists then do not copy the primordial one. }
	if FileExists(FirstSpecFromKey(KeyConfigFile01)) then
		CopyCfgFile := FALSE
	else
		CopyCfgFile := TRUE;
#endif
}

void InstallIdapiConfiguration::ConfigureIdapi(void)
{
	tIdapiRec* ir = NULL;

	// Configure the IDAPI configuration file. Read the existing configuration.
	// This will throw an exception on failure.

	tIdapiRecArray* iFile = ReadIdapiCfg(FirstSpecFromKey(KeyConfigFile01));

	TRY
	{
		// Locate the NET DIR section.
		ir	= LocateLastNetDirSection(iFile);

		// Validate the NetDir (i.e. C:\PXNET). Directory.
		BOOL Changes;
		ValidateNetDir(ir, Changes);

		// Write back the configuration file if changes were made.
		if (Changes)
		{
			BOOL MakeBak = TRUE;
			WriteIdapiCfg(FirstSpecFromKey(KeyConfigFile01), iFile, MakeBak);
		}


		// Finally, write the WIN.INI keys.

		::WriteProfileString("IDAPI", IDAPIKEY_DLLPATH, KeyDllPath);
		::WriteProfileString("IDAPI", IDAPIKEY_CONFIGFILE01, KeyConfigFile01);
	}
	CATCH_ALL(e)
	{
		delete iFile;
		THROW_LAST();
	}
	END_CATCH_ALL

	delete iFile;

#if 0
var
	IniFile	: TIniFile;
	iFile		: TList;
	i			: Integer;
	ir			: TIdapiRec;
	Changes	: Boolean;
	MakeBak	: Boolean;

begin

	{ Configure the IDAPI configuration file. Read the existing configuration.
	This will throw an exception on failure. }
	iFile	:= ReadIdapiCfg(FirstSpecFromKey(KeyConfigFile01));

	Try

		{ Locate the NET DIR section. }
		ir	:= LocateLastNetDirSection(iFile);

		{ Validate the NetDir (i.e. C:\PXNET). Directory. }
		ValidateNetDir(ir, Changes);

		{ Write back the configuration file if changes were made. }
		if Changes then WriteIdapiCfg(FirstSpecFromKey(KeyConfigFile01), iFile, MakeBak);

	Finally
		{ Free the configuration file. }
		for i := 0 to iFile.Count-1 do TObject(iFile.Items[i]).Free;
		iFile.Free;
	end;

	{ Finally, write the WIN.INI keys. }
	Try
		IniFile	:= NIL;
		IniFile	:= TIniFile.Create('WIN.INI');
		IniFile.WriteString('IDAPI', IDAPIKEY_DLLPATH, KeyDllPath);
		IniFile.WriteString('IDAPI', IDAPIKEY_CONFIGFILE01, KeyConfigFile01);
	Finally
		IniFile.Free;
	end;
#endif
}

void InstallIdapiConfiguration::ValidateKeys(BOOL& BothKeysChanged)
{
	BothKeysChanged = (KeyDllPath == "") && (KeyConfigFile01 == "");
	if	(KeyDllPath == "")
	{
		KeyDllPath = DefaultVolume + "IDAPI";
	}
	if (KeyConfigFile01 == "")
	{
		KeyConfigFile01 = KeyDllPath + "\\IDAPI.CFG";
	}
}

InstallIdapiConfiguration::tIdapiRec* InstallIdapiConfiguration::LocateLastNetDirSection(tIdapiRecArray* iFile)
{
	InstallIdapiConfiguration::tIdapiRec* Result = NULL;

	TRY
	{
		// Loop through the configuration file looking for "NET DIR" entries.
		// We need to find 2 consecutive entries: "DRIVERS" & "PARADOX"
		// Multiple copies may exist.

		int Last = -1;
		int i = 0;
		int n = 0;

		while (i < iFile->GetSize()-1-1) // -1 for range [0..n-1], -1 for a look-ahead
		{
			if ((iFile->GetAt(i)->IsRec(irTypeAllOthers3, irSubTypeHidden0, "DRIVERS"))
			 && (iFile->GetAt(i+1)->IsRec(irTypeAllOthers3, irSubTypeHidden0, "PARADOX")))
			{
				Last = i;
				n = 0;
			}
			else
			{
				if (iFile->GetAt(i)->IsRec(irTypeAllOthers3, irSubTypeDisplay1, PXNETDIR))
				{
					n = i;
				}
			}

			i++;
		}

		// Did we find it ?
		if (n == 0)
		{
			THROW(new CExceptionWithMessage("IDAPI config file: NETDIR entry not found."));
		}

		Result = iFile->GetAt(n);
	}
	CATCH_ALL(e)
	{
		THROW_LAST();
	}
	END_CATCH_ALL

	return Result;

#if 0
var
	i		: Integer;
	Last	: Integer;
	n		: Integer;
begin

	{ Loop through the configuration file looking for "NET DIR" entries.
	We need to find 2 consecutive entries: "DRIVERS" & "PARADOX"
	Multiple copies may exist. }

	Last	:= (-1);
	i	:= 0;
	n	:= 0;
	while i < iFile.Count-1-1 { -1 for range [0..n-1], -1 for a look-ahead } do begin

		if TIdapiRec(iFile.Items[i]).IsRec(irTypeAllOthers3, irSubTypeHidden0, 'DRIVERS')
		and TIdapiRec(iFile.Items[i+1]).IsRec(irTypeAllOthers3, irSubTypeHidden0, 'PARADOX')
		then begin
			Last := i;
			n	:= 0;
		end else begin
			if TIdapiRec(iFile.Items[i]).IsRec(irTypeAllOthers3, irSubTypeDisplay1, PXNETDIR)
			then n := i;
		end;

		inc(i);

	end;

	{ Did we find it ? }
	if n = 0 then Raise Exception.Create('IDAPI config file: NETDIR entry not found.');

	{ We found the section. }
	Result	:= iFile.Items[n];
#endif
}

void InstallIdapiConfiguration::ValidateNetDir(tIdapiRec* ir, BOOL& Changes)
{
	// Assume no changes. }
	Changes = FALSE;

	// Validate this is the net dir entry. }
	if (!ir->IsRec(irTypeAllOthers3, irSubTypeDisplay1, "NET DIR"))
	{
		THROW(new CExceptionWithMessage("Internal"));
	}

	// If the directory is blank or the directory does not exist then use the
	// DefaultVolume+'PXNET'
	if ((ir->irpStr2 == "") || (!Util::DirectoryExists(ir->irpStr2)))
	{
		// Change the entry to point to a new directory.
		ir->irpStr2 = DefaultVolume+PXNETDIRECTORY;
		Changes = TRUE;
	}

	// If the directory does not exist then create it.
	TRY
	{
		Util::MakeDirectory(ir->irpStr2);
	}
	CATCH_ALL(e)
	{
		char szMessage[256];
		wsprintf(szMessage, "Can't create the directory %s", (LPCSTR)(ir->irpStr2));
		THROW(new CExceptionWithMessage(szMessage));
	}
	END_CATCH_ALL

#if 0
var
	p	: pString;
begin

	{ Assume no changes. }
	Changes := FALSE;

	{ Validate this is the net dir entry. }
	if NOT ir.IsRec(irTypeAllOthers3, irSubTypeDisplay1, 'NET DIR') then
		Raise Exception.Create('Internal');

	{ If the directory is blank or the directory does not exist then use the
	DefaultVolume+'PXNET' }
	if (ir.irpStr2 = NIL) or (ir.irpStr2^ = '') or (NOT DirectoryExists(ir.irpStr2^))
	then begin

		{ Change the entry to point to a new directory. }
		DisposeStr(ir.irpStr2);
		ir.irpStr2	:= NIL;
		ir.irpStr2	:= NewStr(DefaultVolume+PXNETDIRECTORY);

		Changes	:= TRUE;

	end;

	{ If the directory does not exist then create it. }
	if NOT DirectoryExists(ir.irpStr2^) then ForceDirectories(ir.irpStr2^);
#endif
}

CString InstallIdapiConfiguration::FirstSpecFromKey(const CString& s)
{
	// The ini line may contain 2 or more config files, like this:
	//		CONFIGFILE01=C:\IDAPI\IDAPI.CFG;d:\idapi\idapi.cfg
	//
	// It is unclear on the 'correct' way to handle this easily.
	// The shortcut, implemented here is to just use the first one.

	int i = s.Find(';');

	if (i < 0)
	{
		return CString(s);
	}

	if (i == 0)
	{
		return CString("");
	}

	return s.Left(i);
}
