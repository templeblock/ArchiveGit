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
	var m_iTimeScale = 0;

	// Constants
	var m_iTimerInterval = 100;

	// Create the MPlayer object that will do all the audio playback
	var bUseObjectTagAlways = false;
	var m_oPlayer = new CEmbedQTPlayer(sAudioUrl, bUseObjectTagAlways, '1'/*iWidth 580*/, '1'/*iHeight 150*/, '9'/*Version*/);
	m_oPlayer.addParam('enablejavascript', 'true');
	m_oPlayer.addParam('Controller', 'true'); // shows/hides the controller UI
	m_oPlayer.addParam('AutoPlay', 'false');
	m_oPlayer.addParam('Loop', 'false');

	// public
	function f_Play()					{ var s = null;	if (PlayerOK()) { try { m_oPlayerObject.Play();						Trace('f_Play '				+s); }	catch(e) { OnError('X f_Play '				+e); } } return s; }
	function f_Stop()					{ var s = null;	if (PlayerOK()) { try { m_oPlayerObject.Stop();						Trace('f_Stop '				+s); }	catch(e) { OnError('X f_Stop '				+e); } } return s; }
	function f_Pause()					{ var s = null;	if (PlayerOK()) { try { m_oPlayerObject.Stop();						Trace('f_Pause '			+s); }	catch(e) { OnError('X f_Pause '				+e); } } return s; }
	function f_SetTime(iTime)			{ var s = null;	if (PlayerOK()) { try { m_oPlayerObject.SetTime(iTime);				Trace('f_SetTime '			+s); }	catch(e) { OnError('X f_SetTime '			+e); } } return s; }
	function f_SetAutoPlay(bAutoPlay)	{ var s = null;	if (PlayerOK()) { try { m_oPlayerObject.SetAutoPlay(bAutoPlay);		Trace('f_SetAutoPlay '		+s); }	catch(e) { OnError('X f_SetAutoPlay '		+e); } } return s; }
	function f_SetUrl(sAudioUrl)		{ var s = null;	if (PlayerOK()) { try { m_oPlayerObject.SetURL(sAudioUrl);			Trace('f_SetUrl '			+s); }	catch(e) { OnError('X f_SetUrl '			+e); } } return s; }
	function f_SetVolume(iVolume)		{ var s = null;	if (PlayerOK()) { try { m_oPlayerObject.SetVolume(iVolume);			Trace('f_SetVolume '		+s); }	catch(e) { OnError('X f_SetVolume '			+e); } } return s; }
	function f_SetMute(bMute)			{ var s = null;	if (PlayerOK()) { try { m_oPlayerObject.SetVolume(bMute ? 0 : 255);	Trace('f_SetMute '			+s); }	catch(e) { OnError('X f_SetMute '			+e); } } return s; }
	function f_GetDuration()			{ var s = 0;	if (PlayerOK()) { try { s = m_oPlayerObject.GetDuration();			Trace('f_GetDuration '		+s); }	catch(e) { OnError('X f_GetDuration '		+e); } } return s; }
	function f_GetTimeScale()			{ var s = 0;	if (PlayerOK()) { try { s = m_oPlayerObject.GetTimeScale();			Trace('f_GetTimeScale '		+s); }	catch(e) { OnError('X f_GetTimeScale '		+e); } } return s; }
	function f_GetTime()				{ var s = 0;	if (PlayerOK()) { try { s = m_oPlayerObject.GetTime();				Trace('f_GetTime '			+s); }	catch(e) { OnError('X f_GetTime '			+e); } } return s; }
	function f_GetMetaData(sField)		{ var s = '';	if (PlayerOK()) { try { s = m_oPlayerObject.GetUserData(sField);	Trace('f_GetMetaData '		+s); }	catch(e) {   Trace('X f_GetMetaData '		+e); } } return s; }
	function f_GetPluginStatus()		{ var s = '';	if (PlayerOK()) { try { s = m_oPlayerObject.GetPluginStatus();		Trace('f_GetPluginStatus '	+s); }	catch(e) { OnError('X f_GetPluginStatus '	+e); } } return s; }
	function f_GetPluginVersion()		{ var s = '';	if (PlayerOK()) { try { s = m_oPlayerObject.GetPluginVersion();		Trace('f_GetPluginVersion '	+s); }	catch(e) { OnError('X f_GetPluginVersion '	+e); } } return s; }

	// public
	this.Initialize = Initialize;
	function Initialize(idPlayerContainer, idPlayerObject)
	{
		m_idPlayerObject = idPlayerObject;
		m_oPlayer.Initialize(idPlayerContainer, idPlayerObject);

		// Start the timer to track load progress
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
		var iClipLength = TimeToMSeconds(f_GetDuration());
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
		f_SetTime(MSecondsToTime(m_iLastPlayPosition));
		f_Play();

		// Start the timer to track play position and poll for the end point
		m_oPlayPositionTimer = setInterval(OnPlayPositionTimer, m_iTimerInterval);
	}

	// public
	this.DoStop = DoStop;
	function DoStop(bDuringReplay)
	{
		if (!m_oPlayPositionTimer) // If not playing...
			return;

		var bResetLoopCount = !bDuringReplay;
		if (bResetLoopCount)
			m_iLoopCount = 0;

		clearInterval(m_oPlayPositionTimer); // Stop this timer
		m_oPlayPositionTimer = null;

		f_Pause();
		m_iLastPlayPosition = TimeToMSeconds(f_GetTime());
		Trace('DoStop: ' + m_iLastPlayPosition);
	}

	// public
	this.SetUrl = SetUrl;
	function SetUrl(sAudioUrl)
	{
		m_sAudioUrl = sAudioUrl;
		f_SetUrl(sAudioUrl);
	}

	// public
	this.SetStart = SetStart;
	function SetStart(iStart)
	{
		m_iStart = iStart;
		Trace('SetStart: ' + m_iStart);
	}

	// public
	this.SetEnd = SetEnd;
	function SetEnd(iEnd)
	{
		m_iEnd = iEnd;
		Trace('SetEnd: ' + m_iEnd);
	}

	// public
	this.SetLoop = SetLoop;
	function SetLoop(bLoop)
	{
		m_bLoop = bLoop;
		Trace('SetLoop: ' + m_bLoop);
	}

	// public
	this.SetLoopMax = SetLoopMax;
	function SetLoopMax(iLoopMax)
	{
		m_iLoopMax = iLoopMax;
		Trace('SetLoopMax: ' + m_iLoopMax);
	}

	// public
	this.SetAutoPlay = SetAutoPlay;
	function SetAutoPlay(bAutoPlay)
	{
		m_bAutoPlay = bAutoPlay;
		f_SetAutoPlay(bAutoPlay);

		// If autoplay was indicated, play the sound now
		if (m_bAutoPlay)
			DoPlay(true/*bFromStart*/);
	}

	// public
	this.SetVolume = SetVolume;
	function SetVolume(iVolume)
	{
		f_SetVolume(Math.round(iVolume * 2.55));
		Trace('SetVolume: ' + iVolume);
	}

	// public
	this.GetLength = GetLength;
	function GetLength()
	{
		var iClipLength = TimeToMSeconds(f_GetDuration());
		return iClipLength;
	}

	// public
	this.GetMetaData = GetMetaData;
	function GetMetaData(sField)
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

		var sMetaData = f_GetMetaData(sField);
		return sMetaData;
	}

	// private
	this.OnLoadProgressTimer = OnLoadProgressTimer;
	function OnLoadProgressTimer()
	{
		var iClipLength = 0;
		var sStatus = 'Loading...';
		try
		{
			sStatus = f_GetPluginStatus();
			// One of: 'Waiting', 'Loading', 'Playable', 'Complete', 'Error: <error number>'
			if (sStatus.indexOf('Error') >= 0)
				iClipLength = 60 * 1000;
			else
			if (sStatus.indexOf('Playable') >= 0 || sStatus.indexOf('Complete') >= 0)
				iClipLength = TimeToMSeconds(f_GetDuration());
		}
		catch(e)
		{
		}

		var iLoadPercentage = 0;
		var sProgressMessage = '';
		if (iClipLength <= 0)
		{ // Still loading
			iLoadPercentage = 0;
			sProgressMessage = ' ' + sStatus;
		}
		else
		{ // Fully loaded
			// Since we used AutoStart to start the load process, stop it from playing now
			f_Stop();

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
		var iPosition = TimeToMSeconds(f_GetTime());
		var bPositionChanged = (m_iLastPlayPosition != iPosition);
		m_iLastPlayPosition = iPosition;
		if (bPositionChanged)
		{
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
//jif (document.all) //j If !IE show all Trace messages for now
			return;

		// Call JavaScript function; let the caller know what the error is
		OnError(sMsg);
	}

	// private
	this.PlayerOK = PlayerOK;
	function PlayerOK()
	{
		if (!m_oPlayerObject)
		{
			m_oPlayerObject = document.getElementById(m_idPlayerObject);
			m_oPlayerObject.style.visibility = 'hidden';
		}

		return (m_oPlayerObject != null);
	}

	// private
	this.TimeToMSeconds = TimeToMSeconds;
	function TimeToMSeconds(fTime)
	{
		var iTimeScale = (m_iTimeScale ? m_iTimeScale : f_GetTimeScale());
		if (!iTimeScale) iTimeScale = 44100; m_iTimeScale = iTimeScale;
		return Math.floor((fTime * 1000) / iTimeScale);
	}

	// private
	this.MSecondsToTime = MSecondsToTime;
	function MSecondsToTime(iMSeconds)
	{
		var iTimeScale = (m_iTimeScale ? m_iTimeScale : f_GetTimeScale());
		if (!iTimeScale) iTimeScale = 44100; m_iTimeScale = iTimeScale;
		return (iMSeconds * iTimeScale) / 1000;
	}
}
