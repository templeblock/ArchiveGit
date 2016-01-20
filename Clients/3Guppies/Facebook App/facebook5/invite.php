<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines the basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';

	include_once 'dump.php';

	$pageName = 'Invite Friends';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();
	$userid = (int)Param('user');
	if (!$userid)
		$userid = (int)Param('id', $user); // From the profile-action link
	$firstName = GetUserInfo($userid, 'first_name');
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<title></title>
	<?php include('fbstyles.part.htm'); ?>
	<style type="text/css"><?php include('styles.css'); ?></style>
	<script type="text/javascript"><?php include('fbscript.js'); ?></script>
	<script type="text/javascript">
		function SelectAll(form_id, name, bChecked)
		{
			var formblock = document.getElementById(form_id);
			var forminputs = formblock.getElementsByTagName('input');
			var iCount = (bChecked ?  Math.min(10, forminputs.length) : forminputs.length);
			for (i = 0; i < iCount; i++)
			{
				// regex here to check name attribute
				var regex = new RegExp(name, "i");
				if (regex.test(forminputs[i].getAttribute('name')))
					forminputs[i].checked = bChecked;
			}
		}
		function ConfirmSubmit(form_id)
		{
			var formblock = document.getElementById(form_id);
			var forminputs = formblock.getElementsByTagName('input');
			var iCount = 0;
			for (i = 0; i < forminputs.length; i++)
			{
				if (forminputs[i].checked)
					iCount++;
			}

			if (iCount > 10)
			{
				var sMessage = 'You can only invite 10 friends per day, and you have selected ' + iCount + ' names.\n\nPlease unselect ' + (iCount-10) + ' names and try again.';
				alert(sMessage);
				return false;
			}

			return true;
		}
	</script>
</head>
<body class="fbframe">
	<?php include('header.part.php'); ?>

	<div>
		<div style="padding:10px;">
			<h2>Friends who have added the 3Guppies Mobilizer</h2><br/>
			<?php IncludeUsersToInvite(GetFriendsArray('-with'), 'invite.part.php'); ?>
		</div>
	</div>
	<div style="clear:left;" />
	<br/>
	<br/>
	<form id="form_id" method="get" target="_parent" onsubmit=" return ConfirmSubmit('form_id')" action="<?= $canvasPageUrl ?>send.php">
		<div style="padding:10px;">
			<input type="submit" oncick="" style="cursor:hand; width:20em;" value="Send Invitations to friends selected below" class="inputsubmit" />
			&nbsp; (Limit 10 per day)<br/>
			<br/>
			<a href="#" onclick="SelectAll('form_id', 'inviteSelect', true);">Select 10</a>
			&nbsp;&nbsp;|&nbsp;&nbsp;
			<a href="#" onclick="SelectAll('form_id', 'inviteSelect', false);">Unselect All</a>
		</div>
		<div style="padding:10px;">
			<?php IncludeUsersToInvite(GetFriendsArray('-without'), 'invite.part.php'); ?>
		</div>
		<div style="clear:left;" />
		<input type="hidden" name="invitation" value="1" />
	</form>

	<!--
		Post An Invitation:<br/>
			<br/>
			<?= //GetUserLink('send.php?invitation&', $userid, $firstName); ?><br/>
			<br/>
		Post a simple Notification:<br/>
			<br/>
			<?= //GetUserLink('send.php?notification&', $userid, $firstName); ?><br/>
			<br/>
		Send a Notification, and include an Email:<br/>
			<br/>
			<?= //GetUserLink('send.php?notification&email&', $userid, $firstName); ?><br/>
		Post a mini-feed story to your profile:</br>
			<br/>
			<?= //GetLinkToUser('send.php?minifeed&', null, $firstName); ?><br/>
			<br/>
		Post a News Feed story to your profile:<br/>
			<br/>
			<?= //GetLinkToUser('send.php?newsfeed&', null, $firstName); ?><br/>
			<br/>
			<hr/>
	-->

	<div style="position: relative; bottom: 0;">
		<?php include('footer.part.php'); ?>
	</div>
</body>
</html>
