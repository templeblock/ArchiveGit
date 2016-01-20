<!-- START: ExternalMobilizeView_error.html.tpl -->
{* page layout *}
    {addheaderfile file=$URL_BASE|cat:'css/widget/mobilize_scrape.css'}
	{addheaderfile file=$URL_BASE|cat:'css/widget/widget_error.css'}
 
	
<div class="pageContent" id="mobilizeScrape">


<script type="text/javascript">
  s_pageName = 'Mobilization:Error:No Referring URL';
  s_hier1    = 'Mobilization:Error:No Referring URL';
  s_channel  = 'Mobilization';
</script>

{if $user->hasPrivilege( 'Anonymous User' )}
    {addheaderfile file=$URL_BASE|cat:'css/widget/anon_widget.css'}
    
    <div id="anonScrapeHeader">
    
        <div id="rightHeader">
            <h1>Mobilize Your Media</h1>
            <dl>
                <dt>mo&prime; <span>&bull;</span> bi <span>&bull;</span> lize, <em>v.</em>: </dt>
                <dd>to get it on your phone; to make it mobile.</dd>
            </dl>
        </div>
    
        <div>
            Powered by mixxer<br>
            <img src="{$IMAGES_DIR}ui/main/mixxer_logo.gif" width="128">
        </div>
    </div>
{/if}

<div class="moduleWide" id="box_wrapper">
    <div class="moduleHead"><h2>Which web page are you mobilizing?</h2></div>
    <div class="moduleBody white" style="border: none;">
        <p>
            <strong>Oopsy!</strong> Sorry, we couldn't tell which page you 
            were trying to mobilize.
        </p>
        
        <p>Don't worry – we can fix it. All you have to do is:</p>
        
        <ol id="steps">
            <li>Leave this page open and go back to the tab or window with the page
            you want to mobilize.</li>
            <li>Copy that URL from your browser’s address bar.</li>
            <li>Come back to this page, paste it in the box below, and click Mobilize!</li>
        </ol>
		
        <p id="copyPasteWrapper">
            <img src="{$IMAGES_DIR}ui/widget/address_copy.gif">
        </p>
        
        <form method="GET">
            <input type='text' id='sUrl' name='sUrl'>
            <button type="submit" class="globalBtn burg">Mobilize!</button>
        </form>
        
    </div>
</div>
<!-- /class="moduleWide" -->
