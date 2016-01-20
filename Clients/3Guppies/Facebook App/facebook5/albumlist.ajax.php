<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines some of your basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';
	//include_once 'dump.php';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();
	$userid = (int)Param('user', $user);
	$name = GetUserInfo($userid, 'name');
	$aid = Param('aid');

	IncludeAlbumsInDropDown($userid, $name, $aid);
?>
