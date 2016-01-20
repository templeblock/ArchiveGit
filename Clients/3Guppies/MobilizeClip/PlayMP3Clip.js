// Author: Jim McCurdy, Face To Face Software, jmccurdy@facetofacesoftware.com
//
// A Javascript class that implements an MP3 client-side player using Flash
// This class wraps the functionality of PlayMP3Clip.swf
// EmbedFlash.js must be included before this file
// See http://www.adobe.com/cfusion/knowledgebase/index.cfm?id=50c96388 for more information about the sCrossDomainPolicyUrl
function CPlayMP3Clip(sSwfPath, sAudioUrl, sCrossDomainPolicyUrl)
{
	// private
	var m_idPlayerObject = null;
	var m_oPlayerObject = null;
	var m_bPlayerFunctionFailure = false;

	// Create the Flash object that will host PlayMP3Clip.swf, and do all the audio playback
	var bUseObjectTagAlways = true;
	var m_oPlayer = new CEmbedFlash(sSwfPath, 'PlayMP3Clip.swf', bUseObjectTagAlways, '1'/*iWidth 150*/, '1'/*iHeight 600*/, '9'/*FlashVersion*/, true/*bUseExpressInstall*/);
	m_oPlayer.addParam('enablejavascript', 'true');
	m_oPlayer.addParam('allowScriptAccess', 'always');
	m_oPlayer.addVariable('Url', sAudioUrl);
	if (sCrossDomainPolicyUrl && sCrossDomainPolicyUrl != '')
		m_oPlayer.addVariable('PolicyUrl', sCrossDomainPolicyUrl);

//	m_oPlayer.addVariable('Start', '30000');
//	m_oPlayer.addVariable('End', '35000');
//	m_oPlayer.addVariable('Loop', '0');
//	m_oPlayer.addVariable('LoopMax', '4');
//	m_oPlayer.addVariable('AutoPlay', '1');
//	m_oPlayer.addVariable('Volume', '50');

	// public
	this.Initialize = Initialize;
	function Initialize(idPlayerContainer, idPlayerObject)
	{
		m_idPlayerObject = idPlayerObject;
		m_oPlayer.Initialize(idPlayerContainer, idPlayerObject);
	}

	// public
	this.PlayerIsUpToDate = PlayerIsUpToDate;
	function PlayerIsUpToDate()
	{
		return m_oPlayer.IsUpToDate();
	}
	
	// public
	this.PlayerIsInstalled = PlayerIsInstalled;
	function PlayerIsInstalled()
	{
		return m_oPlayer.IsInstalled();
	}
	
	// public
	this.DoPlay = DoPlay;
	function DoPlay(bFromStart)
	{
		if (PlayerOK() && FunctionOK('DoPlay', typeof(m_oPlayerObject.DoPlay)))
			m_oPlayerObject.DoPlay(bFromStart);
	}

	// public
	this.DoStop = DoStop;
	function DoStop()
	{
		if (PlayerOK() && FunctionOK('DoStop', typeof(m_oPlayerObject.DoStop)))
			m_oPlayerObject.DoStop();
	}

	// public
	this.SetUrl = SetUrl;
	function SetUrl(sAudioUrl)
	{
		if (PlayerOK() && FunctionOK('SetUrl', typeof(m_oPlayerObject.SetUrl)))
			m_oPlayerObject.SetUrl(sAudioUrl);
	}

	// public
	this.SetStart = SetStart;
	function SetStart(iStart)
	{
		if (PlayerOK() && FunctionOK('SetStart', typeof(m_oPlayerObject.SetStart)))
			m_oPlayerObject.SetStart(iStart);
	}

	// public
	this.SetEnd = SetEnd;
	function SetEnd(iEnd)
	{
		if (PlayerOK() && FunctionOK('SetEnd', typeof(m_oPlayerObject.SetEnd)))
			m_oPlayerObject.SetEnd(iEnd);
	}

	// public
	this.SetLoop = SetLoop;
	function SetLoop(bLoop)
	{
		if (PlayerOK() && FunctionOK('SetLoop', typeof(m_oPlayerObject.SetLoop)))
			m_oPlayerObject.SetLoop(bLoop);
	}

	// public
	this.SetLoopMax = SetLoopMax;
	function SetLoopMax(iLoopMax)
	{
		if (PlayerOK() && FunctionOK('SetLoopMax', typeof(m_oPlayerObject.SetLoopMax)))
			m_oPlayerObject.SetLoopMax(iLoopMax);
	}

	// public
	this.SetAutoPlay = SetAutoPlay;
	function SetAutoPlay(bAutoPlay)
	{
		if (PlayerOK() && FunctionOK('SetAutoPlay', typeof(m_oPlayerObject.SetAutoPlay)))
			m_oPlayerObject.SetAutoPlay(bAutoPlay);
	}

	// public
	this.SetVolume = SetVolume;
	function SetVolume(iVolume)
	{
		if (PlayerOK() && FunctionOK('SetVolume', typeof(m_oPlayerObject.SetVolume)))
			m_oPlayerObject.SetVolume(iVolume);
	}

	// public
	this.GetLength = GetLength;
	function GetLength()
	{
		if (PlayerOK() && FunctionOK('GetLength', typeof(m_oPlayerObject.GetLength)))
			return m_oPlayerObject.GetLength();
		return -1;
	}

	// public
	this.GetMetaData = GetMetaData;
	function GetMetaData(sField)
	{
		// sField must be one of the following:
		// 'artist', 'track', 'comment', 'title/songName', 'album', 'genre', 'year'
		if (sField == 'title')
			sField = 'songName';
		if (PlayerOK() && FunctionOK('GetMetaData', typeof(m_oPlayerObject.GetMetaData)))
		{
			var sMetaData = m_oPlayerObject.GetMetaData(sField);
			// Unquote the data
			var regexQuoted = /("[^"]*")/;
			if (regexQuoted.test(sMetaData))
				sMetaData = sMetaData.slice(1, -1)
			return sMetaData;
		}
		return '';
	}

	// private
	this.PlayerOK = PlayerOK;
	function PlayerOK()
	{
		if (!m_oPlayerObject)
			m_oPlayerObject = document.getElementById(m_idPlayerObject);

		return (m_oPlayerObject != null);
	}

	// private
	this.FunctionOK = FunctionOK;
	function FunctionOK(sFunctionName, sFunctionType)
	{
		if (sFunctionType == 'function')
			return true;

		if (!m_bPlayerFunctionFailure)
			alert('Could not access player function "' + sFunctionName + '"');
		
		m_bPlayerFunctionFailure = true;
		return false;
	}
}
