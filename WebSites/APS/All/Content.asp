<%@ LANGUAGE = JavaScript %>
<!--#include file="../All/Script.inc" -->
<% 
	// Be sure this pages always refreshes
	Response.Expires = 0;

	// Pass/submit 'ids' and 'sec' to this page to
	// create a custom page for each school and section
	var ids = Request.QueryString("ids")();
	var sec = Request.QueryString("sec")();

	// Make sure these two values are valid
	ids = FixSchoolID(ids);
	sec = FixSectionID(sec);

	var strSchoolName  = SchoolID2Name(ids);
	var strSectionName = SectionID2Name(sec);
	var strSectionTitle  = SectionID2Title(sec);

	var strBannerFile = '..\\' + strSchoolName + '\\Banner.htm';
	var strContentFooterFile = '..\\' + strSchoolName + '\\ContentFooter.htm';
	var strLinksFile = '..\\' + strSchoolName + '\\' + strSectionName + '\\Links.inc';
	var strTitle = strSchoolName + " " + strSectionTitle+ " Page";
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
%>
<h3><tt><font color="#000080" size="5" face="Arial Black"><%=strSectionTitle%></font></tt></h3>
<h5><em>Note: you may need to reload this page to see the most recent additions.</em></h5>
<%
	Response.Write("<p>");
	IncludeFile(strLinksFile, false, null, true/*bSilent*/);
	Response.Write("</p>");
	IncludeFile(strContentFooterFile, true, null);
%>

</body>

</html>
