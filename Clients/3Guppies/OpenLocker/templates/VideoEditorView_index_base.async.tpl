<!-- START VideoEditorView_index_base.async.tpl -->
{addheaderfile file=$URL_BASE|cat:"css/openlocker/locker.css"}
{assign var="stepCount" value="1"}

<div class="moduleWide">
	<div class="moduleBody" id="mobilizeContainer">
		<form action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm">
	        {if $CannotDetectCarrier && !$myPhone}
                {include file="EditorBase_email.html.tpl"}
            {else}
                <input type="hidden" name="Recipients" value="{$Recipients}" />
            {/if}
            
            <div id="title">
                Title (30 characters maximum)<br />
                <input type="text" id="mobilizeTitle" name="mobilizeTitle" class="mobilizeTitle" value="{$mobilizeTitle}" onkeypress="return preventEnterKey(event)"/>
            </div>
			
			{if !$user->isAuthenticated() && $myPhone}
            	{include file="EditorBase_select_phone.html.tpl" formatMess='video for your phone'}
				<br />
            {/if}
			
			<!-- {*
			{if $user->isAuthenticated()}
				{if $myPhone}
					<p>The video you mobilized will be added to your locker.</p>
				{else}
					<p>The video you mobilized will be sent to your friend.</p>
				{/if}
			{/if}
			*}
			-->
            
            {if $user->isAuthenticated() && $myPhone}
			<!--
                <div id='tags'>
                    <strong>Tags</strong><br />
                    <input type="text" id="mobilizeTags" name="mobilizeTags" class="mobilizeTags" value="{$mobilizeTags}" onkeypress="return preventEnterKey(event)"/>
                </div>
			-->
			<div id="sendNoticeWrapper" style="text-align: center;">
				<label for="SendNotice"><input type="checkbox" name="DontSendNotice" id="SendNotice" value="true" />Just send it to my Mixxer locker<br />(don't send me a message)</label>
			</div>
            {elseif $showCaptcha}
                <div id='captcha'>
                    <p>Type the code below (to prevent spamming)</p>
					<img src="{genurl module="Misc" action="CaptchaImage"}" /><br />
                    {if $CaptchaError}
                        <span style="color: red;">{$CaptchaError}</span><br />
                    {/if}
                    <input type="text" id="mobilizeCaptcha" name="captchaValue" class="mobilizeTitle" />
                </div>
            {/if}

            <div id="finishCancel">
                <button style="padding:2px 5px;" class="globalBtn burg" type="submit">Finish</button>
            </div>
            
            <input type="hidden" name="EditPassThru" value="true" />
            <input type="hidden" name="{$InputType}" value="{$InputValue}" />
            <input type="hidden" name="InputMethod" value="{$InputMethod}" />
            <input type="hidden" name="SourceUrl" value="{$SourceUrl}" />
            <input type="hidden" name="Affiliate" value="{$Affiliate}" />
            <input type="hidden" name="Filename" value="{$ContentInfo.Filename}" />
            <input type="hidden" name="toPhone" value="{$toPhone}" />
            <input type="hidden" name="fromPhone" value="{$fromPhone}" />
            <input type="hidden" name="fromName" value="{$fromName}" />
            <input type="hidden" name="agreeTC" value="{$agreeTC}" />
            {if !$user->isAuthenticated()}
                <input type="hidden" id="idSizeDX" name="EditOptions[deviceWidth]" value="{$DeviceDim.width}" />
                <input type="hidden" id="idSizeDY" name="EditOptions[deviceHeight]" value="{$DeviceDim.height}" />
            {/if}
		</form>
		
	</div>
</div>
<!-- END VideoEditorView_index_base.html.tpl -->
