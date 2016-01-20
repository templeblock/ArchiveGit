/*
// $Workfile: webdef.h $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/webdef.h $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 4     12/22/98 10:29a Johno
// 
// 3     7/17/98 5:54p Johno
// Added WebPublishWarnGet(), WebPublishWarnPut() references.
// 
// 
//
*/
#define SZ_PUBLISH_SECTION          "Publish"
#define SZ_PUBLISH_PATH_ENTRY       "Path"
#define SZ_PUBLISH_DESTURL_ENTRY    "DestURL"
#define SZ_PUBLISH_SITENAME_ENTRY   "SiteName"
#define SZ_PUBLISH_WEBCHECK         "WebCheck"
#define SZ_PUBLISH_WARN					"Warn"

#define SZ_WEBSITE_HOMEPAGE_ENTRY   "HomePage"
#define SZ_WEBSITE_FILEEXT_ENTRY    "FileExt"

//class CWebView;
extern   BOOL DesignCheckStateGet(void);
extern   void DesignCheckStatePut(BOOL b);
extern   BOOL WebPublishWarnGet(void);
extern   void WebPublishWarnPut(BOOL);