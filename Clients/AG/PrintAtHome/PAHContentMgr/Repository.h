#pragma once
#include "XMLDocument2.h"
#include "PAHCollection.h"
#include "PAHProject.h"
#include "Art.h"
#include "Verse.h"
#include "Download.h"


typedef std::map <LPCTSTR, CPAHCollection*> COLLECTIONSTYPE;
typedef std::map <LPCTSTR, CPAHProject*> PROJECTSTYPE;
typedef std::map <LPCTSTR, CArt*> ARTTYPE;
typedef std::map <LPCTSTR, CVerse*> VERSETYPE;

static LPCTSTR REPOSITORY_FILENAME = _T("Repository.xml");
static LPCTSTR REPOSITORY_ROOT = _T("\\PrintAtHome\\Repository\\");
/////////////////////////////////////////////////////////////////////////
class CRepository
{
public:
	CRepository(void);
public:
	virtual ~CRepository(void);

public:
	bool LoadXML(LPCTSTR szURLFileName);
	bool ProcessXMLPack();
	bool DownloadFiles(void);
	bool LoadLocalManifest();
	bool UpdateRepositoryManifest();
	bool BuildManifest();
	bool SaveManifest();
	bool BackupManifest();
	bool ValidateRepositoryManifest();
	bool GetInstallPath(CString &szPath);
	static bool CALLBACK DownloadCallback(LPARAM lParam, void* pDownloadVoid, long lError);
	static void _cdecl XMLCallback(int iLevel, bool bAttribute, LPCTSTR pNodeName, LPCTSTR pValueString, LPARAM lParam);
	LPCTSTR GetRepositoryPath(){return m_szPath;};


protected:
	HANDLE m_hEventDownloadComplete;
	void EmptyCollections();
	void EmptyProjects();
	void EmptyArt();
	void EmptyVerses();
	void Empty();
	bool SaveXMLDoc(CXMLDocument2 &XMLDoc2, LPCTSTR szDestPath);
	bool AddCollection(IXMLDOMNode * pNode);
	bool AddProject(IXMLDOMNode * pNode);
	bool AddArt(IXMLDOMNode * pNode);
	bool AddVerse(IXMLDOMNode * pNode);
	bool ReadItemsByTagName(IXMLDOMElement * pElem, LPCTSTR szTagName);
	bool DownloadComplete(long lError);


private:
	COLLECTIONSTYPE m_Collections;
	PROJECTSTYPE m_Projects;
	ARTTYPE m_Art;
	VERSETYPE m_Verses;
	CDownload m_Download;
	CXMLDocument2 m_XMLDoc;
	CXMLDocument2 m_XMLLocalManifest;
	CString m_szHost;
	CString m_szPath;
	CString m_szLocalManifestPath;
	CString m_szBackupManifestPath;
	CString m_szBackupFolder;
	CString m_szEmptyManifestXML;
};
