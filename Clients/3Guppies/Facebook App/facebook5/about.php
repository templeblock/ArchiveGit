<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines some of your basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';

	$pageName = 'About 3Guppies Mobilizer';

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

	<iframe targetsrc="<?= $aboutPageUrl ?>" style="width:100%; height:90%;" 
		marginwidth="0" marginheight="0" hspace="0" vspace="0" frameborder="0" scrolling="auto" />

	<div style="position: relative; bottom: 0;">
		<?php include('footer.part.php'); ?>
	</div>
</body>
</html>
