// Author: Jim McCurdy, Face To Face Software, jmccurdy@facetofacesoftware.com

package
{
	import flash.system.Security;
	import flash.external.ExternalInterface;
	import flash.display.Stage;
	import flash.display.StageDisplayState;
	import flash.display.Sprite;
	import flash.display.LoaderInfo;
	import flash.display.InteractiveObject;
	import flash.events.*;
	import flash.net.URLRequest;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.media.Sound;
	import flash.media.SoundLoaderContext;
	import flash.media.SoundTransform;
	import flash.media.SoundChannel;
	import flash.media.ID3Info;
	import flash.utils.Timer;
	import flash.ui.ContextMenu;
	import flash.ui.ContextMenuItem;
	import flash.ui.ContextMenuBuiltInItems;
	import mx.controls.Alert;

	public class PlayMP3Clip extends Sprite
	{
		// Set as Flashvars
		private var m_sUrl:String = "";
		private var m_sPolicyUrl:String = "";
		private var m_iStart:int = 0;
		private var m_iEnd:int = 0;
		private var m_bLoop:Boolean = false;
		private var m_iLoopMax:int = 0;
		private var m_bAutoPlay:Boolean = false;
		private var m_iVolume:int = 50;

		// Private
		private var m_tf:TextField;
		private var m_oSound:Sound;
		private var m_oSoundTransform:SoundTransform;
		private var m_oSoundChannel:SoundChannel;
		private var m_oPlayPositionTimer:Timer;
		private var m_iPercentLoaded:int;
		private var m_iLoopCount:int;
		private var m_iLastPlayPosition:int;

		// Constants
		private var m_iTimerInterval:uint = 100;

		function PlayMP3Clip()
		{
			try
			{
//j				super(); // What does this do?

				var menuItem:ContextMenuItem = new ContextMenuItem("PlayMP3Clip Log");
				menuItem.addEventListener(ContextMenuEvent.MENU_ITEM_SELECT, OnMenuSelect);
				menuItem.separatorBefore = false;
				menuItem.enabled = true; 
				menuItem.visible = true;

				var menu:ContextMenu = new ContextMenu();
				menu.customItems.push(menuItem);
				menu.hideBuiltInItems();
				this.contextMenu = menu;

				// Create a text field to display log messages
				// Only seen if the Flash object visible on the page
				var format:TextFormat = new TextFormat();
				format.font = "Verdana";
				format.color = 0x000000;
				format.size = 10;

				m_tf = new TextField();
				m_tf.defaultTextFormat = format;
				m_tf.type = flash.text.TextFieldType.INPUT; // vs. DYNAMIC
				m_tf.text = "PlayMP3Clip.swf started\n";
				m_tf.x = 0;
				m_tf.y = 0;
				m_tf.width = stage.stageWidth;
				m_tf.height = stage.stageHeight;
				m_tf.border = false;
				m_tf.mouseWheelEnabled = true;
				m_tf.multiline = true;
				m_tf.wordWrap = false;
				m_tf.autoSize = flash.text.TextFieldAutoSize.LEFT;
				addChild(m_tf);

				this.scaleX = 3.0;
				this.scaleY = 3.0;
				this.graphics.beginFill(0xD0D0D0);
				this.graphics.drawRect(0, 0, stage.stageWidth, stage.stageHeight);
				this.graphics.endFill();

				// Setup the timer which will watch to see when the end point is hit
				// The timer only runs while the sound is playing
				m_oPlayPositionTimer = new Timer(m_iTimerInterval);
				m_oPlayPositionTimer.addEventListener(TimerEvent.TIMER, OnPlayPositionTimer);

				// Create the sound object and add event handlers
				m_oSound = new Sound();
				m_oSound.addEventListener(Event.ID3, OnID3);
				m_oSound.addEventListener(ProgressEvent.PROGRESS, OnLoadProgress);
				m_oSound.addEventListener(Event.COMPLETE, OnLoadComplete);
				m_oSound.addEventListener(IOErrorEvent.IO_ERROR, OnIOError);

				// Create the sound transform object to control volume
				m_oSoundTransform = new SoundTransform();

				// Set the security so that we will have access to the sound object
				SetSecurity();

				// Allow Javascript functions to call into this class
				// Remember: Javascript errors will occur if the Flash <object> is inside a <form>
				SetJavascriptEntryPoints();

				// Extract the Flashvars
				var FlashVarArray:Object = LoaderInfo(this.root.loaderInfo).parameters;

				// Examine all of the Flashvars
				var sKey:String;
				for (sKey in FlashVarArray)
					Trace("FlashVars: " + sKey + " = " + String(FlashVarArray[sKey]) + "\n");

				// Set the defaults;  do SetUrl loading and SetAutoplay playing last
				SetStart(		FlashVarArray["Start"]		?	int(FlashVarArray["Start"])			:	m_iStart);
				SetEnd(			FlashVarArray["End"]		?	int(FlashVarArray["End"])			:	m_iEnd);
				SetLoop(		FlashVarArray["Loop"]		?	int(FlashVarArray["Loop"])			:	m_bLoop);
				SetLoopMax(		FlashVarArray["LoopMax"]	?	int(FlashVarArray["LoopMax"])		:	m_iLoopMax);
				SetVolume(		FlashVarArray["Volume"]		?	int(FlashVarArray["Volume"])		:	m_iVolume);
				SetPolicyUrl(	FlashVarArray["PolicyUrl"]	?	String(FlashVarArray["PolicyUrl"])	:	m_sPolicyUrl);
				SetUrl(			FlashVarArray["Url"]		?	String(FlashVarArray["Url"])		:	m_sUrl);
				SetAutoPlay(	FlashVarArray["AutoPlay"]	?	int(FlashVarArray["AutoPlay"])		:	m_bAutoPlay);
			}
			catch (error:Error)
			{
				OnError("PlayMP3Clip: " + String(error) + "\n");
			}
		}

		private function OnMenuSelect(event:ContextMenuEvent):void
		{
			try
			{
				Trace("OnMenuSelect:\n");
			}
			catch (error:Error)
			{
				OnError("OnMenuSelect: " + String(error) + "\n");
			}
		}
		
		private function SetSecurity():void
		{
			try
			{
				if (m_sPolicyUrl != "")
					Security.loadPolicyFile(m_sPolicyUrl);
				Security.allowInsecureDomain("*");
				Security.allowDomain("*"); // ((this.root.loaderInfo).loaderURL);
			}
			catch (error:Error)
			{
				OnError("SetSecurity: " + String(error) + "\n");
			}
		}
		
		private function SetJavascriptEntryPoints():void
		{
			try
			{
				var sObjectID:String = ExternalInterface.objectID;
				var bSuccess:Boolean = ExternalInterface.available;
				if (!bSuccess)
				{
					OnError("SetJavascriptEntryPoints: ExternalInterface is not available for Flash objectID " + sObjectID + "\n");
					return;
				}

				// FYI, "Play" and "Stop" cannot be used as Javascript function names (must be reserved words)
				ExternalInterface.addCallback("DoPlay", DoPlay);
				ExternalInterface.addCallback("DoStop", DoStop);
				ExternalInterface.addCallback("SetUrl", SetUrl);
				ExternalInterface.addCallback("SetPolicyUrl", SetPolicyUrl);
				ExternalInterface.addCallback("SetStart", SetStart);
				ExternalInterface.addCallback("SetEnd", SetEnd);
				ExternalInterface.addCallback("SetLoop", SetLoop);
				ExternalInterface.addCallback("SetLoopMax", SetLoopMax);
				ExternalInterface.addCallback("SetAutoPlay", SetAutoPlay);
				ExternalInterface.addCallback("SetVolume", SetVolume);
				ExternalInterface.addCallback("GetLength", GetLength);
				ExternalInterface.addCallback("GetMetaData", GetMetaData);

				Trace("SetJavascriptEntryPoints: Flash objectID " + sObjectID + "\n");
			}
			catch (error:Error)
			{
				OnError("SetJavascriptEntryPoints: " + String(error) + "\n");

//j				// This Alert causes an exception for some reason
//j				// Since the OnError callbacks won't work if there is a security error, throw up an alert
//j				Alert.show("Flash security error", "Flash Error", Alert.OK, this);
			}
		}
		
		public function SetUrl(sUrl:String):void
		{
			try
			{
				m_sUrl = sUrl;
				Trace("SetUrl: " + m_sUrl + "\n");

				// Load the sound
				if (m_sUrl != "") // m_oSound.length will not be accurate until the file is fully loaded
					m_oSound.load(new URLRequest(m_sUrl), new SoundLoaderContext(1000/*bufferSize*/, (m_sPolicyUrl != "")/*checkPolicyFile*/));
			}
			catch (error:Error)
			{
				OnError("SetUrl: " + String(error) + "\n");
			}
		}

		public function SetPolicyUrl(sPolicyUrl:String):void
		{
			m_sPolicyUrl = sPolicyUrl;
			Trace("SetPolicyUrl: " + m_sPolicyUrl + "\n");
		}

		public function SetStart(iStart:int):void
		{
			m_iStart = iStart;
			Trace("SetStart: " + String(m_iStart) + "\n");
		}

		public function SetEnd(iEnd:int):void
		{
			m_iEnd = iEnd;
			Trace("SetEnd: " + String(m_iEnd) + "\n");
		}

		public function SetLoop(bLoop:Boolean):void
		{
			m_bLoop = bLoop;
			Trace("SetLoop: " + String(m_bLoop) + "\n");
		}

		public function SetLoopMax(iLoopMax:int):void
		{
			m_iLoopMax = iLoopMax;
			Trace("SetLoopMax: " + String(m_iLoopMax) + "\n");
		}

		public function SetAutoPlay(bAutoPlay:Boolean):void
		{
			try
			{
				m_bAutoPlay = bAutoPlay;
				Trace("SetAutoPlay: " + String(m_bAutoPlay) + "\n");

				// If autoplay was indicated, play the sound now
				if (m_bAutoPlay)
					DoPlay(true/*bFromStart*/);
			}
			catch (error:Error)
			{
				OnError("SetAutoPlay: " + String(error) + "\n");
			}
		}

		public function SetVolume(iVolume:int):void
		{
			try
			{
				m_iVolume = iVolume;
				Trace("SetVolume: " + String(m_iVolume) + "\n");

				var fVolume:Number = Number(m_iVolume) / 100.0;
				m_oSoundTransform.volume = fVolume;
				if (m_oSoundChannel is SoundChannel)
					m_oSoundChannel.soundTransform.volume = fVolume;
			}
			catch (error:Error)
			{
				OnError("SetVolume: " + String(error) + "\n");
			}
		}

		public function GetLength():int
		{
			var iLength:int = 0;
			try
			{
				iLength = (m_oSound.isBuffering ? -m_oSound.length : m_oSound.length);
				Trace("GetLength: " + String(iLength) + "\n");
			}
			catch (error:Error)
			{
				OnError("GetLength: " + String(error) + "\n");
			}

			return iLength;
		}

		public function GetMetaData(sField:String):String
		{
			var sData:String = "";
			try
			{
				// sField must be one of the following:
				// "artist", "track", "comment", "songName", "albumv, "genre", "year"
				var oSongInfo:ID3Info = ID3Info(m_oSound.id3);
				if (sField == "artist")		sData = oSongInfo.artist;
				if (sField == "track")		sData = oSongInfo.track;
				if (sField == "comment")	sData = oSongInfo.comment;
				if (sField == "songName")	sData = oSongInfo.songName;
				if (sField == "album")		sData = oSongInfo.album;	
				if (sField == "genre")		sData = oSongInfo.genre;
				if (sField == "year")		sData = oSongInfo.year;
				Trace("GetMetaData: " + sField + ": " + sData + "\n");
			}
			catch (error:Error)
			{
				OnError("GetMetaData: " + String(error) + "\n");
			}

			return (sData != "undefined" ? sData : "");
		}

		public function DoPlay(bFromStart:Boolean):void
		{
			try
			{
				if (m_oSound.length <= 0)
				{
					Trace("DoPlay: Nothing to play\n");
					return;
				}

				DoStop(false/*bResetLoopCount*/);

				if (m_iLastPlayPosition < m_iStart || m_iLastPlayPosition >= m_iEnd)
					bFromStart = true;
				if (bFromStart)
					m_iLastPlayPosition = m_iStart;
				if (m_iLastPlayPosition > m_oSound.length)
				{
					Trace("DoPlay: Position Error: " + String(m_iLastPlayPosition) + " > " + String(m_oSound.length) + "\n");
					m_iLastPlayPosition = 0;
				}

				Trace("DoPlay: " + String(m_iLastPlayPosition) + "\n");
				m_oSoundChannel = m_oSound.play(m_iLastPlayPosition, 0/*loops*/, m_oSoundTransform);
				if (m_oSoundChannel is SoundChannel)
					m_oSoundChannel.addEventListener(Event.SOUND_COMPLETE, OnSoundComplete);

				// Start the timer to track play position and poll for the end point
				m_oPlayPositionTimer.start();
			}
			catch (error:Error)
			{
				OnError("DoPlay: " + String(error) + "\n");
			}
		}

		public function DoStop(bResetLoopCount:Boolean = true):void
		{
			try
			{
				m_oPlayPositionTimer.stop();

				if (bResetLoopCount)
					m_iLoopCount = 0;

				if (m_oSoundChannel is SoundChannel)
				{
					m_iLastPlayPosition = m_oSoundChannel.position;
					m_oSoundChannel.stop();
					m_oSoundChannel = null;
					Trace("DoStop: " + String(m_iLastPlayPosition) + "\n");
				}
			}
			catch (error:Error)
			{
				OnError("DoStop: " + String(error) + "\n");
			}
		}

		private function OnSoundComplete(event:Event):void
		{
			try
			{
				Trace("OnSoundComplete:\n");

				DoStop();

				// Call JavaScript function; let the caller know what the play position is
				ExternalInterface.call("OnPlayProgress", m_iLastPlayPosition, true/*bEnd*/);

				if (m_bLoop && (!m_iLoopMax || ++m_iLoopCount < m_iLoopMax))
					DoPlay(true/*bFromStart*/);
			}
			catch (error:Error)
			{
				OnError("OnSoundComplete: " + String(error) + "\n");
			}
		}

		private function OnPlayPositionTimer(event:TimerEvent):void
		{
			try
			{
				if (!(m_oSoundChannel is SoundChannel))
					return;

				var iPosition:int = m_oSoundChannel.position;
				var bPositionChanged:Boolean = (m_iLastPlayPosition != iPosition);
				m_iLastPlayPosition = iPosition;
				if (bPositionChanged)
				{
					//Trace("OnPlayPositionTimer: " + String(m_iLastPlayPosition) + "\n");

					// Call JavaScript function; let the caller know what the play position is
					ExternalInterface.call("OnPlayProgress", m_iLastPlayPosition, false/*bEnd*/);
				}

				// If there is an end point setup, check to see if we have exceeded it
				// Allow for some slop when comparing against the m_iStart due to slight timing inaccuracies
				var bEnd:Boolean = ((m_iEnd && m_iLastPlayPosition >= m_iEnd) || m_iLastPlayPosition < m_iStart-m_iTimerInterval);
				if (bEnd)
					OnSoundComplete(new Event(Event.SOUND_COMPLETE));
			}
			catch (error:Error)
			{
				OnError("OnPlayPositionTimer: " + String(error) + "\n");
			}
		}

		private function OnID3(event:Event):void
		{
			try
			{
				Trace("OnID3: MetaDataAvailable\n");
			
				//var song:Sound = Sound(event.target);
				//var oSongInfo:ID3Info = ID3Info(song.id3);

				// Call JavaScript function; let the caller know that the MetaData is available now
				ExternalInterface.call("OnMetaDataAvailable");
			}
			catch (error:Error)
			{
				OnError("OnID3: " + String(error) + "\n");
			}
		}

		private function OnLoadProgress(event:ProgressEvent):void
		{
			try
			{
				var iPercentLoaded:int = (100 * event.bytesLoaded) / event.bytesTotal;
				var bPercentLoadedChanged:Boolean = (m_iPercentLoaded != iPercentLoaded);
				m_iPercentLoaded = iPercentLoaded;
				if (bPercentLoadedChanged)
				{
					if (!(m_iPercentLoaded % 10))
						Trace("OnLoadProgress: " + String(m_iPercentLoaded) + "%\n");

					// Call JavaScript function
					ExternalInterface.call("OnLoadProgress", m_iPercentLoaded, m_oSound.length);
				}
			}
			catch (error:Error)
			{
				OnError("OnLoadProgress: " + String(error) + "\n");
			}
		}

		private function OnLoadComplete(event:Event):void
		{
			try
			{
				m_iPercentLoaded = 100;
				Trace("OnLoadComplete: 100%\n");

				// Call JavaScript function
				ExternalInterface.call("OnLoadProgress", m_iPercentLoaded, m_oSound.length);
			}
			catch (error:Error)
			{
				OnError("OnLoadComplete: " + String(error) + "\n");
			}
		}

		private function OnIOError(event:IOErrorEvent):void
		{
			try
			{
				if (event.target is Sound && String(event.text) == "Error #2032")
				{
					var sUrl:String = m_sUrl;
					var index:int = sUrl.lastIndexOf("/");
					if (index < 0) index = sUrl.lastIndexOf("\\");
					var sFileName:String = sUrl.substring(index+1);
					OnError("File '" + sFileName + "' could not be read\n");
				}
				else
					OnError("OnIOError: " + String(event.text) + "\n");
			}
			catch (error:Error)
			{
				OnError("OnIOError: " + String(error) + "\n");
			}
		}

		private function Trace(sMsg:String):void
		{
			try
			{
				if (m_tf is TextField)
					m_tf.appendText(sMsg);
				trace(sMsg);
			}
			catch (error:Error)
			{
			}
		}

		private function OnError(sMsg:String):void
		{
			try
			{
				Trace(sMsg);

				// Call JavaScript function; let the caller know what the error is
				ExternalInterface.call("OnError", sMsg);
			}
			catch (error:Error)
			{
			}
		}
	}
}
