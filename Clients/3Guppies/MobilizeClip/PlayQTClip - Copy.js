// Notes: try/catch on all plugin functions
// Author: Jim McCurdy, Face To Face Software, jmccurdy@facetofacesoftware.com
//
// A Javascript class that implements an audio client-side player using QuickTime
// EmbedQTPlayer.js must be included before this file
function CPlayQTClip(sAudioUrl)
{
	// private
	var m_idPlayerObject = null;
	var m_oPlayerObject = null;

	var m_sAudioUrl = sAudioUrl;
	var m_iStart = 0;
	var m_iEnd = 0;
	var m_bLoop = false;
	var m_iLoopMax = 0;
	var m_bAutoPlay = false;
	var m_oPlayPositionTimer = null;
	var m_iLoopCount;
	var m_iLastPlayPosition = 0;
	var m_oLoadProgressTimer = null;

	// Constants
	var m_iTimerInterval = 100;

	// Create the MPlayer object that will do all the audio playback
	var bUseObjectTagAlways = true;
	var m_oPlayer = null;
if (document.all) //j If !IE show the Controller for now
	m_oPlayer = new CEmbedQTPlayer(sAudioUrl, bUseObjectTagAlways, '1'/*iWidth 580*/, '1'/*iHeight 150*/, '9'/*Version*/);
else
	m_oPlayer = new CEmbedQTPlayer(sAudioUrl, bUseObjectTagAlways, '580'/*iWidth 580*/, '150'/*iHeight 150*/, '9'/*Version*/);
	m_oPlayer.addParam('enablejavascript', 'true');
if (document.all) //j If !IE show the Controller for now
	m_oPlayer.addParam('Controller', 'true'); // hides the UI
	m_oPlayer.addParam('AutoPlay', 'false');
	m_oPlayer.addParam('Loop', 'false');

	// public
	this.Initialize = Initialize;
	function Initialize(idPlayerContainer, idPlayerObject)
	{
		m_idPlayerObject = idPlayerObject;
		m_oPlayer.Initialize(idPlayerContainer, idPlayerObject);

		// Start the timer to track load progress
if (document.all) //j If !IE show the Controller for now
		m_oLoadProgressTimer = setInterval(OnLoadProgressTimer, m_iTimerInterval);
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
		if (!PlayerOK())
			return;
if (!document.all) alert('1');
if (!document.all) alert(m_oPlayerObject.GetDuration());
		var iClipLength = TimeToMSeconds(m_oPlayerObject.GetDuration());
if (!document.all) alert(iClipLength);
		if (iClipLength <= 0)
		{
			Trace('DoPlay: Nothing to play');
			return;
		}

		DoStop(true/*bDuringReplay*/);

		if (m_iLastPlayPosition < m_iStart || m_iLastPlayPosition >= m_iEnd)
			bFromStart = true;
		if (bFromStart)
			m_iLastPlayPosition = m_iStart;
		if (m_iLastPlayPosition > iClipLength)
		{
			Trace('DoPlay: Position Error: ' + m_iLastPlayPosition + ' > ' + iClipLength);
			m_iLastPlayPosition = 0;
		}

		Trace('DoPlay: ' + m_iLastPlayPosition);
		m_oPlayerObject.SetTime(MSecondsToTime(m_iLastPlayPosition));
		m_oPlayerObject.Play();

		// Start the timer to track play position and poll for the end point
		m_oPlayPositionTimer = setInterval(OnPlayPositionTimer, m_iTimerInterval);
	}

	// public
	this.DoStop = DoStop;
	function DoStop(bDuringReplay)
	{
		if (!PlayerOK())
			return;

		if (!m_oPlayPositionTimer) // If not playing...
			return;

		var bResetLoopCount = !bDuringReplay;
		if (bResetLoopCount)
			m_iLoopCount = 0;

		clearInterval(m_oPlayPositionTimer); // Stop this timer
		m_oPlayPositionTimer = null;

		m_oPlayerObject.Stop();
		m_iLastPlayPosition = TimeToMSeconds(m_oPlayerObject.GetTime());
		Trace('DoStop: ' + m_iLastPlayPosition);
	}

	// public
	this.SetUrl = SetUrl;
	function SetUrl(sAudioUrl)
	{
		if (!PlayerOK())
			return;

		m_sAudioUrl = sAudioUrl;
		m_oPlayerObject.SetURL(sAudioUrl);
	}

	// public
	this.SetStart = SetStart;
	function SetStart(iStart)
	{
		if (!PlayerOK())
			return;

		m_iStart = iStart;
		Trace('SetStart: ' + m_iStart);
	}

	// public
	this.SetEnd = SetEnd;
	function SetEnd(iEnd)
	{
		if (!PlayerOK())
			return;

		m_iEnd = iEnd;
		Trace('SetEnd: ' + m_iEnd);
	}

	// public
	this.SetLoop = SetLoop;
	function SetLoop(bLoop)
	{
		if (!PlayerOK())
			return;

		m_bLoop = bLoop; //j
		Trace('SetLoop: ' + m_bLoop);

		if (!PlayerOK())//j
			return;

		m_bLoop = bLoop;
		Trace('SetLoop: ' + m_bLoop);
	}

	// public
	this.SetLoopMax = SetLoopMax;
	function SetLoopMax(iLoopMax)
	{
		if (!PlayerOK())
			return;

		m_iLoopMax = iLoopMax;
		Trace('SetLoopMax: ' + m_iLoopMax);
	}

	// public
	this.SetAutoPlay = SetAutoPlay;
	function SetAutoPlay(bAutoPlay)
	{
		if (!PlayerOK())
			return;

		m_bAutoPlay = bAutoPlay;
		m_oPlayerObject.SetAutoPlay(bAutoPlay);

		// If autoplay was indicated, play the sound now
		if (m_bAutoPlay)
			DoPlay(true/*bFromStart*/);
	}

	// public
	this.SetVolume = SetVolume;
	function SetVolume(iVolume)
	{
		if (!PlayerOK())
			return;

		m_oPlayerObject.SetVolume(Math.round(iVolume * 2.55));
		Trace('SetVolume: ' + iVolume);
	}

	// public
	this.GetLength = GetLength;
	function GetLength()
	{
		if (!PlayerOK())
			return -1;

		var iClipLength = TimeToMSeconds(m_oPlayerObject.GetDuration());
		return iClipLength;
	}

	// public
	this.GetMetaData = GetMetaData;
	function GetMetaData(sField)
	{
		if (!PlayerOK())
			return '';

		try
		{
			// sField must be one of the following:
			// 'artist', 'track', 'comment', 'title/songName', 'album', 'genre', 'year'
			if (sField == 'artist')		sField = '©art';			else
			if (sField == 'track')		sField = '©trk';			else
			if (sField == 'comment')	sField = '©cmt';			else
			if (sField == 'title')		sField = '©nam';			else
			if (sField == 'songName')	sField = '©nam';			else
			if (sField == 'album')		sField = '©alb';			else
			if (sField == 'genre')		sField = '©inf';			else
			if (sField == 'year')		sField = '©day';

			var sMetaData = m_oPlayerObject.GetUserData(sField);
			//Trace('GetMetaData: ' + sField + '="' + sMetaData + '"');
			return sMetaData;
		}
		catch(e)
		{
		}

		return '';
	}

	// private
	this.OnLoadProgressTimer = OnLoadProgressTimer;
	function OnLoadProgressTimer()
	{
		if (!PlayerOK())
			return;

		var iClipLength = 0;
		var sStatus = 'Loading...';
		try
		{
			iClipLength = TimeToMSeconds(m_oPlayerObject.GetDuration());
			sStatus = m_oPlayerObject.GetPluginStatus();
		}
		catch(e)
		{
			Trace('OnLoadProgressTimer: GetPluginStatus failed - m_oPlayerObject=' + m_oPlayerObject);
		}

		var iLoadPercentage = 0;
		var sProgressMessage = '';
		if (iClipLength <= 0)
		{
			iLoadPercentage = 0;
			sProgressMessage = ' ' + sStatus;
		}
		else
		{ // Fully loaded
			iLoadPercentage = 100;
			sProgressMessage = '';

			// Stop this timer; we don't need it anymore
			clearInterval(m_oLoadProgressTimer);
			m_oLoadProgressTimer = null;
		}

		OnLoadProgress(iLoadPercentage, iClipLength, sProgressMessage);

		if (!m_oLoadProgressTimer) // if we are done loading
		{
			// Call JavaScript function; let the caller know that the MetaData is available now
			OnMetaDataAvailable();
		}
	}

	// private
	this.OnPlayPositionTimer = OnPlayPositionTimer;
	function OnPlayPositionTimer()
	{
		if (!PlayerOK())
			return;

		var iPosition = TimeToMSeconds(m_oPlayerObject.GetTime());
		var bPositionChanged = (m_iLastPlayPosition != iPosition);
		m_iLastPlayPosition = iPosition;
		if (bPositionChanged)
		{
			//Trace('OnPlayPositionTimer: ' + m_iLastPlayPosition);

			// Call JavaScript function; let the caller know what the play position is
			OnPlayProgress(m_iLastPlayPosition, false/*bEnd*/);
		}

		// If there is an end point setup, check to see if we have exceeded it
		// Allow for some slop when comparing against the m_iStart due to slight timing inaccuracies
		var bEnd = ((m_iEnd && m_iLastPlayPosition >= m_iEnd) || m_iLastPlayPosition < m_iStart-1500/*1.5 seconds*/);
		if (bEnd)
			OnSoundComplete();
	}

	// private
	this.OnSoundComplete = OnSoundComplete;
	function OnSoundComplete()
	{
		Trace('OnSoundComplete: ' + m_iStart + '-' + m_iEnd + ' ' + m_iLastPlayPosition);
		DoStop();

		// Call JavaScript function; let the caller know what the play position is
		OnPlayProgress(m_iLastPlayPosition, true/*bEnd*/);

		if (m_bLoop && (!m_iLoopMax || ++m_iLoopCount < m_iLoopMax))
			DoPlay(true/*bFromStart*/);
	}

	// private
	this.Trace = Trace;
	function Trace(sMsg)
	{
		var oTitle = document.getElementById('mobilizeTitle');
		if (!oTitle || oTitle.value != 'trace')
if (document.all) //j If !IE show all Trace messages for now
			return;

		// Call JavaScript function; let the caller know what the error is
		OnError(sMsg);
	}

	// private
	this.PlayerOK = PlayerOK;
	function PlayerOK()
	{
var bSendDone = false;
if (!document.all) //j
{
	bSendDone = false;//j(!m_oPlayerObject);
	m_oPlayerObject = null;
}
		if (!m_oPlayerObject)
		{
			m_oPlayerObject = document.getElementById(m_idPlayerObject);
if (!document.all) //j
{
//j	alert('oPlayerObject type=' + typeof(m_oPlayerObject) + ', ' + m_oPlayerObject);

	try { s=m_oPlayerObject.GetDuration();			Trace('ok GetDuration			'+s); }	catch(e) {	 Trace('NG GetDuration			'+e); } 
//	try { s=m_oPlayerObject.GetTimeScale();			Trace('ok GetTimeScale			'+s); }	catch(e) {	 Trace('NG GetTimeScale			'+e); } 
//	try { s=m_oPlayerObject.GetTime();				Trace('ok GetTime				'+s); }	catch(e) {	 Trace('NG GetTime				'+e); } 
//	try { s=m_oPlayerObject.SetTime(0);				Trace('ok SetTime				'+s); }	catch(e) {	 Trace('NG SetTime				'+e); } 
//	try { s=m_oPlayerObject.Play();					Trace('ok Play					'+s); }	catch(e) {	 Trace('NG Play					'+e); } 
//	try { s=m_oPlayerObject.Stop();					Trace('ok Stop					'+s); }	catch(e) {	 Trace('NG Stop					'+e); } 
//	try { s=m_oPlayerObject.SetAutoPlay(true);		Trace('ok SetAutoPlay			'+s); }	catch(e) {	 Trace('NG SetAutoPlay			'+e); } 
//	try { s=m_oPlayerObject.SetVolume(255);			Trace('ok SetVolume				'+s); }	catch(e) {	 Trace('NG SetVolume			'+e); } 
//	try { s=m_oPlayerObject.GetUserData('©nam');	Trace('ok GetUserData			'+s); }	catch(e) {	 Trace('NG GetUserData			'+e); } 
//	try { s=m_oPlayerObject.GetPluginStatus();		Trace('ok GetPluginStatus		'+s); }	catch(e) {	 Trace('NG GetPluginStatus		'+e); } 
//	try { s=m_oPlayerObject.GetPluginVersion();		Trace('ok GetPluginVersion		'+s); }	catch(e) {	 Trace('NG GetPluginVersion		'+e); } 
//	try { s=m_oPlayerObject.GetQuickTimeVersion();	Trace('ok GetQuickTimeVersion	'+s); }	catch(e) {	 Trace('NG GetQuickTimeVersion	'+e); } 
	if (bSendDone)
		OnLoadProgress(100, 172000, 'Done');

//j	m_oPlayerObject = document.getElementById(m_idPlayerObject);
//j	alert('oPlayerObject type=' + typeof(m_oPlayerObject) + ', ' + m_oPlayerObject);

	try { s=m_oPlayerObject.GetDuration();			Trace('ok GetDuration			'+s); }	catch(e) {	 Trace('NG GetDuration			'+e); } 
//	try { s=m_oPlayerObject.GetTimeScale();			Trace('ok GetTimeScale			'+s); }	catch(e) {	 Trace('NG GetTimeScale			'+e); } 
//	try { s=m_oPlayerObject.GetTime();				Trace('ok GetTime				'+s); }	catch(e) {	 Trace('NG GetTime				'+e); } 
//	try { s=m_oPlayerObject.SetTime(0);				Trace('ok SetTime				'+s); }	catch(e) {	 Trace('NG SetTime				'+e); } 
//	try { s=m_oPlayerObject.Play();					Trace('ok Play					'+s); }	catch(e) {	 Trace('NG Play					'+e); } 
//	try { s=m_oPlayerObject.Stop();					Trace('ok Stop					'+s); }	catch(e) {	 Trace('NG Stop					'+e); } 
//	try { s=m_oPlayerObject.SetAutoPlay(true);		Trace('ok SetAutoPlay			'+s); }	catch(e) {	 Trace('NG SetAutoPlay			'+e); } 
//	try { s=m_oPlayerObject.SetVolume(255);			Trace('ok SetVolume				'+s); }	catch(e) {	 Trace('NG SetVolume			'+e); } 
//	try { s=m_oPlayerObject.GetUserData('©nam');	Trace('ok GetUserData			'+s); }	catch(e) {	 Trace('NG GetUserData			'+e); } 
//	try { s=m_oPlayerObject.GetPluginStatus();		Trace('ok GetPluginStatus		'+s); }	catch(e) {	 Trace('NG GetPluginStatus		'+e); } 
//	try { s=m_oPlayerObject.GetPluginVersion();		Trace('ok GetPluginVersion		'+s); }	catch(e) {	 Trace('NG GetPluginVersion		'+e); } 
//	try { s=m_oPlayerObject.GetQuickTimeVersion();	Trace('ok GetQuickTimeVersion	'+s); }	catch(e) {	 Trace('NG GetQuickTimeVersion	'+e); } 
}
		}

		return (m_oPlayerObject != null);
	}

	// private
	this.TimeToMSeconds = TimeToMSeconds;
	function TimeToMSeconds(iTime)
	{
		if (!PlayerOK())
			return 0;

		var iTimeScale = m_oPlayerObject.GetTimeScale();
		if (!iTimeScale) iTimeScale = 44100;
		return Math.floor((iTime * 1000) / iTimeScale);
	}

	// private
	this.MSecondsToTime = MSecondsToTime;
	function MSecondsToTime(iMSeconds)
	{
		if (!PlayerOK())
			return 0;

		iTimeScale = m_oPlayerObject.GetTimeScale();
		if (!iTimeScale) iTimeScale = 44100;
		return (iMSeconds * iTimeScale) / 1000;
	}
}
