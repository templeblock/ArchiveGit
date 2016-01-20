<%@ LANGUAGE = JavaScript %>
<!--#include file="../All/Script.inc" -->
<% 
	// Be sure this pages refreshes
	Response.Expires = 0;

	// Pass/submit 'ids' and 'sec' to this page to
	// create a custom page for each school and section
	var ids = Request.QueryString("ids");
	var sec = Request.QueryString("sec");
	var strArticle = Request.QueryString("art");

	// Make sure these two values are valid
	ids = FixSchoolID(ids);
	sec = FixSectionID(sec);
	if (strArticle == "") strArticle = "Article.htm";

	var strSchoolName  = SchoolID2Name(ids);
	var strSectionName = SectionID2Name(sec);
	var strSectionTitle  = SectionID2Title(sec);

	var strBannerFile = '..\\' + strSchoolName + '\\Banner.htm';
	var strArticleFile = '..\\' + strSchoolName + '\\' + strSectionName + '\\' + strArticle;
	var strTitle = strSchoolName + " " + strSectionTitle + " Article - " + strArticle;
%>

<html>

<head>
<title><%=strTitle%></title>
<meta http-equiv="Pragma" content="no-cache">
<meta name="GENERATOR" content="Microsoft FrontPage 4.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta name="Microsoft Border" content="none, default">
</head>

<body stylesrc="BgTemplate.htm" bgcolor="#C8DCFF">

<%
	IncludeFile(strBannerFile, true, null);
	WriteOutSectionLinks(ids, sec);
	IncludeFile("ContentHeader.htm", true, null);
	IncludeFile(strArticleFile, true, null);
	IncludeFile("ContentFooter.htm", true, null);
%>

<p>&nbsp;

</body>

</html>
