{* using variables:
	$userid
	$name
	$firstName
	$aid
	$picUrl
	$bAdded
	$bIsAppUser
	$hisHerTheir
	$himHerThem
	$openlockerMobilizeUrl
	$usersAjaxUrl
	$thumbsAjaxUrl
	$albumsForDropDown
	$arrUsers[n].userid
	$arrUsers[n].name
	$arrUsers[n].selected
	$arrUsers[n].num_photos
*}

{addheaderfile file="http://yui.yahooapis.com/2.3.1/build/yahoo/yahoo-min.js"}
{addheaderfile file="http://yui.yahooapis.com/2.3.1/build/event/event-min.js"}
{addheaderfile file="http://yui.yahooapis.com/2.3.1/build/connection/connection-min.js"}
{addheaderfile file="modules/Facebook/javascript/ShareButton.js"}
{addheaderfile file="modules/Facebook/javascript/MobilizeView.js"}
{addheaderfile file="modules/Facebook/css/FacebookGlobal.css"}
{addheaderfile file="modules/Facebook/css/MobilizeView.css"}

<input type="hidden" id="param_openlockerMobilizeUrl" value="{$openlockerMobilizeUrl}" />
<input type="hidden" id="param_usersAjaxUrl" value="{$usersAjaxUrl}" />
<input type="hidden" id="param_thumbsAjaxUrl" value="{$thumbsAjaxUrl}" />
<input type="hidden" id="param_aid" value="{$aid}" />

<div id="FBmobilize" class="FBbody">
	<table border="0" cellspacing="0" cellpadding="0" border="0" width="620">
		<tr style="vertical-align:top; ">
			<td width="250">
				<div id="preview">
					<div id="mxr-previous">
						<a href="javascript:// Click to view the previous photo" onclick="return PreviousImage();">
						<img onmouseover="this.src='http://cdn.3guppies.com/images/widget/paddle_Lb.gif';" onmouseout="this.src='http://cdn.3guppies.com/images/widget/paddle_La.gif';" src="http://cdn.3guppies.com/images/widget/paddle_La.gif" title="Show the previous image" border="0" alt="" />
						</a>
					</div>
					<div id="phoneBg">
						<div id="mxr-screen">
							<div class="screenCenter">
								<span></span>
								<img onclick="PreviewImage()" id="mxr-image" src="{$picUrl}" style="max-width:128px; max-height:128px; width:expression(this.offsetWidth >= this.offsetHeight && this.offsetWidth > 128 ? 128 : true); height:expression(this.offsetHeight >= this.offsetWidth && this.offsetHeight > 128 ? 128 : true); " border="0" alt="" />
							</div>
						</div>
						<div id="title">
							<input class="textInput" id="mxr-title" value="">
						</div>
					</div>
					<div id="mxr-next">
						<a href="javascript:// Click to view the next photo" onclick="return NextImage();">
						<img onmouseover="this.src='http://cdn.3guppies.com/images/widget/paddle_Rb.gif';" onmouseout="this.src='http://cdn.3guppies.com/images/widget/paddle_Ra.gif';" src="http://cdn.3guppies.com/images/widget/paddle_Ra.gif" title="Show the next image" border="0" alt="" />
						</a>
					</div>
					<div class="clear" style="clear:both;">
					</div>
				</div>
				<div style="text-align:center;">
					<a href="javascript:// Click to send the image to your phone" onclick="return SendToPhone();">
						<img id="mxr-mobilize" src="http://cdn.3guppies.com/images/widget/btn_sendToPhoneUp.gif" onmouseover="this.src='http://cdn.3guppies.com/images/widget/btn_sendToPhoneDown.gif';" onmouseout="this.src='http://cdn.3guppies.com/images/widget/btn_sendToPhoneUp.gif';" border="0" alt="" />
					</a>
				</div>
			</td>
			<td width="370">
				<table border="0" cellspacing="0" cellpadding="0" border="0" width="100%" style="margin-left: 10px; margin-top: 16px;">
					<!-- User Info -->
					<tr style="vertical-align:top;">
						<td>
							<h1>{$name} </h1>
							{if $bIsAppUser}
								<a target="_parent" href="{$smarty.const.FACEBOOK_ACTIVITY_URL}{$userid}">View recent 3Guppies activity</a>
							{else}
								<a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Invite?user={$userid}">Invite to 3Guppies</a>
							{/if}
							</span>
							<br />
							<br />
							<br />
						</td>
					</tr>
					<!-- Choose Friend -->
					<tr>
						<td>
							<h4 style="float: left;">Choose a friend:</h4>
							<a style="float: right; margin-right: 50px;" target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Friends">Browse with Friend Search</a>
							<form style="clear: both;" id="userForm" name="userForm" method="get" target="_parent" action="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize">
								<select id="userList" name="user" style="width:320px;" onchange="document.getElementById('userForm').submit();">
									{foreach item=usr from=$arrUsers}
										<option value="{$usr.userid}" {$usr.selected}>{$usr.name}{$usr.num_photos}</option>
									{/foreach}
								</select>
								<!--input type="submit" value="View" class="inputsubmit" /-->
							</form>
							<br />
							<br />
						</td>
					</tr>
					<!-- Choose Album -->
					<tr>
						<td>
							<h4>Choose an album:</h4>
							<select id="albumList" name="aid" style="width:320px" onchange="GetThumbsFromServer(this.value/*aid*/);">
								{$albumsForDropDown}
							</select>
							<br />
							<br />
						</td>
					</tr>
					<!-- Thumbnails -->
					<tr>
						<td>
							<h4>Results:</h4>
							<div id="idThumbs">
								<br />
								<h1><em>Please wait...</em>&nbsp;&nbsp;&nbsp;&nbsp;<img src="{$IMAGES_DIR}facebook/progressbar.gif" border="0" /></h1>
							</div>
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
</div>

<br />

<div class="share_and_hide s_and_h_big" style="float: left; padding-left: 10px;">
	<a href="{$smarty.const.FACEBOOK_CALLBACK_URL}ShareTarget" title="Send this to friends or post it on your profile." class="share" onclick="return ShareButtonClick(this)" target="_blank">Share</a>
</div>

<div style="float: left;">	&nbsp; this page, or leave a message on Facebook's 
<a target="_parent" href="{$smarty.const.FACEBOOK_DISCUSSION_URL}">3Guppies discussion board</a>.
</div>

<br class="clear" />

{if $bAdded}
	<img src="http://www.rockyou.com/ams/dot.php?d=9e7c41632d72e584593d3174e660ce6c" />
{/if}
