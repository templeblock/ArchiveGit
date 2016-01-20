var g_strThisTopic = '';
var g_strBackTopic = '';
var g_strPrevTopic = '';
var g_strNextTopic = '';

function SetTopic(strTopic, strBackTopic)
{
	g_strThisTopic = strTopic;
	g_strBackTopic = strBackTopic;
}

function GoToTopic(strTopic)
{
	if (strTopic == '')
		return;

	SetTopic(strTopic, g_strThisTopic/*strBackTopic*/);

	ShowHideDiv(g_strBackTopic, false);
	ShowHideDiv(g_strThisTopic, true);
}

function ShowHideDiv(strTopic, bShow)
{
	if (strTopic == '')
		return;

	var id = strTopic;
	var div = document.getElementById(id);
	if (div == null)
		return;

	if (!bShow)
	{
		div.style.visibility = 'hidden';
		div.style.display = 'none';
		div.style.position = 'relative';
	}
	else
	{
		div.style.visibility = 'visible';
		div.style.display = 'block';
		div.style.position = 'relative';

		g_strPrevTopic = div.prev;
		g_strNextTopic = div.next;
	}
}

function GoToContents()
{
	GoToTopic('0');
}

function GoBack()
{
	GoToTopic(g_strBackTopic);
//j	self.history.back();
}

function GoForward()
{
	self.history.forward();
}

function GoToNextTopic()
{
	GoToTopic(g_strNextTopic);
}

function GoToPrevTopic()
{
	GoToTopic(g_strPrevTopic);
}

function PrintTopic()
{
	self.focus();
	self.print();
}

function GetContext()
{
	var strContext = window.location.search;
	if (!strContext || !strContext.length)
	{
		strContext = parent.location.search;
		if (!strContext || !strContext.length)
			return '';
	}

	return strContext.substr(1);
}

function GetOption(strContext, strNameTarget)
{
	if (!strContext || !strContext.length)
		return '';

	strContext += '&';
	var iStart = strContext.indexOf('?') + 1;
	while (iStart >= 0)
	{
		var iEnd = strContext.indexOf('&', iStart);
		if (iEnd <= iStart)
			break;
		var iPair = strContext.indexOf('=', iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			var strName = strContext.substr(iStart, iPair-iStart);
			strName.toLowerCase();
			if (strName == strNameTarget)
			{
				iPair++;
				var strValue = strContext.substr(iPair, iEnd-iPair);
				return unescape(strValue);
			}
		}

		iStart = iEnd + 1;
	}

	return '';
}

function CancelEvent() 
{ 
    window.event.cancelBubble = true; 
    window.event.returnValue = false; 
} 

function BlurLinks()
{
	this.blur();
}

function RemoveFocus()
{
	document.oncontextmenu = CancelEvent; 
	document.onselectstart = CancelEvent; 
	document.ondragstart = CancelEvent; 

	document.onfocus = BlurLinks;
	for (i = 0; i < document.links.length; i++)
		document.links[i].onfocus = BlurLinks;
}
