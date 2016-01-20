// This file implements the initilization, button handlers, and callbacks for the MobilizeClip.js audio clipping UI
//		MobilizeClip.js must be included before this file
//		EmbedFlash.js must be included before this file
//		EmbedWMPlayer.js must be included before this file
//		EmbedQTPlayer.js must be included before this file
//		PlayMP3Clip.js must be included before this file
//		PlayWMPClip.js must be included before this file
//		PlayQTClip.js must be included before this file
// The following HTML elements are assumed to exist by this implementation:
//		idClipLoop, idClipStart, idClipEnd, idClipLength, idClipPlayPosition, idClipMetaData

var m_sLastError = '';
var m_MobilizeClip = null;
var m_PlayAudioClip = null;

// FactoryMP3MobilizeCrop() constructs the Audio clipper using the args object
//	@param Object args Contains the following indexes
//	+ 'swfPath' : string, the path th the PlayMP3Clip.swf file
//	+ 'previewUrl' : string, the full url to the Audio file
//	+ 'crossDomainPolicyUrl' : string, the full url to the crossdomain XML policy file
//	+ 'imagesUrl' : string, the full url to the base dir of clipper images
//	+ 'trackSeconds' : track length in seconds
function FactoryMP3MobilizeCrop(args)
{
	var sSwfPath = args.swfPath;
	var sAudioUrl = args.previewUrl;
	var sCrossDomainPolicyUrl = args.crossDomainPolicyUrl;
	var imagesUrl = args.imagesUrl;
	var msClipLength = args.trackSeconds * 1000;

	// Use the Flash player for MP3 files only; otherwise use Windows Media Player
	var iIndex = sAudioUrl.lastIndexOf('.');
	var sExtension = sAudioUrl.substr(iIndex);
	sExtension.toLowerCase();
	var bUseFlash = (sExtension.indexOf('.mp3') >= 0);
	var bUseQuickTime = (sExtension.indexOf('.m4a') >= 0);
	if (bUseFlash)
		m_PlayAudioClip = new CPlayMP3Clip(sSwfPath, sAudioUrl, sCrossDomainPolicyUrl);
	else
	if (bUseQuickTime)
		m_PlayAudioClip = new CPlayQTClip(sAudioUrl);
	else
		m_PlayAudioClip = new CPlayWMPClip(sAudioUrl);

	m_MobilizeClip = new CMobilizeClip(
        msClipLength,	// length of the clip in milliseconds
        30000,			// length of the maximum selection in milliseconds
		imagesUrl + 'rt_clip/Background.gif',
		500, 70,		// width and height of the background image
		imagesUrl + 'rt_clip/HandleLeft.gif',
		imagesUrl + 'rt_clip/HandleRight.gif',
		16, 90,			// width and height of the handle image
		6,				// width of the overhang on each side of the handle in the image
		2, 80, 65, 60,	// width of the selection border, and the width of the length, start, and end labels
		OnClipChanged);
	
	if (window.isLoaded)
		Initialize();
	else
		addLoadEvent(Initialize);
}

function Initialize()
{
	if (!m_MobilizeClip || !m_PlayAudioClip)
		return;

	try
	{
		m_MobilizeClip.Initialize('idClipDisplayContainer', 'idClipStart', 'idClipEnd', 0/*msNewClipLength*/);
		m_PlayAudioClip.Initialize('idPlayMP3ClipContainer', '_idPlayerObject');

		// Update the UI when the player isn't installed
		var bPlayerIsInstalled = m_PlayAudioClip.PlayerIsInstalled();
		var bPlayerIsUpToDate = m_PlayAudioClip.PlayerIsUpToDate();
		var bPlayerOK = (bPlayerIsInstalled && bPlayerIsUpToDate);

		YAHOO.util.Dom.setStyle('idClipInfoFlashOK', 'display', (bPlayerOK ? 'block' : 'none'));
		YAHOO.util.Dom.setStyle('idClipInfoFlashNotOK', 'display', (!bPlayerOK ? 'block' : 'none'));
		YAHOO.util.Dom.setStyle('idClipInfoNoFlash', 'display', (!bPlayerIsInstalled ? 'block' : 'none'));
		YAHOO.util.Dom.setStyle('idClipInfoOldFlash', 'display', (bPlayerIsInstalled && !bPlayerIsUpToDate ? 'block' : 'none'));
	}
	catch(e)
	{
		//alert('Initialize Error: ' + e.message);
	}
}

function addLoadEvent(fnOnload)
{
	if (typeof(window.onload) != 'function')
		window.onload = fnOnload;
	else
	{
		var fnOldOnload = window.onload;
		window.onload = function()
		{
			fnOldOnload();
			fnOnload();
		}
	}
}

function OnClipPlayFullSong()
{
	SetLoop();
	m_PlayAudioClip.SetStart(0);
	m_PlayAudioClip.SetEnd(0);
	m_PlayAudioClip.DoPlay(true/*bFromStart*/);
}

function OnClipPlayFromStart()
{
	SetLoop();
	SetStartEnd();
	m_PlayAudioClip.DoPlay(true/*bFromStart*/);
}

function OnClipPlay()
{
	SetLoop();
	SetStartEnd();
	m_PlayAudioClip.DoPlay(false/*bFromStart*/);
}

function OnClipStop()
{
	m_PlayAudioClip.DoStop();
}

function OnClipLoop(oLoopCheckbox)
{
	SetLoop(oLoopCheckbox);
}

function OnClipVolume(iVolumePercent)
{
	m_PlayAudioClip.SetVolume(iVolumePercent);
}

// A CMobilizeClip callback; called OnMouseUp by the MobilizeClip UI
function OnClipChanged()
{
	SetStartEnd();
}

function SetLoop(oLoopCheckbox)
{
	if (!oLoopCheckbox)
		oLoopCheckbox = document.getElementById('idClipLoop');
	var bLoop = oLoopCheckbox.checked;
	m_PlayAudioClip.SetLoop(bLoop);
}

function SetStartEnd()
{
	// Read the hidden variables from the MobilizeClip UI, and pass them on to the m_PlayAudioClip object
	var iStart = document.getElementById('idClipStart').value;
	var iEnd = document.getElementById('idClipEnd').value;
	m_PlayAudioClip.SetStart(iStart);
	m_PlayAudioClip.SetEnd(iEnd);
}

// A PlayMP3Clip.swf or PlayWMPClip.js or PlayQTClip.js callback
function OnLoadProgress(iLoadPercentage, msClipLength, sProgressMessage)
{
	var bDone = (iLoadPercentage == 100);
	if (bDone && !m_MobilizeClip.IsInitialized())
		m_MobilizeClip.Initialize('idClipDisplayContainer', 'idClipStart', 'idClipEnd', msClipLength);
	
	var object = document.getElementById('idClipLength');
	if (object)
	{
		if (typeof(sProgressMessage) == 'undefined')
			sProgressMessage = (bDone ? '' : ' (' + iLoadPercentage + '%)')
		var sMsg = FormatTime(msClipLength, false/*bShowTenths*/) + sProgressMessage;
		object.innerHTML = sMsg + ' ' + m_sLastError;
	}
}

// A PlayMP3Clip.swf or PlayWMPClip.js or PlayQTClip.js callback
function OnPlayProgress(msPlayPosition, bEnd)
{
	var object = document.getElementById('idClipPlayPosition');
	if (object)
	{
		var sMsg = FormatTime(msPlayPosition, true/*bShowTenths*/);
		object.innerHTML = sMsg;
	}

	// Update the user interface
	m_MobilizeClip.SetPlayPosition(msPlayPosition);
}

// A PlayMP3Clip.swf or PlayWMPClip.js or PlayQTClip.js callback
function OnMetaDataAvailable()
{
	var object = document.getElementById('mobilizeTitle');
	if (object)
	{
		var sTitle = m_PlayAudioClip.GetMetaData('title');
		if (sTitle != '')
			object.value = sTitle;
	}
	
	object = document.getElementById('idClipMetaData');
	if (object)
	{
		var sMsg = '';
		var sData = '';
		sData = m_PlayAudioClip.GetMetaData('artist');
		if (sData != '') sMsg += '  artist=' + sData;
		sData = m_PlayAudioClip.GetMetaData('title');
		if (sData != '') sMsg += '  title=' + sData;
		sData = m_PlayAudioClip.GetMetaData('album');
		if (sData != '') sMsg += '  album=' + sData;
		sData = m_PlayAudioClip.GetMetaData('track');
		if (sData != '') sMsg += '  track=' + sData;
		sData = m_PlayAudioClip.GetMetaData('comment');
		if (sData != '') sMsg += '  comment=' + sData;
		sData = m_PlayAudioClip.GetMetaData('genre');
		if (sData != '') sMsg += '  genre=' + sData;
		sData = m_PlayAudioClip.GetMetaData('year'); 
		if (sData != '') sMsg += '  year=' + sData;

		object.innerHTML = sMsg;
	}
}

// A PlayMP3Clip.swf or PlayWMPClip.js or PlayQTClip.js callback
function OnError(sError)
{
	if (m_sLastError != '') m_sLastError += '<br>';
	m_sLastError += sError;
	var object = document.getElementById('idClipLength');
	if (object)
		object.innerHTML = m_sLastError;
}

function FormatTime(iMilliSeconds, bShowTenths)
{
	var iTenths = Math.round(iMilliSeconds / 100);
	var iSeconds = Math.floor(iTenths / 10);
	var iMinutes = Math.floor(iSeconds / 60);
	iSeconds %= 60;
	iTenths %= 10;
	var sMinutes = (!iMinutes ? '0' : iMinutes);
	var sSeconds = (iSeconds < 10 ? '0' + iSeconds : iSeconds);
	var sTenths = (bShowTenths ? '.' + iTenths : '');
	return sMinutes + ':' + sSeconds + sTenths;
}
