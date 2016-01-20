// The following classes were modeled after http://blog.deconcept.com/swfobject/,
// but simplified for readability and maintainability
function CVersion(arrVersion)
{
	this.m_major = (arrVersion && arrVersion[0] ? parseInt(arrVersion[0]) : 0);
	this.m_minor = (arrVersion && arrVersion[1] ? parseInt(arrVersion[1]) : 0);
	this.m_rev   = (arrVersion && arrVersion[2] ? parseInt(arrVersion[2]) : 0);
	//alert('New Version: ' + this.m_major + '.' + this.m_minor + '.' + this.m_rev);

	this.IsNewer = function(version)
	{
		var bOK = true;
		if (this.m_major < version.m_major) bOK = false;	else
		if (this.m_major > version.m_major) bOK = true;		else
		if (this.m_minor < version.m_minor) bOK = false;	else
		if (this.m_minor > version.m_minor) bOK = true;		else
		if (this.m_rev   < version.m_rev)   bOK = false;
		//alert('Is ' + this.m_major + '.' + this.m_minor + '.' + this.m_rev + ' newer than ' + version.m_major + '.' + version.m_minor + '.' + version.m_rev + '? ' + bOK);
		return bOK;
	}
}

function CEmbedFlash(sSwfPath, sSwf, bUseObjectTagAlways, iWidth, iHeight, Version, bUseExpressInstall, sBgColor, sQuality, sExpressInstallRedirectUrl)
{
	// private
	var m_bUpToDate = false;
	var m_bInstalled = false;
	var m_sSwfPath = sSwfPath;
	var m_sSwf = sSwf;
	var m_bUseObjectTagAlways = bUseObjectTagAlways;
	var m_iWidth = iWidth;
	var m_iHeight = iHeight;

	m_params = new Object();
	m_variables = new Object();

	var arrVersion = (Version ? Version.toString().split('.') : null);
	m_VersionRequired = new CVersion(arrVersion);

	m_bUseExpressInstall = bUseExpressInstall;
	m_sExpressInstallRedirectUrl = (sExpressInstallRedirectUrl ? sExpressInstallRedirectUrl : window.location);

	addParam('bgcolor', (sBgColor ? sBgColor : '#ffffff'))
	addParam('quality', (sQuality ? sQuality : 'high'));

	InstallEventHandlers();

	// public
	this.addParam = addParam;
	function addParam(name, value)
	{
		m_params[name] = value;
	}
	
	// public
	this.addVariable = addVariable;
	function addVariable(name, value)
	{
		m_variables[name] = value;
	}
	
	// public
	this.IsUpToDate = IsUpToDate;
	function IsUpToDate()
	{
		return m_bUpToDate;
	}
	
	// public
	this.IsInstalled = IsInstalled;
	function IsInstalled()
	{
		return m_bInstalled;
	}
	
	// public
	this.Initialize = Initialize;
	function Initialize(idPlayerContainer, idPlayerObject)
	{
		var oPlayerContainer = document.getElementById(idPlayerContainer);
		if (!oPlayerContainer)
			return;

		var VersionInstalled = GetVersionInstalled();
		m_bUpToDate = VersionInstalled.IsNewer(m_VersionRequired);
		//alert('VersionInstalled: ' + VersionInstalled.m_major + '.' + VersionInstalled.m_minor + '.' + VersionInstalled.m_rev);

		// Check to see if we can do an express install
		var Version6065 = new CVersion([6,0,65]);
		m_bInstalled = VersionInstalled.IsNewer(Version6065);
		var bDoExpressInstall = (m_bUseExpressInstall && m_bInstalled && !m_bUpToDate);

		if (m_bUpToDate || bDoExpressInstall)
		{
			PrePlayerCreate(idPlayerObject);
			oPlayerContainer.innerHTML = GenerateHTML(idPlayerObject, bDoExpressInstall);
			PostPlayerCreate(idPlayerObject);
		}

		oPlayerContainer.style.visibility = 'visible';
		return true;
	}

	// private
	this.GenerateHTML = GenerateHTML;
	function GenerateHTML(idPlayerObject, bDoExpressInstall)
	{
		var sMimeType = 'application/x-shockwave-flash';
		var sClassId = 'D27CDB6E-AE6D-11cf-96B8-444553540000';
		var sPluginsPage = 'http://www.adobe.com/go/getflashplayer/';
		var sCodebase = 'http://fpdownload.adobe.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,0,0';
		var sAudioUrlParamName = 'movie';
		var sAudioUrlValue = m_sSwfPath + m_sSwf;

		var bUseEmbedTag = (m_bUseObjectTagAlways ? false : (navigator.plugins && navigator.mimeTypes && navigator.mimeTypes.length));
		var bUseIEObjectTag = (document.all != null);

		var iWidth = m_iWidth;
		var iHeight = m_iHeight;

		var sVariables = '';
		for (var key in m_variables)
			sVariables += key + '=' + m_variables[key] + '&';

		if (bDoExpressInstall)
		{
			// Change our own title, so this window can be closed after the install
			var sDocTitle = document.title.slice(0, 47) + ' - Flash Player Installation';
			document.title = sDocTitle;
			sVariables += 'MMplayerType=' + (bUseEmbedTag ? 'PlugIn' : 'ActiveX') + '&';
			sVariables += 'MMredirectURL=' + escape(m_sExpressInstallRedirectUrl) + '&';
			sVariables += 'MMdoctitle=' + sDocTitle + '&';
			sAudioUrlValue = m_sSwfPath + 'playerProductInstall.swf';
			if (iWidth < 214) iWidth = 214;
			if (iHeight < 137) iHeight = 137;
			idPlayerObject = '_idPlayerProductInstall';
		}

		var sHTML = '';
		if (bUseEmbedTag)
		{ // Embed tag for Firefox
			sHTML += '<embed id="' + idPlayerObject + '" width="' + iWidth + '" height="' + iHeight + '" ';
			sHTML += 'src="' + sAudioUrlValue + '" ';
			sHTML += 'type="' + sMimeType + '" ';
			sHTML += 'pluginspage="' + sPluginsPage + '" ';
			for (var key in m_params)
				sHTML += [key] + '="' + m_params[key] + '" ';
			if (sVariables.length > 0)
				sHTML += 'flashvars="' + sVariables + '"';
			sHTML += ' />\n';
		}
		else
		{
			sHTML += '<object id="' + idPlayerObject + '" width="' + iWidth + '" height="' + iHeight + '" ';
			if (!bUseIEObjectTag)
			{ // Object tag for Firefox
				sHTML += 'data="' + sAudioUrlValue + '" ';
				sHTML += 'type="' + sMimeType + '" ';
				sHTML += 'pluginspage="' + sPluginsPage + '" ';
			}
			else
			{ // Object tag for IE
				sHTML += 'classid="clsid:' + sClassId + '" ';
				sHTML += 'codebase="' + sCodebase + '" ';
			}

			sHTML += '>\n';
			sHTML += '<param name="' + sAudioUrlParamName + '" value="' + sAudioUrlValue + '" />\n';

			for (var key in m_params)
				sHTML += '<param name="' + key + '" value="' + m_params[key] + '" />\n';
			if (sVariables.length > 0)
				sHTML += '<param name="flashvars" value="' + sVariables + '" />\n';
			sHTML += '</object>\n';
		}

		return sHTML;
	}

	// private
	// We need to trick I6/IE7 because if you put an object inside a form, 
	// IE's implementation of Javascript wants to look for 'window.yourObject'
	// and not 'document.form[x].yourObject'.
	// If there is a second nested form, the problem does not exist
	this.PrePlayerCreate = PrePlayerCreate;
	function PrePlayerCreate(idPlayerObject)
	{
		if (navigator.appName.toLowerCase() != 'microsoft internet explorer')
			return;

		if (typeof(window[idPlayerObject]) != 'undefined')
			return;

		// Assign a fake player object as a placeholder
		var oFakePlayerObject = new Object();
		window[idPlayerObject] = oFakePlayerObject;
	}

	// private
	// This fixes another IE6/IE7 bug.
	// After a page is cached by IE and reloaded, the SWF is loaded before it's embedded,
	// so any callback functions that the SWF tries to setup when it loads (like the JS->Flash ExternalInterface code)
	// will fail with an error 'objectID' is undefined. Then when you try to use the callback function you get Object 
	// doesn't support this property or method because the functions didn't get assigned to the player object properly.
	this.PostPlayerCreate = PostPlayerCreate;
	function PostPlayerCreate(idPlayerObject)
	{
		if (navigator.appName.toLowerCase() != 'microsoft internet explorer')
			return;

		var oFakePlayerObject = window[idPlayerObject];
		if (typeof(oFakePlayerObject) != 'object')
			return;

		// Fix external interface functions; the inner loop is only entered on a page reload
		var oPlayerObject = document.getElementById(idPlayerObject);
		for (var functionName in oFakePlayerObject)
		{
			if (typeof(oPlayerObject[functionName]) == 'undefined')
				__myflash__addCallback(oPlayerObject, functionName);
		}

		// Assign the real player object
		window[idPlayerObject] = oPlayerObject;
	}

	// private
	function __myflash__addCallback(instance, functionName)
	{
		eval("instance[functionName] = function() { return this.CallFunction('<invoke name=\"" + functionName.toString() + "\" returntype=\"javascript\">' + __flash__argumentsToXML(arguments, 0) + '</invoke>'); }");
	}

	// private
	this.GetVersionInstalled = GetVersionInstalled;
	function GetVersionInstalled()
	{
		try
		{
			var oPlugin = null;
			if (window.ActiveXObject)
			{
				oPlugin = new ActiveXObject('ShockwaveFlash.ShockwaveFlash');
				if (oPlugin)
				{
					var sVersion = oPlugin.GetVariable('$version');
					var oVersion = new CVersion(sVersion.split(' ')[1].split(','));
					//alert('oVersion="' + oVersion.m_major + '.' + oVersion.m_minor + '.' + oVersion.m_rev);
					return oVersion;
				}
			}
			else
			if (navigator.plugins && navigator.mimeTypes && navigator.mimeTypes.length)
			{
				oPlugin = navigator.plugins['Shockwave Flash'];
				if (oPlugin && oPlugin.description)
					return new CVersion(oPlugin.description.replace(/([a-zA-Z]|\s)+/, '').replace(/(\s+r|\s+b[0-9]+)/, '.').split('.'));
			}
		}
		catch(e)
		{
			// No Flash
		}

		return new CVersion();
	}

	// private
	// Install onbeforeunload and onunload handlers to fix player bugs
	this.InstallEventHandlers = InstallEventHandlers;
	function InstallEventHandlers()
	{
		if (typeof(window.onbeforeunload) != 'function')
			window.onbeforeunload = BeforeUnload;
		else
		{
			var oldBeforeUnload = window.onbeforeunload;
			window.onbeforeunload = function()
			{
				BeforeUnload();
				oldBeforeUnload();
			}
		}

		if (typeof(window.onunload) != 'function')
			window.onunload = Unload;
		else
		{
			var oldUnload = window.onunload;
			window.onunload = function()
			{
				Unload();
				oldUnload();
			}
		}
	}

	// private
	// Fix for Flash9 bug; see http://blog.deconcept.com/2006/07/28/swfobject-143-released/
	// It seems that placing two ExternalInterface-activated player objects on a page at the
	// same time (in IE) causes an infinite recursion with __flash_unloadHandler
	this.BeforeUnload = BeforeUnload;
	function BeforeUnload()
	{
		// Avoid these two Flash functions due to the Flash 9 bug
		__flash_unloadHandler = function(){};
		__flash_savedUnloadHandler = function(){};
		//alert('Leaving BeforeUnload');
	}

	// private
	// Fix for IE6 bug; see http://blog.deconcept.com/2006/05/18/flash-player-bug-streaming-content-innerhtml-ie/
	// When you embed a swf using innerHTML in IE 6, and stream content to the player, 
	// when the user leaves the page (either by reloading it, hitting the back button, 
	// or closing the browser window with another browser window open), the audio will keep 
	// playing until the video ends or until the user closes all of their open IE windows.
	this.Unload = Unload;
	function Unload()
	{
		if (window.opera || !document.all)
			return;
		var objects = document.getElementsByTagName('OBJECT');
		for (var i=0; i < objects.length; i++)
		{
			objects[i].style.display = 'none';
			for (var x in objects[i])
			{
				// Avoid executing these player function objects
				if (typeof(objects[i][x]) == 'function')
					objects[i][x] = function(){};
			}
		}

		//alert('Leaving Unload');
	}
}
