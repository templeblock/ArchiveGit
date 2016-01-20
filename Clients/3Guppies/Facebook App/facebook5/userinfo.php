<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines the basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';
	include_once 'dump.php';

	$pageName = 'Get More Info';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();
	$userid = (int)Param('user');
	if (!$userid)
		$userid = (int)Param('id', $user); // From the profile-action link

	$fields = array(
		'first_name',			'status',				'hometown_location',	'current_location',
		'hs_info',				'meeting_sex',			'meeting_for',			'affiliations',
		'education_history',	'work_history',			'sex',					'is_app_user',
		'relationship_status',	'significant_other_id',	'birthday',				'political',
		'religion',				'activities',			'interests',			'music',
		'tv',					'movies',				'books',				'quotes',
		'about_me',				'timezone',				'notes_count',			'wall_count',
		'profile_update_time');

	$userinfo = GetUserInfoEx($userid, $fields);
	$firstName = $userinfo['first_name'];

	$statusMessage = '';
	$status = $userinfo['status'];
	if ($status)
		$statusMessage = $status['message'] . ' (as of ' . StdDate((int)$status['time']) . ')';

	$hometown = '';
	$hometown_location = $userinfo['hometown_location'];
	if ($hometown_location)
		$hometown = $hometown_location['city'] . ', ' . $hometown_location['state'] . ' ' . $hometown_location['zip'] . ' ' . $hometown_location['country'];
	
	$current = '';
	$current_location = $userinfo['current_location'];
	if ($current_location)
		$current = $current_location['city'] . ', ' . $current_location['state'] . ' ' . $current_location['zip'] . ' ' . $current_location['country'];

	$high_school = '';
	$hs_info = $userinfo['hs_info'];
	if ($hs_info)
		$high_school = $hs_info['hs1_name'] . ', ' . $hs_info['grad_year'];

	$meeting_sex = null;
	$arrMeetingSex = $userinfo['meeting_sex'];
	foreach ($arrMeetingSex as $sex)
	{
		if ($meeting_sex)
			$meeting_sex .= ', ';
		$meeting_sex .= $sex;
	}

	$meeting_for = null;
	$arrMeetingFor = $userinfo['meeting_for'];
	foreach ($arrMeetingFor as $for)
	{
		if ($meeting_for)
			$meeting_for .= ', ';
		$meeting_for .= $for;
	}

	$networks = null;
	$affiliations = $userinfo['affiliations'];
	foreach ($affiliations as $affiliation)
	{
		if ($networks)
			$networks .= '; ';
		$networks .= $affiliation['name'];
		$status = $affiliation['status'];
		if ($status)
			$networks .= ' ' . substr($status, 0, 4);
		$year = $affiliation['year'];
		if ($year)
			$networks .= ' ' . $year;
	}

	$education = null;
	$schools = $userinfo['education_history'];
	foreach ($schools as $school)
	{
		if ($education)
			$education .= '; ';
		$education .= $school['name'];
		$year = $school['year'];
		if ($year)
			$education .= ' ' . $year;
		$concentrations = $school['concentrations'];
		foreach ($concentrations as $concentration)
			$education .= ', ' . $concentration;
	}

	$work_history = null;
	$jobs = $userinfo['work_history'];
	foreach ($jobs as $job)
	{
		if ($work_history)
			$work_history .= '; ';
		$work_history .= $job['company_name'];
		$position = $job['position'];
		if ($position)
			$work_history .= ' ' . $position;
		$description = $job['description'];
		if ($description)
			$work_history .= ' (' . $description . ')';
		$location = $job['location'];
		if ($location['city'] || $location['state'])
			$work_history .= ',';
		if ($location['city'])
			$work_history .= ' ' . $location['city'];
		if ($location['state'])
			$work_history .= ' ' . $location['state'];
	}

//j	dump(GetGroupMembersArray($userid), false);
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
		<?php IncludeUsers(array($userid), 'friends.part.php'); ?>
		<?php //j IncludeUsers(GetGroupMembersArray($userid), 'friends.part.php'); ?>
		<hr/>
	</div>

	<div style="padding: 10px; font-family: Verdana, Arial; font-size: 10px;">
		Your session key is '<?= $facebook->api_client->session_key;	?>'	<br/>
		in_frame			'<?= $facebook->in_frame();					?>'	<br/>
		in_fb_canvas		'<?= $facebook->in_fb_canvas();				?>'	<br/>
		current_url			'<?= $facebook->current_url();				?>'	<br/>
		get_facebook_url	'<?= $facebook->get_facebook_url();			?>'	<br/>
		get_install_url		'<?= $facebook->get_install_url();			?>'	<br/>
		get_add_url			'<?= $facebook->get_add_url();				?>'	<br/>
		get_login_url		'<?= $facebook->get_login_url(null, null);	?>'	<br/>
		These are your most recent Facebook notifications: <?= dump($facebook->api_client->notifications_get(), true); ?><br/>

		<h3>Here is some more information about <?= $firstName; ?>:</h3>
		<?= $firstName ?>'s Status: <?= $statusMessage; ?><br/>
			<br/>
		<?= $firstName ?> belongs to the following groups:<br/><br/>
		<?= GetNameArrayAsList(GetGroupsNameArray($userid)); ?>
			<br/>

		<?= $firstName ?> is an appuser: <?= $userinfo['is_app_user']; ?><br/><br/>
		<?= $firstName ?>'s Profile Update Time: <?= $userinfo['profile_update_time']; ?><br/><br/>
		<?= $firstName ?>'s Sex: <?= $userinfo['sex']; ?><br/><br/>
		<?= $firstName ?> is interested in: <?= $meeting_sex; ?><br/><br/>
		<?= $firstName ?>'s Relationship Status: <?= $userinfo['relationship_status']; ?><br/><br/>
		<?= $firstName ?>'s Significant Other: <?= $userinfo['significant_other_id']; ?><br/><br/>
		<?= $firstName ?> is looking for: <?= $meeting_for; ?><br/><br/>
		<?= $firstName ?>'s Birthday: <?= $userinfo['birthday']; ?><br/><br/>
		<?= $firstName ?>'s Political Views: <?= $userinfo['political']; ?><br/><br/>
		<?= $firstName ?>'s Religious Views: <?= $userinfo['religion']; ?><br/><br/>
		<?= $firstName ?>'s Hometown Location: <?= $hometown; ?><br/><br/>
		<?= $firstName ?>'s Current Location: <?= $current; ?><br/><br/>
		<?= $firstName ?>'s Activities: <?= $userinfo['activities']; ?><br/><br/>
		<?= $firstName ?>'s Interests: <?= $userinfo['interests']; ?><br/><br/>
		<?= $firstName ?>'s High School: <?= $high_school; ?><br/><br/>
		<?= $firstName ?>'s Favorite Music: <?= $userinfo['music']; ?><br/><br/>
		<?= $firstName ?>'s Favorite TV Shows: <?= $userinfo['tv']; ?><br/><br/>
		<?= $firstName ?>'s Favorite Movies: <?= $userinfo['movies']; ?><br/><br/>
		<?= $firstName ?>'s Favorite Books: <?= $userinfo['books']; ?><br/><br/>
		<?= $firstName ?>'s Favorite Quotes: <?= $userinfo['quotes']; ?><br/><br/>
		<?= $firstName ?>'s About Me section: <?= $userinfo['about_me']; ?><br/><br/>
		<?= $firstName ?>'s Networks: <?= $networks; ?><br/><br/>
		<?= $firstName ?>'s Education History: <?= $education; ?><br/><br/>
		<?= $firstName ?>'s Work History: <?= $work_history; ?><br/><br/>
		<?= $firstName ?>'s Timezone: <?= $userinfo['timezone']; ?><br/><br/>
		<?= $firstName ?>'s Notes Count: <?= $userinfo['notes_count']; ?><br/><br/>
		<?= $firstName ?>'s Wall Count: <?= $userinfo['wall_count']; ?><br/><br/>
	</div>

	<?= dump($userinfo, true); ?>
	<?= dump(GetGroupMembersNameArray($userid), true); ?>

	<div style="position: relative; bottom: 0;">
		<?php include('footer.part.php'); ?>
	</div>
</body>
</html>
