<!-- START VideoEditorView_index_base.html.tpl -->
{addheaderfile file=$URL_BASE|cat:"css/openlocker/locker.css"}
{assign var="stepCount" value="1"}


<div class="moduleWide fileFormat">
	<div class="moduleHead userPanel"><h2>Format This Video</h2></div>
	<div class="moduleBody white" id="mobilizeContainer">
		<form action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm" style="margin: 10px;">
	        {if $CannotDetectCarrier && !$myPhone}
                {include file="EditorBase_email.html.tpl"}
            {else}
                <input type="hidden" name="Recipients" value="{$Recipients}" />
            {/if}
 	
            {if $user->isAuthenticated() && $myPhone}
                <div style="margin-bottom: 20px;">
				<br />
                <label for="SendNotice"><input type="checkbox" name="DontSendNotice" id="SendNotice" value="true" style="margin: 0 5px -1px 0;" />Just send it to my Mixxer locker (don't send me a message)</label>
                <br /><br />
				</div>
            {else}
                {if $myPhone}
                    {include file="EditorBase_select_phone.html.tpl" formatMess='video for your phone'}
                {/if}
            {/if}
            
            <div id='title'>
                <p>Title (30 characters maximum)</p>
                <input size="30" type="text" id="mobilizeTitle" name="mobilizeTitle" class="mobilizeTitle" value="{$mobilizeTitle}" onkeypress="charCount(30,'mobilizeTitle','mobilizeTitleMsg');return preventEnterKey(event)" onkeyup="charCount(30,'mobilizeTitle','mobilizeTitleMsg');" onblur="hideCharCount('mobilizeTitleMsg');"/>
                <div id="mobilizeTitleMsg">&#160;</div>
            </div>
            
            {if $user->isAuthenticated() && $myPhone}
                <div id='tags'>
                    <p>Tags</p>
                    <input size="30" type="text" id="mobilizeTags" name="mobilizeTags" class="mobilizeTags" value="{$mobilizeTags}" onkeypress="return preventEnterKey(event)"/>
                </div>
            {elseif $showCaptcha}
                <div id='captcha'>
                     <p>Type the code below (to prevent spamming)</p>
                    <img src="{genurl module="Misc" action="CaptchaImage"}" /><br />
                    <p>Type Code Here</p>
                    {if $CaptchaError}
                        <span style="color: red;">{$CaptchaError}</span><br />
                    {/if}
                    <input type="text" id="mobilizeCaptcha" name="captchaValue" class="mobilizeTitle" />
                </div>
            {/if}

            <div id="finishCancel">
                <button style="padding:2px 5px;" class="globalBtn burg" type="submit"{if $CannotDetectCarrier && (!$carrierId || $carrierId < 0)} onclick="if(document.getElementById('carrierSelect').value == -1){ldelim}alert('Please select a carrier'); return false{rdelim}"{/if}>Finish</button>
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
