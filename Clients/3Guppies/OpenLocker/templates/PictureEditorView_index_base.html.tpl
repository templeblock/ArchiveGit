<!-- START PictureEditorView_index_base.html.tpl -->
{addheaderfile file=$URL_BASE|cat:"css/openlocker/locker.css"}
{assign var="stepCount" value="1"}

<div class="moduleWide peekabooBugFix fileFormat">
	<div class="moduleHead userPanel"><h2>Crop This Picture</h2></div>
	<div class="moduleBody white" id="mobilizeContainer">
		<form id="uploadForm" action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm" style="margin: 10px;">
        {if $CannotDetectCarrier && !$myPhone}
            {include file="EditorBase_email.html.tpl"}
        {else}
            <input type="hidden" name="Recipients" value="{$Recipients}" />
        {/if}
 
        {if $user->isAuthenticated()}
		    {if $myPhone}
            <div id="sendNoticeRow" style="margin: 10px 0 20px 0;">
                <label for="SendNotice"><input type="checkbox" name="DontSendNotice" id="SendNotice" value="true" style="margin: 0 5px -1px 0;" />Just send it to my Mixxer locker (don't send me a message)</label>
            </div>
			{/if}
			
			<div id="cropperInstructions" class="stepWrapper">
                {if $NoCroppingAllowed}
                <h3>We're sorry, animated gifs cannot be cropped at this time.</h3>
                {else}
                <h3>Click and drag handles to crop.</h3>
                {/if}
            </div>
		
		<br />
        {else}
			{if !$user->isAuthenticated() && $myPhone}
				{include file="EditorBase_select_phone.html.tpl" formatMess="picture for your phone's screen"}
				{assign var="stepCount" value="3"}
				<br /><br />
			{/if}
            <div id="cropperInstructions" class="stepWrapper">
                {if $NoCroppingAllowed}
                <h3>We're sorry, animated gifs cannot be cropped at this time.</h3>
                {else}
                <!-- <img class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount++}.gif" /> -->
                <h3>Click and drag handles to crop.</h3>
                {/if}
            </div>
        {/if}

        <table id="cropperArea">
            <tr id="cropperRow">
                <td style="width:400px; height:300px;">
                    <div id="idCropDisplayContainer" style="margin-right:10px;">
                        <img id="idCropDisplay" src="{$IMAGES_DIR}cropper/Dummy.jpg" alt="" />
                    </div>
                </td>
                <td style="vertical-align:top; text-align:center;">
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
                <p>Title (30 characters maximum)</p>
                <input size="30" type="text" id="mobilizeTitle" name="mobilizeTitle" class="mobilizeTitle" value="{$mobilizeTitle}" onkeypress="charCount(30,'mobilizeTitle','mobilizeTitleMsg');return preventEnterKey(event)" onkeyup="charCount(30,'mobilizeTitle','mobilizeTitleMsg');" onblur="hideCharCount('mobilizeTitleMsg');"/>
                <div id="mobilizeTitleMsg">&#160;</div>
        </div>
        
        {if $user->isAuthenticated() && $myPhone}
            <div id="tagsPrivacyAvatar">
                <div>
                    <strong>Tags</strong><br />
                    <input size="30" type="text" id="mobilizeTags" name="mobilizeTags" class="mobilizeTags" value="{$mobilizeTags}" onkeypress="return preventEnterKey(event)"/>
                </div>
                <div style="margin: 5px 0;">
                    <strong>Privacy</strong> (you can change your settings at any time)<br />
                    <label for="mobilizePrivacyPub"><input type="radio" name="mobilizePrivacy" value="1" checked="checked" id="mobilizePrivacyPub"> <img src="{$IMAGES_DIR}ui/icons/icon_public_mono.gif" /> This content is public and shared with the Mixxer community.</label><br />
                    <label for="mobilizePrivacyPri"><input type="radio" name="mobilizePrivacy" value="0" id="mobilizePrivacyPri"> <img src="{$IMAGES_DIR}ui/icons/icon_private_mono.gif" /> This content is private and can only be viewed by me.</label>
                </div>
                <div>
                    <hr style="margin: 8px 0;" />
                    <label for="mobilizeAvatar"><input type="checkbox" name="mobilizeAvatar" id="mobilizeAvatar" value="1"> <img src="{$IMAGES_DIR}ui/icons/icon_avatar_mono.gif" /> Make this file my Mixxer avatar.</label>
                </div>
            </div>
        {elseif $showCaptcha}
            <div id="captcha">
				<p>Type the code below (to prevent spamming)</p>
                <img src="{genurl module="Misc" action="CaptchaImage"}" /><br />
                {if $CaptchaError}
                    <span style="color: red;">{$CaptchaError}</span><br />
                {/if}
                <input type="text" id="mobilizeCaptcha" name="captchaValue" class="mobilizeTitle" />
            </div>
        {/if}
        
        <div id="finishCancel">
            <button style="padding:2px 5px;" class="globalBtn burg" type="submit"{if $CannotDetectCarrier && (!$carrierId || $carrierId < 0)} onclick="if(document.getElementById('carrierSelect').value == -1){ldelim}alert('Please select a carrier'); return false{rdelim}"{/if}>Finish</button>
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

<!-- END PictureEditorView_index_base.html.tpl -->
