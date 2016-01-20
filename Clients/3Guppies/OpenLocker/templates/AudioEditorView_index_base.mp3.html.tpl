<!-- START AudioEditorView_index_base.mp3.html.tpl -->
{assign var="stepCount" value="1"}

<div class="moduleWide fileFormat">
	<div class="moduleHead userPanel"><h2>Clip This Ringtone</h2></div>
	<div class="moduleBody white" id="mobilizeContainer" style="padding:0;">
		<form action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm">

			{if $user->isAuthenticated() && $myPhone}
                <div id="sendNoticeWrapper">
                    <label for="SendNotice"><input type="checkbox" name="DontSendNotice" id="SendNotice" value="true" style="margin: 0 5px -1px 0;" />Just send it to my Mixxer locker (don't send me a message)</label>
                </div>
            {/if}

			<br />

			<input type="hidden" id="idClipStart" name="EditOptions[Ringtone.Start]" value="0" />
			<input type="hidden" id="idClipEnd" name="EditOptions[Ringtone.End]" value="0" />
			<input type="hidden" name="EditOptions[Ringtone.TimeFormat]" value="milliseconds" />

            {if !$user->isAuthenticated()}
            
                {if $myPhone}
                    {include file="EditorBase_select_phone.html.tpl"  formatMess='audio for your phone'}
                    {assign var='stepCount' value='3'}
					<br />
                {else}
                    {assign var='stepCount' value='2'}
                {/if}
                <div id="cropperInstructions" class="stepWrapper">
                    <!-- <img class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount++}.gif" /> -->
                    <h3>Click and drag handles to clip.</h3>
                </div>
            {else}
                <div id="cropperInstructions" class="stepWrapper">
                    <h3>Click and drag handles to clip.</h3>
                </div>
            {/if}
			
			<br />
            
            
            <!-- This content of this container will be replaced with the Flash object, hosting PlayMP3Clip.swf -->
            <!-- Note: Javascript errors occur if the Flash object is inside a form, unless you add an extra/nested form tag -->
            <!-- This extra/nested form tag allows the Flash player to callback and find the Flash object within a form -->
			<div id="idPlayMP3ClipContainer" style="text-align:center;"></div>

			<div id="idClipInfoFlashNotOK" style="display:none;">
				<div style="text-align:center;">
					<br />
					<span id="idClipInfoOldFlash">
						Your Adobe Flash Player is not up to date.<br />
					</span>
					<span id="idClipInfoNoFlash">
						Adobe Flash Player is not installed on your system.<br />
					</span>
					You can clip your tone without Flash,<br />
					but you need the latest version to preview it before you finish.<br />
					<br />
					<a href="http://www.adobe.com/go/getflash/" target="_blank">Click here to get the latest version of Flash!</a><br />
					<br />
				</div>
				<b>File : </b> {$ContentInfo.Filename}<br />
				<br />
			</div>
			<div id="idClipInfoFlashOK" style="display:none;">
				<b>File : </b> {$ContentInfo.Filename}<br />
				<b>Clip Length : </b><span id="idClipLength">Loading...</span><br />
				<b>Play Position : </b><span id="idClipPlayPosition"></span><br />
				<!--<b>Meta Data : </b><span id="idClipMetaData"></span><br />-->
				<br />
				<table id="idControlsWrapper">
					<tr>
						<td valign="middle">
							<a id="idClipPlayFromStart" href="javascript:// Play the clip from the beginning" onclick="OnClipPlayFromStart();">
							<img src="http://svvscdn.mixxer.com/images/rt_clip/PlayFromStart.gif" border="0" alt="" /></a>
						</td>
						<td valign="middle">
							<a id="idClipPlay" href="javascript:// Play the clip" onclick="OnClipPlay();"><img src="http://svvscdn.mixxer.com/images/rt_clip/Play.gif" border="0" alt="" /></a>
						</td>
						<td valign="middle">
							<a id="idClipStop" href="javascript:// Stop the clip" onclick="OnClipStop();"><img src="http://svvscdn.mixxer.com/images/rt_clip/Stop.gif" border="0" alt="" /></a>
						</td>
						<td valign="middle">
							<input type="checkbox" id="idClipLoop" checked="checked" onclick="OnClipLoop(this);"/>&nbsp;<label for="idClipLoop">Loop</label>
						</td>
					</tr>
				</table>
			</div>

			<table id="cropWrapper" cellpadding="5" cellspacing="0" style="line-height: 150%;">
				<tr>
					<td>
						<div id="idClipDisplayContainer" style="width:530px; height:130px;" />
					</td>
				</tr>
            </table>
            
            <div id='title'>
                <p>Title (30 characters maximum)</p>
                <input size="30" type="text" id="mobilizeTitle" name="mobilizeTitle" class="mobilizeTitle" value="{$mobilizeTitle}" onkeypress="return preventEnterKey(event)" onkeyup="charCount(30,'mobilizeTitle','mobilizeTitleMsg');" onblur="hideCharCount('mobilizeTitleMsg');"/>
                <div id="mobilizeTitleMsg">&#160;</div>
            </div>
            
			{if $showCaptcha}
            
                <div id='captcha'>
                    <p>Type the code below (to prevent spamming)</p>
                    <img src="{genurl module="Misc" action="CaptchaImage"}" /><br />
                    <strong>Type Code Here</strong><br />
                    
                    {if $CaptchaError}
                        <span style="color: red;">{$CaptchaError}</span><br />
                    {/if}
                    
                    <input type="text" id="mobilizeCaptcha" name="captchaValue" class="mobilizeTitle" />
                </div>
                
            {/if}

            <div id="finishCancel">
                <button id="finishButton" style="padding:2px 5px;" class="globalBtn burg" type="submit"{if $CannotDetectCarrier && (!$carrierId || $carrierId < 0)} onclick="if(document.getElementById('carrierSelect').value == -1){ldelim}alert('Please select a carrier'); return false{rdelim}"{/if}>Finish</button>
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

<!-- END AudioEditorView_index_base.mp3.html.tpl -->
