var ForReading = 1, ForWriting = 2, ForAppending = 8;
var ForTargetOnly = 1;

function alert(Message)
{
	WScript.Echo(Message);
}

//	fso.GetParentFolderName(filepath)	Path, no Filename, no trailing slash
//	fso.GetDriveName(filepath)			\\Gromit\wallaby (empty for http path)
//	fso.GetBaseName(filepath)			Filename, no extension
//	fso.GetAbsolutePathName(filepath)	Full path (weird for http path)
//	fso.GetExtensionName(filepath)		Extension
//	fso.GetFile(filepath)				Full path (File not found for http path)
//	fso.GetFileName(filepath)			Filename with extension

function OpenFSO()
{
	try
	{
		var fso = new ActiveXObject('Scripting.FileSystemObject');
		if (!fso)
			throw '';
		return fso;
	}
	catch (error)
	{
		var FsoError = 'Your security settings are preventing access to the file utilities.  To correct this problem, use the Internet Explorer Tools menu to access the InternetOptions/Security/LocalIntranet/DefaultLevel button.  Then set the Security Level slider to "Low".';
		alert(error.description + '\n\n' + FsoError);
		return null;
	}
}

function OpenSaveDialog(bSave, Filter, DefaultExt, InitDir)
{
	// Filter is of the form: 'Text Settings Files (*.txt)|*.txt|All Files (*.*)|*.*';
	var dialog = null;
	try
	{
	//j	dialog = new ActiveXObject('Excel.Application');
	//j	dialog = document.dialog;
		dialog = new ActiveXObject('MSComDlg.CommonDialog');
		if (!dialog)
			throw '';
	}
	catch (error)
	{
		alert('The common dialogs do not exist');
		return null;
	}

	// Some common dialog constants
	var OFN_HIDEREADONLY = 0x4;
	var OFN_CREATEPROMPT = 0x2000;
	var OFN_EXPLORER = 0x80000;
	var OFN_LONGNAMES = 0x200000;
	
	// Setup the dialog parameters
	dialog.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_CREATEPROMPT | OFN_HIDEREADONLY;
	dialog.MaxFileSize = 260;		// allocate space for file name
	dialog.InitDir = InitDir;		// set opening directory
	dialog.DefaultExt = DefaultExt;	// default extension
	dialog.Filter = Filter;			// show file types
	dialog.FilterIndex = 1;			// show all files
	dialog.FileName = '';			// set default filename

	if (bSave)
		dialog.ShowSave();
	else
		dialog.ShowOpen();

	// Retrieve the result
  	var filepath = dialog.FileName;
	if (!filepath)
		filepath = '';
	return filepath;
}

function DeleteFolder(fso, Folder)
{
	try
	{
		if (fso.FolderExists(Folder))
			var f = fso.DeleteFolder(Folder);
		return true;
	}
	catch (error)
	{
		var FsoError = 'Could not create Folder "' + Folder + '"';
		alert(error.description + '\n\n' + FsoError);
		return false;
	}
}

function MakeFolder(fso, Folder)
{
	try
	{
		if (!fso.FolderExists(Folder))
			var f = fso.CreateFolder(Folder);
		return true;
	}
	catch (error)
	{
		var FsoError = 'Could not create Folder "' + Folder + '"';
		alert(error.description + '\n\n' + FsoError);
		return false;
	}
}

function CopyFile(fso, Source, Destination)
{
	try
	{
		if (fso.FileExists(Destination))
			return true;
		fso.CopyFile(Source, Destination, true/*bOverwrite*/);
		return true;
	}
	catch (error)
	{
		var FsoError = 'Could not copy "' + Source + '" to "' + Destination + '"';
		alert(error.description + '\n\n' + FsoError);
		return false;
	}
}

function FilterQuotes(str)
{
	if (str.indexOf('"') < 0)
		return str;

//j	alert('FilterQuotes before: ' + str);
	var regexp = /""/gi;
	str = str.replace(regexp, "'");
	regexp = /"/gi;
	str = str.replace(regexp, "");
//j	alert('FilterQuotes after: ' + str);
	return str;
}

function Trim(str)
{
	// Match spaces at beginning and end of text and replace with null strings
	return str.replace(/^\s+/,'').replace(/\s+$/,'');
}

function FilterFileName(FileName)
{
	Trim(FileName);
	// Remove any illegal UNC filename characters
	var regexp = /[?*|":<>\\\/\t]/gi;
	return FileName.replace(regexp, '-');
}

function WriteEmptyFile(fso, FileName)
{
	try
	{
		var tso = fso.OpenTextFile(FileName, ForWriting, true/*bCreate*/);
		tso.WriteLine('');
		tso.Close();
		return true;
	}
	catch (error)
	{
		var FsoError = 'Could not write to file "' + FileName + '"';
		alert(error.description + '\n\n' + FsoError);
		return false;
	}
}

function WriteXML(fso, FileName, PRODUCT_NUMBER, REPORT_TYPE, TITLE, PRODUCT_SEQUENCE, PATHNUMBER, SMALL_X, SMALL_Y, LARGE_X, LARGE_Y, INSIDEVERSE)
{
	try
	{
		if (!fso.FileExists(FileName))
		{
			var tso = fso.OpenTextFile(FileName, ForWriting, true/*bCreate*/);
			tso.WriteLine('<?xml version="1.0" encoding="iso-8859-1" ?>');
			tso.WriteLine('<?xml-stylesheet type="text/xsl" href="../../thumbs.xsl" ?>');
			tso.WriteLine('<!--Copyright 2004 AmericanGreetings.com-->');
			tso.WriteLine('<thumbs title="Category ' + PATHNUMBER + '">');
			tso.WriteLine('</thumbs>');
			tso.Close();
		}

		var TemporaryFolder = 2;
		var tfolder = fso.GetSpecialFolder(TemporaryFolder);
		var NewFileName = tfolder.Path + '\\' + fso.GetTempName();

		var tso = fso.OpenTextFile(FileName, ForReading, false/*bCreate*/);
		var tso_new = fso.OpenTextFile(NewFileName, ForWriting, true/*bCreate*/);

		// Copy the file line by line - do not write the last line
		while (!tso.AtEndOfStream)
		{  
			var Line = tso.ReadLine();
			if (!tso.AtEndOfStream)
				tso_new.WriteLine(Line);
		}

		if (REPORT_TYPE == 'CD')
		{ // Labels
			INSIDEVERSE = 
			'<b>CD/DVD Label Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'If you choose to print 2 copies, the same design will print twice on the page--filling both CD/DVD label stickers.<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your labels on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'LB')
		{ // Labels
			INSIDEVERSE = 
			'<b>Label Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'If you choose to print multiple copies, the same design will be repeated for each labels on the page.<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your labels on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'NC' || REPORT_TYPE == 'HP')
		{ // Notecards
			INSIDEVERSE = 
			'<b>Notecard Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your cards on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'HL' && ForTargetOnly == 1)
		{ // Holiday Cards deleted Borderless help 
			INSIDEVERSE = 
			'<b>4 by 8 Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.<br/><br/>' + 
			'3. Double click on the Add-a-Photo message (within the workspace) to add your digital photo.<br/><br/>' + 
			'4. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your cards on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'HL' && ForTargetOnly == 0)
		{ // Holiday Cards
			INSIDEVERSE = 
			'<b>4 by 8 Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.<br/><br/>' + 
			'3. Double click on the Add-a-Photo message (within the workspace) to add your digital photo.<br/><br/>' + 
			'4. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>For Borderless Designs:</b>  ' + 
			'Click on the Print button, click the Preferences button, and within your specific printer driver, find and check the Borderless Printing option. (Note: not all printers support borderless printing.  See Help for details).<br/><br/>' +
			'<b>Tips:</b>  ' + 
			'Print your cards on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'BE')
		{ // Business Cards
			INSIDEVERSE = 
			'<b>Business Card Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your business cards on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'BM')
		{ // Bookmarks
			INSIDEVERSE = 
			'<b>Bookmark Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your bookmarks on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'CR')
		{ // Certificates
			INSIDEVERSE = 
			'<b>Certificate Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your projects on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'GS')
		{ // Gift Tags
			INSIDEVERSE = 
			'<b>Gift Tag Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your gift tags on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'IT' || REPORT_TYPE == 'TS')
		{ // Iron-on Transfers
			INSIDEVERSE = 
			'<b>Iron-on Transfer Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your iron-ons on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'S1' ||
			REPORT_TYPE == 'S2' ||
			REPORT_TYPE == 'S3' ||
			REPORT_TYPE == 'S4' ||
			REPORT_TYPE == 'S5')
		{ // Scrapbooks
			INSIDEVERSE = 
			'<b>Scrapbook Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your scrapbook pages on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'SA')
		{ // Stationery
			INSIDEVERSE = 
			'<b>Stationery Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your stationery on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'XA')
		{ // Announcements
			INSIDEVERSE = 
			'<b>Announcement Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your announcements on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'XE')
		{ // Envelopes
			INSIDEVERSE = 
			'<b>Envelope Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your envelope on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		else
		if (REPORT_TYPE == 'XI')
		{ // Invitations
			INSIDEVERSE = 
			'<b>Invitation Instructions:</b><br/><br/>' + 
			'1. To personalize and print this project, click on the preview image to the left to open the workspace.<br/><br/>' + 
			'2. Personalize the text and graphics.  Select the text to change it or its size, color, and font.  Even add digital photos!<br/><br/>' + 
			'3. Click on the Print button, select the number of copies you\'d like to print and go!.  ' + 
			'<br/><br/>' + 
			'<b>Tips:</b>  ' + 
			'Print your invitations on plain paper first to check the alignment. spelling. and overall look of the project. <br/><br/>';
		}
		
		TITLE = TITLE.replace(/&/gi, escape('&'));
		TITLE = TITLE.replace(/</gi, escape('<'));
		TITLE = TITLE.replace(/>/gi, escape('>'));

		INSIDEVERSE = INSIDEVERSE.replace(/&/gi, escape('&'));
		INSIDEVERSE = INSIDEVERSE.replace(/</gi, escape('<'));
		INSIDEVERSE = INSIDEVERSE.replace(/>/gi, escape('>'));

		tso_new.WriteLine('	<thumb name="' + TITLE + 
			'" product="' + PRODUCT_NUMBER + 
			'" type="' + REPORT_TYPE + 
			'" seq="' + PRODUCT_SEQUENCE + 
			'" tw="' + SMALL_X + 
			'" th="' + SMALL_Y + 
			'" pw="' + LARGE_X + 
			'" ph="' + LARGE_Y + 
			'" verse="' + INSIDEVERSE + '" />');
		tso_new.WriteLine('</thumbs>');
		tso_new.Close();
		tso.Close();

		fso.DeleteFile(FileName);
		fso.MoveFile(NewFileName, FileName);
		return true;
	}
	catch (error)
	{
		var FsoError = 'Could not write to file "' + FileName + '"';
		alert(error.description + '\n\n' + FsoError);
		return false;
	}
}

function FileCountLines(FileName)
{
	var fso = OpenFSO();
	if (!fso)
		return 0;

	var iCount = 0;
	var tso = fso.OpenTextFile(FileName, ForReading);
	if (!tso)
		return 0;

	while (!tso.AtEndOfStream)
	{
		var Line = tso.ReadLine();
		iCount++;
	}

	return iCount;
}

function ProcessCSV(FileName)
{
	var fso = OpenFSO();
	if (!fso)
		return;

	if (FileName)
	{
		if (!fso.FileExists(FileName))
		{
			alert('File ' + FileName + ' does not exist');
			FileName = null;
		}
	}

	if (!FileName)
	{
		FileName = OpenSaveDialog(false/*bSave*/,
			'Comma Separated Values Files (*.csv)|*.csv|All Files (*.*)|*.*'/*Filter*/,
			'txt'/*DefaultExt*/,
			'c:\\My Projects\\AgApplications\\'/*InitDir*/);
		if (!FileName)
			return;
	}

	var MaxLines = FileCountLines(FileName);
	var MaxFields = 15;

	var tso = fso.OpenTextFile(FileName, ForReading);

	// Skip first line which contains format definition
	tso.SkipLine(); 

	// Compute the output folder
	var RootFolder = FileName;
	var index = RootFolder.lastIndexOf('\\');
	index = RootFolder.lastIndexOf('\\', index-1);
	RootFolder = RootFolder.substr(0, index+1);

	// Read the file line by line
	var ProgressPage = RootFolder + 'ProgressDialog.htm';
	var Progress = new ProgressDialog(ProgressPage);
	Progress.SetHeading('Offline Content Builder');

	var ContentFolder = RootFolder + 'content';
	if (!MakeFolder(fso, ContentFolder))
		return;

	ContentFolder += '\\pack';
	if (!MakeFolder(fso, ContentFolder))
		return;

	var LinelistFolder = ContentFolder + '\\linelist';
	if (!DeleteFolder(fso, LinelistFolder))
		return;
	if (!MakeFolder(fso, LinelistFolder))
		return;

	var ThumbslistFolder = ContentFolder + '\\thumbslist';
	if (!DeleteFolder(fso, ThumbslistFolder))
		return;
	if (!MakeFolder(fso, ThumbslistFolder))
		return;

	var ProductFolder = ContentFolder + '\\product';
	if (!MakeFolder(fso, ProductFolder))
		return;

	while (!tso.AtEndOfStream)
	{  
		var Line = tso.ReadLine();
		
		// Change the comma delimiter to a tilde, except within double quotes
		var bQuoted = false;
		var NewLine = '';
		for (x = 0; x < Line.length; x++) 
		{
			var ch = Line.charAt(x);
			if (ch == '\"')
				bQuoted = !bQuoted;
			else
			if (ch == ',' && !bQuoted)
				ch = '~';
			NewLine += ch;
		}
		
		var Fields = NewLine.split('~');
		if (Fields.length != MaxFields)
		{
			alert('The following record has ' + Fields.length + ' fields.  It should have ' + MaxFields + '.\n\n' + Line);
			continue;
		}

		var L1				= Fields[1];
		var L2				= Fields[2];
		var L3				= Fields[3];
		var L4				= Fields[4];
		var L5				= Fields[5];
		var PRODUCT_NUMBER	= Fields[6];
		var REPORT_TYPE		= Fields[0];
		var TITLE			= Fields[8];
		var PRODUCT_SEQUENCE= Fields[7];
		var PATHNUMBER		= Fields[9];
		var SMALL_X			= Fields[10];
		var SMALL_Y			= Fields[11];
		var LARGE_X			= Fields[12];
		var LARGE_Y			= Fields[13];
		var INSIDEVERSE		= Fields[14];
		
		L1 = FilterFileName(L1);
		L2 = FilterFileName(L2);
		L3 = FilterFileName(L3);
		L4 = FilterFileName(L4);
		L5 = FilterFileName(L5);

		// One problem with the CSV file is that double quotes will expand to double-double-quotes and cause the 
		// entire string to be wrapped with another set of double quotes. This function will take of that problem
		INSIDEVERSE = FilterQuotes(INSIDEVERSE);
		TITLE = FilterQuotes(TITLE);
		TITLE = TITLE.replace(/<br>/gi, ' ');
		TITLE = TITLE.replace(/&#133;/gi, '...');

		Progress.SetMessage(TITLE);
		Progress.SetProgress(tso.Line, MaxLines);

		var LinelistFolderDeep = LinelistFolder;

		if (L1 != '')
		{
			LinelistFolderDeep += '\\' + L1;
			if (!MakeFolder(fso, LinelistFolderDeep))
				break;
		}
		if (L2 != '')
		{
			LinelistFolderDeep += '\\' + L2;
			if (!MakeFolder(fso, LinelistFolderDeep))
				break;
		}
		if (L3 != '')
		{
			LinelistFolderDeep += '\\' + L3;
			if (!MakeFolder(fso, LinelistFolderDeep))
				break;
		}
		if (L4 != '')
		{
			LinelistFolderDeep += '\\' + L4;
			if (!MakeFolder(fso, LinelistFolderDeep))
				break;
		}
		if (L5 != '')
		{
			LinelistFolderDeep += '\\' + L5;
			if (!MakeFolder(fso, LinelistFolderDeep))
				break;
		}

		var FileName;
		FileName = LinelistFolderDeep + '\\' + PATHNUMBER;
		if (!WriteEmptyFile(fso, FileName))
			break;

		FileName = ThumbslistFolder + '\\' + PATHNUMBER + '.xml';
		if (!WriteXML(fso, FileName, PRODUCT_NUMBER, REPORT_TYPE, TITLE, PRODUCT_SEQUENCE, PATHNUMBER, SMALL_X, SMALL_Y, LARGE_X, LARGE_Y, INSIDEVERSE))
			break;
		
		var ProductFolderDeep = ProductFolder + '\\' + PRODUCT_NUMBER;
		if (!MakeFolder(fso, ProductFolderDeep))
			break;

		var ContentRoot = 'V:\\websitesimg\\american\\html\\product\\';
		var Source, Destination;

		FileName = '\\' + PRODUCT_NUMBER + 'f.ctp';
		Source = ContentRoot + 'createprint' + FileName;
		Destination = ProductFolderDeep + FileName;
		CopyFile(fso, Source, Destination);

		FileName = '\\' + PRODUCT_NUMBER + 't.gif';
		Source = ContentRoot + 'thumbs' + FileName;
		Destination = ProductFolderDeep + FileName;
		CopyFile(fso, Source, Destination);

		FileName = '\\' + PRODUCT_NUMBER + 'd.gif';
		Source = ContentRoot + '180278' + FileName;
		Destination = ProductFolderDeep + FileName;
		CopyFile(fso, Source, Destination);
	}

	tso.Close();
	Progress.Close();
	Progress = null;
	alert('Finished');
}

function ProgressDialog(ProgressPage)
{
	this.READYSTATE_COMPLETE = 4;  // from enum of tagREADYSTATE

	// Instantiate IE
	this.oIE = WScript.CreateObject('InternetExplorer.Application', 'oIE_');
	this.oIE.Left = 40;
	this.oIE.Top = 80;
	this.oIE.Width = 400;
	this.oIE.Height = 205;
	this.oIE.MenuBar = false;
	this.oIE.ToolBar = false;
	this.oIE.StatusBar = false;
	this.oIE.Navigate2(ProgressPage);

	try
	{
		// Wait for html page to load
		while (this.oIE.ReadyState != this.READYSTATE_COMPLETE)
			WScript.Sleep(100);

		// When ready, get the document object
		while (!this.oIE.document || !this.oIE.document.body)
			;

		this.oDoc = this.oIE.document;
		this.oIE.Visible = true;
	}
	catch (error)
	{
		alert(error.description);
	}

	this.SetHeading = function(Heading)
	{
		try
		{
			this.oDoc.all.item('Heading').innerHTML = Heading;
			return true;
		}
		catch (error)
		{
		//j	alert(error.description);
			return false;
		}
	}
	this.SetMessage = function(Message)
	{
		try
		{
			this.oDoc.all.item('Message').innerHTML = Message;
			return true;
		}
		catch (error)
		{
		//j	alert(error.description);
			return false;
		}
	}

	this.SetProgress = function(min, max)
	{
		try
		{
			if (min > max) min = max;
			var iPct = parseInt(((100 * min) + max/2) / max);
			if (!this.ProgressBarWidth)
				this.ProgressBarWidth = 250;
			this.oDoc.all.item('ProgressBar').style.width = (min * this.ProgressBarWidth) / max;  // Set the progress bar width
			this.oDoc.all.item('PercentComplete').innerHTML = iPct + '% Complete';
			return true;
		}
		catch (error)
		{
		//j	alert(error.description);
			return false;
		}
	}
	this.Close = function()
	{
		try
		{
			// Close IE
			this.oIE.Visible = false;
			this.oDoc = null;
			this.oIE = null;
			return true;
		}
		catch (error)
		{
			alert(error.description);
			return false;
		}
	}
}

ProcessCSV(null);

WScript.Quit();
