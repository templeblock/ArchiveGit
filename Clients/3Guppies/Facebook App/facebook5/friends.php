<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines the basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';

	$pageName = 'Mobilize Your Friends Photos';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();
	$userid = (int)Param('user', $user);
	$userName = GetUserInfo($userid, 'first_name');
	
	$searchTerm = Param('searchTerm', null);
	$searchType = Param('searchType', $searchTerm);
	if (!$searchType)
		$searchType = '-updated'; // The default searchType
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

	<div id="friendtables">
		<?php
			$users = GetFriendsArray($searchType);
			include('friendsearch.part.php');
			IncludeUsers($users, 'friends.part.php');
		?>
	</div>

	<div style="position: relative; bottom: 0;">
		<?php include('footer.part.php'); ?>
	</div>
</body>
</html>
