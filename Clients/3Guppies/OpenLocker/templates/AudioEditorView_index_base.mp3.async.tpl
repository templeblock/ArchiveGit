<!-- START AudioEditorView_index_base.mp3.async.tpl -->
{assign var="stepCount" value="1"}

<div class="moduleWide">
	<div class="moduleBody" id="mobilizeContainer">
		<form action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm">
			<input type="hidden" id="idClipStart" name="EditOptions[Ringtone.Start]" value="0" />
			<input type="hidden" id="idClipEnd" name="EditOptions[Ringtone.End]" value="0" />
			<input type="hidden" name="EditOptions[Ringtone.TimeFormat]" value="milliseconds" />

            {if !$user->isAuthenticated()}
                <div id="cropperInstructions" class="stepWrapper">
                    <!-- <img class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount++}.gif" /> -->
                    <h3>Click and drag handles to clip.</h3>
                </div>
            {else}
                <div id="cropperInstructions" class="stepWrapper">
                    <h3>Click and drag handles to clip.</h3>
                </div>
            {/if}
            
            
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
				<strong>File : </strong> {$ContentInfo.Filename}<br />
				<br />
			</div>
			<div id="idClipInfoFlashOK" style="display:none;">
			<!--
				<strong>Title : </strong> {$ContentInfo.Filename}<br />
				<strong>Clip Length : </strong><span id="idClipLength">Loading...</span><br />
				<strong>Play Position : </strong><span id="idClipPlayPosition"></span><br />
				-->
				<!--<strong>Meta Data : </strong><span id="idClipMetaData"></span><br />-->
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

			<table id="cropWrapper" cellpadding="5" cellspacing="0" style="width:530px; height:130px; line-height: 150%;">
				<tr>
					<td>
						<div id="idClipDisplayContainer" style="width:530px; height:130px;"></div>
					</td>
				</tr>
            </table>
            
            <div id="title">
                Title (30 characters maximum)<br />
                <input type="text" id="mobilizeTitle" name="mobilizeTitle" class="mobilizeTitle" value="{$mobilizeTitle}" onkeypress="return preventEnterKey(event)"/>
            </div>
			
			{if !$user->isAuthenticated() && $myPhone}
				{include file="EditorBase_select_phone.html.tpl"  formatMess='audio for your phone'}
				<br />
				{assign var='stepCount' value='3'}
			{else}
				{assign var='stepCount' value='2'}
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
			
			
			{if $user->isAuthenticated() && $myPhone}
                <div id="sendNoticeWrapper" style="text-align: center;">
                    <label for="SendNotice"><input type="checkbox" name="DontSendNotice" id="SendNotice" value="true" />Just send it to my Mixxer locker<br />(don't send me a message)</label>
                </div>
            {/if}

            <div id="finishCancel">
                <button id="finishButton" style="padding:2px 5px;" class="globalBtn burg" type="submit">Finish</button>
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

<!-- END AudioEditorView_index_base.mp3.async.tpl -->
