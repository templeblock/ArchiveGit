<!-- BEGIN AudioEditorView_index_base.html.tpl -->
{assign var="stepCount" value="1"}
<div class="moduleWide fileFormat">
	<div class="moduleHead"><h2>Clip This Ringtone</h2></div>
	<div class="moduleBody white" id="mobilizeContainer" style="padding: 0;">
		<form action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm" style="margin:10px;">
		
		{if $user->isAuthenticated() && $myPhone}
                <div id='sendNoticeWrapper'>
                    <label for="SendNotice"><input type="checkbox" name="DontSendNotice" id="SendNotice" value="true" />Just send it to my Mixxer locker (don't send me a message)</label>
                </div>
            {/if}
            
            {if !$user->isAuthenticated()}
                {include file="EditorBase_select_phone.html.tpl"  formatMess='ringtone for your phone'}
                {assign var='stepCount' value='3'}
            {/if}
            
            <div id="setStartEndTime">
                {if $user->isAuthenticated() && $myPhone}
                <img id="defaultAudioEditorStep" class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount}.gif" />
                {/if}

                <div id='carrierPhoneContainer' style='float: left'>
                    <strong>Set start and stop of ringtone being uploaded</strong><br>
                    <span>Start at <input name="EditOptions[Ringtone.Start]" type="text" value="00" size="2"> seconds</span> |
                    <span>Stop at <input name="EditOptions[Ringtone.End]" type="text" value="30" size="2"> seconds (30 seconds maximum length)</span>
                </div>
            </div>
            
            <div id='title' class='defaultAudio'>
                <strong>Title</strong> (30 characters maximum)<br />
                <input size="30" type="text" id="mobilizeTitle" name="mobilizeTitle" class="mobilizeTitle" value="{$mobilizeTitle}" onkeypress="return preventEnterKey(event)" onkeyup="charCount(30,'mobilizeTitle','mobilizeTitleMsg');" onblur="hideCharCount('mobilizeTitleMsg');"/>
                <div id="mobilizeTitleMsg">&#160;</div>
            </div>

            {if $showCaptcha}
            
                <div id='captcha'>
                    To prevent spamming, type the code shown in the box below.<br /><br />
                    <img src="{genurl module="Misc" action="CaptchaImage"}" /><br /><br />
                    <strong>Type Code Here</strong><br />
                    
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
<!-- END AudioEditorView_index_base.html.tpl -->
