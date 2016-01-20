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

function CEmbedQTPlayer(sAudioUrl, bUseObjectTagAlways, iWidth, iHeight, Version)
{
	// private
	var m_bUpToDate = false;
	var m_bInstalled = false;
	var m_sAudioUrl = sAudioUrl;
	var m_bUseObjectTagAlways = bUseObjectTagAlways;
	var m_iWidth = iWidth;
	var m_iHeight = iHeight;

	m_params = new Object();
	m_variables = new Object();

	var arrVersion = (Version ? Version.toString().split('.') : null);
	m_VersionRequired = new CVersion(arrVersion);

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
		m_bUpToDate = true;

//j		var Version6065 = new CVersion([6,0,65]);
//j		m_bInstalled = VersionInstalled.IsNewer(Version6065);
		m_bInstalled = true;

		if (m_bUpToDate)
		{
			PrePlayerCreate(idPlayerObject);
			oPlayerContainer.innerHTML = GenerateHTML(idPlayerObject);
			PostPlayerCreate(idPlayerObject);
		}

		oPlayerContainer.style.visibility = 'visible';
		return true;
	}

	// private
	this.GenerateHTML = GenerateHTML;
	function GenerateHTML(idPlayerObject)
	{
		var sMimeType = 'audio/aac'; // video/quicktime
		var sClassId = '02BF25D5-8C17-4B23-BC80-D3488ABDDC6B';
		var sPluginsPage = 'http://www.apple.com/quicktime/download/';
		var sCodebase = 'http://www.apple.com/qtactivex/qtplugin.cab';
		var sAudioUrlParamName = 'src';
		var sAudioUrlValue = m_sAudioUrl;

		var bUseEmbedTag = (m_bUseObjectTagAlways ? false : (navigator.plugins && navigator.mimeTypes && navigator.mimeTypes.length));
		var bUseIEObjectTag = (document.all != null);

		var iWidth = m_iWidth;
		var iHeight = m_iHeight;

		var sHTML = '';
		if (bUseEmbedTag)
		{ // Embed tag for Firefox
			sHTML += '<embed id="' + idPlayerObject + '" width="' + iWidth + '" height="' + iHeight + '" ';
			sHTML += 'src="' + sAudioUrlValue + '" ';
			sHTML += 'type="' + sMimeType + '" ';
			sHTML += 'pluginspage="' + sPluginsPage + '" ';
			for (var key in m_params)
				sHTML += [key] + '="' + m_params[key] + '" ';
			for (var key in m_variables)
				sHTML += [key] + '="' + m_variables[key] + '" ';
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
			for (var key in m_variables)
				sHTML += '<param name="' + key + '" value="' + m_variables[key] + '" />\n';
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
	this.PostPlayerCreate = PostPlayerCreate;
	function PostPlayerCreate(idPlayerObject)
	{
	}

	// private
	this.GetVersionInstalled = GetVersionInstalled;
	function GetVersionInstalled()
	{
		try
		{
			var oPlugin = null;
			if (window.ActiveXObject)
				oPlugin = new ActiveXObject('QuickTimeCheckObject.QuickTimeCheck.1');
			else
			//if (window.GeckoActiveXObject)
			//	oPlugin = new GeckoActiveXObject('QuickTimeCheckObject.QuickTimeCheck');
			//else
			if (navigator.mimeTypes && navigator.mimeTypes.length)
			{
				var mimeType = navigator.mimeTypes['audio/aac']; // video/quicktime
				if (mimeType)
				{
					oPlugin = mimeType.enabledPlugin;
					//alert(mimeType.description + '\n' + mimeType.type + '\n' + oPlugin.description + '\n' + oPlugin.filename + '\n' + oPlugin.length + '\n' + oPlugin.name);
				}
			}

			if (oPlugin)
			{
				var sVersion = '0.0.0.0';
				try { sVersion = oPlugin.GetPluginVersion(); alert(sVersion); }
				catch(e) {}
//j				catch(e) { alert('GetVersionInstalled failed; plugin="' + oPlugin); }
				var oVersion = new CVersion(sVersion.split('.'));
				//alert('oVersion="' + oVersion.m_major + '.' + oVersion.m_minor + '.' + oVersion.m_rev);
				return oVersion;
			}
		}
		catch(e)
		{
			// No QuickTime
		}

		return new CVersion();
	}
}
