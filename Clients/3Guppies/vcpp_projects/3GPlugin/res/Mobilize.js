	var oWindow = (external.menuArguments ? external.menuArguments : window);
	var oEvent = oWindow.event;
	var oContent = (oEvent ? oEvent.srcElement : null);

	var sBaseUrl = 'http://www.3guppies.com/';
	var sMobilizeUrl = sBaseUrl + 'ringtones/module/OpenLocker/action/Mobilize?extcmp=IEXP01000&src=ieplugin&Url=';
	var sMobilizeTextUrl = sBaseUrl + 'widget/textme/main?extcmp=IEXP01000&src=ieplugin&Postfix=';
	var sMobilizeAll = sBaseUrl + 'ringtones/module/OpenLocker/action/ExternalMobilize?extcmp=IEXP01000&src=ieplugin&sUrl=';
	var aSupportedFileTypes = new Array(
		/*Audio files*/	'mp3','wma','aac','wav','smaf','mel','qcp','awb','m4a','m4p','mid','ogg','rm',
		/*Video files*/	'flv','asf','wmv','avi','mov','mp4','mpg','mpeg','qt','3gp','ogm',
		/*Image files*/	'gif','jpg','jpeg','png','dib','bmp','qtif');
	var bSilent = true;

/*	var sMobilizer1 = '\
	<mobilizer track="3GUC10140"> \
		<style type="text/css"> \
			.mobilizer4 {width: 295px; height: 66px} .mobilizer4 .b1 a {width: 169px;} \
			.mobilizer4 .b2 a {width: 61px;} \
			.mobilizer4 .b3 a {width: 65px;} \
			.mobilizer4 div, .mobilizer4 a {height: 51px; background-repeat: no-repeat;} \
			.mobilizer4 a.top {height: 15px;} \
			.mobilizer4 div {float: left; background-position: 0 -51px;} \
			.mobilizer4 .b1 {background-image: url(http://svvscdn.3gupload.com/images/widget/alt_fire_b1.gif);} \
			.mobilizer4 .b2 {background-image: url(http://svvscdn.3gupload.com/images/widget/alt_fire_b2.gif);} \
			.mobilizer4 .b3 {background-image: url(http://svvscdn.3gupload.com/images/widget/alt_fire_b3.gif);} \
			.mobilizer4 a {display: block; width: 100%; height: 100%; overflow: hidden; background-position: top left;} \
			.mobilizer4 .b1 a {background-image: url(http://svvscdn.3gupload.com/images/widget/alt_fire_b1.gif);} \
			.mobilizer4 .b2 a {background-image: url(http://svvscdn.3gupload.com/images/widget/alt_fire_b2.gif);} \
			.mobilizer4 .b3 a {background-image: url(http://svvscdn.3gupload.com/images/widget/alt_fire_b3.gif);} \
			.mobilizer4 a:hover {background-image: none;} \
		</style> \
		<div class="mobilizer4"> \
			<a class="top" target="_blank" href="http://mobilize.3gupload.com/?src=mobiwidg&extcmp=3GUC10140">\
				<img src="http://svvscdn.3gupload.com/images/widget/alt_fire_top.gif" border="0" />\
			</a> \
			<div class="b1">\
				<a href="http://mobilize.3gupload.com/?src=mobiwidg&extcmp=3GUC10140" target="_blank">\
				</a>\
			</div> \
			<div class="b2">\
				<a href="http://www.3gupload.com/MySpace?src=mobiwidg&extcmp=3GUC10140" target="_blank">\
				</a>\
			</div> \
			<div class="b3">\
				<a href="http://www.3gupload.com/widget/tweakit?src=mobiwidg&extcmp=3GUC10140" target="_blank">\
				</a>\
			</div> \
		</div> \
	</mobilizer>';

	var sMobilizer2 = '\
	<mobilizer track="3GUA10110">\
		<style type="text/css"> \
			.mobilizer1 {width: 295px; height: 220px} \
			.mobilizer1 .b1 a {width: 169px;} \
			.mobilizer1 .b2 a {width: 61px;} \
			.mobilizer1 .b3 a {width: 65px;} \
			.mobilizer1 div, .mobilizer1 a {height: 51px; background-repeat: no-repeat;} \
			.mobilizer1 a.top {height: 149px;} \
			.mobilizer1 div {float: left; background-position: 0 -51px;} \
			.mobilizer1 .b1 {background-image: url(http://svvscdn.3gupload.com/images/widget/b1.gif);} \
			.mobilizer1 .b2 {background-image: url(http://svvscdn.3gupload.com/images/widget/b2.gif);} \
			.mobilizer1 .b3 {background-image: url(http://svvscdn.3gupload.com/images/widget/b3.gif);} \
			.mobilizer1 a {display: block; width: 100%; height: 100%; overflow: hidden; background-position: top left;} \
			.mobilizer1 .b1 a {background-image: url(http://svvscdn.3gupload.com/images/widget/b1.gif);} \
			.mobilizer1 .b2 a {background-image: url(http://svvscdn.3gupload.com/images/widget/b2.gif);} \
			.mobilizer1 .b3 a {background-image: url(http://svvscdn.3gupload.com/images/widget/b3.gif);} \
			.mobilizer1 a:hover {background-image: none; color: FF0000;} \
			.mobilizer1 a.foot {clear: both; width: 294px; height: 15px; font-size: 10px; text-align: right; font-weight: normal;} \
		</style> \
		<div class="mobilizer1"> \
			<a class="top" target="_blank" href="http://mobilize.3gupload.com/?src=mobiwidg&extcmp=3GUA10110">\
			<img src="http://svvscdn.3gupload.com/images/widget/top.gif" border="0" />\
			</a> \
			<div class="b1">\
				<a target="_blank" href="http://mobilize.3gupload.com/?src=mobiwidg&extcmp=3GUA10110">\
				</a>\
			</div> \
			<div class="b2">\
				<a target="_blank" href="http://www.3gupload.com/MySpace?src=mobiwidg&extcmp=3GUA10110">\
				</a>\
			</div> \
			<div class="b3">\
				<a target="_blank" href="http://www.3gupload.com/widget/tweakit?src=mobiwidg&extcmp=3GUA10110">\
				</a>\
			</div> \
			<a target="_blank" href="http://www.3gupload.com/MySpace?src=mobiwidg&extcmp=3GUA10110" class="foot">3Guppies\'s Widget for MySpace \
			</a> \
		</div>\
	</mobilizer>';

	var sMobilizer3 = '\
	<mobilizer track="3GUC10160"> \
		<style type="text/css"> \
			.mobilizer6 {width: 297px; height: 139px} \
			.mobilizer6 a {display: block; background-position: top center;} \
			.mobilizer6 a:hover {background-position: bottom center;} \
			.mobilizer6 .col1 {float: left; padding-top: 11px;} \
			.mobilizer6 .col2 {float: left;} \
			.mobilizer6 .col3 {float: left; width: 53px; height: 139px;} \
			.mobilizer6 .col4 {float: left;} \
			.mobilizer6 .b1, .mobilizer6 .b1 a {width: 53px; height: 47px; background-image: url(http://svvscdn.3gupload.com/images/widget/SKZ_butt1.gif);} \
			.mobilizer6 .b2, .mobilizer6 .b2 a {width: 53px; height: 46px; background-image: url(http://svvscdn.3gupload.com/images/widget/SKZ_butt2.gif);} \
			.mobilizer6 .b3, .mobilizer6 .b3 a {width: 53px; height: 46px; background-image: url(http://svvscdn.3gupload.com/images/widget/SKZ_butt3.gif);} \
		</style> \
		<div class="mobilizer6"> \
			<a class="col1" target="_blank" href="http://mobilize.3gupload.com/?src=mobiwidg&extcmp=3GUC10160">\
			<img src="http://svvscdn.3gupload.com/images/widget/SKZ_ban1B.gif" border="0" />\
			</a> \
				<a class="col2" target="_blank" href="http://mobilize.3gupload.com/?src=mobiwidg&extcmp=3GUC10160">\
					<img src="http://svvscdn.3gupload.com/images/widget/SKZ_ban1A.gif" border="0" />\
				</a> \
			<div class="col3"> \
				<div class="b2">\
					<a target="_blank" href="http://www.3gupload.com/widget/tweakit?src=mobiwidg&extcmp=3GUC10160">\
					</a>\
				</div> \
				<div class="b1">\
					<a target="_blank" href="http://mobilize.3gupload.com/?src=mobiwidg&extcmp=3GUC10160">\
					</a>\
				</div> \
				<div class="b3">\
					<a target="_blank" href="http://www.3gupload.com/MySpace?src=mobiwidg&extcmp=3GUC10160">\
					</a>\
				</div> \
				</div> \
				<div class="col4">\
					<img src="http://svvscdn.3gupload.com/images/widget/SKZ_ban2.gif" border="0" />\
				</div> \
				<div style="clear: both;">\
			</div> \
		</div> \
	</mobilizer>';
*/
	var sMobilizer4 = '\
<a target="_blank" href="http://mobilize.3gupload.com/?src=mobiwidg&extcmp=3GUC10160">\
<img src="http://svvscdn.3Guppies.com/images/widget/top.gif" border="0" />\
</a>';

	function IsFileTypeSupported(sUrl, bSilent)
	{
		for (var i=0; i<aSupportedFileTypes.length; i++)
		{
			var re = new RegExp("^.*\\." + aSupportedFileTypes[i] + '$', 'i');
			var sFileTypeSupported = sUrl.match(re);
			if (sFileTypeSupported)
				return true;
		}

		if (!bSilent)
			return confirm('That content type is not supported:\n\n' + sUrl + '\n\nDo you want to try anyway?');
		return false;
	}

	function MobilizeThis()
	{
		if (!oContent) 
		{
			MobilizeAll();
			return;
		}

		// Mobilize This!
		switch (oContent.tagName)
		{
			case 'A':
			{
				MobilizeLink();
				break;
			}
			case 'IMG':
			{
				MobilizeImage();
				break;
			}
			case 'INPUT':
			{
				MobilizeInput();
				break;
			}
			default:
			{
				MobilizeText();
				break;
			}
		}
	}

	function MobilizeLink()
	{
		var sUrl = oContent.href;
		if (!IsFileTypeSupported(sUrl, true/*bSilent*/))
		{
			MobilizeAll(sUrl);
			return;
		}
		
		var sDescription = oContent.innerText;
		var sContent = encodeURIComponent(sUrl);
		if (bSilent || confirm('Are you sure you want to Mobilize the following link?\n\n' + sUrl + '\n\nDescription: ' + sDescription))
			LaunchMobilizeWindow(sMobilizeUrl + sContent);
	}
	
	function MobilizeImage()
	{
		var sUrl = oContent.src;
		var sDescription = oContent.alt;
		var sContent = encodeURIComponent(sUrl);
		if (bSilent || confirm('Are you sure you want to Mobilize the following image?\n\n' + sUrl + '\n\nDescription: ' + sDescription))
			LaunchMobilizeWindow(sMobilizeUrl + sContent);
	}
	
	function MobilizeInput()
	{
		var sText = oContent.value;
		var sDescription = oContent.title;
		var sContent = escape(sText);
		if (bSilent || confirm('Are you sure you want to Mobilize the following text?\n\n' + sText + '\n\nDescription: ' + sDescription))
			LaunchMobilizeWindow(sMobilizeTextUrl + sContent); //j LaunchMobilizePopup(sMobilizeTextUrl + sContent, '3Guppies_send2phone', 800, 600);
	}
	
	function MobilizeText()
	{
		var sText = GetSelectedText(oContent);
		if (sText == '')
			sText = GetObjectText(oContent);
		var sContent = escape(sText);
		if (bSilent || confirm('Are you sure you want to Mobilize the following text?\n\n' + sText))
			LaunchMobilizeWindow(sMobilizeTextUrl + sContent); //j LaunchMobilizePopup(sMobilizeTextUrl + sContent, '3Guppies_send2phone', 800, 600);
	}
	
	function MobilizeAll(sUrl)
	{
		if (!sUrl || typeof sUrl.type == 'undefined')
			sUrl = oWindow.location.href;
		var sContent = encodeURIComponent(sUrl);
		if (bSilent || confirm('Are you sure you want to Mobilize the entire page?\n\n' + sUrl))
			LaunchMobilizeWindow(sMobilizeAll + sContent);
	}
	
	function MobilizeVideo()
	{
		var sUrl = oWindow.location.href;
		var arrFiles = ExtractVideoUrls();
		if (!arrFiles || !arrFiles.length)
			return;

		sUrl = arrFiles[0];
		var sContent = encodeURIComponent(sUrl);
		if (bSilent || confirm('Are you sure you want to Mobilize the video on this page?\n\n' + sUrl))
			LaunchMobilizeWindow(sMobilizeUrl + sContent);

//j		// This will start a download
//j		oWindow.location.href = arrFiles[1];
	}
	
	function MobilizePaste()
	{
		if (!oContent)
			return false;
		if (typeof oContent.type == 'undefined')
			return false;
		if (typeof oContent.type.indexOf('text') >= 0)
			return false;
		// 'ctl00$Main$postComment$commentTextBox'
		oContent.value += sMobilizer4;
		return true;
	}
	
	function GetSelectedText(oContent)
	{
		if (oWindow.document.getSelection)
			return oWindow.document.getSelection();

		if (oWindow.document.selection && oWindow.document.selection.createRange)
		{
			var range = oWindow.document.selection.createRange();
			return range.text;
		}
	}

	function GetObjectText(oContent)
	{
		if (!oContent)
			return '';
		if (typeof oContent.textContent != 'undefined')
			return oContent.textContent;
		if (typeof oContent.innerText != 'undefined')
			return oContent.innerText;
		if (typeof oContent.text != 'undefined')
			return oContent.text;
		switch (oContent.nodeType)
		{
			case 3:
			case 4:
				return oContent.nodeValue;
				break;

			case 1:
			case 11:
				var innerText = '';
				for (var i=0; i<oContent.childNodes.length; i++)
					innerText += GetObjectText(oContent.childNodes[i]);
				return innerText;
				break;

			default:
				return '';
		}
	}

	function LaunchMobilizeWindow(sUrl)
	{
		try
		{
			var o3gPlugin = new ActiveXObject('3GPlugin.BrowserHelper');
			if (o3gPlugin)
			{
				var sReturn = o3gPlugin.LaunchMobilizeWindow(oWindow, sUrl);
				if (sReturn == 'OK')
					return true;
			}
		}
		catch (e)
		{
		}

		var wnd = oWindow.open(sUrl);
		if (wnd)
			wnd.focus();
		return true;
	}
		
	function LaunchMobilizePopup(sUrl, sTargetName, width, height)
	{
		var l = (screen.width - width) / 2;
		var t = (screen.height - height) / 2;
		if (l < 0) l = 0;
		if (t < 0) t = 0;
		var w = width; //j - 12; // IE6 adds 12 for the left and right non-client areas
		var h = height; //j - 61; // IE6 adds 61 for the top, caption, statusbar, and bottom non-client areas
		var sSize = 'width=' + w + ',height=' + h + ',left=' + l + ',top=' + t;
		var sFeatures = 'location=0,menubar=0,toolbar=0,status=0,scrollbars=1,resizable=1,' + sSize;

		try
		{
			var o3gPlugin = new ActiveXObject('3GPlugin.BrowserHelper');
			if (o3gPlugin)
			{
				var sReturn = o3gPlugin.LaunchMobilizePopup(oWindow, sUrl, sTargetName, sFeatures);
				if (sReturn == 'OK')
					return true;
			}
		}
		catch (e)
		{
		}

		var wnd = oWindow.open(sUrl, '3Guppies_send2phone', sFeatures);
		if (wnd)
			wnd.focus();
		return true;
	}

	function GetEmbedMovie(oFlash)
	{
		var sMovie = unescape(oFlash.getAttribute('src'));
		return sMovie;
	}

	function GetObjectMovie(oFlash)
	{
		var oNodeList = oFlash.childNodes;
		for (var i=0; i<oNodeList.length; i++)
		{
			var oItem = oNodeList[i];
			if (oItem.getAttribute('name') == 'movie')
			{
				var sMovie = unescape(oItem.getAttribute('value'));
				return sMovie;
			}
		}

		return null;
	}

	function GetEmbedFlashVars(oFlash)
	{
		var sFlashVars = unescape(oFlash.getAttribute('flashvars'));
		return sFlashVars;
	}

	function GetObjectFlashVars(oFlash)
	{
		var oNodeList = oFlash.childNodes;
		for (var i=0; i<oNodeList.length; i++)
		{
			var oItem = oNodeList[i];
			if (oItem.getAttribute('name') == 'flashvars')
			{
				var sFlashVars = unescape(oItem.getAttribute('value'));
				return sFlashVars;
			}
		}

		return null;
	}

	function ExtractVideoUrls()
	{
		var sHost = oWindow.location.hostname;
		var mediahost = null;
		if      (sHost.match(/youtube\./i))     mediahost = 'YouTube';		// confirmed
		else if (sHost.match(/myspace\./i))     mediahost = 'MySpace';		// confirmed
		else if (sHost.match(/google\./i))      mediahost = 'Google';		// confirmed
		else if (sHost.match(/break\./i))       mediahost = 'Break.com';	// confirmed
		else if (sHost.match(/dailymotion\./i)) mediahost = 'Dailymotion';	// confirmed
		else if (sHost.match(/putfile\./i))     mediahost = 'Putfile';		// confirmed
		else if (sHost.match(/sevenload\./i))   mediahost = 'Sevenload';	// does not work XMLHttpRequest().open access denied
		else if (sHost.match(/metacafe\./i))    mediahost = 'Metacafe';		// does not work XMLHttpRequest().open access denied
		else if (sHost.match(/clipfish\.de/i))  mediahost = 'Clipfish';		// does not work XMLHttpRequest().open access denied
		else if (sHost.match(/myvideo\.de/i))   mediahost = 'MyVideo';		// does not work
		// If mediahost is null, we will try all methods

		var arrFiles = new Array();
		var iCount = 0;

		// Check MySpace first; it is the simplest case
		if (!mediahost || mediahost == 'MySpace')
		{
			var sPageUrl = oWindow.location.href;
			if (sPageUrl.match(/videoID=/i))
			{
				var sVideoId = sPageUrl.match(/videoID=[0-9]+/i)[0];
				sVideoId = sVideoId.substr(8); // after 'videoid='
				var iLength = sVideoId.length;
				var sZeros = '000000000000';
				sZeros = sZeros.substr(iLength); // pad the sVideoId with zeros to make it 12 digits
				var sFront = sVideoId.substr(0, iLength-5); // lop off the last 5 digits and keep the rest
				var sLast4 = sVideoId.substr(iLength-4); // take the last 4 digits and reverse them
				var s1 = sLast4.substr(0, 1);
				var s2 = sLast4.substr(1, 1);
				var s3 = sLast4.substr(2, 1);
				var s4 = sLast4.substr(3, 1);
				var sUrl = 'http://content.movies.myspace.com/' + sZeros + sFront + '/' + s4 + s3 + '/' + s2 + s1 + '/' + sVideoId + '.flv';
				arrFiles[iCount++] = sUrl;
				arrFiles[iCount++] = 'flv';
				return arrFiles;
			}
		}

		// Check all the Flash embeds and objects on the page
		var oNodeListObjects = oWindow.document.getElementsByTagName('object');
		var oNodeListEmbeds = oWindow.document.getElementsByTagName('embed');
//j		alert('objects=' + oNodeListObjects.length + ', embeds=' + oNodeListEmbeds.length);
		var iFlashCount = oNodeListObjects.length + oNodeListEmbeds.length;
		for (var i=0; i<iFlashCount; i++)
		{
			var bObject = (i < oNodeListObjects.length);
			var index = i - (bObject ? 0 : oNodeListObjects.length);
			var oItem = (bObject ? oNodeListObjects[index] : oNodeListEmbeds[index]);
			var sMovie = (bObject ? GetObjectMovie(oItem) : GetEmbedMovie(oItem));
			if (!sMovie)
				continue;

			var sFlashVars = (bObject ? GetObjectFlashVars(oItem) : GetEmbedFlashVars(oItem));
//j			alert(i + '('+index+'):\n\nmovie=' + sMovie + '\n\nvars=' + sFlashVars);

			if (!mediahost || mediahost == 'Google')
			{
				if (sMovie && sMovie.indexOf('videoUrl=') >= 0)
				{
					var sUrl = sMovie.substr(sMovie.indexOf('videoUrl=')+9);
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'flv';
				}
/*				if (oWindow.document.getElementById('macdownloadlink'))
				{
					var sUrl = oWindow.document.getElementById('macdownloadlink').href;
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'avi';
				}
				if (oWindow.document.getElementById('ipoddownloadlink'))
				{
					var sUrl = oWindow.document.getElementById('ipoddownloadlink').href;
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'mp4,iPod';
				}
				if (oWindow.document.getElementById('pspdownloadlink'))
				{
					var sUrl = oWindow.document.getElementById('pspdownloadlink').href;
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'mp4,psp';
				}
*/
			}

			if (!mediahost || mediahost == 'YouTube')
			{
				if (sMovie && sMovie.indexOf('video_id=') >= 0)
				{
					var sUrl = sMovie.substr(sMovie.indexOf('video_id=')+9);
					sUrl = 'http://www.youtube.com/get_video?video_id=' + sUrl;
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'flv';
				}
			}

			if (!mediahost || mediahost == 'Break.com')
			{
				if (sFlashVars && sFlashVars.indexOf('sVidLoc=') >= 0)
				{ 
					var urlstart = sFlashVars.indexOf('sVidLoc=')+8;
					var urlend = sFlashVars.indexOf('&', urlstart);
					var sUrl = sFlashVars.substring(urlstart, urlend);
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'flv';
				}
			}

			if (!mediahost || mediahost == 'Dailymotion')
			{
				if (sFlashVars && sFlashVars.indexOf('url=') >= 0)
				{
					var urlstart = sFlashVars.indexOf('url=')+4;
					var urlend = sFlashVars.indexOf('&', urlstart);
					var sUrl = sFlashVars.substring(urlstart, urlend);
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'flv';
				}
			}

			if (!mediahost || mediahost == 'Putfile')
			{
				if (sFlashVars && sFlashVars.indexOf('flv=') >= 0)
				{ 
					var urlstart = sFlashVars.indexOf('flv=')+4;
					var urlend = sFlashVars.indexOf('&', urlstart);
					var sUrl = sFlashVars.substring(urlstart, urlend);
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'flv';
				}

				if (sMovie && sMovie.indexOf('.wmv') >= 0)
				{
					var sUrl = sMovie;
					var sExtension = sUrl.substring(sUrl.lastIndexOf('.') + 1);
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = sExtension;
				}
			}

/* //j
			if (!mediahost || mediahost == 'MyVideo')
			{
				if (sMovie.indexOf('.llnwd.net/') >= 0 && sMovie.indexOf('V=../movie') >= 0 && sMovie.indexOf('.flv') >= 0)
				{
					var sUrl = sMovie.substring(0,sMovie.indexOf('.llnwd.net/')+14)+sMovie.substring(sMovie.indexOf('V=../movie')+5,sMovie.indexOf('.flv')+4);
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'flv';
				}
			}

			if (!mediahost || mediahost == 'Sevenload')
			{
				if (sFlashVars && sFlashVars.indexOf('id=') >= 0)
				{
					var urlstart = sFlashVars.indexOf('id=')+3;
					var urlend = sFlashVars.indexOf('&', urlstart);
					var sUrl = sFlashVars.substring(urlstart, urlend);
					sUrl = 'http://sevenload.de/api/player/id/' + sUrl;

					try
					{
						var xmlhttp = new XMLHttpRequest();
						if (xmlhttp)
						{
							xmlhttp.open('GET', sUrl, false);
							xmlhttp.send(null);
							var xmlobject = xmlhttp.responseXML;
							sUrl = xmlobject.getElementsByTagName('source')[0].getAttribute('url');
							arrFiles[iCount++] = sUrl;
							arrFiles[iCount++] = 'flv';
						}
					}
					catch(e) {}
				}
			}
			
			if (!mediahost || mediahost == 'Clipfish')
			{
				if (sMovie.indexOf('videoid=') >= 0)
				{
					var urlstart = sMovie.indexOf('videoid=')+8;
					var urlend;
					if (sMovie.indexOf('&', urlstart) >= 0)
						urlend = sMovie.indexOf('&', urlstart)
					else
						urlend = sMovie.length

					var sUrl = 'http://www.clipfish.de/video_n.php?videoid='+unescape(sMovie.substring(urlstart, urlend));
					try
					{
						var xmlhttp = new XMLHttpRequest();
						if (xmlhttp)
						{
							xmlhttp.open('GET', sUrl, false);
							xmlhttp.send(null);
							var xmlobject = xmlhttp.responseText;
							sUrl = xmlobject.substring(xmlobject.indexOf('&url=')+5,xmlobject.indexOf('.flv')+4);
							arrFiles[iCount++] = sUrl;
							arrFiles[iCount++] = 'flv';
						}
					}
					catch(e) {}
				}
			}
*/ //j
		}
		
		if (iCount)
			return arrFiles;

/* //j
		// Check all the script objects on the page
		var oNodeListScripts = oWindow.document.getElementsByTagName('script');
		for (var i=0; i<oNodeListScripts.length; i++)
		{
 			if (!mediahost || mediahost == 'YouTube')
			{
				var sUrl = oNodeListScripts[i].text.match(/video_id=\S+&.+&t=.+&f/i);
				if (sUrl)
				{
					sUrl = String(sUrl).replace(/(video_id=\S+)&.+(&t=.+)&f/i,'http://www.youtube.com/get_video?$1$2');
					arrFiles[iCount++] = sUrl;
					arrFiles[iCount++] = 'flv';
				}
			}

			if (!mediahost || mediahost == 'Metacafe')
			{
				var sUrl = oNodeListScripts[i].text.match(/itemID=[0-9]+/i);
				if (sUrl)
				{
					sUrl = 'http://www.metacafe.com/fplayer.php?itemID=' + String(sUrl).match(/[0-9]+/i) + '&t=embedded';

					try
					{
						var xmlhttp = new XMLHttpRequest();
						if (xmlhttp)
						{
							xmlhttp.open('GET', sUrl, false);
							xmlhttp.send(null);
							var xmlobject = (new DOMParser()).parseFromString(xmlhttp.responseText,'application/xml');
							sUrl = xmlobject.getElementsByTagName('playlist')[0].childNodes[1].getAttribute('url');
							arrFiles[iCount++] = sUrl;
							arrFiles[iCount++] = 'flv';
						}
					}
					catch(e) {}
				}
			}
		}

		if (iCount)
			return arrFiles;
*/ //j
		return null;
	}
