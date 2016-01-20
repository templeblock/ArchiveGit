<%@ LANGUAGE = JScript %>
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

	var RequestSA = Server.CreateObject("SoftArtisans.FileUp");

	// Post the following values from the submit form
	var strHeadline = RequestSA.Form("Headline");
	var strBody = RequestSA.Form("Body");
	var strSection = RequestSA.Form("Section");

	// We are actually going to ignore the passed section, and get it from the form field
	sec = strSection;
	sec = FixSectionID(sec);

	var strSchoolName  = SchoolID2Name(ids);
	var strSectionName = SectionID2Name(sec);
	var strSectionTitle  = SectionID2Title(sec);
	var strTitle = strSchoolName + " " + strSectionTitle;

	var strBannerFile = '..\\' + strSchoolName + '\\Banner.htm';

	var strLinksFile = '..\\' + strSchoolName + '\\' + strSectionName + '\\Links.inc';
	strLinksFile = Server.MapPath(strLinksFile);

	var strFolder = '..\\' + strSchoolName + '\\' + strSectionName;
	strFolder = Server.MapPath(strFolder);

	var strArticleFileName = GetUniqueFileName();
	var strArticlePath = '..\\' + strSchoolName + '\\' + strSectionName + "\\" + strArticleFileName;
	strArticlePath = Server.MapPath(strArticlePath);

	// Create an instance of the FileSystemObject
	var objFSO = new ActiveXObject("Scripting.FileSystemObject");
	var objFolder;
	if (objFSO.FolderExists(strFolder))
		objFolder = objFSO.GetFolder(strFolder);
	else
		objFolder = objFSO.CreateFolder(strFolder);

	// Setup some file constants
	var kForReading = 1, kForWriting = 2, kForAppending = 8;
	var kOverwrite = true;
	var kAllowCreation = true;
	var kDontAllowCreation = false;

	if (!RequestSA.IsEmpty)
	{ // A filename was specified, so copy the file
		// Use the article name as a folder: create it
		var objFolder;
		if (objFSO.FolderExists(strArticlePath))
			objFolder = objFSO.GetFolder(strArticlePath);
		else
			objFolder = objFSO.CreateFolder(strArticlePath);

		// Figure out what filename to add to the name and path
		var strFilename = RequestSA.UserFilename;
		strFilename = '\\' + strFilename.substr(strFilename.lastIndexOf('\\') + 1);

		strArticleFileName += strFilename;
		strArticlePath += strFilename;

		Server.ScriptTimeOut = 200;
		RequestSA.Form("Filename").SaveAs(strArticlePath);
		//Response.Write("<BR> Total Bytes Written: " + RequestSA.TotalBytes);
	}
	else
	{ // No filename was specified, so create one from the form fields
		strArticleFileName += '.htm';
		strArticlePath += '.htm';

		// Create (or overwrite) the new article file
		var objFile;
		if (objFSO.FileExists(strArticlePath))
			objFile = objFSO.OpenTextFile(strArticlePath, kForWriting, kAllowCreation);
		else
			objFile = objFSO.CreateTextFile(strArticlePath, kOverwrite);

		// Output the article
		objFile.WriteLine('<html>');
		objFile.WriteLine('<head>');
		objFile.WriteLine('<title>Included Article File</title>');
		objFile.WriteLine('</head>');
		objFile.WriteLine('<body>');
		objFile.WriteLine('<h3><tt><font color="#000080" size="5" face="Arial Black">');
		objFile.WriteLine(Server.HTMLEncode(strHeadline));
		objFile.WriteLine('</font></tt></h3>');
		objFile.WriteLine('</h2><br>');
		objFile.WriteLine('<p>');
		objFile.WriteLine(strBody);
		objFile.WriteLine('</p>');
		objFile.WriteLine('</body>');
		objFile.WriteLine('</html>');

		// Close the article file and dispose of it
		objFile.Close();
		objFile = null;
	}
	
	// Now add a link to the article in the Links file!
	// Create (or append to) the Links file
	var objFile;
	if (objFSO.FileExists(strLinksFile))
		objFile = objFSO.OpenTextFile(strLinksFile, kForAppending, kAllowCreation);
	else
		objFile = objFSO.CreateTextFile(strLinksFile, kOverwrite);

	// Output the new link
	var date = new Date;
	var dateToday =
		(date.getMonth()+1).toString(10) + '/' +
		(date.getDay()+1).toString(10) + '/' +
		(date.getYear()).toString(10);
	date = null;

	objFile.WriteLine('<b><a href="../All/Viewer.asp?ids=' + ids + '&amp;sec=' + sec + '&amp;art=' + strArticleFileName + '">' + Server.HTMLEncode(strHeadline) + '</a></b><i> ' + dateToday + '</i><br>');

	// Close the links file and dispose of it
	objFile.Close();
	objFile = null;

	objFSO = null;

	Response.Redirect("Confirm.asp?ids=" + ids + "&sec=" + sec + "&art=" + strArticleFileName + "&hed=" + strHeadline);
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
<h3><tt><font color="#000080" size="5" face="Arial Black">Uploading article...</font></tt></h3>
<%
	IncludeFile("ContentFooter.htm", true, null);
%>

</body>

</html>
