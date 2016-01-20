<%
function GetFileContents(strFileName)
{
	var strFileContents = null;
	try
	{
		var objFSO = Server.CreateObject('Scripting.FileSystemObject');
		// Open the file and pass it to a TextStream Object (objText).
		// The 'MapPath' function of the Server Object is used to get the physical path for the file.
		var objText = objFSO.OpenTextFile(Server.MapPath(strFileName));

		// Read and return the contents of the file as a string.
		strFileContents = objText.ReadAll();

		objText.Close();
		objText = null;
		objFSO = null;
	}

	catch(e)
	{
		strFileContents = null;
	}
	return strFileContents;
}

function FileExists(strFileName)
{
	try
	{
		var objFSO = Server.CreateObject('Scripting.FileSystemObject');
		// Open the file and pass it to a TextStream Object (objText).
		// The 'MapPath' function of the Server Object is used to get the physical path for the file.
		var bExists = objFSO.FileExists(Server.MapPath(strFileName));
		objFSO = null;
		return bExists;
	}

	catch(e)
	{
		return false;
	}

	return false;
}
%>
