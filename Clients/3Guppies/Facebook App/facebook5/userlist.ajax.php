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
	$counts = (int)Param('counts', 0);
	$bAddPhotoCount = ($counts > 0 ? true : false);

	IncludeUsersInDropDown(GetFriendsArray(), $bAddPhotoCount, $userid);
?>
