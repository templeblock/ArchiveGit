//j To determine is the user is online
//j window.clientInformation.cookieEnabled

var CharArray = [
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/',
	'='
];

var CharStr = CharArray.toString().replace(/,/g, '');

function StartIZE()
{
	return RaiseClickEvent('LaunchApp', 'IZE');
//	var oShell = new ActiveXObject("Shell.Application");
//	var cmdToRun = "C:\Program Files\HP\Image Zone Express\Hp_IZE.exe";
//	oShell.ShellExecute(cmdToRun, "", "", "open", "1");
}
		
function GetStartupStateEx()
{
	var obj = GetObject('mdb');
	if (obj)
		return (obj.value == 'false' ? 'false' : 'true');

	return 'true';
}
								
function SetStartupStateEx(startState)
{
	var obj = GetObject('mdb');
	if (obj)
		obj.value = startState;
	
	return RaiseClickEvent('RegStartup', startState);
}

function setFrameSrcEx(ShState)
{
	var obj = GetObject('ShowOnStartup');
	if (obj)
		obj.value = ShState;
		
	setFrameSrc();
}

function setFrameSrc()
{
	var obj = GetObject('ShowOnStartup');
	if (obj)
	{
		if (obj.value == '1')
			Home();
		else
			GetStarted();
	}
}
			
function Encode64(str)
{
	var i = 0;
	var output = '';
	do {
		var chr1 = str.charCodeAt(i++);
		var chr2 = str.charCodeAt(i++);
		var chr3 = str.charCodeAt(i++);

		var enc1 = chr1 >> 2;
		var enc2 = ((chr1 &  3) << 4) | (chr2 >> 4);
		var enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
		var enc4 = chr3 & 63;

		if (isNaN(chr2))
			enc3 = enc4 = 64;
		else
		if (isNaN(chr3))
			enc4 = 64;

		output += (CharArray[enc1] + CharArray[enc2] + CharArray[enc3] + CharArray[enc4]);

	} while (i < str.length);

	return output;
}

function Decode64(str)
{
	// Remove all characters that are not A-Z, a-z, 0-9, +, /, or =
	var Search = /[^A-Za-z0-9+/=]/g;
	if (Search.exec(str))
	{
	//	alert('There were invalid base64 characters in the encoded text.');
	//	str = str.replace(Search, '');
		return str;
	}

	var i = 0;
	var output = '';
	do {
		var enc1 = CharStr.indexOf(str.charAt(i++));
		var enc2 = CharStr.indexOf(str.charAt(i++));
		var enc3 = CharStr.indexOf(str.charAt(i++));
		var enc4 = CharStr.indexOf(str.charAt(i++));

		var chr1 = (enc1 << 2) | (enc2 >> 4);
		var chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
		var chr3 = ((enc3 & 3) << 6) | enc4;

		output += String.fromCharCode(chr1);

		if (enc3 != 64)
			output += String.fromCharCode(chr2);

		if (enc4 != 64)
			output += String.fromCharCode(chr3);

	} while (i < str.length);

	return output;
}

function ampersand()
{
	return '&';
}

function GetSeparator()
{
	return ' > ';
}

function IsAg()
{
	return true;
}

function IsBm()
{
	return false;
}

function IsMs()
{
	return false;
}

function IsUk()
{
	return false;
}

function GetHostUrl()
{
	return 'www.americangreetings.com';
}

function GetSiteUrl()
{
	var strUrl = 'http://' + GetHostUrl() + '/cnp/index.pd?chp=';
	return strUrl + 'agcnp1';
}

function GetSource()
{
	return '&source=hp';
}

function WriteHostURL()
{
	document.write(GetHostUrl());
}

function GoToSite(strSource)
{
	var strURL = GetSiteUrl();
	if (strSource)
		strURL += GetSource() + strSource;
	window.open(strURL);
	return false;
}

function GetPath(strURL)
{
	var index = strURL.lastIndexOf('?');
	if (index >= 0)
		strURL = strURL.substring(0, index + 1);
	index = strURL.lastIndexOf('/');
	if (index >= 0)
		strURL = strURL.substring(0, index + 1);

	return strURL;
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

function HistoryObject()
{
	this._ArrayBackUrl = new Array();
	this._ArrayBackCat = new Array();
	this._ArrayCurrUrl = null;
	this._ArrayCurrCat = null;
	this._ArrayForwUrl = new Array();
	this._ArrayForwCat = new Array();

	this.SetUrl = function(strUrl, strCategory)
	{
		if (this._ArrayCurrUrl == strUrl)
			return this._ArrayCurrUrl;

		if (this._ArrayCurrUrl)
		{
			Push(this._ArrayBackUrl, this._ArrayCurrUrl);
			Push(this._ArrayBackCat, this._ArrayCurrCat);
		}
		this._ArrayCurrUrl = strUrl;
		this._ArrayCurrCat = strCategory;
		this._ArrayForwUrl.length = 0;
		this._ArrayForwCat.length = 0;
		SetCategoryLabel(this._ArrayCurrCat);
		return this._ArrayCurrUrl;
	}

	this.Forward = function()
	{
		var nCount = this._ArrayForwUrl.length;
		if (!nCount)
			return null;
		Push(this._ArrayBackUrl, this._ArrayCurrUrl);
		Push(this._ArrayBackCat, this._ArrayCurrCat);
		this._ArrayCurrUrl = Pop(this._ArrayForwUrl);
		this._ArrayCurrCat = Pop(this._ArrayForwCat);
		SetCategoryLabel(this._ArrayCurrCat);
		return this._ArrayCurrUrl;
	}

	this.Back = function()
	{
		var nCount = this._ArrayBackUrl.length;
		if (!nCount)
			return null;
		Push(this._ArrayForwUrl, this._ArrayCurrUrl);
		Push(this._ArrayForwCat, this._ArrayCurrCat);
		this._ArrayCurrUrl = Pop(this._ArrayBackUrl);
		this._ArrayCurrCat = Pop(this._ArrayBackCat);
		SetCategoryLabel(this._ArrayCurrCat);
		return this._ArrayCurrUrl;
	}

	this.GetCategory = function()
	{
		return this._ArrayCurrCat;
	}
}

function Push(array, value)
{
//	array.push(value);
	array[array.length] = value;
}

function Pop(array)
{
	if (array.length <= 0)
		return null;
//	return array.pop();
	var Element = array[array.length-1];
	array.length--;
	return Element;
}

function SetCategoryLabel(strCategory)
{
	var Navbar = GetNavbar();
	if (!Navbar)
		return;
	var object = Navbar.document.getElementById('category');
	if (object)
		object.innerText = strCategory;
	var object2 = Navbar.document.getElementById('categoryLabel');
	if (object2)
		object2.innerText = 'Category: ';
//	var Topbar = GetTopbar();
//	if (!Topbar)
//		return;
//	var object = Topbar.document.getElementById('category');
//	if (object)
//		object.innerText = strCategory;
//	var object2 = Topbar.document.getElementById('categoryLabel');
//	if (object2)
//		object2.innerText = 'Category: ';
}

function GetCategoryLabel()
{
	var Navbar = GetNavbar();
	if (!Navbar)
		return;
	var object = Navbar.document.getElementById('category');
	if (object)
		return object.innerText;

	return '';
//	var Topbar = GetTopbar();
//	if (!Topbar)
//		return '';

//	var object = Topbar.document.getElementById('category');
//	if (object)
//		return object.innerText;

//	return '';
}

function GetParent()
{
	if (!parent)
		return null;
	return parent.parent;
}

function GetHistory()
{
	var Wnd = GetParent();
	if (!Wnd)
		return null;
	return Wnd.History;
}

function GetNavbar()
{
	var Main = GetMain();
	if (!Main)
		return null;
	return Main.frames['browsenav'];
}

function GetTopbar()
{
	var Wnd = GetParent();
	if (!Wnd)
		return null;
	return Wnd.frames['topbar'];
}

function GetMain()
{
	var Wnd = GetParent();
	if (!Wnd)
		return null;
	return Wnd.frames['main'];
}

function GetLinelist()
{
	var Main = GetMain();
	if (!Main)
		return null;
	return Main.frames['linelist'];
}

function GetThumbs()
{
	var Main = GetMain();
	if (!Main)
		return null;
	return Main.frames['thumbs'];
}

function GetObject(strId)
{
	var Wnd = GetParent();
	if (!Wnd)
		return null;
	return Wnd.document.all(strId);
}

function RaiseClickEvent(strId, strArgs)
{
	var Obj = GetObject(strId);
	if (!Obj)
		return false;

	Obj.className = strArgs;
	Obj.click();
	return false;
}

function Help(iTopic)
{
	var strURL = 'cphelp.htm?topic=' + iTopic;
	return RaiseClickEvent('Help', strURL);
}

function AutoUpdate()
{
	return RaiseClickEvent('AutoUpdate', 'here');
}

function Launch(strProductFile)
{
	var strURL = 'cporder.htm';
	// Without a product file argument, the user will be prompted for a file name
	if (strProductFile && strProductFile != '')
		strURL += '?product=' + strProductFile;
	return RaiseClickEvent('Launch', strURL);
}

function ThumbsSetUrl(strUrl)
{
	var Thumbs = GetThumbs();
	if (!Thumbs)
	{
		alert(strUrl);
		return false;
	}
	if (Thumbs.location.href == strUrl)
		return false;
	Thumbs.location.href = strUrl;

	var strUrlEx = strUrl.replace('../', '');
	var History = GetHistory();
	if (History)
		History.SetUrl(strUrlEx, GetCategoryLabel());
	return false; // click handler return value
}

function GetStarted()
{
	var Topbar = GetTopbar();
	if (Topbar)
		Topbar.location.href = 'topbar.htm';
	var Main = GetMain();
	if (Main)
		Main.location.href = 'browse.htm';

//j	var History = GetHistory();
//j	if (History)
//j		History.SetUrl('browse.htm', GetCategoryLabel());

	return false; // click handler return value
}

function Home()
{
	var Topbar = GetTopbar();
	if (Topbar)
		Topbar.location.href = 'topbar1.htm';
	var Main = GetMain();
	if (Main)
		Main.location.href = 'welcome.htm';
	return false; // click handler return value
}

function Back()
{
	var History = GetHistory();
	if (!History)
		return false;
	var strUrl = History.Back();
	if (!strUrl)
		return false;
	var Thumbs = GetThumbs();
	if (!Thumbs)
		return false;
	Thumbs.location.href = strUrl;
	var Linelist = GetLinelist();
	if (!Linelist)
		return false;

	Linelist.OnSetCategory(History.GetCategory());
	return false; // click handler return value
}

function Forward()
{
	var History = GetHistory();
	if (!History)
		return false;
	var strUrl = History.Forward();
	if (!strUrl)
		return false;
	var Thumbs = GetThumbs();
	if (!Thumbs)
		return false;
	Thumbs.location.href = strUrl;
	var Linelist = GetLinelist();
	if (!Linelist)
		return false;

	Linelist.OnSetCategory(History.GetCategory());
	return false; // click handler return value
}

function BreadCrumbs()
{
	Back();
//j	var History = GetHistory();
//j	if (!History)
//j		return false;
//j	var Linelist = GetLinelist();
//j	if (Linelist)
//j		Linelist.OnSetCategory(History.GetCategory());
	return false; // click handler return value
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
		//j	strName = strName.toLowerCase();
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

function LoadImageArray(strImgPath, ImageList)
{
	if (document.images == null)
		return null;

	var Images = new Array();
	var n = 0;
	for (i in ImageList)
	{
		Images[n] = new Image();
		Images[n].src = strImgPath + ImageList[i];
		n++;
	}

//	alert(Images.length);
//	for (i in Images)
//	{
//		if (!Images[i].complete)
//			alert(Images[i].src);
//	}

	return Images;
}

function PreloadAppImages(strImgPath)
{
	// List the images
	var ImageList = new Array (
		'aglogo.gif',
		'back.gif',
		'bgtop.gif',
	//c	'button2.gif',
		'collage.jpg',
		'file.gif',
		'folder.gif',
		'forward.gif',
		'getstarted.gif',
		'header1.gif', //h
		'header.gif', //h
		'home.gif',
		'hplogo.gif', //h
		'minus.gif',
		'plus.gif',
		'space.gif',
		'welcome.gif'
	);

	return LoadImageArray(strImgPath, ImageList);
}
