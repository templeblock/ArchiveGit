<%
///////////////////////////////////////////////////////////////////////////////
function GetUniqueFileName()
{
	var date = new Date;
	var mo  = (date.getMonth()+1 + 100).toString(10).substr(1);
	var dd  = (date.getDay()+1   + 100).toString(10).substr(1);
	var yy  = (date.getYear()    + 100).toString(10).substr(1);
	var hh  = (date.getHours()   + 100).toString(10).substr(1);
	var mi  = (date.getMinutes() + 100).toString(10).substr(1);
	var ss  = (date.getSeconds() + 100).toString(10).substr(1);
	var mil = (date.getMilliseconds() + 1000).toString(10).substr(1);
	var strName = mo + dd + yy + hh + mi + ss + mil;
	return strName;
}

///////////////////////////////////////////////////////////////////////////////
function WriteOutSectionLinks(idSchool, idSection)
{
	strArguments = '?ids=' + idSchool + '&amp;sec=';
	strContentRef = '..\\All\\Content.asp' + strArguments;
	strArguments += idSection;
	
	Response.Write('<p><font color="#800000" size="3" face="Arial Narrow">');
	Response.Write('| <a href="' + strContentRef + 'HE">Headlines</a> ');
	Response.Write('| <a href="' + strContentRef + 'EV">Upcoming events</a> ');
	Response.Write('| <a href="' + strContentRef + 'CA">Calendar and scheduling</a> ');
	Response.Write('| <a href="' + strContentRef + 'ST">Student life</a> ');
	Response.Write('| <a href="' + strContentRef + 'FA">Faculty and staff</a> ');
	Response.Write('|<br>');
	Response.Write('| <a href="' + strContentRef + 'AC">Academics and curriculum</a> ');
	Response.Write('| <a href="' + strContentRef + 'EX">Extra curricular activities</a> ');
	Response.Write('| <a href="' + strContentRef + 'GC">Guidance central</a> ');
	Response.Write('| <a href="' + strContentRef + 'AB">About our school</a> ');
	Response.Write('|<br>');
//	Response.Write('| <a href="Search.htm' + strArguments + '">Search</a> ');
	Response.Write('| <a href="..\\All\\Submit.asp' + strArguments + '">Submit</a> ');
	Response.Write('|</font></p>');
}

///////////////////////////////////////////////////////////////////////////////
function IncludeFile(strFilename, bBodyOnly, objOutputFile, bSilent)
{
	var objFSO = new ActiveXObject("Scripting.FileSystemObject");
	if (!objFSO)
		return;

	// Check to be sure the file exists
	strFilename = Server.MapPath(strFilename);
	if (!objFSO.FileExists(strFilename))
	{
		if (!bSilent)
		{
			if (objOutputFile)
				objOutputFile.Write(strFilename + " does not exist")
			else
				Response.Write(strFilename + " does not exist");
		}
		objFSO = null;
		return;
	}

	// Open the file to be included
	var kForReading = 1, kForWriting = 2, kForAppending = 8;
	var kOverwrite = true;
	var kAllowCreation = true;
	var kDontAllowCreation = false;

	// Open the file
	var objOTF = objFSO.OpenTextFile(strFilename, kForReading, kDontAllowCreation );
	if (!objOTF)
		return;

	if (bBodyOnly)
	{
		// Copy the file, line by line, only in between the BODY tags
		var bBodyFound = false;
		while (!objOTF.AtEndOfStream)
		{
			var strInclude = objOTF.ReadLine();
			var strTest = strInclude.toLowerCase();
			strTest = Server.HTMLEncode(strTest );
			if (strTest.indexOf(Server.HTMLEncode("</body")) >= 0)
				bBodyFound = false;
			else
			if (strTest.indexOf(Server.HTMLEncode("<body")) >= 0)
				bBodyFound = true;
			else
			if (bBodyFound)
			{
				if (objOutputFile)
					objOutputFile.Write(strInclude);
				else
					Response.Write(strInclude);
			}
		}
	}
	else
	{
		// Copy the file in one big block read/write
		var strInclude = objOTF.ReadAll();
		if (objOutputFile)
			objOutputFile.Write(strInclude);
		else
			Response.Write(strInclude);
	}

	objOTF.Close();
	objOTF = null;
	objFSO = null;
}

///////////////////////////////////////////////////////////////////////////////
function FixSchoolID(idSchool)
{
	if (idSchool != "AH" &&
	    idSchool != "BA" &&
	    idSchool != "DO" &&
	    idSchool != "SA" &&
	    idSchool != "SH" &&
	    idSchool != "SO" &&
	    idSchool != "WE" &&
	    idSchool != "WM" &&
	    idSchool != "DI")
		return "DI";

	return idSchool;
}

///////////////////////////////////////////////////////////////////////////////
function FixSectionID(idSection)
{
	if (idSection != "AB" &&
	    idSection != "EV" &&
	    idSection != "CA" &&
	    idSection != "ST" &&
	    idSection != "FA" &&
	    idSection != "AC" &&
	    idSection != "EX" &&
	    idSection != "GC" &&
	    idSection != "HE")
		return "HE";

	return idSection;
}

///////////////////////////////////////////////////////////////////////////////
function SchoolID2Name(idSchool)
{
	var strSchoolName;
	if (idSchool == "AH")	strSchoolName = "AndoverHigh";					else
	if (idSchool == "BA")	strSchoolName = "Bancroft";						else
	if (idSchool == "DO")	strSchoolName = "Doherty";						else
	if (idSchool == "SA")	strSchoolName = "Sanborn";						else
	if (idSchool == "SH")	strSchoolName = "Shawsheen";						else
	if (idSchool == "SO")	strSchoolName = "South";							else
	if (idSchool == "WE")	strSchoolName = "WestEl";							else
	if (idSchool == "WM")	strSchoolName = "WestMiddle";					else
	if (idSchool == "DI")	strSchoolName = "District";						else
					strSchoolName = "District";
	return strSchoolName;
}

///////////////////////////////////////////////////////////////////////////////
function SectionID2Name(idSection)
{
	var strSectionName
	if (idSection == "AB")	strSectionName = "About";							else
	if (idSection == "EV")	strSectionName = "Events";						else
	if (idSection == "CA")	strSectionName = "Calendar";						else
	if (idSection == "ST")	strSectionName = "Students";						else
	if (idSection == "FA")	strSectionName = "Faculty";						else
	if (idSection == "AC")	strSectionName = "Academics";					else
	if (idSection == "EX")	strSectionName = "ExtraCurr";					else
	if (idSection == "GC")	strSectionName = "Guidance";						else
	if (idSection == "HE")	strSectionName = "Headlines";					else
					strSectionName = "Headlines";
	return strSectionName;
}

///////////////////////////////////////////////////////////////////////////////
function SectionID2Title(idSection)
{
	var strSectionTitle;
	if (idSection == "AB")	strSectionTitle = "About our school";				else
	if (idSection == "EV")	strSectionTitle = "Upcoming events";				else
	if (idSection == "CA")	strSectionTitle = "Calendar and scheduling";		else
	if (idSection == "ST")	strSectionTitle = "Student life";					else
	if (idSection == "FA")	strSectionTitle = "Faculty and staff";			else
	if (idSection == "AC")	strSectionTitle = "Academics and curriculum";	else
	if (idSection == "EX")	strSectionTitle = "Extra curricular activities";else
	if (idSection == "GC")	strSectionTitle = "Guidance central";				else
	if (idSection == "HE")	strSectionTitle = "Headlines";						else
								strSectionTitle = "Headlines";
	return strSectionTitle;
}

///////////////////////////////////////////////////////////////////////////////
function ListAllUsers(db)
{
	var ors = db.Execute("select * from users"); 
	%>
<table border="1">
	<tr>
		<td>
			<%
	while (!ors.eof)
	{
			%>
	<tr>
		<%
		for (i=0; i<ors.fields.count; i++)
		{
		%>
		<td valign="top"><%= ors(i)%></td>
		<%
		}
		%>
	</tr>
	<%
		ors.MoveNext();
	}

	%>
</table>
<%
	ors.close(); 
} 

///////////////////////////////////////////////////////////////////////////////
function ValidateUser(strUserName, strPassword, strDatabasePath)
{
	var db = Server.CreateObject("ADODB.Connection");
	if (db == null)
		return "Couldn't create a database object.";
	db.Open("DRIVER={Microsoft Access Driver (*.mdb)}; DBQ=" + Server.Mappath(strDatabasePath));
	//	return "Couldn't open database file '" + Server.Mappath(strDatabasePath) + "'.";

	//ListAllUsers(db);
	var sqltemp = "select * from users where User='" + strUserName + "' and Password='" + strPassword + "'";
	var RecordSet = db.Execute(sqltemp);

	var strReturnedMessage = "";

	if (RecordSet.eof)
	{
		strReturnedMessage = "There is no user '" + strUserName + "' with that password on file.  Please try again.";
	}
	//else
	//{
	//	Session("User") = RecordSet("User")();
	//	Session("SecurityLevel") = RecordSet("SecurityLevel")();
	//}

	RecordSet.Close();
	db.Close();
	return strReturnedMessage;
}

///////////////////////////////////////////////////////////////////////////////
function FindImageReference(strFilename, iCount)
{
	if (iCount <= 0)
		return "";
		
	var objFSO = new ActiveXObject("Scripting.FileSystemObject");
	if (!objFSO)
		return "";

	// Check to be sure the file exists
	strFilename = Server.MapPath(strFilename);
	if (!objFSO.FileExists(strFilename))
	{
		Response.Write("<br>" + strFilename + " does not exist");
		objFSO = null;
		return "";
	}

	// Open the file to be included
	var kForReading = 1, kForWriting = 2, kForAppending = 8;
	var kDontAllowCreation = false;

	// Open the file
	var objOTF = objFSO.OpenTextFile(strFilename, kForReading, kDontAllowCreation);
	if (!objOTF)
	{
		Response.Write("<br>" + strFilename + " could not be read");
		objFSO = null;
		return "";
	}

	// Read the file, line by line, to check for "img src=" tags
	var strContent = "";
	var i = 0;
	while (!objOTF.AtEndOfStream)
	{
		i++;
		strContent = objOTF.ReadLine();
		strContent.toUpperCase();
		var iSrc = strContent.indexOf("SRC=");
		if (iSrc <= 0)
			continue;
		// Found one!
		if (--iCount == 0)
			break;
	}

	objOTF.Close();
	objOTF = null;
	objFSO = null;

	if (iCount > 0)
		return "";
				
	strContent = strContent.substr(iSrc+1);
	var iOpenQuote = strContent.indexOf('"');
	strContent = strContent.substr(iOpenQuote+1);
	var iCloseQuote = strContent.indexOf('"');
	strContent = strContent.substr(0, iCloseQuote);
	return strContent;
}
%>
