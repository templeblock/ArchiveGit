<!-- START PictureEditorView_index_base.async.tpl -->
{addheaderfile file=$URL_BASE|cat:"css/openlocker/locker.css"}
{assign var="stepCount" value="1"}

<div class="moduleWide">
	<div class="moduleBody" id="mobilizeContainer">
		<form id="uploadForm" action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm">
        {if $user->isAuthenticated()}
            <div id="cropperInstructions" class="stepWrapper peekabooBugFix">
                {if $NoCroppingAllowed}
                <h3>We're sorry, animated gifs cannot be cropped at this time.</h3>
                {else}
                <h3>Click and drag handles to select the part of this picture you want to keep.</h3>
                {/if}
            </div>
        {else}
            <div id="cropperInstructions" class="stepWrapper">
                {if $NoCroppingAllowed}
                <h3>We're sorry, animated gifs cannot be cropped at this time.</h3>
                {else}
                <!-- <img class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount++}.gif" /> -->
                <h3>Click and drag handles to select the part of this picture you want to keep.</h3>
                {/if}
            </div>
        {/if}
		
		<br />

        <table id="cropperArea">
            <tr id="cropperRow">
                <td style="width:400px; vertical-align: top;">
                    <div id="idCropDisplayContainer" style="margin-right:10px;">
                        <img id="idCropDisplay" src="{$IMAGES_DIR}cropper/Dummy.jpg" alt="" />
                    </div>
                </td>
                <td style="vertical-align:top; text-align:center; display: none;">
                    <div id="idPhonePreviewContainer"{if !$user->isAuthenticated() || !$myPhone} style="display: none;"{/if}>
                        <div style="text-align:left;{if $NoCroppingAllowed}visibility:hidden;{/if}" id="idStretchOptions" >
                            <label for="Freeform"><input type="radio" name="EditOptions[Image.Resize]" value="Freeform" id="Freeform" onclick="MobilizeCrop.OnFreeform()" checked="checked" /><span id='idFreeformLabel'>&nbsp;Freeform (Crop/resize)</span></label><br />
                            <label for="StretchToFit"><input type="radio" name="EditOptions[Image.Resize]" value="Stretch" id="StretchToFit" onclick="MobilizeCrop.OnStretchToFit()" /><span id='idStretchLabel'>&nbsp;Stretch to fit</span></label><br />
                            <label for="MaintainAspect"><input type="radio" name="EditOptions[Image.Resize]" value="FitScreen" id="MaintainAspect"  onclick="MobilizeCrop.OnMaintainAspect()" /><span id='idAspectLabel'>&nbsp;Perfect fit (Keep shape)</span></label><br />
                        </div>
    
                        <div style="margin-top:20px; margin-bottom:10px;" id="idCropDeviceModel">
                            <b>My {if $DeviceModel}{$DeviceModel}{else}Phone{/if}</b>
                        </div>
                        <table cellpadding="0px" cellspacing="0px">
                            <tr>
                                <td style="padding:0px; width:11px; height:11px; background-image:url({$IMAGES_DIR}cropper/GenericPhoneLT.jpg); background-repeat:no-repeat;" />
                                <td style="padding:0px; width:11px; height:11px; background-image:url({$IMAGES_DIR}cropper/GenericPhoneCT.jpg); background-repeat:repeat-x;" />
                                <td style="padding:0px; width:11px; height:11px; background-image:url({$IMAGES_DIR}cropper/GenericPhoneRT.jpg); background-repeat:no-repeat;" />
    
                            </tr>
                            <tr>
                                <td style="padding:0px; width:11px; height:11px; background-image:url({$IMAGES_DIR}cropper/GenericPhoneLC.jpg); background-repeat:repeat-y;" />
                                <td style="padding:0px; background-color:black;">
                                    <div style="width:128px; height:160px; text-align: left; vertical-align: top;">
                                        <div id="idPhoneDisplayContainer">
                                            <img id="idPhoneDisplay" src="{$IMAGES_DIR}cropper/Dummy.jpg" alt="" />
                                        </div>
                                    </div>
    
                                </td>
                                <td style="padding:0px; width:11px; height:11px; background-image:url({$IMAGES_DIR}cropper/GenericPhoneRC.jpg); background-repeat:repeat-y;" />
                            </tr>
                            <tr>
                                <td style="padding:0px; width:11px; height:16px; background-image:url({$IMAGES_DIR}cropper/GenericPhoneLB.jpg); background-repeat:no-repeat;" />
                                <td style="padding:0px; width:11px; height:16px; background-image:url({$IMAGES_DIR}cropper/GenericPhoneCB.jpg); background-repeat:repeat-x;" />
                                <td style="padding:0px; width:11px; height:16px; background-image:url({$IMAGES_DIR}cropper/GenericPhoneRB.jpg); background-repeat:no-repeat;" />
                            </tr>
                        </table>
                    </div>
                </td>
            </tr> <!-- /id='cropperRow' -->
        </table> <!-- /id='cropperArea' -->

        <div id="title">
			<span>Title (30 characters maximum)</span><br />
        	<input type="text" id="mobilizeTitle" name="mobilizeTitle" class="mobilizeTitle" value="{$mobilizeTitle}" onkeypress="return preventEnterKey(event)" />
        </div>
		
		{if $CannotDetectCarrier && !$myPhone}
            {include file="EditorBase_email.html.tpl"}
        {else}
            <input type="hidden" name="Recipients" value="{$Recipients}" />
        {/if}
		
		{if !$user->isAuthenticated() && $myPhone}
			{include file="EditorBase_select_phone.html.tpl" formatMess="picture for your phone's screen"}
			{assign var="stepCount" value="3"}
			<br />
		{/if}
        
        {if $user->isAuthenticated() && $myPhone}
			<!-- TAGS | PRIVACY | AVATAR -->
			<input type="hidden" name="mobilizeTags" id="mobilizeTags" class="mobilizeTags" value="{$mobilizeTags}" onkeypress="return preventEnterKey(event)"/>
			<input type="hidden" name="mobilizePrivacy" value="1" checked="checked" id="mobilizePrivacyPub">
			<input type="hidden" name="mobilizePrivacy" value="0" id="mobilizePrivacyPri">
			<input type="hidden" name="mobilizeAvatar" id="mobilizeAvatar" value="0">
        {elseif $showCaptcha}
            <div id="captcha" class="peekabooBugFix">
                <p>Type the code below (to prevent spamming)</p>
                <img src="{genurl module="Misc" action="CaptchaImage"}" /><br />
				{if $CaptchaError}
                    <span style="color: red;">{$CaptchaError}</span><br />
                {/if}
				<input type="text" id="mobilizeCaptcha" name="captchaValue" class="mobilizeTitle" />
            </div>
        {/if}
		
		{if $user->isAuthenticated()}
			{if $myPhone}
				<!-- <p>This picture will be added to your locker.</p> -->
				<div id="sendNoticeRow" style="text-align: center;">
					<label for="SendNotice"><input type="checkbox" name="DontSendNotice" id="SendNotice" value="true" />Just send it to my Mixxer locker<br />(don't send me a message)</label>
				</div>
			{else}
				<!-- <p>This picture will be sent to your friend.</p> -->
			{/if}
        {/if}
        
        <div id="finishCancel">
            <button style="padding:2px 5px;" class="globalBtn burg" type="submit">Finish</button>
        </div>
		
        <input type="hidden" name="EditOptions[idCropIX]" id="idCropIX" value="0" />
        <input type="hidden" name="EditOptions[idCropIY]" id="idCropIY" value="0" />
        <input type="hidden" name="EditOptions[idCropDX]" id="idCropDX" value="0" />
        <input type="hidden" name="EditOptions[idCropDY]" id="idCropDY" value="0" />
        <input type="hidden" name="EditOptions[idSizeDX]" id="idSizeDX" value="0" />
        <input type="hidden" name="EditOptions[idSizeDY]" id="idSizeDY" value="0" />
        <input type="hidden" name="EditOptions[idCropOccur]" id="idCropOccur" value="0" />
        <input type="hidden" name="EditPassThru" value="true" />
        <input type="hidden" name="Filename" value="{$Filename}" />
        <input type="hidden" name="{$InputType}" value="{$InputValue}" />
        <input type="hidden" name="InputMethod" value="{$InputMethod}" />
        <input type="hidden" name="Affiliate" value="{$Affiliate}" />
        <input type="hidden" name="SourceUrl" value="{$SourceUrl}" />
        <input type="hidden" name="agreeTC" value="{$agreeTC}" />
        <input type="hidden" name="fromPhone" value="{$fromPhone}" />
        <input type="hidden" name="toPhone" value="{$toPhone}" />
        <input type="hidden" name="fromName" value="{$fromName}" />
        </form>
        
    </div> <!-- /class='content' -->
</div>

<!-- END PictureEditorView_index_base.async.tpl -->
