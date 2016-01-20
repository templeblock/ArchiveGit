<%@ LANGUAGE = JScript %>
<!--#include file="../All/Script.inc" -->
<% 
	///////////////////////////////////////////////////////////////////////////////
	var strSubmitted = "";
	var strBaseFilename = "";
	var strFilename = "";
	var strCount = "";
		
	// Pass in: 
	strSubmitted = Request.QueryString("submitted")();
	strBaseFilename= Request.QueryString("base")();
	strCount = Request.QueryString("count")();

	var strArgs = "";
	if (strSubmitted == '1')
	{
		var RequestSA = Server.CreateObject("SoftArtisans.FileUp");
		strFilename = RequestSA.UserFilename;
		if (strFilename != "")
		{
			strFilename = "..\\Work\\" + strFilename.substr(strFilename.lastIndexOf('\\') + 1);
			Server.ScriptTimeOut = 200;
			RequestSA.Form("Filename").SaveAs(Server.MapPath(strFilename));
		}

		RequestSA = null;
		
		if (strBaseFilename == "")
			strBaseFilename = strFilename;
		strFilename = FindImageReference(strBaseFilename, strCount)

		if (strFilename == "")
		{
			Response.Write("<br>Submit complete for file: '" + strBaseFilename + "'");
			Response.Write("<br>All dependant files have been sent");
			// Instead of the lines below, we could send them back to some starting point
			Response.Write("<br>");
			Response.Write("<br>Select any HTML file to submit:");
			strArgs = "?submitted=1&amp;count=1";
		}
		else
		{
			Response.Write("<br>Searching file for dependancy #" + strCount + " in file '" + strBaseFilename + "'");
			Response.Write("<br>Found dependency #" + strCount + ": '" + strFilename + "'");
			Response.Write("<br>");
			Response.Write("<br>Find this file and submit it:");
			var iCount = strCount.valueOf();
			iCount++;
			strArgs = "?submitted=1&amp;count=" + iCount.toString() + "&amp;base=" + strBaseFilename;
		}
	}
	else
	{
		Response.Write("<br>Select any HTML file to submit:");
		strFilename = "";
		strArgs = "?submitted=1&amp;count=1&amp;base=";
	}
%>
<html>

<head>
<title>Submission Form</title>
<meta http-equiv="Pragma" content="no-cache">
<meta name="GENERATOR" content="Microsoft FrontPage 4.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta name="Microsoft Border" content="none">
</head>

<body stylesrc="BgTemplate.htm" bgcolor="#C8DCFF">

<form method="post" action="TestFindImg.asp<%=strArgs%>" enctype="multipart/form-data">
	<p><input name="Filename" value="<%=strFilename%>" size="40" type="file"><br>
	</p>
	<p><input type="submit" value="Submit this file" name="Submit"> <input type="submit" value="Skip this file" name="Skip"></p>
</form>
&nbsp;

</body>

</html>
