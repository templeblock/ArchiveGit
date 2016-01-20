<!-- START: ExternalMobilizeView_input.html.tpl -->
{* page layout *}
    {addheaderfile file=$URL_BASE|cat:'css/widget/mobilize_scrape.css'}

{* base yui scripts *}
    {addheaderfile file=$URL_BASE|cat:"javascript/yui/container/container%ext.js"}
    {addheaderfile file=$URL_BASE|cat:"javascript/yui/container/assets/container.css"}
    {addheaderfile file=$URL_BASE|cat:"javascript/yui/connection/connection%ext.js"}

{* scraper object script *}
    {addheaderfile file=$URL_BASE|cat:'javascript/yui/mixxer/widget/scraper.js'}

{* Simulate successive page views *}
    {addheaderfile file=$URL_BASE|cat:'javascript/yui/mixxer/omniture.js'}



<script type="text/javascript">
    s_channel = "Mobilization";
    s_prop2 = "";
    
    var scrapeUrl = '{$openl_exter_scrapeUrl}';
    
    {* Building up all the necessary omniture arguments for page load *}
    
    {if $openl_exter_ffPlugin} {literal}
        s_pageName = "Mobilization:Plug-in:Firefox:Load Media";
        s_hier1 = "Mobilization:Plug-in:Firefox:Load Media";
        
        var anonChooseMediaOmniArgs = {
              s_pageName: "Mobilization:Plug-in:Firefox:Choose Media",
              s_hier1: "Mobilization:Plug-in:Firefox:Choose Media"
        };

        var credentialOmniArgs = {
            s_pageName: "Mobilization:Plug-in:Firefox:Private Profile:Enter Credential",
            s_hier1: "Mobilization:Plug-in:Firefox:Private Profile:Enter Credential"
        };
        
        var privChooseMediaOmniArgs = {
            s_pageName: "Mobilization:Plug-in:Firefox:Private Profile:Choose Media",
            s_hier1: "Mobilization:Plug-in:Firefox:Private Profile:Choose Media"
        };
        
    {/literal} {elseif $openl_exter_iePlugin} {literal}
        s_pageName = "Mobilization:Plug-in:IE:Load Media";
        s_hier1 = "Mobilization:Plug-in:IE:Load Media";
    
        var anonChooseMediaOmniArgs = {
              s_pageName: "Mobilization:Plug-in:IE:Choose Media",
              s_hier1: "Mobilization:Plug-in:IE:Choose Media"
        };
        
        var credentialOmniArgs = {
            s_pageName: "Mobilization:Plug-in:IE:Private Profile:Enter Credential",
            s_hier1: "Mobilization:Plug-in:IE:Private Profile:Enter Credential"
        };
        
        var privChooseMediaOmniArgs = {
            s_pageName: "Mobilization:Plug-in:IE:Private Profile:Choose Media",
            s_hier1: "Mobilization:Plug-in:IE:Private Profile:Choose Media"
        };
    {/literal} {else} {literal}
        s_pageName = "Mobilization:Load Media";
        s_hier1 = "Mobilization:Load Media";
    
        var anonChooseMediaOmniArgs = {
              s_pageName: "Mobilization:Choose Media",
              s_hier1: "Mobilization:Choose Media"
        };

        var credentialOmniArgs = {
            s_pageName: "Mobilization:Private Profile:Enter Credential",
            s_hier1: "Mobilization:Private Profile:Enter Credential"
        };
        
        var privChooseMediaOmniArgs = {
            s_pageName: "Mobilization:Private Profile:Choose Media",
            s_hier1: "Mobilization:Private Profile:Choose Media"
        };
        
    {/literal} {/if}
    
    
    
    {literal}
        var cfg = {
            anonChooseMediaOmniArgs: anonChooseMediaOmniArgs,
            
            credentialOmniArgs: credentialOmniArgs,
            
            privChooseMediaOmniArgs: privChooseMediaOmniArgs
            
            {/literal}{if $openl_exter_extcmp}
                , extCmpTrackCode: '{$openl_exter_extcmp}'
            {/if}{literal}
        
        };
        
        try {
            var widgetScraper = new YAHOO.mixxer.Widget.Scraper( scrapeUrl, cfg );
        } catch( e ) {
            console.log( 'error creating new widget scraper: ' + e.message );
        }
    {/literal}
</script>



<div class="pageContent" id="mobilizeScrape">

	<div id="head">
		<div class="left"><a href="{$URL_BASE}">Powered by Mixxer</a></div>
		<div class="right"><!-- {if $user->isAuthenticated()}Hello, (var username) | <a href="">Log Out</a>{else}<a href="">Log In</a> | <a href="{genurl module='Checkout' action='RegisterStart'}">Join</a>{/if} --></div>
		<p class="clear"></p>
	</div>
	

	<!-- LEFT SIDE -->
	<div id="left">
		<div id="preview">
			<div id="mxr-previous"><a href="javascript:;"><img src="{$IMAGES_DIR}widget/paddle_La.gif" border="0" onmouseover="this.src='{$IMAGES_DIR}widget/paddle_Lb.gif';" onmouseout="this.src='{$IMAGES_DIR}widget/paddle_La.gif';" /></a></div>
			<div id="phoneBg">
				<div id="mxr-screen"></div>
				<!-- V2 : EDIT MEDIA LINK
				<div id="editMedia"><a href="">click to edit</a>
					on Picture thumbs = "click to edit"
					on Video thumbs = "click to preview"
					on Audio thumbs = "click to edit"
					on no thumb = <nothing>
				</div> -->
				<div id="title"><input id="mxr-title" type="text" /></div>
			</div>
			<div id="mxr-next"><a href="javascript:;"><img src="{$IMAGES_DIR}widget/paddle_Ra.gif" border="0" onmouseover="this.src='{$IMAGES_DIR}widget/paddle_Rb.gif';" onmouseout="this.src='{$IMAGES_DIR}widget/paddle_Ra.gif';" /></a></div>
			<div class="clear"></div>
		</div>
		
		<div class="textCenter">
			<!-- this link invokes div id="send" -->
			<img id="mxr-mobilize" src="{$IMAGES_DIR}widget/btn_sendToPhoneUp.gif" border="0" />
		</div>
		
		<div id="serviceInfo">
			<h1>Mobilize It!</h1>
			<p>The Mobilizer Widget is the easiest tool to get media files from the web to any cell phone. <strong>It's FREE and simple!</strong></p>
			<ul class="generic">
				<li><a href="{$URL_BASE}widget">Get your own</a> Mobilizer Widget</li>
				<li><a href="{$URL_BASE}MySpace">Learn more</a> or <a href="{$URL_BASE}help#contact">get help</a></li>
				<li><a href="{$URL_BASE}widget/remove">Remove</a> from your layout </li>
			</ul>
		</div>
		
		<!-- BBBOnline banner -->
		<div class="textCenter">
            <a href="{genurl module='Misc' action='Privacy'}" target="_blank">
                <img src="{$IMAGES_DIR}finalmark.gif" border="0" style="margin:10px 0px 0px 0px;">
            </a><br />
            <a href="http://www.bbbonline.org/cks.asp?id=207052110315788140" target="_blank">
                <img src="{$IMAGES_DIR}PrivacySeal3.gif" border="0" style="margin:5px 0px 0px 0px;">
            </a><br />
            <a href="http://www.bbbonline.org/cks.asp?id=106060575922" target="_blank">
                <img src="{$IMAGES_DIR}ReliabilitySeal3.gif" border="0" style="margin:5px 0px 0px 0px;">
            </a>
        </div>
		<br />
		<br />
		
		<blockquote style="margin: 0 0 0 10px; width: 90%;" class="finePrint textCenter">
		* Note: your wireless carrier may<br />charge extra to receive SMS and access the internet from your cell phone.
		</blockquote>
	</div>
		
		
	<!-- RIGHT SIDE -->
	<div id="right">
	
		<div id="scrapeInfo">
            <h4>Results from</h4>
            <h2 id="mxr-myspaceUserName">UserName</h2>
			<span id="mxr-prettyUrl"><a href="{$openl_exter_scrapeUrl}" target="_blank">{$openl_exter_scrapeUrl|truncate:55:"...":true}</a></span>
		</div>
		<div id="scrapeLoad">working...<br /><img src="{$IMAGES_DIR}ui/progWheel_trans.gif" border="0" /></div>
		<div class="clear"></div>
		
		<div id="scrapeNav">
			<ul>
				<li id="mxr-picTab" class="active"><a href="javascript:;">Pictures</a> <span id='mxr-picCount'></span></li>
				<li id="mxr-videoTab"><a href="javascript:;">Videos</a> <span id='mxr-videoCount'></span></li>
				<li id="mxr-audioTab"><a href="javascript:;">Audio</a> <span id='mxr-audioCount'></span></li>
			</ul>
			<div class="clear"></div>
		</div>
		
		<div id="results">
			<div id="mxr-pictures">
			</div>
			<div id="mxr-video">
			</div>
			<div id="mxr-audio">
                <!-- concatenate these results at 16 characters-->
                <!-- icon opens preview, title mobilizes track -->
			</div>
			<div class="clear"></div>
		</div>
		
		<br />
        
		
        
		<div class="moduleBody white" id="actions">
        
            <p><a id="mxr-scrapeWithAds" href="{genurl module="OpenLocker" action="ExternalMobilize"}">Show all media</a>
            (this may include logos, icons and ads on the page)</p>
            
			<div class="moduleHead"><h2>Modify Search</h2></div>
			<form id="mxr-rescrape" name="rescrape">
				<p>URL to Mobilize</p>
				<input id="mxr-scrapeOtherUrlInput" type="text" class="text" name="newUrl" size="35" style="margin-left: 100px;" /><br />
				<input id="mxr-credCheckbox" type="checkbox" name="" value="" /> <label for="mxr-credCheckbox">Login to MySpace to access private profiles</label>
				<br /><br />		
				<div id="mxr-mySpaceCred">
					<h4>MySpace email</h4>
					<input name="myspaceEmail" class="text" type="text" size="25" />
					<br />
					<h4>MySpace password</h4>
					<input name="myspacePass" class="text" type="password" size="25" />
					<p class="greyText"><em>(We do NOT store your login information)</em></p>
				</div>
				<br />
				<!-- Rollover doesn't work in IE6 : Need to fix onload.js -->
				<div style="text-align: center;"><input class="globalBtn burg" value="refresh results" style="width: 8em;" type="submit" /></div>
			</form>
		</div>
		
		<!-- FREE SAMPLES AREA
		<div class="moduleBody white" id="samples">
			<div class="moduleHead"><h2>Get FREE Samples</h2></div>
			<div class="mxr-thumb"></div>
			<div class="mxr-thumb"></div>
			<div class="mxr-thumb"></div>
			<div class="mxr-thumb"></div>
			<ul>
				<li><a href="#"><img src="{$IMAGES_DIR}widget/icon_audioUp.gif" onmouseover="this.src='{$IMAGES_DIR}widget/icon_audioDown.gif';" onmouseout="this.src='{$IMAGES_DIR}widget/icon_audioUp.gif';" border="0" style="margin-bottom: -5px;" /></a> <a href="#">Fake audio tr...</a></li>
				<li><a href="#"><img src="{$IMAGES_DIR}widget/icon_audioUp.gif" onmouseover="this.src='{$IMAGES_DIR}widget/icon_audioDown.gif';" onmouseout="this.src='{$IMAGES_DIR}widget/icon_audioUp.gif';" border="0" style="margin-bottom: -5px;" /></a> <a href="#">Fake audio tr...</a></li>
			</ul>
			<div class="clear"></div>
		</div>
		-->
	
	</div>
	<!-- end RIGHT SIDE -->
	
	<div class="clear">
	&nbsp;
	</div>

</div>
<!-- end Page Content -->








{* Scrape Results Container *}
<div id="mxr-scrapeResults"></div>

{* Hidden mp3 player container *}
<div id="mxr-hiddenMP3"></div>

{* YUI form dialog container *}
<div id="mxr-yuiFormDialog"><div class="bd"></div></div>


<div id="mxr-noMediaResults">
    <p><strong class="error">Sorry, we didn't find any audio files, pictures or video to mobilize here.</strong></p>
	<p>You can try another URL or include logos, icons and small images below.</p>
    <p>Wanna try it out?  Mobilize your own files from your PC or grab one from another member at 
	<a href="{$URL_BASE}" target="_blank">www.mixxer.com</a>.</p>
</div>

<div id="mxr-noAudioResults">
    <p><strong class="error">Sorry, we didn't find any audio files to mobilize.</strong></p>
    <p>Most of the music in streams and Flash players is copyrighted and would
    be illegal to download without permission.</p>
    <p>If you own a CD though, you can <a href="{genurl module='OpenLocker' action='Mobilize'}">make your own ringtone</a> for free.</p>
</div>

<div id="mxr-noVideoResults">
    <p><strong class="error">Sorry, we didn't find any video files to mobilize.</strong></p>
    <p>Right now Mixxer support YouTube embedded in MySpace, and support for
    the MySpace player is coming soon.</p>
    <p>You can mobilize a sample video from <a href="{$URL_BASE}cell-phone-videos">www.mixxer.com</a>.</p>

</div>

<div id="mxr-noPicResults">
    <p><strong class="error">Sorry, we didn't find any picture files to mobilize.</strong>
	<br />
	You can try another URL or include logos, icons and small images below.</p>
    <p>Wanna try it out? Mobilize your own pictures or grab one from another
    member at <a href="{$URL_BASE}cell-phone-pictures">www.mixxer.com</a>.</p>
</div>

<div id="mxr-noUrlResults">
     <p><strong class="error">Oops! We couldn't tell which page you were trying to mobilize.</strong>
	 <br />
	 Don't worry – we can fix it. All you have to do is:</p>
	 <ol>
	 	<li>Leave this page open and go back to the tab or window with the page you want to mobilize.</li>      
		<li>Copy that URL from your browser’s address bar.</li>
		<li>Come back to this page, paste it in the box below, and click refresh results!</li>
	 </ol>
	 <div class="textCenter"><img src="{$IMAGES_DIR}widget/URLHowTo.gif" boreder="0" /></div>
</div>

<div id="mxr-timeoutResults">
	<p class="error"><strong>Ugh.</strong></p>
	<p>That page took too much time to load. [@$#%!]</p>
	<p>You can try again or go to a different page.</p>
	<p>Mobilize your own files at <a href="{$URL_BASE}">www.mixxer.com</a>.</p>
</div>

<div id="mxr-privateProfileResults">
    <p class="error"><strong>Whoops.</strong></p>
	<p>This profile is private, and <span>USERNAME</span> must add you as a friend to see his/her profile.</p>
	<p>You can try again with different login info or go to a different profile.</p>
	<p>Mobilize your own files at <a href="{$URL_BASE}">www.mixxer.com</a>.</p>
</div>

<div id="mxr-noAccessResults">
	<p class="error"><strong>Whoops.</strong></p>
	<p>That page couldn't be found or is inaccessible.</p>
    <p>You can try again or go to a different profile.</p>
    <p>Mobilize your own files at <a href="{$URL_BASE}">www.mixxer.com</a>.</p>
</div>

<div id="mxr-sysErrorResults">
	<p class="error"><strong>Uh-oh.</strong></p>
	<p>That didn't work. My bad.</p>
    <p>You can try again or go to a different profile.</p>
    <p>Mobilize your own files at <a href="{$URL_BASE}">www.mixxer.com</a>.</p>
</div>
<!-- END: ExternalMobilizeView_input.html.tpl -->
