<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines the basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';
	include_once 'dump.php';

	$pageName = 'Send ';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();

	$to = (int)Param('user');
	if (!$to)
		$to = (int)Param('id'); // From the invite form

	$nameRecipient = GetUserInfo($to, 'first_name');

	$inviteSelect = Param('inviteSelect'); // From the invite form
	if (!$to && $inviteSelect)
	{
		foreach ($inviteSelect as $invitee)
		{
			$userid = (int)$invitee;
			if (!$to)
				$to = $userid;
			else
				$to .= ',' . $userid;

			$firstName = GetUserInfo($userid, 'first_name');
			if (!$nameRecipient)
				$nameRecipient = $firstName;
			else
				$nameRecipient .= ', ' . $firstName;

//j			dump($_REQUEST);
//j			dump($inviteSelect);
//j			echo 'InviteeName ' . $firstName . '<br/>';
		}
	}

	$bMiniFeed = ParamIsSet('minifeed');
	$bNewsFeed = ParamIsSet('newsfeed');
	$bInvitation = ParamIsSet('invitation');
	$bRequest = ParamIsSet('request');
	$bNotification = ParamIsSet('notification');
	$bGift = ParamIsSet('gift');
	$bEmail = ParamIsSet('email');

	$command = '';
	$bOK = false;
	if ($bMiniFeed)
	{
		$command = 'a Mini-Feed story';
		$pageName .= $command;
		$nameRecipient = 'yourself';
		$title = ' posted this Mini-Feed story with <a href="' . $canvasPageUrl . 'mobilize.php">3Guppies Mobilizer</a>.';
		$body = 'These stories can be up to 200 characters long, plus markup.  The titles can only be 60 characters long, plus markup.';
		$bOK = SendMiniFeed($title, $body);
	}
	else
	if ($bNewsFeed)
	{
		$command = 'a News Feed story';
		$pageName .= $command;
		$nameRecipient = 'yourself';
		$title = ' This News Feed story was posted by <a href="' . $canvasPageUrl . 'mobilize.php">3Guppies Mobilizer</a>.';
		$body = 'These stories can be up to 200 characters long, plus markup.  The titles can only be 60 characters long, plus markup.';
		$bOK = SendNewsFeed($title, $body);
	}
	else
	if ($bNotification)
	{
		$command = 'a notification';
		if ($bEmail)
			$command .= ' with an email';
		$pageName .= $command;

		$body = ' wants you to know that you can "mobilize" your Facebook photos with the 3Guppies Mobilizer.  Add the <a href="' . $canvasPageUrl . 'mobilize.php">3Guppies Mobilizer</a> to your Facebook profile so you don\'t miss out.';
		$emailbody = ($bEmail ? $body : null);
		$bOK = SendNotification($to, $body, $emailbody);
	}
	else
	if ($bInvitation || $bRequest)
	{
		$command = ($bInvitation ? 'an invitation' : 'a request');
		$pageName .= $command;
		$bOK = SendStandardInvitation($to, $bInvitation);
	}
	else
	if ($bGift)
	{
		$command = 'a gift';
		$pageName .= $command;
		$bOK = SendStandardGift($user, $to);
	}
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<title></title>
	<?php include('fbstyles.part.htm'); ?>
	<style type="text/css"><?php include('styles.css'); ?></style>
	<script type="text/javascript"><?php include('fbscript.js'); ?></script>
</head>
<body class="fbframe">
	<?php include('header.part.php'); ?>

	<div style="padding: 10px; font-family: Verdana, Arial; font-size: 10px;">
			<br/>
	<?php if ($bOK): ?>
		<h3>
		You just posted <?= $command ?> to <?=$nameRecipient?>.
		</h3>
	<?php else: ?>
		<h3>
		We were unable to post <?= $command ?> to <?=$nameRecipient?>.<br/><br/>
		This is probably due to the fact that Facebook only allows 10 to be sent in a 24 hour period.<br/><br/>
		Please try again later.
		</h3>
	<?php endif; ?>
	</div>
	<div style="position: absolute; bottom: 0;">
		<?php include('footer.part.php'); ?>
	</div>

</body>
</html>
