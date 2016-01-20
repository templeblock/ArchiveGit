// Example: 
//	if (!window.dialogArguments || window.dialogArguments == "")
//		window.dialogArguments = "button=OK;button=Cancel;icon=IconQuestion.gif;title=Your Application;message=Your message.;";

var strFirstButton = null;
var strLastButton = null;
var bOK = false;
var bYes = false;
var bNo = false;
var bAbort = false;
var bRetry = false;
var bIgnore = false;
var bCancel = false;

document.onkeydown = KeyDown;

function SetShadowColor(name, clr)
{
	for (var x=10; x<20; x++)
	{
		eval('document.all.c' + x + name + '.style.backgroundColor="' + clr + '"');
	}
}
			
function SetFaceColor(name, clr)
{
	for (var x=1; x<10; x++)
	{
		eval('document.all.c' + x + name + '.style.backgroundColor="' + clr + '"');
	}
}

function OverHP(name)
{
	SetFaceColor(name, "#5391cd");
	SetShadowColor(name, "#206ebf");
}

function OutHP(name)
{
	var ShadowClr = "#ffffff";
	var FaceClr = "#ffffff";
	
	if (name == "OK" || name == "Yes" || name == "Abort")
	{
		FaceClr = "#335c8f";
		ShadowClr = "#003366";
	}
	else
	{
		FaceClr = "#7591ac";
		ShadowClr = "#345c85";
	}
	SetFaceColor(name, FaceClr);
	SetShadowColor(name, ShadowClr);
}
			
function KeyDown(e)
{
	var iKey = event.keyCode;
	if (!iKey)
		return;

	if (iKey == 32)
		iKey = 13;

	if (iKey == 79 & bOK)				ReturnValue('OK');			else
	if (iKey == 89 & bYes)				ReturnValue('Yes');			else
	if (iKey == 78 & bNo)				ReturnValue('No');			else
	if (iKey == 65 & bAbort)			ReturnValue('Abort');		else
	if (iKey == 82 & bRetry)			ReturnValue('Retry');		else
	if (iKey == 73 & bIgnore)			ReturnValue('Ignore');		else
	if (iKey == 67 & bCancel)			ReturnValue('Cancel');		else
	if (iKey == 27 && strLastButton)	ReturnValue(strLastButton);	else
	if (iKey == 13 && strFirstButton)	ReturnValue(strFirstButton);
}

function AddButtonsHP()
{
	if (!window.dialogArguments)
		return;
	var args = new Array();
	if (!args)
		return;
	var params = new Array();
	if (!params)
		return;

	args = window.dialogArguments.split(";");
	for (nIndex = 0; nIndex < args.length; nIndex++)
	{
		params = args[nIndex].split("=");
		if (params[0] == 'button')
		{
			var strButton = params[1];
			AddButtonHP(strButton);
			strLastButton = strButton;
			if (!strFirstButton)
				strFirstButton = strButton;
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

function AddButtons() 
{
	if (!window.dialogArguments)
		return;
	var args = new Array();
	if (!args)
		return;
	var params = new Array();
	if (!params)
		return;

	args = window.dialogArguments.split(";");
	for (nIndex = 0; nIndex < args.length; nIndex++)
	{
		params = args[nIndex].split("=");
		if (params[0] == 'button')
		{
			var strButton = params[1];
			AddButton(strButton);
			strLastButton = strButton;
			if (!strFirstButton)
				strFirstButton = strButton;
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

function AddButtonHP(name)
{
	var ShadowClr = "ffffff";
	var FaceClr = "ffffff";
				
	if (name == "OK" || name == "Yes" || name == "Abort")
	{
		FaceClr = "335c8f";
		ShadowClr = "003366";
	}
	else
	{
		FaceClr = "7591ac";
		ShadowClr = "345c85";
	}
	
	document.write('<td>																			');
	document.write('<table width="80" height="20" cellpadding="0" cellspacing="0" border="0">		');
	document.write('	<tr height="1">																');
	document.write('		<td bgcolor="' + FaceClr + '" id="c1' + name + '" width="1"></td>		');
	document.write('		<td bgcolor="' + FaceClr + '" id="c2' + name + '" width="1"></td>		');
	document.write('		<td bgcolor="' + FaceClr + '" id="c3' + name + '"></td>					');
	document.write('		<td bgcolor="ffffff" width="1"></td>									');
	document.write('		<td bgcolor="ffffff" width="1"></td>									');
	document.write('	</tr>																		');
	document.write('	<tr height="1">																');
	document.write('		<td bgcolor="' + FaceClr + '" id="c4' + name + '" width="1"></td>		');
	document.write('		<td bgcolor="' + FaceClr + '" id="c5' + name + '" width="1"></td>		');
	document.write('		<td bgcolor="' + FaceClr + '" id="c6' + name + '"></td>					');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c10' + name + '" width="1"></td>	');
	document.write('		<td bgcolor="ffffff" width="1"></td>									');
	document.write('	</tr>																		');
	document.write('	<tr height="16">															');
	document.write('		<td bgcolor="' + FaceClr + '" id="c7' + name + '" width="1"></td>		');
	document.write('		<td bgcolor="' + FaceClr + '" id="c8' + name + '" width="1"></td>		');
	document.write('		<td bgcolor="' + FaceClr + '" id="c9' + name + '" align="center"		');
	document.write('			valign="center" tabindex="1" hidefocus="true"						');
	document.write('			onmouseover="OverHP(\'' + name + '\')"								');
	document.write('			onmouseout="OutHP(\'' + name + '\')"								');
	document.write('			onclick="ReturnValue(\'' + name + '\');">							');
	document.write('				<span id="' + name + '" style="{color:#ffffff;font-size:11px;font-family:arial;font-weight:600}">	');
	document.write('					' + name + '</span></td>									');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c11' + name + '" width="1"></td>	');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c12' + name + '" width="1"></td>	');
	document.write('	</tr>																		');
	document.write('	<tr height="1">																');
	document.write('		<td bgcolor="ffffff" width="1"></td>									');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c13' + name + '" width="1"></td>	');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c14' + name + '"></td>				');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c15' + name + '" width="1"></td>	');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c16' + name + '" width="1"></td>	');
	document.write('	</tr>																		');
	document.write('	<tr height="1">																');
	document.write('		<td bgcolor="ffffff" width="1"></td>									');
	document.write('		<td bgcolor="ffffff" width="1"></td>									');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c17' + name + '"></td>				');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c18' + name + '" width="1"></td>	');
	document.write('		<td bgcolor="' + ShadowClr + '" id="c19' + name + '" width="1"></td>	');
	document.write('	</tr>																		');
	document.write('</table>																		');
	document.write('</td>																			');
}

function AddButton(name) 
{
	document.write('<td>																			');
	document.write('<table width="80" height="24" cellpadding="0" cellspacing="0" border="0">		');
	document.write('	<tr height="1">																');
	document.write('		<td bgcolor="f4f4f8" colspan="3"></td>									');
	document.write('		<td bgcolor="000000" width="1"></td>									');
	document.write('	</tr>																		');
	document.write('	<tr height="21">															');
	document.write('		<td bgcolor="f4f4f8" width="1"></td>									');
	document.write('		<td bgcolor="d0d0d0" align="center" valign="center" id="c' + name + '"	');
	document.write('			tabindex="1" hidefocus="true"										');
	document.write('			onmouseover="Over(\'' + name + '\')"								');
	document.write('			onmouseout="Out(\'' + name + '\')"									');
	document.write('			onmousedown="Down(\'' + name + '\')"								');
	document.write('			onmouseup="Up(\'' + name + '\')"									');
	document.write('			onclick="ReturnValue(\'' + name + '\');"							');
	document.write('			><span id="' + name + '" style=										');
	document.write('			"{color:#000000;font-size:12px;font-family:arial;font-weight:400}">	');
	document.write('		' + name + '</span></td>												');
	document.write('		<td bgcolor="0099cc" width="1"></td>									');
	document.write('		<td bgcolor="000000" width="1"></td>									');
	document.write('	</tr>																		');
	document.write('	<tr height="1">																');
	document.write('		<td bgcolor="f4f4f8" width="1"></td>									');
	document.write('		<td bgcolor="0099cc" colspan="2"></td>									');
	document.write('		<td bgcolor="000000" colspan="1"></td>									');
	document.write('	</tr>																		');
	document.write('	<tr height="1">																');
	document.write('		<td bgcolor="000000" colspan="4"></td>									');
	document.write('	</tr>																		');
	document.write('</table>																		');
	document.write('</td>																			');
}

function Load() 
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

function UpEx(name, id)
{
	eval('document.all.' + id + '.src="' + name + '"');
}

function DownEx(name, id)
{
	eval('document.all.' + id + '.src="' + name + '"');
}

function OverEx(name, id)
{
	eval('document.all.' + id + '.src="' + name + '"');
}

function OutEx(name, id)
{
	eval('document.all.' + id + '.src="' + name + '"');
}

function Up(name)
{
	eval('document.all.c' + name + '.style.backgroundColor="#d0d0d0"');
}

function Down(name)
{
	eval('document.all.c' + name + '.style.backgroundColor="#0099cc"');
}

function Over(name)
{
	eval('document.all.c' + name + '.style.backgroundColor="#f4f4f8"');
	eval('document.all.c' + name + '.style.cursor="hand"');
	eval('document.all.' + name + '.style.marginLeft="4";');
}

function Out(name)
{
	eval('document.all.c' + name + '.style.backgroundColor="#d0d0d0"');
	eval('document.all.' + name + '.style.marginLeft="0";');
}

function ReturnValue(name)
{
	window.returnValue = name;
	window.close();
}

function GetControlValue(argname)
{
	if (!window.dialogArguments)
		return null;
	var args = new Array();
	if (!args)
		return null;
	var params = new Array();
	if (!params)
		return null;

	args = window.dialogArguments.split(";");
	for (nIndex = 0; nIndex < args.length; nIndex++)
	{
		params = args[nIndex].split("=");
		var bMatch = (argname == params[0]);
		if (bMatch)
			return params[1];
	}

	return null;
}
