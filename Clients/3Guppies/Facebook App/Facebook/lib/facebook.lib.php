<?php
	define('FACEBOOK_USE_FQLQUERY', true);
	include_once 'dump.php';

	define('FACEBOOK_VISITORS_LOCKDIR', '/tmp/FacebookVistorsLock');
	define('FACEBOOK_VISITORS_LOGFILE', '/tmp/FacebookVisitorsLog.txt');
	function LogLock($tries = 15)
	{ 
		$count = 1; 
		while (!mkdir(FACEBOOK_VISITORS_LOCKDIR, 0755))
		{
			sleep(1/*$seconds*/); 
			if (++$count > $tries)
				return false; 
		} 

		return true; 
	} 

	function LogUnlock()
	{
		rmdir(FACEBOOK_VISITORS_LOCKDIR);
	}

	function LogLine($text)
	{
		//dump($_SERVER, false);
		$agent = (isset($_SERVER['HTTP_USER_AGENT']) ? $_SERVER['HTTP_USER_AGENT'] : 'No user agent');
		$query = (isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : 'No query string');
		$ip = (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : 'No remote IP address');
		$visitTime = date('r'); //Example: Thu, 21 Dec 2000 16:01:07 +0200

		$logEntry = "Visited: $visitTime\r\n" . "IP: $ip\r\n" . "User Agent: $agent\r\n";
		//$queryArray = split('&', $query);
		//foreach ($queryArray as $q)
		//	$logEntry .= "Query: $q\r\n";
		$logEntry .= "Text: $text\r\n" . "\r\n";

		if (!LogLock())
			return;

		$fp = fopen(FACEBOOK_VISITORS_LOGFILE, 'a');
		if ($fp)
		{
			fwrite($fp, $logEntry);
			fclose($fp);
		}

		LogUnlock();
	}

	function Param($name, $defaultvalue = null)
	{
		$value = null;

		if (isset($_REQUEST[$name]))
			$value = $_REQUEST[$name];
		if (isset($_REQUEST['amp;' . $name]))
		{
			if (is_array($value))
				$value = array_merge($value, $_REQUEST['amp;' . $name]);
			else
				$value = $_REQUEST['amp;' . $name];
		}

		if (!$value)
			$value = $defaultvalue;
			
		return $value;
	}

	function ParamIsSet($name)
	{
		if (isset($_REQUEST[$name]))
			return true;
		if (isset($_REQUEST['amp;' . $name]))
			return true;

		return false;
	}

	function Img($href)
	{
		return '<img border="0" src="' . $href . '" />';
	}

	function StdDate($datetime = null)
	{
		if (!$datetime)
			$datetime = time();
		return date('m/d/y \a\\t h:i A', $datetime);
	}

	// @param Number $date - The timestamp
	// @param Boolean $relative - If true, then dates will show as "2 hours ago", "5 weeks ago" etc (works up to 1 year)
	// @param Number $std_format - Quick date format option to use from $std_formats (0 - 11)
	// @param String $custom_format - Custom date format will be used instead
	// @param Boolean $today_yesterday - If true, then today and yesterday will be shown (best to use $custom_format with this)
	// @param String $today_str - String to be used to show for today
	// @param String $yester_str - String to be used to show for yesterday
	// @return String - The formatted date
	function TheDate($date, $relative = false, $std_format = 4, $custom_format = null, $today_yesterday = false, $today_str = "Today, at ", $yester_str = "Yesterday, at")
	{
		if (!$date)
			return '--';

		$marks = array(
			"MIN"  => 60,
			"HOUR" => 3600,
			"DAY"  => 86400,
			"WEEK" => 604800,
			"YEAR" => 31449600
		);

		$prefix = '';
		if ($today_yesterday)
		{
			$today = gmdate("d,m,Y", time());
			$yesterday = gmdate("d,m,Y", (time() - $marks["DAY"]));
			$this_time = gmdate('d,m,Y', $date);

			if ($this_time == $today)
				$prefix = $today_str;
			if ($this_time == $yesterday)
				$prefix = $yester_str;
		}

		$std_formats = array(
			"d/m/y",					// #0 - 07/05/07
			"d/m/y, h:i A",				// #1 - 07/05/07, 12:40 PM
			"d/m/y \a\\t h:i A",		// #2 - 07/05/07 at 12:42 PM
			"jS M Y",					// #3 - 7th May 2007
			"jS M Y, h:i A",			// #4 - 7th May 2007, 12:42 PM
			"jS M Y \a\\t h:i A",		// #5 - 7th May 2007 at 12:42 PM
			"M jS Y",					// #6 - May 7th 2007
			"M jS Y, h:i A",			// #7 - May 7th 2007, 12:43 PM
			"M jS Y \a\\t h:i A",		// #8 - May 7th 2007 at 12:43 PM
			"jS M,  Y, \a\\t h:i A",	// #9 - 7th May, 2007, at 12:57 PM
			"D, jS \of F, Y",			// #10 - Mon, 7th of May, 2007
			"M Y"						// #11 - May 2007
		);

		$format = ($custom_format ? $custom_format : $std_formats[$std_format]);
		if (!$relative)
			return $prefix . gmdate($format, $date);

		// Relative date handling

		$difference = time() - $date;
		if ($difference < $marks["HOUR"])
		{
			$minutes = intval($difference / $marks["MIN"]);
			return ($minutes < 1 ? 1 : $minutes) . " minute" . ($minutes <= 1 ? "" : "s") . " ago";
		}
		if ($difference < $marks["DAY"])
		{
			$hours = intval($difference / $marks["HOUR"]);
			return ($hours < 1 ? 1 : $hours) . " hour" . ($hours <= 1 ? "" : "s") . " ago";
		}
		if ($difference < ($marks["WEEK"] * 4))
		{
			$days = intval($difference / $marks["DAY"]);
			return ($days < 1 ? 1 : $days) . " day" . ($days <= 1 ? "" : "s") . " ago";
		}
		if ($difference < $marks["YEAR"])
		{
			$weeks = intval($difference / $marks["WEEK"]);
			return ($weeks < 1 ? 1 : $weeks) . " week" . ($weeks <= 1 ? "" : "s") . " ago";
		}

		return $prefix . gmdate($format, $date);
	}

	function GetUserInfo($user, $field)
	{
		global $facebook;
		$usersinfo = $facebook->api_client->users_getInfo($user, $field);
		if (!$usersinfo) return null;
		return $usersinfo[0][$field];
	}

	function GetUserInfoEx($user, $fields)
	{
		global $facebook;
		$usersinfo = $facebook->api_client->users_getInfo($user, $fields);
		if (!$usersinfo) return null;
		return $usersinfo[0];
	}

	function GetUsersInfoEx($users, $fields)
	{
		global $facebook;
		$usersinfo = $facebook->api_client->users_getInfo($users, $fields);
		return $usersinfo;
	}

	function GetNameArrayAsList($array)
	{
		$indent = '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
		$output = '';
		foreach ($array as $entry)
			$output .= $indent . $entry . '<br/>';

		return $output;
	}

	function GetLinkToUser($hrefpage, $userid, $name)
	{
		$userarg = ($userid ? 'user=' . $userid : '');
		$output = '<a target="_parent" href="' . FACEBOOK_CANVAS_URL . $hrefpage . $userarg . '">' . $name . '</a>';
		return $output;
	}

    function GetFriendsArray($subset = null)
	{
		global $facebook;
		if (!$subset || $subset == '-all') // Get all friends
			return $facebook->api_client->friends_get();

		if ($subset == '-with') // Get all friends with the app
			return $facebook->api_client->friends_getAppUsers();

		if ($subset == '-hastagged') // Get all users that the user has tagged
			return GetHasTaggedArray($facebook->get_loggedin_user());

		if ($subset == '-taggedby') // Get all users who have been tagged by the user
			return GetTaggedByArray($facebook->get_loggedin_user());

		if ($subset == '-taggedwith') // Get all users who have been tagged with the user
			return GetTaggedWithArray($facebook->get_loggedin_user());

		// Do a friend search
		$outputarray = array();
		$friends = $facebook->api_client->friends_get();

		if ($subset == '-without') // Get all friends without the app
		{
			$usersinfo = GetUsersInfoEx($friends, 'is_app_user');
			foreach ($usersinfo as $userinfo)
			{
				if (!$userinfo['is_app_user'])
					$outputarray[] = $userinfo['uid'];
			}
		}
		else
		if ($subset == '-updated') // Get all friends who have recently updated their profile
		{
			$usersinfo = GetUsersInfoEx($friends, 'profile_update_time');
			foreach ($usersinfo as $userinfo)
			{
				if ($userinfo['profile_update_time'])
					$outputarray[] = $userinfo['uid'];
			}
		}
		else
		if ($subset == '-hs') // Get all friends who have the same high school
		{
			$hs_info = GetUserInfo($facebook->get_loggedin_user(), 'hs_info');
			if (!$hs_info)
				return $outputarray;
			$user_highschool = $hs_info['hs1_name'];
			if (!$user_highschool || $user_highschool == '')
				return $outputarray;

			$usersinfo = GetUsersInfoEx($friends, 'hs_info');
			foreach ($usersinfo as $userinfo)
			{
				$hs_info = $userinfo['hs_info'];
				if (!$hs_info)
					continue;
				$highschool = $hs_info['hs1_name'];
				if (!strcasecmp($user_highschool, $highschool))
					$outputarray[] = $userinfo['uid'];
			}
		}
		else
		if ($subset == '-home') // Get all friends who have the same hometown
		{
			$hometown_location = GetUserInfo($facebook->get_loggedin_user(), 'hometown_location');
			if (!$hometown_location)
				return $outputarray;
			$user_hometown = $hometown_location['city'] . $hometown_location['state'];
			if (!$user_hometown || $user_hometown == '')
				return $outputarray;

			$usersinfo = GetUsersInfoEx($friends, 'hometown_location');
			foreach ($usersinfo as $userinfo)
			{
				$hometown_location = $userinfo['hometown_location'];
				if (!$hometown_location)
					continue;
				$hometown = $hometown_location['city'] . $hometown_location['state'];
				if (!strcasecmp($user_hometown, $hometown))
					$outputarray[] = $userinfo['uid'];
			}
		}
		else
		if ($subset == '-city') // Get all friends who have the same current location
		{
			$current_location = GetUserInfo($facebook->get_loggedin_user(), 'current_location');
			if (!$current_location)
				return $outputarray;
			$user_city = $current_location['city'] . $current_location['state'];
			if (!$user_city || $user_city == '')
				return $outputarray;

			$usersinfo = GetUsersInfoEx($friends, 'current_location');
			foreach ($usersinfo as $userinfo)
			{
				$current_location = $userinfo['current_location'];
				if (!$current_location)
					continue;
				$city = $current_location['city'] . $current_location['state'];
				if (!strcasecmp($user_city, $city))
					$outputarray[] = $userinfo['uid'];
			}
		}
		else
		if ($subset == '-college') // Get all friends who share a common college
		{
			$education_history = GetUserInfo($facebook->get_loggedin_user(), 'education_history');
			if (!$education_history)
				return $outputarray;
			$user_schoolname = '';
			foreach ($education_history as $school)
			{
				$user_schoolname .= '~' . strtolower($school['name']);
			}
			if ($user_schoolname == '')
				return $outputarray;

			$usersinfo = GetUsersInfoEx($friends, 'education_history');
			foreach ($usersinfo as $userinfo)
			{
				$education_history = $userinfo['education_history'];
				if (!$education_history)
					continue;
				foreach ($education_history as $school)
				{
					$schoolname = '~' . strtolower($school['name']);
					if (strpos($user_schoolname, $schoolname) === false)
						continue;
					$outputarray[] = $userinfo['uid'];
					break;
				}
			}
		}
		else
		if ($subset == '-region') // Get all friends who share a common region
		{
			$affiliations = GetUserInfo($facebook->get_loggedin_user(), 'affiliations');
			if (!$affiliations)
				return $outputarray;
			$user_networks = '';
			foreach ($affiliations as $affiliation)
			{
				if ($affiliation['type'] != 'region')
					continue;
				$user_networks .= '~' . strtolower($affiliation['name']);
			}
			if ($user_networks == '')
				return $outputarray;

			$usersinfo = GetUsersInfoEx($friends, 'affiliations');
			foreach ($usersinfo as $userinfo)
			{
				$affiliations = $userinfo['affiliations'];
				if (!$affiliations)
					continue;
				foreach ($affiliations as $affiliation)
				{
					if ($affiliation['type'] != 'region')
						continue;
					$networks = '~' . strtolower($affiliation['name']);
					if (strpos($user_networks, $networks) === false)
						continue;
					$outputarray[] = $userinfo['uid'];
					break;
				}
			}
		}
		else
		if ($subset == '-work') // Get all friends who share a common workplace
		{
			$jobs = GetUserInfo($facebook->get_loggedin_user(), 'work_history');
			if (!$jobs)
				return $outputarray;
			$user_company_name = '';
			foreach ($jobs as $job)
			{
				$user_company_name .= '~' . strtolower($job['company_name']);
			}
			if ($user_company_name == '')
				return $outputarray;

			$usersinfo = GetUsersInfoEx($friends, 'work_history');
			foreach ($usersinfo as $userinfo)
			{
				$jobs = $userinfo['work_history'];
				if (!$jobs)
					continue;
				foreach ($jobs as $job)
				{
					$company_name = '~' . strtolower($job['company_name']);
					if (strpos($user_company_name, $company_name) === false)
						continue;
					$outputarray[] = $userinfo['uid'];
					break;
				}
			}
		}
		else
		{
			// Search the user name
			$usersinfo = GetUsersInfoEx($friends, 'name');
			foreach ($usersinfo as $userinfo)
			{
				$name = $userinfo['name'];
				if (strpos(strtolower($name), strtolower($subset)) === false)
					continue;

				$outputarray[] = $userinfo['uid'];
			}
		}

		return $outputarray;
	}

	function GetViewUsersForFriends($users, $bAddPhotoCount)
	{
		$arrUsers = array();
		$usersinfo = GetUsersInfoEx($users, 'name, first_name, is_app_user, sex, affiliations, pic');
		foreach ($usersinfo as $userinfo)
		{
			$userid = $userinfo['uid'];
			$picUrl = $userinfo['pic'];
			if (!$picUrl) $picUrl = FACEBOOK_DEFAULT_PIC_URL;
			$sex = $userinfo['sex'];
			$hisHerTheir = ''; //j'their';
			if ($sex == 'male') $hisHerTheir = 'his';
			if ($sex == 'female') $hisHerTheir = 'her';
			$himHerThem = ''; //j 'them';
			if ($sex == 'male') $himHerThem = 'him';
			if ($sex == 'female') $himHerThem = 'her';
			$affiliations = $userinfo['affiliations'];
			$networks = null;
			foreach ($affiliations as $affiliation)
			{
				$networks .= $affiliation['name'];
				$status = $affiliation['status'];
				if ($status)
				{
					if (substr($status, 0, 4) == 'Alum')
						$status = 'Alum';
					$networks .= ' ' . $status;
				}
				$year = $affiliation['year'];
				if ($year)
					$networks .= ' ' . $year;
				$networks .= '<br/>';
			}

			$numAlbums = '';
			$numPhotos = ($bAddPhotoCount ? GetNumPhotos($userid, $numAlbums) : '');
		
			$arrUsers[] = array(
				'userid' => $userinfo['uid'],
				'name' => $userinfo['name'],
				'firstName' => $userinfo['first_name'],
				'bIsAppUser' => $userinfo['is_app_user'],
				'picUrl' => $picUrl,
				'hisHerTheir' => $hisHerTheir,
				'himHerThem' => $himHerThem,
				'numPhotos' => $numPhotos,
				'numAlbums' => $numAlbums,
				'networks' => $networks,
			);
		}

		usort($arrUsers, create_function('$a, $b', 'return strcasecmp($a["name"],$b["name"]);'));
		return $arrUsers;
	}

	function GetViewUsersPhotoCounts($users)
	{
		$arrUsers = array();
		foreach ($users as $userid)
		{
			$numAlbums = 0;
			$numPhotos = GetNumPhotos($userid, $numAlbums);
		
			$arrUsers[] = array(
				'userid' => $userid,
				'numPhotos' => $numPhotos,
				'numAlbums' => $numAlbums,
			);
		}

		return $arrUsers;
	}

	function GetViewUsersForInvite()
	{
		$arrUsers = array();
		$usersinfo = GetUsersInfoEx(GetFriendsArray(), 'name, is_app_user, pic_square');
		foreach ($usersinfo as $userinfo)
		{
			$picUrl = $userinfo['pic_square'];
			if (!$picUrl) $picUrl = FACEBOOK_DEFAULT_PIC_URL;
			$arrUsers[] = array(
				'userid' => $userinfo['uid'],
				'name' => $userinfo['name'],
				'bIsAppUser' => $userinfo['is_app_user'],
				'picUrl' => $picUrl
			);
		}
		
		usort($arrUsers, create_function('$a, $b', 'return strcasecmp($a["name"],$b["name"]);'));
		return $arrUsers;
	}

	function GetViewUsersForMobilize($users, $bAddPhotoCount, & $targetuserid)
	{
		global $facebook;

		$loggedinUser = $facebook->get_loggedin_user();
		if (!$users)
			$users = array();

		$bAddLoggedinUser = true;
		$bAddTargetUser = ($targetuserid != $loggedinUser);
		foreach ($users as $userid)
		{
			if ($userid == $loggedinUser)
				$bAddLoggedinUser = false;
			if ($userid == $targetuserid)
				$bAddTargetUser = false;
		}
	
		if ($bAddLoggedinUser)
			array_push($users, $loggedinUser);
		if ($bAddTargetUser)
			array_push($users, $targetuserid);

		$arrUsers = array();
		$usersinfo = GetUsersInfoEx($users, 'name');
		foreach ($usersinfo as $userinfo)
		{
			$userid = $userinfo['uid'];
			if ($targetuserid == '') $targetuserid = $userid;
			$bSelected = ($userid == $targetuserid);
			$selected = ($bSelected ? 'selected="selected"' : '');
			$numAlbums = 0;
			$numPhotos = ($bSelected || $bAddPhotoCount ? GetNumPhotos($userid, $numAlbums) : 0);
			$num_photos = ($numPhotos ? ' (' . $numPhotos . ')' : '');
			$arrUsers[] = array(
				'userid' => $userinfo['uid'],
				'name' => $userinfo['name'],
				'selected' => $selected,
				'num_photos' => $num_photos,
			);
		}
		
		usort($arrUsers, create_function('$a, $b', 'return strcasecmp($a["name"],$b["name"]);'));
		return $arrUsers;
	}

	function GetAlbumsArray($userid)
	{
		global $facebook;
		if (FACEBOOK_USE_FQLQUERY)
		{
			$query = 'SELECT aid, name, size FROM album WHERE owner=' . $userid;
			$albums = $facebook->api_client->fql_query($query);
		}
		else
			$albums = $facebook->api_client->photos_getAlbums($userid, null);

		return $albums;
	}

	function GetAlbumPhotosArray($aid)
	{
		global $facebook;
		$photos = $facebook->api_client->photos_get(null, $aid, null);
		return $photos;
	}

	function GetTaggedPhotosArray($userid)
	{
		global $facebook;
		$photos = $facebook->api_client->photos_get($userid, null, null);
		return $photos;
	}

	function GetPhotosTagsArray($photos)
	{
		global $facebook;
		$pids = GetPhotosPidsArray($photos);
		$tags = $facebook->api_client->photos_getTags($pids);
		return $tags;
	}

	function GetNumPhotosInAlbums($userid, & $numAlbums)
	{
		global $facebook;
		if (FACEBOOK_USE_FQLQUERY)
		{
			$query = 'SELECT size FROM album WHERE owner=' . $userid;
			$albums = $facebook->api_client->fql_query($query);
		}
		else
			$albums = $facebook->api_client->photos_getAlbums($userid, null);

		$numAlbums = 0;
		$numPhotos = 0;
		foreach ($albums as $album)
		{
			$numAlbums++;
			$numPhotos += (int)$album['size'];
		}

		return $numPhotos;
	}

	function GetNumPhotosTagged($userid)
	{
		global $facebook;
		if (FACEBOOK_USE_FQLQUERY)
		{
			$query = 'SELECT pid FROM photo_tag WHERE subject=' . $userid;
			$pids = $facebook->api_client->fql_query($query);
			if (!$pids)
				return 0;

			return count($pids);
		}
		else
		{
			$photos = $facebook->api_client->photos_get($userid, null, null);
			if (!$photos)
				return 0;

			return count($photos);
		}
		
		return 0;
	}

	function GetNumPhotos($userid, & $numAlbums)
	{
		$numPhotos = 1; // Everyone has a profile picture
		$numPhotos += GetNumPhotosInAlbums($userid, $numAlbums);
		$numPhotos += GetNumPhotosTagged($userid);
		return $numPhotos;
	}

	function GetAlbumsForDropDown($userid, $name, & $targetaid)
	{
		$output = '';
		$albums = GetAlbumsArray($userid);
		foreach ($albums as $album)
		{
			$aid = $album['aid'];
			if ($targetaid == '') $targetaid = $aid;
			$bSelected = (!strcmp($aid, $targetaid));
			$selected = ($bSelected ? 'selected="selected"' : '');
			$text = $album['name'];
			$numPhotos = $album['size'];
			$num_photos = ($numPhotos ? ' (' . $numPhotos . ')' : '');
			$output .= '<option value="' . $aid . '" ' . $selected . '>' . $text . $num_photos . '</option>';
		}

		// Add an entry for tagged pictures
		$numPhotos = GetNumPhotosTagged($userid);
		if ($numPhotos)
		{
			$aid = 1;
			if ($targetaid == '') $targetaid = $aid;
			$bSelected = (!strcmp($aid, $targetaid));
			$selected = ($bSelected ? 'selected="selected"' : '');
			$text = 'Photos in which ' . $name . ' has been tagged';
			$num_photos = ($numPhotos ? ' (' . $numPhotos . ')' : '');
			$output .= '<option value="' . $aid . '" ' . $selected . '>' . $text . $num_photos . '</option>';
		}

		// Add an entry for profile pictures
		$aid = 2;
		if ($targetaid == '') $targetaid = $aid;
		$bSelected = (!strcmp($aid, $targetaid));
		$selected = ($bSelected ? 'selected="selected"' : '');
		$text = $name . '\'s profile picture';
		$output .= '<option value="' . $aid . '" ' . $selected . '>' . $text . '</option>';

		return $output;
	}

	function GetPhotosViewTitle($firstName, $userid, $aid)
	{
		if ($aid == 1)
			return ''; // Photos in which the user has been tagged

		if ($aid == 2)
			return ''; // The user's Profile Picture

		return ' ' . $firstName . '\'s album'; // Photos from the user's album
	}

	function GetViewAlbumForMobilize($userid, $aid)
	{
		if ($aid == 1)
		{
			$photos = GetTaggedPhotosArray($userid);
			return GetViewPhotosForMobilize($userid, $photos);
		}

		if ($aid == 2)
		{
			return GetViewProfilePhotoForMobilize($userid);
		}

		return GetViewPhotosForMobilize($userid, GetAlbumPhotosArray($aid));
	}

	function GetViewProfilePhotoForMobilize($userid)
	{
		// FYI, 'pic' is max 100x300, 'pic_small' is max 50x150, 'pic_big' is max 200x600, 'pic_square' is max 50x50
		$userinfo = GetUserInfoEx($userid, 'name, pic_big, pic_small');
		$src_thumb = $userinfo['pic_small']; // pic_square also available
		$src_fullsize = $userinfo['pic_big'];
		if (!$src_thumb) $src_thumb = FACEBOOK_DEFAULT_PIC_URL;
		if (!$src_fullsize) $src_fullsize = FACEBOOK_DEFAULT_PIC_URL;

		$arrPhotos = array();
		$arrPhotos[] = array(
			'src_thumb' => $src_thumb,
			'src_fullsize' => $src_fullsize . '?pid=' . $userid, // pass a userid as a pid to get the profile picture
			'caption' => $userinfo['name'],
		);

		return $arrPhotos;
	}

	function GetViewPhotosForMobilize($userid, $photos)
	{
		$name = GetUserInfo($userid, 'name');
		$arrPhotos = array();
		foreach ($photos as $photo)
		{
			$owner = GetUserInfo($photo['owner'], 'name');
			$caption = str_replace(array("\r\n", "\r", "\n"), ' ', $photo['caption']);
			$arrPhotos[] = array(
				'src_thumb' => $photo['src'],  // src_small also available
				'src_fullsize' => $photo['src_big'] . '?pid=' . $photo['pid'],
				'caption' => $caption . ($owner != $name ? ' by ' . $owner : ''),
			);
		}

		return $arrPhotos;
	}

	function GetPhotosOwnerArray($photos, $useridToIgnore = null)
	{
		$outputarray = null;
		foreach ($photos as $photo)
		{
			$userid = $photo['owner'];
			if ($userid == $useridToIgnore)
				continue;
			$outputarray[] = $userid;
		}

		return $outputarray;
	}

	function GetPhotosPidsArray($photos)
	{
		$outputarray = array();
		foreach ($photos as $photo)
		{
			$pid = $photo['pid'];
			$outputarray[] = $pid;
		}

		return $outputarray;
	}

	function GetTaggedByArray($userid)
	{
		global $facebook;
		if (FACEBOOK_USE_FQLQUERY)
		{
			$query = 'SELECT owner FROM photo WHERE pid IN (SELECT pid FROM photo_tag WHERE subject=' . $userid . ')';
			$photos = $facebook->api_client->fql_query($query);
		}
		else
			$photos = $facebook->api_client->photos_get($userid, null, null);

		return GetPhotosOwnerArray($photos, $userid);
	}

	function GetHasTaggedArray($userid)
	{
		global $facebook;
		$query = 'SELECT subject FROM photo_tag WHERE pid IN (SELECT pid FROM photo WHERE aid IN (SELECT aid FROM album WHERE owner=' . $userid . '))';
		$tags = $facebook->api_client->fql_query($query);
		return ConvertTagsArrayToUserArray($tags, $userid);
	}

	function GetTaggedWithArray($userid)
	{
		global $facebook;
		if (FACEBOOK_USE_FQLQUERY)
		{
			$query = 'SELECT subject FROM photo_tag WHERE subject!=' . $userid . ' AND pid IN (SELECT pid FROM photo_tag WHERE subject=' . $userid . ')';
			$tags = $facebook->api_client->fql_query($query);
		}
		else
		{
			$photos = $facebook->api_client->photos_get($userid, null, null);
			$tags = GetPhotosTagsArray($photos);
		}

		return ConvertTagsArrayToUserArray($tags, $userid);
	}

	function ConvertTagsArrayToUserArray($tags, $useridToIgnore = null)
	{
		$outputarray = null;
		foreach ($tags as $tag)
		{
			$userid = $tag['subject'];
			if (!$userid || $userid == $useridToIgnore)
				continue;
			$outputarray[] = $userid;
		}

		return $outputarray;
	}

	function GetGroupMembersArray($userid)
	{
		global $facebook;
		$groups = $facebook->api_client->groups_get($userid, null);
		$outputarray = array();
		foreach ($groups as $group)
		{
			$gid = $group['gid'];
			$membersAll = $facebook->api_client->groups_getMembers($gid);
			$members = $membersAll['members'];
			$outputarray = array_merge($outputarray, $members);
		}

		return $outputarray;
	}

	function GetGroupMembersNameArray($userid)
	{
		$members = GetGroupMembersArray($userid);

		$outputarray = array();
		$usersinfo = GetUsersInfoEx($members, 'name');
		foreach ($usersinfo as $userinfo)
		{
			$outputarray[] = $userinfo['name'];
		}

		return $outputarray;
	}

	function GetGroupsNameArray($userid)
	{
		global $facebook;
		$groups = $facebook->api_client->groups_get($userid, null);
		$outputarray = array();
		foreach ($groups as $group)
		{
			$name = $group['name'];
			$outputarray[] = $name;
		}

		return $outputarray;
	}

	function HttpGetUrl($url)
	{
		$urlinfo = parse_url($url);
		$host = $urlinfo['host'];
		$headers = array(
			'Accept: */*',
			'Host: ' . $host
		);

		$curlHandle = curl_init($url);

		curl_setopt($curlHandle, CURLOPT_HTTPHEADER, $headers);
		curl_setopt($curlHandle, CURLOPT_URL, $url);
		curl_setopt($curlHandle, CURLOPT_HEADER, false);
		curl_setopt($curlHandle, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($curlHandle, CURLOPT_REFERER, $url);
		//curl_setopt($curlHandle, CURLOPT_POSTFIELDS,'email='.urlencode($login_email).'&pass='.urlencode($login_pass).'&login=Login');
		//curl_setopt($curlHandle, CURLOPT_POST, false);
		//curl_setopt($curlHandle, CURLOPT_FOLLOWLOCATION, true);
		//curl_setopt($curlHandle, CURLOPT_SSL_VERIFYPEER, false);
		//curl_setopt($curlHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3");

		$body = curl_exec($curlHandle);
		$responseInfo = curl_getinfo($curlHandle);
		$responseInfo['body'] = $body;
		curl_close($curlHandle);
		return $responseInfo;
	}

	function FacebookMobilizationSuccess($title, $imageUrl)
	{
		global $facebook;
		$actorUserid = $facebook->get_loggedin_user();

		$appLinkUrl = FACEBOOK_CANVAS_URL . 'Mobilize?user=' . $actorUserid;
		$imagePidOrUrl = ExtractPidFromUrl($imageUrl);
		if ($imagePidOrUrl && $imagePidOrUrl != '')
			$imageLinkUrl = $appLinkUrl; //j= FACEBOOK_URL . 'photo.php?pid=' . $imagePidOrUrl . '&id=' . $actorUserid;
		else
			$imageLinkUrl = $appLinkUrl;
		if (!$imagePidOrUrl) $imagePidOrUrl = $imageUrl;

		$appLink = '<a href="' . $appLinkUrl . '">' . FACEBOOK_APPNAME . '</a>';
		$photoNameTitle = '';
		$photoNameBody = '';
		if ($title && $title != '' && $title != '(no title)')
		{
			$br = '<b class=clearfix />'; // can't user tag <br />
			$photoNameTitle = ''; //j No name in the title for now; previously, $photoNameTitle = 'named "' . $title . '"';
			$photoNameBody = '<b><i><a class=clearfix href="' . $appLinkUrl . '">"' . $title . '"</a></i></b>';
			//$photoNameBody = $br . '<b><i><a href="' . $appLinkUrl . '">"' . $title . '"</a></i></b>' . $br;
			//j$photoNameBody = $br . '<b><i><a href="' . $appLinkUrl . '" class=photobox_text>"' . $title . '"</a></i></b>' . $br;
		}

		$titleTemplate = '{actor} sent a photo {photoName} to a mobile phone.';
		$titleData = '{"photoName": "' . addslashes($photoNameTitle) . '"}';
		$bodyTemplate = '{photoNameBody}Send your photos and your friends photos to any mobile phone -- <b>FREE</b> -- with <b>{appLink}</b>.';
		$bodyData  = '{"appLink": "' . addslashes($appLink) . '", "photoNameBody": "' . addslashes($photoNameBody) . '"}';
		$bOK = SendFeedTemplate($actorUserid, $titleTemplate, $titleData, $bodyTemplate, $bodyData, $imagePidOrUrl, $imageLinkUrl);

		$feedTitle = 'You sent a photo ' . $photoNameTitle . ' to a mobile phone.';
		$feedBody = $photoNameBody . 'Send your photos and your friends photos to any mobile phone -- <b>FREE</b> -- with <b>' . $appLink . '</b>.';
		$bOK1 = SendNewsFeed($feedTitle, $feedBody, $imagePidOrUrl, $imageLinkUrl);

		// For debugging...
		//$message = '';
		//$message .= '$imageLinkUrl = [' . $imageLinkUrl . ']<br/>';
		//$message .= '$imagePidOrUrl = [' . $imagePidOrUrl . ']<br/>';
		//$message .= '$titleTemplate = [' . $titleTemplate . ']<br/>';
		//$message .= '$titleData = [' . $titleData . ']<br/>';
		//$message .= '$bodyTemplate = [' . $bodyTemplate . ']<br/>';
		//$message .= '$bodyData = [' . $bodyData . ']<br/>';
		//$message .= '$feedTitle = [' . $feedTitle . ']<br/>';
		//$message .= '$feedBody = [' . $feedBody . ']<br/>';
		//$title = $message . $title;

		// Let the user know if either of the feed stories do not get sent
		if (!$bOK || !$bOK1)
		{
			$message = '';
			if (!$bOK)
				$message .= 'Mini-Feed story not posted.<br/>';
			if (!$bOK1)
				$message .= 'News Feed story not posted.<br/>';
			$message .= '<br/>';

			$title = $message . $title;
		}

		return $title;
	}

	function SendStandardInvitation($to)
	{
		$typeword = FACEBOOK_APPNAME;
		// Warning: double quotes in the content string will screw up the invite signature process
		$content  = '3Guppies let\'s you send photos from Facebook to any mobile phone in the US -- <b><i>FREE</i></b>.  You can make a wallpaper or screensaver for yourself or surprise a friend with a picture message!  You can even crop the photos first to better fit your phone.  So start telling your stories with photos. After all, a picture is worth a thousand words.';
		$content .= '<fb:req-choice url=\'' . FACEBOOK_CANVAS_URL . 'Mobilize\' label=\'Check out ' . FACEBOOK_APPNAME . '\' />';
		$actionText = 'Spread the word!  Help your friends discover ' . FACEBOOK_APPNAME . '.';
		$bOK = SendRequest($to, $typeword, $content, $actionText);
		return $bOK;
	}

	function SendStandardGift($user, $to)
	{
		$giftImageUrl = FACEBOOK_IMAGES_URL . 'gift.jpg';
		$giftLinkUrl = FACEBOOK_CANVAS_URL . 'Mobilize';
		$giftName = 'A Gift Package';
		$feedTitle = ' sent a gift with <a href="' . FACEBOOK_CANVAS_URL . 'Mobilize">' . FACEBOOK_APPNAME . '</a>.';
		$feedBody = '<fb:userlink uid="' . $user . '" /> sent <fb:userlink uid="' . $to . '" shownetwork="false" /> a ' . $giftName . ' with <a href="' . FACEBOOK_CANVAS_URL . 'Mobilize">' . FACEBOOK_APPNAME . '</a>. &nbsp;';
		$feedBody .= 'Do you want to <a href="' . FACEBOOK_CANVAS_URL . 'Mobilize"> send a mobile gift</a> to a friend?';
		$bOK = SendMiniFeed($feedTitle, $feedBody, $giftImageUrl, $giftLinkUrl);
		return $bOK;
	}
	
	function ExtractPidFromUrl($imageUrl)
	{
		$target = '?pid=';
		$startPos = strpos($imageUrl, $target);
		if ($startPos === false)
			return null;
		$startPos += strlen($target);
		return substr($imageUrl, $startPos);
	}

	function SendFeedTemplate($userid, $titleTemplate, $titleData, $bodyTemplate, $bodyData,
		$imagePidOrUrl = null, $imageLinkUrl = null)
	{
		// Publishes a Mini-Feed story to the loggedin user
		// and publishes News Feed stories to the friends of that user
		// $titleTemplate is limited to 60 characters (excluding tags) and must contain the token {actor}
		// $titleData
		// $bodyTemplate is limited to 200 characters (excluding tags)
		// $bodyTemplateData
		// Included images must have an associated link
		// Applications are limited to calling feed_publishTemplatizedAction 10 times for each user in a rolling 48-hour window

		global $facebook;
		$bodyGeneral = '';
		$targetIds = '';
		$result = $facebook->api_client->feed_publishTemplatizedAction(
			$userid, $titleTemplate, $titleData, $bodyTemplate, $bodyData, $bodyGeneral, $imagePidOrUrl, $imageLinkUrl,
			null/*$image2*/, null/*$image2_link*/, null/*$image3*/, null/*$image3_link*/, null/*$image4*/, null/*$image4_link*/,
			$targetIds);
		$bOK = (is_array($result) ? $result[0] : false);
		return $bOK;
	}

	function SendMiniFeed($feedTitle, $feedBody, $imagePidOrUrl = null, $imageLinkUrl = null)
	{
		// Publishes a Mini-Feed story to the loggedin user
		// and publishes News Feed stories to the friends of that user
		// $feedTitle is limited to 60 characters (excluding tags)
		// $feedBody is limited to 200 characters (excluding tags)
		// Included images must have an associated link
		// feed_publishActionOfUser sends are restricted to 10 times per user in any 48 hour period

		global $facebook;
		$result = $facebook->api_client->feed_publishActionOfUser($feedTitle, $feedBody, $imagePidOrUrl, $imageLinkUrl);
		$bOK = (is_array($result) ? $result[0] : false);
		return $bOK;
	}

	function SendNewsFeed($feedTitle, $feedBody, $imagePidOrUrl = null, $imageLinkUrl = null)
	{
		// Publishes a News Feed story to the loggedin user
		// $feedTitle is limited to 60 characters (excluding tags)
		// $feedBody is limited to 200 characters (excluding tags)
		// Included images must have an associated link
		// feed_publishStoryToUser sends restricted to once per user every 12 hours (Unlimited to app developer's newsfeed)
	
		global $facebook;
		$result = $facebook->api_client->feed_publishStoryToUser($feedTitle, $feedBody, $imagePidOrUrl, $imageLinkUrl);
		$bOK = (is_array($result) ? $result[0] : false);
		return $bOK;
	}

	function SendRequest($to, $typeword, $content, $actionText, $bInvitation = true)
	{
		global $facebook;

		$bInviteAll = (!$to || $to == '' ? true : false);
		$excludeFriends = GetFriendsArray($bInviteAll ? '-with' : '-all');
		$excludeFriendsStr = '';
		foreach ($excludeFriends as $userid)
		{
			if (strpos($to, (string)$userid) !== false)
				continue;
			if ($excludeFriendsStr != '')
				$excludeFriendsStr .= ',';
			$excludeFriendsStr .= $userid;
		}

		$params = array();
		$params['api_key'] = FACEBOOK_API_KEY;
		$params['content'] = ($content);//htmlentities()
		$params['type'] = ($typeword);
		$params['action'] = (FACEBOOK_CANVAS_URL . 'Mobilize' . ($bInviteAll ? '' : '?user=' . $to));
		$params['actiontext'] = ($actionText);
		$params['invite'] = ($bInvitation ? 'true' : 'false');
		$params['rows'] = '5';
		$params['max'] = '20';
		$params['exclude_ids'] = $excludeFriendsStr;
		$params['sig'] = $facebook->generate_sig($params, FACEBOOK_SECRET);

		$qstring = '';
		foreach ($params as $key => $value)
		{
			if ($qstring != '')
				$qstring .= '&';
			$qstring .= "$key=$value";
		}

		$inviteUrl  = FACEBOOK_URL . 'multi_friend_selector.php?' . $qstring;
		$facebook->redirect($inviteUrl);
		return true;
	}

	function SendNotification($to, $body, $emailbody)
	{
		// Send a notification to a set of users
		// You can send emails to users that have added the application without any confirmation,
		// or you can direct a user of your application to the URL returned by this function to
		// email users who have not yet added your application
		// You can also send messages to the user's notification page without needing any confirmation
		// If a URL is returned, redirect the user to that URL to confirm sending of the request
		// $body // FBML content for the notifications page
		// $emailbody // Optional FBML content for the email. If not passed, no email will be sent

		global $facebook;
		$result = $facebook->api_client->notifications_send($to, $body, $emailbody);
		$url = $result;
		if (isset($url) && $url)
		{
			$facebook->redirect($url . '&canvas=1&next=Invite');
			return true;
		}

		$bOK = ($result && $result != '');
		return $bOK;
	}

	function VerifySignature($parameters = null)
	{
		// Note: Facebook does not include the fb_sig prefix when calculating the signature of the request. 
		// Default to using the POST array as the request
		if (!is_array($parameters) || count($parameters) <= 0)
			$parameters = $_POST;

		// Sort the request array alphabetically by keys
		ksort($parameters);

		// Reformat the array
		$prefix = 'fb_sig_';
		$parameters_str = '';
		foreach($parameters as $key=>$val)
		{
			if (strlen($key) > strlen($prefix)) // ignores $key == 'fb_sig'
			{
				$key = substr($key, strlen($prefix)); // remove the prefix
				$parameters_str .= $key . '=' . $val; // concatenates the data in order
			}
		}

		// Compute and compare the signatures
		$signature = md5($parameters_str . FACEBOOK_SECRET);
		$fb_sig = (isset($parameters['fb_sig']) ? $parameters['fb_sig'] : ''); // the signature given in the request
		$bVerified = ($signature == $fb_sig);
		return $bVerified;
	}
?>
