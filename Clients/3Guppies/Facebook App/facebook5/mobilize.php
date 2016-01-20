<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines the basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';
	include_once 'dump.php';

	$pageName = 'Mobilize Photos';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();
	$userid = (int)Param('user');
	if (!$userid)
		$userid = (int)Param('id', $user); // From the profile-action link

	$bJustAdded = ParamIsSet('added');
	if ($bJustAdded)
		LogLine('Added $user');

	$fbml  = '<fb:profile-action url="' . $canvasPageUrl . 'mobilize.php">3Guppies Mobilizer</fb:profile-action>';
//j	$profileboxurl = $appHomeUrl . 'profilebox.php';
//j	$facebook->api_client->fbml_refreshRefUrl($profileboxurl);
//j	$fbml .= '<fb:ref url="' . $profileboxurl . '" />';
	$facebook->api_client->profile_setFBML($fbml);

	$aid = Param('aid');

	$userinfo = GetUserInfoEx($userid, 'name, first_name, pic_big, is_app_user, sex');
	$name = $userinfo['name'];
	$firstName = $userinfo['first_name'];
	$picUrl = $userinfo['pic_big'];
	if (!$picUrl) $picUrl = $defaultPicUrl;
	$bIsAppUser = $userinfo['is_app_user'];
	$inviteOrAnnounce = ($bIsAppUser ? $firstName . ' uses 3Guppies' : GetLinkToUser('send.php?invitation&', $userid, 'Invite ' . $firstName . ' to add 3Guppies'));
	$sex = $userinfo['sex'];
	$hisHerTheir = ''; //j'their';
	if ($sex == 'male') $hisHerTheir = 'his';
	if ($sex == 'female') $hisHerTheir = 'her';
	$himHerThem = ''; //j 'them';
	if ($sex == 'male') $himHerThem = 'him';
	if ($sex == 'female') $himHerThem = 'her';
	$numPhotos = GetNumPhotos($userid);

	$activityUrl = $facebookUrl . 'minifeed.php?id=' . $userid . '&filter=23&app_id=3097070316';
	
//j	$= Param('crumbs');
//j	if (!$crumbsOrig)
//j		$crumbsOrig = $facebook->get_loggedin_user();
//j	if ($crumbsOrig)
//j		$crumbsOrig .= ',';
//j	$crumbs = $crumbsOrig . $userid;

//j	$facebook->require_login();
	$facebook->require_frame();
	$facebook->require_add();
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<title></title>
	<?php include('fbstyles.part.htm'); ?>
	<style type="text/css"><?php include('styles.css'); ?></style>
	<?php include('share.part.htm'); ?>
	<script type="text/javascript"><?php include('fbscript.js'); ?></script>
	<script type="text/javascript" src="http://yui.yahooapis.com/2.2.2/build/yahoo/yahoo-min.js"></script>
	<script type="text/javascript" src="http://yui.yahooapis.com/2.2.2/build/connection/connection-min.js"></script>
	<script type="text/javascript">
/* Unused Javascript for the PreviewImage functionality
		function PreviewImage(sUrl, sCaption, sAlbum)
		{
			var idPopup = 'idPreviewImage';
			var oPopup = document.getElementById(idPopup);
			if (!oPopup)
				return false;
				
			var bShow = (oPopup.style.display != 'block');
			if (bShow)
			{
				if (sUrl)
					document.getElementById('photoimg').src = sUrl;
				if (sCaption)
					document.getElementById('photocaption').innerText = sCaption;
				if (sAlbum)
					document.getElementById('photoalbum').innerText = 'From your album: ' + sAlbum;
				oPopup.style.display = 'block';
			}
			else
			{
				document.getElementById('photoimg').src = '';
				document.getElementById('photocaption').innerText = '';
				document.getElementById('photoalbum').innerText = '';
				oPopup.style.display = 'none';
			}
			
			return false;
		}
*/
		var oCurrentThumb = null;

		function SelectImage(oThumb, sUrl, sCaption)
		{
			oCurrentThumb = oThumb;
			var oImage = document.getElementById('mxr-image');
			if (oImage)
				oImage.src = sUrl;

			var oTitle = document.getElementById('mxr-title');
			if (oTitle)
				oTitle.value = sCaption;

			return false;
		}

		function NextImage()
		{
			if (!oCurrentThumb)
				oCurrentThumb = document.getElementById('mxr-thumb');
			else
				oCurrentThumb = oCurrentThumb.nextSibling;
			if (!oCurrentThumb)
				return;

			oCurrentThumb.onclick();
			return false;
		}

		function PreviousImage()
		{
			if (!oCurrentThumb)
				oCurrentThumb = document.getElementById('mxr-thumb');
			else
				oCurrentThumb = oCurrentThumb.previousSibling;
			if (!oCurrentThumb)
				return;

			oCurrentThumb.onclick();
			return false;
		}

		function SendToPhone(sMobilizeUrl)
		{
			var sUrl = '';
			var oImage = document.getElementById('mxr-image');
			if (oImage)
				sUrl = oImage.src;

			var sCaption = '';
			var oTitle = document.getElementById('mxr-title');
			if (oTitle)
				sCaption = oTitle.value;

			// Send the notification before going out to the mobilize page
			
			
			window.location.href = sMobilizeUrl + escape(sUrl) + '&caption=' + escape(sCaption); 
			return false;
		}

		function BodyOnload()
		{
			GetUserListFromServer(false);
			return false;
		}

		function GetUserListFromServer(bAddPhotoCount)
		{
			//alert('"' + bAddPhotoCount + '"');
			var sUrl = '<?= $appHomeUrl ?>userlist.ajax.php?user=<?= $userid ?>&counts=' + (bAddPhotoCount ? '1' : '0');
			var callbacks = {
				success: function(oResponse)
				{
					//alert(oResponse.responseText);
					//alert(oList.innerHTML);
					var oList = document.getElementById('userList');
					if (!oList)
						return;
					oList.disabled = false;
if (document.all)
	oResponse.responseText = '<option>truncatethis</option>' + oResponse.responseText;
					oList.innerHTML = oResponse.responseText;
if (document.all)
	oList.outerHTML = oList.outerHTML;

					if (!bAddPhotoCount)
						setTimeout('GetUserListFromServer(true)', 100);
				},
				failure: function(oResponse)
				{
					//alert('fail');
					var oList = document.getElementById('userList');
					if (!oList)
						return;
					oList.disabled = false;
					//alert('GetUserListFromServer(): AJAX failure: ' + 
					//	' tId=' + oResponse.tId +
					//	' status='+ oResponse.status +
					//	' statusText=' + oResponse.statusText
					//	);
				}
			}

			var transaction = YAHOO.util.Connect.asyncRequest('GET', sUrl, callbacks, null);
			return false;
		}

		function GetAlbumListFromServer(bAddPhotoCount)
		{
			var sUrl = '<?= $appHomeUrl ?>albumlist.ajax.php?user=<?= $userid ?>&aid=<?= $aid ?>';
			var callbacks = {
				success: function(oResponse)
				{
					var oList = document.getElementById('albumList');
					if (!oList)
						return;
					oList.disabled = false;
					oList.innerHTML = oResponse.responseText;
				},
				failure: function(oResponse)
				{
					if (!oList)
						return;
					oList.disabled = false;
					//alert('GetAlbumListFromServer(): AJAX failure: ' + 
					//	' tId=' + oResponse.tId +
					//	' status='+ oResponse.status +
					//	' statusText=' + oResponse.statusText
					//	);
				}
			}

			var transaction = YAHOO.util.Connect.asyncRequest('GET', sUrl, callbacks, null);
			return false;
		}
	</script>
	<style type="text/css">
		#preview {width: 236px; height: 250px; margin: 0 auto 1em auto;}
		#preview #mxr-previous, 
		#preview #mxr-next {float: left; width: 18px; padding-top: 95px;}
		#preview #mxr-previous a, 
		#preview #mxr-next a {display: block; height: 48px;}
		#preview #mxr-previous a:hover, 
		#preview #mxr-next a:hover {background-position: top center;}
		#preview #phoneBg {float: left; width: 200px; height: 250px; background-image: url(http://cdn.3guppies.com/images/widget/grphx-phone-blank-333.gif); background-repeat:no-repeat;}
		#preview #mxr-screen {width: 128px; height: 128px; margin: 52px 0 49px 35px; background-color: #333; border: 1px solid #000;}
		.screenCenter {width: 128px; height: 128px; text-align: center; vertical-align: middle; display: table-cell;}
		.screenCenter * {vertical-align: middle;}
		.screenCenter img {max-width: 128px; max-height: 128px; }
		/*\*//*/
		.screenCenter {display: block;}
		.screenCenter span {display: inline-block; height: 100%; width: 1px;}
		/**/

		#preview #title div {width: auto; padding: 0; margin-left: 50px; background-color: #7F7F7F; cursor: default; text-align: left; color: #FFF;}
		#preview #title input {border: 1px solid #4DB3D0; width: 130px; height: 16px; padding: 0; margin-left: 49px; margin-top: -2px; filter:alpha(opacity=50); opacity: 0.5; -moz-opacity:0.5;}
		#mxr-mobilize {cursor: pointer; cursor: hand;}

		.translucentPopup {filter: alpha(opacity=80); background-color: #eeeeee; display: none; left: 0px; top: 0px; width: 100%; height: 100%; position: absolute; color: navy; border: 1 #f77d30 solid; }

	</style>
	<!--[if IE]>
	<style type="text/css">
		.screenCenter span {display: inline-block;height: 100%;}
	</style>
	<![endif]-->
</head>
<body class="fbframe" onload="return BodyOnload();">
	<?php include('header.part.php'); ?>

	<div>
		<table border="0" cellspacing="0">
			<tr style="vertical-align:top; ">
				<td>
					<div id="preview">
						<div id="mxr-previous">
							<a href="javascript:// Click to view the previous photo" onclick="return PreviousImage();">
							<img onmouseover="this.src='http://cdn.3guppies.com/images/widget/paddle_Lb.gif';" onmouseout="this.src='http://cdn.3guppies.com/images/widget/paddle_La.gif';" src="http://cdn.3guppies.com/images/widget/paddle_La.gif" border="0">
							</a>
						</div>
						<div id="phoneBg">
							<div id="mxr-screen">
								<div class="screenCenter">
									<span></span>
									<img id="mxr-image" src="<?= $picUrl ?>" style="max-width: 128px; max-height: 128px;">
								</div>
							</div>
							<div id="title">
								<input class="textInput" id="mxr-title" value="">
							</div>
						</div>
						<div id="mxr-next">
							<a href="javascript:// Click to view the next photo" onclick="return NextImage();">
							<img onmouseover="this.src='http://cdn.3guppies.com/images/widget/paddle_Rb.gif';" onmouseout="this.src='http://cdn.3guppies.com/images/widget/paddle_Ra.gif';" src="http://cdn.3guppies.com/images/widget/paddle_Ra.gif" border="0">
							</a>
						</div>
						<div class="clear" style="clear:both;">
						</div>
					</div>
					<div style="text-align:center;">
						<a href="javascript:// Click to send the image to your phone" onclick="return SendToPhone('<?= $siteMobilizeUrl ?>');">
							<img id="mxr-mobilize" src="http://cdn.3guppies.com/images/widget/btn_sendToPhoneUp.gif" border="0" />
						</a>
					</div>
				</td>
				<td>
					<table border="0" cellspacing="0">
						<tr style="vertical-align:top; ">
							<td>
								<table style="width:100%;"><tr>
									<td style="width:200px;"><h1><?= $name ?></h1></td>
									<td style="text-align:right;">
										<?= $inviteOrAnnounce ?><br/>
										<?php if ($bIsAppUser): ?>
										<a target="_parent" href="<?= $activityUrl ?>">Show <?= $firstName ?>'s 3Guppies activity</a><br/>
										<?php endif; ?>
									</td>
								</tr></table>
								<br/>
								<br/>
							</td>
						</tr>
						<tr>
							<td>
								<table style="width:100%;"><tr>
									<td><h2>Now Viewing User:</h2></td>
									<td style="text-align:right;">
										<a target="_parent" href="<?= $canvasPageUrl ?>friends.php">Full Search of Your Friends</a><br/>
										<a target="_parent" href="<?= $canvasPageUrl ?>friendsofriends.php?user=<?= $userid ?>"><?= $firstName ?>'s Tagged Photo Friends</a><br/>
									</td>
								</tr></table>
								<br/>
								<form id="userForm" name="userForm" method="get" target="_parent" action="<?= $canvasPageUrl ?>mobilize.php">
									<select id="userList" name="user" style="width:320px;" onchange="document.getElementById('userForm').submit();">
										<?php IncludeUsersInDropDown(array($userid), false/*$bAddPhotoCount*/, $userid); ?>
									</select>
									<!--input type="submit" value="View" class="inputsubmit" /-->
								</form>
								<br/>
								<br/>
							</td>
						</tr>
						<tr>
							<td>
								<h2>Now Viewing Album:<br/></h2>
								<br/>
								<form id="albumForm" name="albumForm" method="get" target="_parent" action="<?= $canvasPageUrl ?>mobilize.php">
									<select id="albumList" name="aid" style="width:320px" onchange="document.getElementById('albumForm').submit();">
										<?php IncludeAlbumsInDropDown($userid, $name, $aid); ?>
									</select>
									<!--input type="submit" value="View" class="inputsubmit" /-->
									<input type="hidden" name="user" value="<?= $userid ?>" />
								</form>
								<br/>
								<br/>
							</td>
						</tr>
					</table>
				</td>
			</tr>
		</table>
	</div>

	<hr/>

	<div style="padding: 10px; font-family: Verdana, Arial; font-size: 10px;">
		<h3>Click to Mobilize</h3>
			<br/>
		<?= GetPhotosAsMobilizeLinks($userid, $aid); ?>
	</div>
	<div style="clear:left;" />
		<br/>
		<br/>
		<br/>

	<hr/>

	<span class="share_and_hide s_and_h_big" style="display:inline;">
		<a href="http://apps.facebook.com/guppies/mobilize.php" title="Send this to friends or post it on your profile." class="share" onclick="return share_click(this)" target="_blank">Share</a>
	</span>
	<span style="display:inline; vertical-align:middle;">
		&nbsp; this page, or leave a message on Facebook's <a target="_parent" href="http://www.facebook.com/board.php?uid=3097070316">3Guppies bulletin board</a>.<br/>
	</span>
	<br/>

	<div style="position: relative; bottom: 0;">
		<?php include('footer.part.php'); ?>
	</div>
	
<!-- Unused HTML for the PreviewImage functionality -->
	<div id="idPreviewImage" class="translucentPopup screenCenter" onclick="PreviewImage()">
		<span></span>
		<img id="photoimg" src="" style="max-width:630px; max-height:630px;" border="0" /> <!-- replace photoimg.src -->
		<div>
			<div>
				<span id="photocaption"></span> <!-- replace photocaption.innerText -->
			</div>
			<div>
				<span id="photoalbum"></span> <!-- replace photoalbum.innerText -->
			</div>
		</div>
	</div>

</body>
</html>
