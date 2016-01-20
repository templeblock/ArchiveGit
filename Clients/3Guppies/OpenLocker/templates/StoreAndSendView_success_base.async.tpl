<!-- TEMP START: StoreAndSendView_success.async.tpl -->

<div class="moduleWide">
    <div class="moduleBody">
		<div id="sendSuccess">
			<div style="margin: 0 10px 0 60px;">
				<h1>Done! You've Just Mobilized!</h1>
				<p><strong>{$Title}</strong> is on its way to {if $SendNotice}<strong>{$ToPhoneNumber}</strong>{else}your locker{/if}!</p>
			</div>
			<br />
			<h3>What do you want to do now?</h3>
			<ul class="generic mobilizSuccessLinks">
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
</div>
<!-- TEMP END: StoreAndSendView_success.async.tpl -->
