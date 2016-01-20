<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines some of your basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';

//j	$facebook = new Facebook($api_key, $secret);
//j	$user = $facebook->get_loggedin_user();
//j	$userid = (int)Param('fb_sig_user', $user);
//j	uninstall_user($userid);

	$userid = (int)Param('fb_sig_user', $user);
	LogLine('Removed $user');
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<title></title>
</head>
<body>
	<div style="padding: 10px; font-family: Verdana, Arial; font-size: 10px;">
		This is the 3Guppies Mobilizer Post Remove page.<br/>
	</div>
</body>
</html>
