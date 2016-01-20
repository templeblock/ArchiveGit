<!-- BEGIN AudioEditorView_index_base.default.async.tpl -->
{assign var="stepCount" value="1"}

<div class="moduleWide scrapeDialog" id="audioEditor2">
	<div class="moduleBody" id="mobilizeContainer">
		<form action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm">

            <div id="setStartEndTime">
                <!-- {if $user->isAuthenticated() && $myPhone}
                <img id="defaultAudioEditorStep" class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount}.gif" />
                {/if} -->

                <div id="carrierPhoneContainer">
                    <h3>Set start and stop times for this audio file</h3>
                    <p>Start at <input name="EditOptions[Ringtone.Start]" type="text" value="00" size="2" style="margin: 4px 0;"> seconds<br />
					Stop at <input name="EditOptions[Ringtone.End]" type="text" value="30" size="2"> seconds (30 seconds maximum length)</p>
                </div>
            </div>

			<br />
            
            <div id="title" class="defaultAudio">
                Title (30 characters maximum)<br />
                <input type="text" id="mobilizeTitle" name="mobilizeTitle" class="mobilizeTitle" value="{$mobilizeTitle}" onkeypress="return preventEnterKey(event)"/>
            </div>
			
			{if !$user->isAuthenticated() && $myPhone}
                {include file="EditorBase_select_phone.html.tpl"  formatMess='ringtone for your phone'}
                {assign var='stepCount' value='3'}
				<br />
            {/if}

            {if $showCaptcha}
                <div id='captcha'>
                    <p>Type the code below (to prevent spamming)</p>
					<img src="{genurl module="Misc" action="CaptchaImage"}" /><br />
                    {if $CaptchaError}
                        <span style="color: red;">{$CaptchaError}</span><br />
                    {/if}
                    <input type="text" id="mobilizeCaptcha" name="captchaValue" class="mobilizeTitle" />
                </div>
            {/if}
			
			<!--
			{*
			{if $user->isAuthenticated()}
				{if $myPhone}
					<p>The audio you mobilized will be added to your locker.</p>
				{else}
					<p>The audio you mobilized will be send to your friend.</p>
				{/if}
			{/if}
			*}
			-->
			
			{if $user->isAuthenticated() && $myPhone}
                <div id="sendNoticeWrapper" style="text-align: center;">
                    <label for="SendNotice"><input type="checkbox" name="DontSendNotice" id="SendNotice" value="true" />Just send it to my Mixxer locker<br />(don't send me a message)</label>
                </div>
            {/if}

            <div id="finishCancel">
                <button style="padding:2px 5px;" class="globalBtn burg" type="submit">Finish</button>
            </div>
            
            <input type="hidden" name="EditPassThru" value="true" />
            <input type="hidden" name="Filename" value="{$ContentInfo.Filename}" />
            <input type="hidden" name="{$InputType}" value="{$InputValue}" />
            <input type="hidden" name="InputMethod" value="{$InputMethod}" />
            <input type="hidden" name="SourceUrl" value="{$SourceUrl}" />
            <input type="hidden" name="Affiliate" value="{$Affiliate}" />
            <input type="hidden" name="Recipients" value="{$Recipients}" />
            <input type="hidden" name="agreeTC" value="{$agreeTC}" />
            <input type="hidden" name="fromPhone" value="{$fromPhone}" />
            <input type="hidden" name="toPhone" value="{$toPhone}" />
            <input type="hidden" name="fromName" value="{$fromName}" />
        
        </form>

	</div>
</div>
<!-- END AudioEditorView_index_base.default.async.tpl -->
