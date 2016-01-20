<!-- TEMP START: StoreAndSendView_success.html.tpl -->
{addheaderfile file=$URL_BASE|cat:"css/openlocker/locker.css"}
<script type="text/javascript">
    s_pageName = "{$OmniturePageName}";
    s_events = "event3";
    s_channel = "{$OmnitureChannel}";
    s_prop2 = "{$OmnitureProp2}";
    s_hier1 = "{$OmniturePageName}";
</script>


<div class="moduleWide">
	<div class="moduleHead"><h2>You've Just Mobilized!</h2></div>
    <div class="moduleBody">
		<h3>Done!</h3>
		<p><strong>{$Title}</strong> is on its way to {if $SendNotice}<strong>{$ToPhoneNumber}</strong>{else}your locker{/if}!</p>
		<br />
		<h3>What do you want to do now?</h3>
		<ul class="generic peekabooBugFix mobilizSuccessLinks">
		{foreach from=$links item=linkInfo}
			<li>{$linkInfo.pre}<a {if $linkInfo.id}id="{$linkInfo.id}" {/if}href="{$linkInfo.url}">{$linkInfo.name}</a>{$linkInfo.post}</li>
		{/foreach}
		</ul>
		<br />
        <h3>Getting stuff on your phone is easy!</h3>
        {if $SendViaSMS}
            <img src="{$URL_BASE}images/success_image_sms.gif" />
        {else}
            <img src="{$URL_BASE}images/success_image_mms.gif" />
        {/if}
    </div>
</div>
<!-- TEMP END: StoreAndSendView_success.html.tpl -->
