// ==UserScript==
// @name          Download Video
// @namespace     http://1024k.de/bookmarklets/video-bookmarklets.html
// @description	  Save video clips from YouTube, Google Video, Myspace, Metacafe, Break.com, Putfile, Dailymotion and Sevenload.
// @include       http://*youtube.com/*
// @include       http://*video.google.*/*
// @include       http://*myspace.com/*
// @include       http://*metacafe.com/*
// @include       http://*break.com/*
// @include       http://*putfile.com/*
// @include       http://*dailymotion.com/*
// @include       http://*sevenload.de/*
// @include       http://*myvideo.de/*
// @include       http://*clipfish.de/*
// ==/UserScript==

var loc = window.location.href; //j'http://video.google.com/videoplay?docid=3030582436688674946';
var host = window.location.hostname; //j'video.google.com';
//loc = 'http://vids.myspace.com/index.cfm?fuseaction=vids.individual&videoid=1314498994';
//host = 'vids.myspace.com';
//loc = 'http://video.google.com/videoplay?docid=3030582436688674946';
//host = 'video.google.com';
//loc = 'http://www.youtube.com/watch?v=kKUxynUtwBc';
//host = 'www.youtube.com';

function in2html(txt)
{
	if (!txt)
		return '';
	var txthtml = txt.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
//alert('in2html=' + txthtml);
	return txthtml;
}

function rev(str)
{
//alert('rev='+str);
	if (!str)
		return '';
	var revstr = '';
	for (i=str.length-1;i>=0;i--)
		revstr += str.charAt(i)

	return revstr;
}

function extracturls()
{
	var found = false;
	var mediahost = false;
	var embmedia = new Array();
	var source;
	var filetype;
	var fileURL = new Array();
	var urlstart;
	var urlend;
	var vid;
	var scriptmedia = new Array();

	if      (host.match(/google\./i)      !=null) {mediahost='Google'}
	else if (host.match(/break\./i)       !=null) {mediahost='Break.com'}
	else if (host.match(/dailymotion\./i) !=null) {mediahost='Dailymotion'}
	else if (host.match(/myvideo\.de/i)   !=null) {mediahost='MyVideo'}
	else if (host.match(/sevenload\./i)   !=null) {mediahost='Sevenload'}
	else if (host.match(/clipfish\.de/i)  !=null) {mediahost='Clipfish'}
	else if (host.match(/putfile\./i)     !=null) {mediahost='Putfile'}

	if (mediahost!=false)
	{
		embmedia=document.getElementsByTagName('embed');
		for(i=0;i<embmedia.length;++i)
		{
			source=unescape(embmedia[i].getAttribute('src'));
			switch (mediahost)
			{
			case 'Google':source=in2html(source.substr(source.indexOf('videoUrl=')+9));
					filetype='(.flv)';
					break;
			case 'Break.com': if(source.match(/FlashPlayer/i)!=null)
							  {
								  urlstart=embmedia[i].getAttribute('flashvars').indexOf('sVidLoc=')+8;
								  urlend=embmedia[i].getAttribute('flashvars').indexOf('&',urlstart);
								  source=unescape(embmedia[i].getAttribute('flashvars').substring(urlstart,urlend));
								  fileURL[fileURL.length]=source+mediahost+filetype;
								  source=in2html(source.replace(/\.flv/,'.wmv'));
								  filetype='(.wmv)'
							  }
							  else
							  {
								  filetype='(.wmv)';
								  fileURL[fileURL.length]=source+mediahost+filetype;
								  source=in2html(source.replace(/\.wmv/,'.flv'));
								  filetype='(.flv)'
							  }
							  break;
			case 'Dailymotion': if (embmedia[i].getAttribute('flashvars')!=null)
								{
									source=unescape(embmedia[i].getAttribute('flashvars'));
									filetype='(.flv)';
									urlstart=source.indexOf('url=')+4;
									urlend=source.indexOf('&',urlstart);
									source=in2html(source.substring(urlstart,urlend));
								}
								else
								{
									source=null;
								}
								break;
			case 'MyVideo':     if(embmedia[i].parentNode.id=='FLV')
								{
									source=in2html(source.substring(0,source.indexOf('.llnwd.net/')+14)+source.substring(source.indexOf('V=../movie')+5,source.indexOf('.flv')+4));
									filetype='(.flv)';
								}
								else
								{
									source=null;
								}
								break;
			case 'Sevenload':   if(embmedia[i].getAttribute('flashvars').indexOf('id=')!=-1)
								{
									urlstart=embmedia[i].getAttribute('flashvars').indexOf('id=')+3;
									urlend=embmedia[i].getAttribute('flashvars').indexOf('&',urlstart);
									source='http://sevenload.de/api/player/id/'+unescape(embmedia[i].getAttribute('flashvars').substring(urlstart,urlend));

									var xmlhttp=false;

									if(window.XMLHttpRequest)
									{
										try {xmlhttp=new XMLHttpRequest();}
										catch(e) {xmlhttp=false;}
									}

									if(xmlhttp!=false)
									{
										xmlhttp.open("GET",source,false);
										xmlhttp.send(null);
										var xmlobject=xmlhttp.responseXML;
										source=in2html(xmlobject.getElementsByTagName('source')[0].getAttribute('url'));
										filetype='(.flv)';
									}
									else
									{
										source=null
									}
								}
								else
								{
									source=null
								}
								break;
			case 'Clipfish':    if(source.indexOf('videoid=')!=-1)
								{
									urlstart=source.indexOf('videoid=')+8;
									if (source.indexOf('&',urlstart)!=-1)
									{
										urlend=source.indexOf('&',urlstart)
									}
									else
									{
										urlend=source.length
									}
									source='http://www.clipfish.de/video_n.php?videoid='+unescape(source.substring(urlstart,urlend));

									var xmlhttp=false;

									if(window.XMLHttpRequest)
									{
										try {xmlhttp=new XMLHttpRequest();}
										catch(e) {xmlhttp=false;}
									}

									if(xmlhttp!=false)
									{
										xmlhttp.open("GET",source,false);
										xmlhttp.send(null);
										var xmlobject=xmlhttp.responseText;
										source=in2html(xmlobject.substring(xmlobject.indexOf('&url=')+5,xmlobject.indexOf('.flv')+4));
										filetype='(.flv)';
									}
									else
									{
										source=null
									}
								}
								else
								{
									source=null
								}
								break;
			default:break;
			}
			if(source!=null)
			{
				fileURL[fileURL.length]=source+mediahost+filetype;
				break;
			}
		}
	}

	if (host.match(/google\./i)!=null)
	{
		if(document.getElementById('macdownloadlink')!=null)
		{
			source=in2html(document.getElementById('macdownloadlink').href);
			filetype='(.avi)';
			fileURL[fileURL.length]=source+mediahost+filetype;
		}
		if(document.getElementById('ipoddownloadlink')!=null)
		{
			source=in2html(document.getElementById('ipoddownloadlink').href);
			filetype='(.mp4) (iPod)';
			fileURL[fileURL.length]=source+mediahost+filetype;
		}
		if(document.getElementById('pspdownloadlink')!=null)
		{
			source=in2html(document.getElementById('pspdownloadlink').href);
			filetype='(.mp4) (PSP)';
			fileURL[fileURL.length]=source+mediahost+filetype;
		}
	}

	if      (host.match(/youtube\./i)  !=null) {mediahost='YouTube'}
	else if (host.match(/metacafe\./i) !=null) {mediahost='Metacafe'}

	if (mediahost!=false)
	{
		scriptmedia=document.getElementsByTagName('script');
		for(i=0;i<scriptmedia.length;++i)
		{
			switch (mediahost)
			{
			case 'YouTube': source=scriptmedia[i].text.match(/video_id=\S+&.+&t=.+&f/i);
				if (source!=null)
				{
					source=in2html(String(source).replace(/(video_id=\S+)&.+(&t=.+)&f/i,'http:\/\/www.youtube.com\/get_video?$1$2'));
					filetype='(.flv)';
					fileURL[fileURL.length]=source+mediahost+filetype;
				}
				break;
			case 'Metacafe': source=scriptmedia[i].text.match(/itemID=[0-9]+/i);
				if (source!=null)
				{
					source='http://www.metacafe.com/fplayer.php?itemID=' + String(source).match(/[0-9]+/i) + '&t=embedded';
					filetype='(.flv)';
					var xmlhttp=false;

					if(window.XMLHttpRequest)
					{
						try {xmlhttp=new XMLHttpRequest();}
						catch(e) {xmlhttp=false;}
					}

					if(xmlhttp!=false)
					{
						xmlhttp.open("GET",source,false);
						xmlhttp.send(null);
						var xmlobject=(new DOMParser()).parseFromString(xmlhttp.responseText,"application/xml");
						source=in2html(xmlobject.getElementsByTagName('playlist')[0].childNodes[1].getAttribute('url'));
						filetype='(.flv)';
						fileURL[fileURL.length]=source+mediahost+filetype;
						found=true;
					}
					break;
				}
			default:break;
			}
		}

		if (mediahost=='Metacafe' && found!=true)
		{
			embmedia=document.getElementsByTagName('embed');
			for(i=0;i<embmedia.length;++i)
			{
				source=unescape(embmedia[i].getAttribute('src'));
				source=in2html(source.substring(73,source.indexOf('&')));
				fileURL[fileURL.length]=source+mediahost+filetype;
			}
		}

	}

	if (host.match(/myspace\./i)!=null)
	{
		mediahost='MySpace';
		if (loc.match(/videoID=/i)!=null)
		{
			vid=loc.match(/videoID=[0-9]+/i)[0].substr(8);
			source=in2html('http:\/\/content.movies.myspace.com\/'+Math.pow(10,7-(vid.length-5)).toString().substr(1)+vid.substr(0,vid.length-5)+'\/'+rev(vid.substring(vid.length-2))+'\/'+rev(vid.substring(vid.length-4,vid.length-2))+'\/'+vid+'.flv');
			filetype='(.flv)';
			fileURL[fileURL.length]=source+mediahost+filetype;
		}
	}

	alert('source='+source);
	alert('mediahost='+mediahost);
	alert('filetype='+filetype);
	for (i=0; i<fileURL.length; i++)
		alert('fileURL['+i+']='+fileURL[i]);
}

//if ((host.match(/google\./i) !=null) 
//|| (host.match(/dailymotion\./i)!=null)) { window.addEventListener('load',extracturls,false) }
//else { extracturls() }
extracturls();
