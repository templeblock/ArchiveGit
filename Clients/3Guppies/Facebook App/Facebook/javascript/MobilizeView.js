AddEventBase(window, 'load', BodyOnload)

var sWaitMessage = '';
function BodyOnload()
{
	var oTargetElement = document.getElementById('idThumbs');
	if (oTargetElement)
		sWaitMessage = oTargetElement.innerHTML;

	GetThumbsFromServer(null/*aid*/);
	GetUsersFromServer(false);
	return false;
}

function GetUsersFromServer(bAddPhotoCount)
{
	var oTargetElement = document.getElementById('userList');
	if (!oTargetElement)
		return;

	var usersAjaxUrl = document.getElementById('param_usersAjaxUrl').value;
	var sUrl = usersAjaxUrl + (bAddPhotoCount ? '1' : '0');
	var callbacks = {
		success: function(oResponse)
		{
			//alert(oResponse.responseText);
			//alert(oTargetElement.innerHTML);
			if (oResponse.responseText.indexOf('<') > 0)
			{
				if (document.all) // A hack to fix an IE bug when calling innerHTML for a <select> tag
					oResponse.responseText = '<option>truncatethis</option>' + oResponse.responseText;
				oTargetElement.innerHTML = oResponse.responseText;
				if (document.all)
					oTargetElement.outerHTML = oTargetElement.outerHTML;
			}

			if (!bAddPhotoCount)
				setTimeout('GetUsersFromServer(true)', 100);
		},
		failure: function(oResponse)
		{
			//alert('GetUsersFromServer(): AJAX failure: ' + 
			//	' tId=' + oResponse.tId +
			//	' status='+ oResponse.status +
			//	' statusText=' + oResponse.statusText
			//	);
		}
	}

	try
	{
		var transaction = YAHOO.util.Connect.asyncRequest('GET', sUrl, callbacks, null);
		//YAHOO.util.Connect.abort(transaction);
	}
	catch(e)
	{
		alert('Error updating the user photo counts: ' + e.description);
	}
	return false;
}

function GetThumbsFromServer(aid)
{
	var thumbsAjaxUrl = document.getElementById('param_thumbsAjaxUrl').value;
	if (!aid)
		aid = document.getElementById('param_aid').value;
	var sUrl = thumbsAjaxUrl + aid;
	var oTargetElement = document.getElementById('idThumbs');
	if (oTargetElement)
		oTargetElement.innerHTML = sWaitMessage;
	var callbacks = {
		success: function(oResponse)
		{
			//alert(oResponse.responseText);
			//alert(oTargetElement.innerHTML);
			var oTargetElement = document.getElementById('idThumbs');
			if (oTargetElement && (oResponse.responseText.indexOf('<') > 0))
				oTargetElement.innerHTML = oResponse.responseText;
		},
		failure: function(oResponse)
		{
			//alert('GetThumbsFromServer(): AJAX failure: ' + 
			//	' tId=' + oResponse.tId +
			//	' status='+ oResponse.status +
			//	' statusText=' + oResponse.statusText
			//	);
		}
	}

	try
	{
		var transaction = YAHOO.util.Connect.asyncRequest('GET', sUrl, callbacks, null);
		//YAHOO.util.Connect.abort(transaction);
	}
	catch(e)
	{
		alert('Error updating the thumbnails: ' + e.description);
	}
	return false;
}

var m_oCurrentThumbContainer = null;

function SelectImage(oThumbContainer)
{
	if (m_oCurrentThumbContainer)
		m_oCurrentThumbContainer.className = 'unselectedThumb';

	if (!oThumbContainer)
		return;

	oThumbContainer.className = 'selectedThumb';

	var sCaption = '';
	var sPhotoUrl = '';
	for (i=0; i<oThumbContainer.childNodes.length; i++)
	{
		var node = oThumbContainer.childNodes[i];
		if (node.nodeName != 'IMG')
			continue;

		sCaption = node.getAttribute('title');
		sPhotoUrl = node.getAttribute('photo');
		break;
	}

	m_oCurrentThumbContainer = oThumbContainer;
	var oImage = document.getElementById('mxr-image');
	if (oImage)
	{
		oImage.src = sPhotoUrl;
		oImage.title = sCaption;
	}

	var oTitle = document.getElementById('mxr-title');
	if (oTitle)
		oTitle.value = sCaption;

	return false;
}

function NextImage()
{
	var oThumbContainer = m_oCurrentThumbContainer;
	if (oThumbContainer)
	{
		while (oThumbContainer = oThumbContainer.nextSibling)
		{
			if (oThumbContainer.nodeName == "SPAN")
				break;
		}
	}

	if (!oThumbContainer)
		oThumbContainer = document.getElementById('mxr-thumb');
	if (oThumbContainer && oThumbContainer.onclick)
		oThumbContainer.onclick();
	return false;
}

function PreviousImage()
{
	var oThumbContainer = m_oCurrentThumbContainer;
	if (oThumbContainer)
	{
		while (oThumbContainer = oThumbContainer.previousSibling)
		{
			if (oThumbContainer.nodeName == "SPAN")
				break;
		}
	}

	if (!oThumbContainer)
		oThumbContainer = document.getElementById('mxr-thumb');
	if (oThumbContainer && oThumbContainer.onclick)
		oThumbContainer.onclick();
	return false;
}

function SendToPhone()
{
	var sPhotoUrl = '';
	var oImage = document.getElementById('mxr-image');
	if (oImage)
		sPhotoUrl = oImage.src;

	var sCaption = '';
	var oTitle = document.getElementById('mxr-title');
	if (oTitle)
		sCaption = oTitle.value.replace(/\"/g, '\'');

	var openlockerMobilizeUrl = document.getElementById('param_openlockerMobilizeUrl').value;
	window.location.href = openlockerMobilizeUrl + escape(sPhotoUrl) + '&Filename=' + encodeURIComponent(sCaption); 
	return false;
}

function SetMaxWidth(oElement, iSize)
{
	return (oElement.offsetWidth >= oElement.offsetHeight && oElement.offsetWidth > iSize ? iSize : true);
}

function SetMaxHeight(oElement, iSize)
{
	return (oElement.offsetHeight >= oElement.offsetWidth && oElement.offsetHeight > iSize ? iSize : true);
}

// Set autoclose true to have the window close automatically
// Set autoclose false to allow multiple popup windows
var bAutoClose = true;
var bClickToClose = true;

function PreviewImage()
{
	var oThumbContainer = m_oCurrentThumbContainer;
	if (!oThumbContainer)
		return;

	var sCaption = '';
	var sPhotoUrl = '';
	for (i=0; i<oThumbContainer.childNodes.length; i++)
	{
		var node = oThumbContainer.childNodes[i];
		if (node.nodeName != 'IMG')
			continue;

		sCaption = node.getAttribute('title');
		sPhotoUrl = node.getAttribute('photo');
		break;
	}

	var imgWin = window.open('about:blank', '', 'scrollbars=no, width=150, height=100, left=50, top=50');
	if (!imgWin)
		return;

	with (imgWin.document)
	{
		writeln('<html><head><title>Loading...</title>');
		writeln('<style>body{margin:0px;}</style>');
		writeln('<sc'+'ript>');
		writeln('function reSizeToImage(){');
		writeln('var width  = document.images[0].width;');
		writeln('var height = document.images[0].height;');
		if (window.innerWidth)
		{ // w3c (firefox)
			writeln('window.innerWidth  = width;');
			writeln('window.innerHeight = height;');
		}
		else
		if (document.body.clientWidth)
		{ // ie quirks mode
			writeln('var dx = width  - document.body.clientWidth;');
			writeln('var dy = height - document.body.clientHeight;');
			writeln('window.resizeBy(dx, dy);');
		}
		else
		if (document.documentElement && document.documentElement.clientWidth)
		{ // ie strict mode
			writeln('var dx = width  - document.documentElement.clientWidth;');
			writeln('var dy = height - document.documentElement.clientHeight;');
			writeln('window.resizeBy(dx, dy);');
		}
		writeln('document.title="' + sCaption + '";');
		writeln('self.focus();');
		writeln('}');
		writeln('</sc'+'ript>');
		writeln('</head><body bgcolor="000000" scroll="no" onload="reSizeToImage()"');
		if (bClickToClose)
			writeln(' onclick="self.close()"');
		if (bAutoClose)
			writeln(' onblur="self.close()"');
		writeln('>');
		writeln('<img src="' + sPhotoUrl + '" title="' + sCaption + '" style="display:block" alt="" /></body></html>');
		close();		
	}
}

function AddEventBase(object, type, myfunc)
{
	if (object.addEventListener)
		object.addEventListener(type, myfunc, false);
	else
	if (object.attachEvent)
		object.attachEvent('on' + type, myfunc);
}
