// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer, Inc.					       |
// +---------------------------------------------------------------------------+

/**
 * The Mixxer Mobilizes takes the content you want, and makes it mobile. This
 * requires a free mixxer account, and that you be logged in.
 *
 * I have tried to document this as best I can, to serve as a reference for
 * other implementations.
 *
 * @author Blake Matheny
 */

/**
 * @brief Constructor for Mixxer_Mobilizer
 *
 * This is the constructor for the mixxer mobilizer. It initializes an
 * extensions array which tells the application which extensions are supported
 * for mobilization. This may not be entirely acurate, but it's a start.
 */
function Mixxer_Mobilizer()
{
	/* The file extensions that are recognized */
	this.extensions = new Array(
			'mid','midi','mp3','qcp','wav',
			'jpg','gif','png','bmp','pmd',
			'3gp','flv','rm','qt','ogm','mpg','mpeg','mp4','mov','avi','wmv','asf');
	/* Base URL to send to. Modify for development */
	this.base_url = 'http://www.mixxer.com/ringtones/';

	/* Internal counter of embeds found. */
	this.frameCounter = 0;

	/**
	 * @brief Setup an event listener in the contentArea for the contextMenu
	 *
	 * Set mobilizer.showContextMenu as the eventlistener to determine whether a
	 * popup should show or not in the contentareacontextmenu.
	 */
	this.onLoad=function()
	{
		document.getElementById('contentAreaContextMenu').addEventListener('popupshowing',
				function() {
					mobilizer.showContextMenu.call(mobilizer);
				},
				false);
		try {
			var appcontent = document.getElementById('appcontent');
			if ( appcontent && !appcontent.processed)
			{
				appcontent.processed = true;
				appcontent.addEventListener('DOMContentLoaded',
						function() {
							mobilizer.doObjectOverlay.call(mobilizer,MixxerContentArea.contentWindow);
						},
						false);
			}
		} catch ( e ) {
			alert(e); /* TODO: What should we do here? */
		}
	}

	/**
	 * @brief Get arguments suitable for a contentWindow.open call
	 *
	 * Return a comma seperated list of options that are suitable to be
	 * passed to an open() call.
	 *
	 * TODO: Calculate left and top so the window is positioned correctly
	 *
	 * @param width int integer defining the width of the popup
	 * @param height int integer defining the height of the popup
	 *
	 * @return string options for the popup window
	 */
	this.getPopupArgs=function(width,height)
	{
		var window_args = new Object();
		window_args['scrollbars'] = 'no';
		window_args['menubar'] = 'no';
		window_args['width'] = width;
		window_args['height'] = height;
		window_args['resizable'] = 'yes';
		window_args['toolbar'] = 'no';
		window_args['status'] = 'no';
		window_args['left'] = '0';
		window_args['top'] = '0';
		var arg_array = new Array();
		for ( var i in window_args )
		{
			arg_array[arg_array.length] = i + '=' + window_args[i];
		}
		return arg_array.join();
	}

	/**
	 * @brief Determine whether to show the mobilizer or not
	 *
	 * This code determines, based on context, whether something can be mobilized
	 * or not. Currently this means:
	 *	 * Images
	 *	 * Selected Text
	 *	 * Files with extensions in the extensions array
	 * If not one of the above, don't show the extension
	 */
	this.showContextMenu=function()
	{
		document.getElementById('context-mobilizer-all').hidden = false;

		if ( !gContextMenu )
		{
			document.getElementById('context-mobilizer').hidden = true;
			return;
		}

		if ( gContextMenu.onLoadedImage || gContextMenu.isTextSelected )
		{
			document.getElementById('context-mobilizer').hidden = false;
		}
		else if ( gContextMenu.onLink )
		{
			var link = gContextMenu.linkURL;
			for ( var i = 0; i < this.extensions.length; i++ )
			{
				var re = new RegExp("^.*\\." + this.extensions[i] + '$', 'i');
				if ( link.match(re) )
				{
					document.getElementById('context-mobilizer').hidden = false;
					return;
				}
			}
			document.getElementById('context-mobilizer').hidden = true;
		}
		else
		{
			document.getElementById('context-mobilizer').hidden = true;
		}
	}

	/**
	 * @brief Determine what do do if the 'Mobilize' menu item is clicked
	 *
	 * This method determines what action to take if the 'Mobilize' menu item is
	 * clicked.
	 */
	this.onMenuItemCommand=function(e)
	{
		if ( !gContextMenu )
			return;

		if ( gContextMenu.isTextSelected )
			return this.handleSelectedText();
		else if ( gContextMenu.onLoadedImage )
			return this.handleLoadedImage();
		else if ( gContextMenu.onLink )
			return this.handleOnLink();
	}

	/**
	 * @brief Handle right click on a link
	 *
	 * Open a new mobilizer tab with the mobilizer in the background
	 */
	this.handleOnLink=function()
	{
		var t = gContextMenu.linkURL;
		return this.openNewMobilizerTab(t,false);
	}

	/**
	 * @brief Handle right click on an image
	 *
	 * Open a new mobilizer tab with the mobilizer in the foreground
	 */
	this.handleLoadedImage=function()
	{
		var t = gContextMenu.imageURL;
		return this.openNewMobilizerTab(t,true);
	}

	this.openNewExternalMobilizerTab=function(selected)
	{
		var browser = window.getBrowser();
		var newTab = browser;
		var tabUrl = this.base_url + 'wShowAll/true/module/OpenLocker/action/ExternalMobilize?sUrl=';
		var url = getWebNavigation().currentURI.spec;
		tabUrl += encodeURIComponent(url);
		newTab = newTab.addTab(tabUrl);
		if ( selected )
			browser.selectedTab = newTab;
		return;
	}

	/**
	 * @brief open a new mobilizer tab
	 *
	 * @param url string URL to open in a tab
	 * @param selected boolean whether tab is selected or not
	 */
	this.openNewMobilizerTab=function(url,selected)
	{
		var browser = window.getBrowser();
		var newTab = browser;
		var tabUrl = this.base_url + 'module/OpenLocker/action/Mobilize?Url=';
		tabUrl += encodeURIComponent(url);
		newTab = newTab.addTab(tabUrl);
		if ( selected )
			browser.selectedTab = newTab;
		return;
	}

	/**
	 * @brief Deal with highlighted text
	 *
	 * This method deals with text that has been selected for being
	 * mobilized.
	 *
	 * @see Mixxer_Mobilizer.getPopupArgs
	 * @see Mixxer_Mobilizer.onMenuItemCommand
	 */
	this.handleSelectedText=function()
	{
		var t = this.getSelectedContent(MixxerContentArea.contentWindow);
		if ( t )
		{
			var url = this.base_url + 'no_border/1/module/Pager?Postfix=' + encodeURIComponent(t);
			MixxerContentArea.contentWindow.open(url, 'mixxer_send2phone', this.getPopupArgs(400,350));
		}
		else
		{
			alert('No text was selected');
		}
		return;
	}

	/**
	 * @brief If text was selected, return it
	 *
	 * @param mw Object window object
	 *
	 * This is a recursive method that returns the text that was selected
	 * by a user.
	 *
	 * This was inspired by GSMS_getSelection from the google send to
	 * phone extension.
	 */
	this.getSelectedContent=function(mw)
	{
		if ( mw && mw.window )
		{
			if ( mw.window.getSelection() && mw.window.getSelection().toString() )
			{
				return mw.window.getSelection().toString();
			}
			for ( var i = 0; i < mw.frames.length; i++ )
			{
				var s = this.getSelectedContent(mw.frames[i]);
				if ( s )
					return s;
			}
		}
		return '';
	}

	this.doObjectOverlay=function(mw)
	{
		if ( !mw || !mw.window )
		{
			return;
		}
		
		var rawEmbeds = mw.document.getElementsByTagName('embed');

		for ( var t = 0; t < rawEmbeds.length; t++ )
		{
			var e = rawEmbeds[t];
			if (e.hasAttribute("mobilizerFramedObject"))
				continue;
			var wmode = e.getAttribute( 'wmode' );
			if( !wmode || wmode != 'transparent' )
			{
				var par = e.parentNode;
				
				var nextSib = e.nextSibling;
				
				e.setAttribute( 'wmode', 'transparent' );
				par.removeChild( e );
				
				if( nextSib )
					par.insertBefore( e, nextSib );
				else
					par.appendChild( e );
			}
			
			var url = mw.document.createElement('a');
			url.href = e.src;
			if ( url.protocol == undefined || !url.protocol )
				url.protocol = mw.document.location.protocol;
			if ( url.hostname == undefined || !url.hostname )
				url.hostname = mw.document.location.hostname;
			if ( url.pathname == undefined || !url.pathname )
				url.pathname = mw.document.location.pathname;
			if ( url.hostname == 'www.mixxer.com' )
				return;

			this.makeObjectFrame(e, url);
		}

		var frameLength = mw.frames.length;
		if( frameLength > 0 )
		{
			for ( var i = 0; i < frameLength; i++ )
			{
				this.doObjectOverlay(mw.frames[i]);
			}
		}
	}

	this.makeObjectFrame=function(node, contentURL)
	{
		if (node.hasAttribute("mobilizerFramedObject")) return;

		// Create frame node
		var frame, subDiv1, subDiv2, span, img;
		frame = node.ownerDocument.createElement('div'); 
		subDiv1 = node.ownerDocument.createElement('div');
		subDiv2 = node.ownerDocument.createElement('div'); // frame of tab
		span = node.ownerDocument.createElement('span');
		img = node.ownerDocument.createElement('img');

		// Style frame node
		frame.setAttribute("style", "margin: 0px; padding: 0px; overflow: visible;");
		subDiv1.setAttribute("style", "height: 0px; width: 100%; overflow: visible;");
		subDiv2.setAttribute("style", "padding: 1px; border-style: ridge ridge none ridge; border-width: 2px 2px 0px 2px; -moz-border-radius-topleft: 10px; -moz-border-radius-topright: 10px; -moz-opacity: 1; background-color: white; position: relative; top: -22.5px; left: 3px; z-index: 900;  width: 96px; height: 19px; cursor: pointer; overflow: visible;");
		span.setAttribute("style", "font-family: Sans-serif; font-size: 11px; font-style: normal; font-variant: normal; font-weight: normal; line-height: 180%; text-decoration: none; -moz-opacity: 1; color: black;");
		subDiv1.setAttribute("align", "left"); // center-align the tab.
		subDiv2.setAttribute("align", "center"); // left tab-text.

		// Adjust frame-width
		frame.style.width = node.getAttribute('width'); 
		frame.style.display = 'none';

			this.frameCounter += 1;
		var frameName = "mobilizer-frame-n"+this.frameCounter;
		var textName = "mobilizer-text-n"+this.frameCounter;
		frame.id = frameName;
		subDiv2.id = "mobilizer-tab-n"+this.frameCounter;


		// Assemble frame node
		frame.appendChild(subDiv1);
		subDiv1.appendChild(subDiv2);
		span.innerHTML = '<img style="width: 19px; height: 19px;" src="chrome://mobilizer/skin/mobilizer.png" align="left" border="0"/> <div style="margin-left: 19px; text-align: center;" id="'+textName+'">Mobilize This!</div>';
		subDiv2.appendChild(span);


		// Prevent Object Frame being added multiple times.
		node.setAttribute("mobilizerFramedObject", true);
		// Add cross-node references
		var callback = function(){return mobilizer.openNewMobilizerTab.call(mobilizer,contentURL, false);};
		var getFrame = function(){return frame;};
		var getNode  = function(){return node;};
		frame._mobilizerObject = node; // add Node reference to frame
		if (node.nodeName.toLowerCase() != "applet")
		{
			node._mobilizerFrame = frame; // objects: add Frame reference to node
			node.setAttribute("mobilizerFramedObject2", true);
		}

		frame.setAttribute("mobilizerFrame", true); // to identify the outer-frame on tab-click
		subDiv2.addEventListener('click', callback, false); // event listener, for tab-click


		var highlight = function()
		{
			var e = node.ownerDocument.getElementById(textName);
			e.style.textDecoration='underline';
			e.style.color='red';
		};
		var unHighlight = function()
		{
			var e = node.ownerDocument.getElementById(textName);
			e.style.textDecoration='none';
			e.style.color='black';
		}
		var showMe = function()
		{
			var e = node.ownerDocument.getElementById(frame.id);
			e.style.display="block";
			if ( e.getAttribute('interval') )
				clearInterval(e.getAttribute('interval'));
			node.ownerDocument.getElementById(subDiv2.id).style.opacity = 1;
		}
		var hideMe = function()
		{
			var millisec = 3000;
			var speed = Math.round(millisec / 100);
			var timer = 0;

			var e = node.ownerDocument.getElementById(frame.id);
			if ( e.getAttribute('interval') )
				clearInterval(e.getAttribute('interval'));

			e.setAttribute('tempInterval', 100);
			e.setAttribute('interval',
				setInterval(
					function(){
						var e = node.ownerDocument.getElementById(subDiv2.id);
						var o = node.ownerDocument.getElementById(frame.id);
						e.style.opacity = o.getAttribute('tempInterval')/100;
						o.setAttribute('tempInterval', o.getAttribute('tempInterval') - 3);
						if ( o.getAttribute('tempInterval') <= 0 )
						{
							clearInterval(o.getAttribute('interval'));
							o.style.display = 'none';
						}
					}, speed));
		}


		// If user mouses over the tab highlight it
		span.addEventListener('mouseover', showMe, false);
		span.addEventListener('mouseover', highlight, false);

		// If the user moves away from the tab un-highlight it
		span.addEventListener('mouseout', unHighlight, false);
		span.addEventListener('mouseout', hideMe, false);


		// If the user moves over the object, show the tab and unhighlight it
		node.addEventListener('mouseover', unHighlight, false);
		node.addEventListener('mouseover', showMe, false);

		node.addEventListener('mouseout', hideMe, false);

		node.setAttribute("mobilizerFrameName", frameName); // links applets to their frame, via unique-id
		var style = node.ownerDocument.defaultView.getComputedStyle(node, "");
		if (!frame.style.width) frame.style.width = style.getPropertyValue("width");

		var currentOffset = node, y = node.offsetTop, evalCode;

		while (currentOffset.offsetParent)
		{
			y += currentOffset.offsetParent.offsetTop;
			currentOffset = currentOffset.offsetParent;
		}

		if (y < 20)
		{
			var subFrame = frame.firstChild, subFrame2 = frame.firstChild.firstChild;
			subFrame2.style.setProperty("-moz-border-radius-bottomleft", "10px", null);
			subFrame2.style.setProperty("-moz-border-radius-bottomright", "10px", null);
			subFrame2.style.setProperty("border-style", "none ridge ridge ridge", null);
			subFrame2.style.setProperty("border-width", "0px 2px 2px 2px", null);
			subFrame2.style.setProperty("top", "0px", null);
			subFrame2.style.setProperty("right", "-5px", null);
			subFrame2.style.removeProperty("left");
			subFrame2.style.removeProperty("-moz-border-radius-topleft");
			subFrame2.style.removeProperty("-moz-border-radius-topright");
			subFrame.setAttribute("align", "left");

			if (node.nextSibling)
				evalCode = "node.parentNode.insertBefore(frame, node.nextSibling);";
			else evalCode = "node.parentNode.appendChild(frame);";
		}
		else evalCode = "node.parentNode.insertBefore(frame, node);";

		var parentiFrame = node.ownerDocument.defaultView.frameElement;
		if (parentiFrame != null)
		{
			var nodeHeight = parseInt(style.getPropertyValue("height").replace(/(\d+).*$/, "$1"));
			var parentiFrameStyle = node.ownerDocument.defaultView.getComputedStyle(parentiFrame, "");
			var parentiFrameHeight = parseInt(parentiFrameStyle.getPropertyValue("height").replace(/(\d+).*$/, "$1"));

			if (nodeHeight + 20 + y > parentiFrameHeight)
			{
				parentiFrame.style.setProperty("height", nodeHeight+20+y, "important");
				parentiFrame.height = nodeHeight+20+y;
			}
		}

		eval(evalCode);
		node.style.removeProperty("visibility");
	}

}

/**
 * Initialize a new Mixxer_Mobilizer object and add an event listener.
 */
var mobilizer = new Mixxer_Mobilizer();
var MixxerContentArea = document.getElementById('content');
window.addEventListener('DOMContentLoaded', function() { mobilizer.onLoad.call(mobilizer); }, false);

/*
 * Forced delay for pages that load their embeds via a javascript method
 */
document.addEventListener( 'DOMContentLoaded', function() {
		window.setTimeout( function() {
			mobilizer.doObjectOverlay.call(mobilizer,MixxerContentArea.contentWindow);
		}, 1000 );
}, false );
