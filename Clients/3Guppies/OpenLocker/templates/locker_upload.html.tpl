<!-- BEGIN locker_upload.html.tpl : Imported by MobilizeView_input.html.tpl -->
		<div class="peekabooBugFix">
			{if $user->isAuthenticated()}
				<h3 style="padding-left: 0;">Upload To Your Locker And Cell Phone!</h3>
				<p style="margin-left: 0;">Upload files to your Locker, where you can store them and easily get them on your phone.<br />
			{else}
				<h3 style="padding-left: 0;">Upload To Your Cell Phone!</h3>
				<p style="margin-left: 0;">Upload a file to your cell phone.<br />
			{/if}
			Browse to the file you want and hit Upload.</p>
		</div>
		<br />
		<br />
		<form style="margin: 0; padding: 0;" action="{genurl module="OpenLocker" action="Mobilize"}" method="post" enctype="multipart/form-data" name="uploadForm" onsubmit="return submitUpload(this, 'submitBtn')">
			<input type="hidden" name="MAX_FILE_SIZE" value="57671680">
			<input id="userfile" name="userfile" type="file" style="margin: 0; padding: 2px; height: 20px;">
			<button class="globalBtn burg" type="submit" id="mobilizeSubmit" name="submitBtn" style="margin: 0 2px;">Upload</button>
			<br />
				{if !$IsAnonymous}
				<br />
				<input type="checkbox" style="float: left; margin: 0; padding: 0;" name="EditOptions[Global.NoFormat]">
				<div class="floatLeft">
					<p style="margin-bottom: 0;">Keep original file format.</p>
					<blockquote class="greyText smaller" style="margin: 0 0 0 18px;">* Note: the original file format may not be compatible with your phone.</blockquote>
				</div>
				<div class="clear"></div>
				{/if}
		</form>
		
		<br />
		<br />
		
		<p style="margin: 0;">You can upload these file types :</p>
			<ul style="list-style-type: none; margin: 3px 0 0 10px; padding: 0;">
				<li><strong>Ringtones</strong>: mp3, wma, m4a/aac, mid, smaf, mel, qcp, awb</li>
				<li><strong>Pictures</strong>: gif, jpg, png, bmp</li>
				<li><strong>Videos</strong>: flv, asf, wmv, avi, mov, mp4, mpg, qt, 3gp</li>
			</ul>
		
		<div id="olUpload_wait" style="display:none">
			<div class="waitBox" style="margin: 10px 0;">
				<img src="{$IMAGES_DIR}ui/groups/loading.gif"/> Uploading Your File...
			</div>
		</div>
		
		<div id="olUpload_error" style="display:none;">
			<div class="waitBox">
				You must select a file &nbsp;<button class="globalBtn burg" onclick="document.getElementById('olUpload_error').style.display = 'none';">Ok</button>
			</div>
		</div>
<!-- END locker_upload.html.tpl -->
