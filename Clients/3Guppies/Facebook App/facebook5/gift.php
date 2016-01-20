<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines some of your basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';

	$pageName = 'Send a Gift';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();
	$userid = (int)Param('user', $user);
	$firstName = GetUserInfo($userid, 'first_name');
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<title></title>
	<?php include('fbstyles.part.htm'); ?>
	<style type="text/css"><?php include('styles.css'); ?></style>
	<script type="text/javascript"><?php include('fbscript.js'); ?></script>
</head>
<body class="fbframe">
	<?php include('header.part.php'); ?>

	<div id="friendtables">
		<?php IncludeUsers(array($userid), 'friends.part.php'); ?>
		<hr/>
	</div>

	<div style="padding: 10px; font-family: Verdana, Arial; font-size: 10px;">
			<br/>
		<h3>Send the Gift to <?= $firstName ?>:</h3><br/>
			<br/>
		<table>
		<tr>
		<td><img src="<?= $appHomeUrl ?>gift.jpg" border="1" alt="" /></td>
		<td><?= GetUserLink('send.php?gift&', $userid, $firstName); ?></td>
		</tr>
		</table>
	</div>

	<div style="position: absolute; bottom: 0;">
		<?php include('footer.part.php'); ?>
	</div>
</body>
</html>
