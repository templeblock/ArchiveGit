<%@ LANGUAGE = JScript %>
<!--#include file="../All/Script.inc" -->
<% 
	// Pass/submit 'ids' and 'sec' to this page to
	// create a custom page for each school and section
	var ids = Request.QueryString("ids")();
	var sec = Request.QueryString("sec")();
	var hed = Request.QueryString("hed")();
	var art = Request.QueryString("art")();

	// Make sure these two values are valid
	ids = FixSchoolID(ids);
	sec = FixSectionID(sec);

	var strSchoolName  = SchoolID2Name(ids);
	var strSectionName = SectionID2Name(sec);
	var strSectionTitle  = SectionID2Title(sec);
	var strTitle = strSchoolName + " " + strSectionTitle;

	var strBannerFile = '..\\' + strSchoolName + '\\Banner.htm';
%>

<html>

<head>
<title>Article Submission Confirmed</title>
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
<h3><tt><font color="#000080" size="5" face="Arial Black"><%=strSectionTitle%>                                submission confirmed</font></tt></h3>
<p>The article entitled <b>'<%=hed%>'</b> has been submitted to the <b>'<%=strTitle%>'</b> web site.</p>
<p>Click <a href="Viewer.asp?ids=<%=ids%>&amp;sec=<%=sec%>&amp;art=<%=art%>">here</a> to see your new article.</p>
<p>Click <a href="Content.asp?ids=<%=ids%>&amp;sec=<%=sec%>">here</a> to see where your new article appears in the list.</p>
<%
	IncludeFile("ContentFooter.htm", true, null);
%>

</body>

</html>
