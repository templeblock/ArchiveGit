<%@ LANGUAGE = JScript %>
<!--#include file="../All/Script.inc" -->
<% 
	// Be sure this pages always refreshes
	Response.Expires = 0;

	// Pass/submit 'UserName' and 'Password' to this page to attempt to login
	var strUserName = Request.Form("UserName")();
	var strPassword = Request.Form("Password")();

	// Pass/submit 'ids' and 'sec' to this page to
	// create a custom page for each school and section
	var ids = Request.QueryString("ids")();
	var sec = Request.QueryString("sec")();

	// Make sure these two values are valid
	ids = FixSchoolID(ids);
	sec = FixSectionID(sec);

	var strSchoolName  = SchoolID2Name(ids);
	var strSectionName = SectionID2Name(sec);

	var strBannerFile = '..\\' + strSchoolName + '\\Banner.htm';
	var strDatabaseFile = '..\\' + strSchoolName + '\\passwords.mdb';

%>
<html>

<head>
<title>Submission Form</title>
<meta http-equiv="Pragma" content="no-cache">
<meta name="GENERATOR" content="Microsoft FrontPage 4.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta name="Microsoft Border" content="none, default">
</head>

<body stylesrc="BgTemplate.htm" bgcolor="#C8DCFF">

<%
	// See if we have already logged in once this session
	var bOk = (Session("bOk"+ids) == "1" ? true : false);

	// If this is a new login attempt, test out the user and password
	var strReturnedMessage = "";
	if (bOk)
		strReturnedMessage = "Welcome back to the " + strSchoolName + " Submit Page!";
	else
	if (Request.Form("Action") == "Login")
	{
		strReturnedMessage = ValidateUser(strUserName, strPassword, strDatabaseFile);
		if (strReturnedMessage == "")
		{
			strReturnedMessage = "Login successful!";
			bOk = true;
		}
		else
		{
			bOk = false;
			strUserName = "";
			strPassword = "";
		}
	
		Session("bOk"+ids) = (bOk ? "1" : "0");
	}
%>
<%
	IncludeFile(strBannerFile, true, null);
	WriteOutSectionLinks(ids, sec);
	IncludeFile("ContentHeader.htm", true, null);
 
	Response.Write(strReturnedMessage);
	if (!bOk)
	{
%>

<form method="post" action="Submit.asp?ids=<%=ids%>&amp;sec=<%=sec%>">
	<font color="#000080" size="5" face="Arial Black">Please log in in order to submit your article.</font>
	<table border="0" width="488" cellpadding="2">
		<tr>
			<td width="199">
				<p align="right"><strong><font face="Arial Black" color="#0000FF">UserName:</font></strong></td>
			<font color="#000080" face="Arial Black">
			<td width="331"><input type="text" size="37" name="UserName" tabindex="1"></td>
			</tr>
		</font>
		<tr>
			<td width="199">
				<p align="right"><strong><font color="#0000FF" face="Arial Black">Password:</font></strong></td>
			<font color="#000080" face="Arial Black">
			<td width="331"><input type="password" size="37" name="Password" tabindex="2"></td>
			</tr>
			<tr>
				<td width="199">
					<p align="right"></td>
				<td width="331"><input type="submit" name="Action" value="Login" tabindex="3"></td>
			</tr>
		</font>
	</table>
</form>
<%
}
else
{
	Session("bAlreadySubmitted") = "0";
%>
<form method="post" action="Upload.asp?ids=<%=ids%>&amp;sec=<%=sec%>" enctype="multipart/form-data">
	<font color="#000080" size="5" face="Arial Black">Submit your article by following these simple steps.</font>
	<table border="0" cellpadding="10" width="570">
		<tr>
			<td align="right" width="134"><img src="../../images/1.gif" width="39" height="46"></td>
			<td width="372"><font face="Arial" size="2">Enter a headline for your article.<br>
				<input name="Headline" value size="56"></font></td>
		</tr>
		<tr>
			<td align="right" width="134"><font color="#000080" size="5" face="Arial Black">A</font><img src="../../images/2.gif" width="38" height="47"></td>
			<td width="372"><font face="Arial" size="2">If you don't have a document file to upload, enter the text of your article, or paste it from the clipboard.&nbsp; It can include HTML
				formatting.<br>
				</font><textarea rows="10" name="Body" cols="56"></textarea></td>
		</tr>
		<tr>
			<td align="right" width="134"><font color="#000080" size="5" face="Arial Black">B</font><img src="../../images/2.gif" width="38" height="47"></td>
			<td width="372"><font face="helvetica,arial" size="2">If you wish to upload any document files instead of creating a new document from the text above, select them below.&nbsp; If you
				select any files here, the text above will be ignored.<br>
				</font><input name="Filename" value size="40" type="file"> <input name="Filename1" value size="40" type="file"> <input name="Filename2" value size="40" type="file"> <input
				name="Filename3" value size="40" type="file"> <input name="Filename4" value size="40" type="file"></td>
		</tr>
		<tr>
			<td align="right" width="134"><img src="../../images/3.gif" width="65" height="47"></td>
			<td width="372"><font face="Arial" size="2">Select the section where you wish to have your article appear.<br>
				</font><select name="Section" size="1">
					<option value="HE" selected>Headlines</option>
					<option value="EV">Upcoming events</option>
					<option value="CA">Calendar and schedules</option>
					<option value="ST">Student life</option>
					<option value="FA">Faculty and staff</option>
					<option value="AC">Academics and curriculum</option>
					<option value="EX">Extra curricular activities</option>
					<option value="GC">Guidance central</option>
					<option value="AB">About our school</option>
				</select></td>
		</tr>
		<tr>
			<td align="right" width="134"><img src="../../images/4.gif" width="53" height="45"></td>
			<td width="372"><input type="submit" value="Submit and view your new article"></td>
		</tr>
	</table>
	<br>
	<br>
	<br>
</form>
<%
}
	IncludeFile("ContentFooter.htm", true, null);
%>

<p>&nbsp;

</body>

</html>
