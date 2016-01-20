// Example: 
//	if (!window.dialogArguments || window.dialogArguments == "")
//		window.dialogArguments = "button=OK;button=Cancel;icon=IconQuestion.gif;title=Your Application;message=Your message.;";

var strLastButton = null;
var bOK = false;
var bYes = false;
var bNo = false;
var bAbort = false;
var bRetry = false;
var bIgnore = false;
var bCancel = false;

document.onkeydown = KeyDown;

function KeyDown(e)
{
	try
	{
		var iKey = event.keyCode;
		if (!iKey)
			return;

		if (iKey == 79 & bOK)				ReturnValue('OK');			else
		if (iKey == 89 & bYes)				ReturnValue('Yes');			else
		if (iKey == 78 & bNo)				ReturnValue('No');			else
		if (iKey == 65 & bAbort)			ReturnValue('Abort');		else
		if (iKey == 82 & bRetry)			ReturnValue('Retry');		else
		if (iKey == 73 & bIgnore)			ReturnValue('Ignore');		else
		if (iKey == 67 & bCancel)			ReturnValue('Cancel');		else
		if (iKey == 27 && strLastButton)	ReturnValue(strLastButton);
	}
	catch(e)
	{
		//alert('KeyDown exception');
	}
}

function AddButtons() 
{
	try
	{
		if (!window.dialogArguments)
			return;

		var strArgs = window.dialogArguments.toString();
		var args = strArgs.split(";");
		for (nIndex = 0; nIndex < args.length; nIndex++)
		{
			var params = args[nIndex].split("=");
			if (params[0] == 'button')
			{
				var strButton = params[1];
				AddButton(strButton);
				strLastButton = strButton;
				if (strButton == 'OK')		bOK		= true; else
				if (strButton == 'Yes')		bYes	= true; else
				if (strButton == 'No')		bNo		= true; else
				if (strButton == 'Abort')	bAbort	= true; else
				if (strButton == 'Retry')	bRetry	= true; else
				if (strButton == 'Ignore')	bIgnore	= true; else
				if (strButton == 'Cancel')	bCancel	= true;
			}
		}
	}
	catch(e)
	{
		//alert('AddButtons exception');
	}
}

function AddButton(name) 
{
	var key = name.substr(0,1);
	document.write('<td valign="middle">'																				);
	document.write('	<input type="button" accesskey="' + key + '"'									);
	document.write('		id="c' + name + '"'															);
	document.write('		value="' + name + '"'														);
	document.write('		onclick="ReturnValue(\'' + name + '\');"'									);
	document.write('		style="width:80px; color:#000000; font-size:12px; font-family:arial; font-weight:400"'	);
	document.write('	/>'																				);
	document.write('</td>'																				);
}

function Load() 
{
	try
	{
		var arg = null;
		arg = GetControlValue('title');
		if (arg && document.all.title)
			document.all.title.innerText = arg;
		arg = GetControlValue('message');
		if (arg)
			document.all.message.innerText = arg;
		arg = GetControlValue('icon');
		if (arg && document.all.icon)
			document.all.icon.src = arg;
	}
	catch(e)
	{
		//alert('Load exception');
	}
}

function ReturnValue(name)
{
	try
	{
		window.returnValue = name;
		window.close();
	}
	catch(e)
	{
		//alert('ReturnValue exception');
	}
}

function GetControlValue(argname)
{
	try
	{
		if (!window.dialogArguments)
			return null;

		var strArgs = window.dialogArguments.toString();
		var args = strArgs.split(";");
		for (nIndex = 0; nIndex < args.length; nIndex++)
		{
			var params = args[nIndex].split("=");
			var bMatch = (argname == params[0]);
			if (bMatch)
				return params[1];
		}
	}
	catch(e)
	{
		//alert('GetControlValue exception');
	}

	return null;
}
