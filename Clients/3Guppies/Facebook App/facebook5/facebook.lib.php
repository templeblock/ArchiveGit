<?php
	$bUseFqlQuery = true;

	function LogLine($text)
	{
return;
		$agent = $_SERVER['HTTP_USER_AGENT'];
		$uri = $_SERVER['REQUEST_URI'];
		$ip = $_SERVER['REMOTE_ADDR'];
		$ref = $_SERVER['HTTP_REFERER'];
		$visitTime = date('r'); //Example: Thu, 21 Dec 2000 16:01:07 +0200

		$logLine = '$visitTime - IP: $ip || User Agent: $agent || Page: $uri || Referrer: $ref || Text: $text\n';
		$fp = fopen('/tmp/facebook/visitorLog.txt', 'a');
		if (!$fp)
			return;
		fputs($fp, $logLine);
		fclose($fp);
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

	function TransformArray($array)
	{
		$outputarray = array();
		foreach ($array as $entry)
			$outputarray[] = $entry;

		return $outputarray;
	}

	function GetMobilizeUrl($url)
	{
		global $siteMobilizeUrl;
		return $siteMobilizeUrl . urlencode($url);
	}
	
	function StdDate($datetime = null)
	{
		if (!$datetime)
			$datetime = time();
		return date('D, M j, Y, g:iA', $datetime);
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

	function GetCrumbs($user)
	{
		global $canvasPageUrl;
		$crumbs = Param('crumbs');
		if (!$crumbs || $crumbs == '')
			return;

		$users = split(',', $crumbs);
		$crumbs = null;
		$output = 'Tagged Photos Friends: ';
		$first = true;
		foreach ($users as $userid)
		{
			if ($crumbs) $crumbs .= ',';
			$crumbs .= $userid;
			$url = $canvasPageUrl . 'friendsofriends.php?user=' . $userid . '&crumbs=' . $crumbs;
			$name = GetUserInfo($userid, 'name');
			if (!$first)
				$output .= ', ';
			$first = false;
			$output .= '<a target="_parent" href="' . $url . '">' . $name . '</a>';
		}

		return $output;
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
		global $canvasPageUrl;
		$userarg = ($userid ? 'user=' . $userid : '');
		$output = '<a target="_parent" href="' . $canvasPageUrl . $hrefpage . $userarg . '">' . $name . '</a>';
		return $output;
	}

    function GetFriendsArray($subset = null)
	{
		global $facebook;
//j$userid = (int)$facebook->get_loggedin_user();//j
		if (!$subset || $subset == '-all') // Get all friends
//jif ($userid == 537626338)//j
//j$subset = '-updated';//j
//jelse//j
			return $facebook->api_client->friends_get();

		if ($subset == '-with') // Get all friends with the app
			return $facebook->api_client->friends_getAppUsers();

		// Do a friend search
		$outputarray = array();
//jif ($userid == 537626338)//j
//j$friends = GetGroupMembersArray($userid);//j
//jelse//j
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

	function IncludeUsersInDropDown($users, $bAddPhotoCount, & $targetuserid)
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

		$usersinfo = GetUsersInfoEx($users, 'name');
		foreach ($usersinfo as $userinfo)
		{
			$userid = $userinfo['uid'];
			$name = $userinfo['name'];
			if ($targetuserid == '') $targetuserid = $userid;
			$bSelected = ($userid == $targetuserid);
			$selected = ($bSelected ? 'selected="selected"' : '');
			$numPhotos = ($bSelected || $bAddPhotoCount ? GetNumPhotos($userid) : 0);
			$num_photos = ($numPhotos ? ' (' . $numPhotos . ')' : '');
			$output = '<option value="' . $userid . '" ' . $selected . '>' . $name . $num_photos . '</option>';
			echo $output;
		}
	}

	function GetUserLink($hrefpage, $userid, $name)
	{
		$indent = '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
		return $indent . GetLinkToUser($hrefpage, $userid, $name) .  '<br/>';
	}

	function IncludeUsers($users, $template)
	{
		global $facebook;
		global $facebookUrl;
		global $appHomeUrl;
		global $canvasPageUrl;
		global $defaultPicUrl;

//j		$crumbsOrig = Param('crumbs');
//j		if (!$crumbsOrig)
//j			$crumbsOrig = $facebook->get_loggedin_user();
//j		if ($crumbsOrig)
//j			$crumbsOrig .= ',';

		$aid = Param('aid');
	
		$usersinfo = GetUsersInfoEx($users, 'name, first_name, is_app_user, sex, affiliations, pic');
		foreach ($usersinfo as $userinfo)
		{
			$userid = $userinfo['uid'];
			$name = $userinfo['name'];
			$firstName = $userinfo['first_name'];
			$bIsAppUser = $userinfo['is_app_user'];
			$sex = $userinfo['sex'];
			$affiliations = $userinfo['affiliations'];
			$picUrl = $userinfo['pic'];

			if (!$picUrl) $picUrl = $defaultPicUrl;
			$hisHerTheir = ''; //j'their';
			if ($sex == 'male') $hisHerTheir = 'his';
			if ($sex == 'female') $hisHerTheir = 'her';
			$himHerThem = ''; //j 'them';
			if ($sex == 'male') $himHerThem = 'him';
			if ($sex == 'female') $himHerThem = 'her';
			$inviteOrAnnounce = ($bIsAppUser ? $firstName . ' uses 3Guppies' : GetLinkToUser('send.php?invitation&', $userid, 'Invite ' . $firstName . ' to add 3Guppies'));
//j			$crumbs = $crumbsOrig . $userid;
			$numPhotos = GetNumPhotos($userid);

			$networks = null;
			foreach ($affiliations as $affiliation)
			{
				$networks .= $affiliation['name'];
				$status = $affiliation['status'];
				if ($status)
					$networks .= ' ' . substr($status, 0, 4);
				$year = $affiliation['year'];
				if ($year)
					$networks .= ' ' . $year;
				$networks .= '<br/>';
			}

			include($template);
		}
	}

	function IncludeUsersToInvite($users, $template)
	{
		global $facebook;
		global $facebookUrl;
		global $canvasPageUrl;
		global $defaultPicUrl;

		$usersinfo = GetUsersInfoEx($users, 'name, is_app_user, pic_square');
		foreach ($usersinfo as $userinfo)
		{
			$name = $userinfo['name'];
			$bIsAppUser = $userinfo['is_app_user'];
			$picUrl = $userinfo['pic_square'];
			if (!$picUrl) $picUrl = $defaultPicUrl;

			include($template);
		}
	}

	function GetAlbumsArray($userid)
	{
		global $facebook;
		global $bUseFqlQuery;
		if ($bUseFqlQuery)
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

	function GetNumPhotosInAlbums($userid)
	{
		global $facebook;
		global $bUseFqlQuery;
		if ($bUseFqlQuery)
		{
			$query = 'SELECT size FROM album WHERE owner=' . $userid;
			$albums = $facebook->api_client->fql_query($query);
		}
		else
			$albums = $facebook->api_client->photos_getAlbums($userid, null);

		$numPhotos = 0;
		foreach ($albums as $album)
			$numPhotos += (int)$album['size'];

		return $numPhotos;
	}

	function GetNumPhotosTagged($userid)
	{
		global $facebook;
		global $bUseFqlQuery;
		if ($bUseFqlQuery)
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

	function GetNumPhotos($userid)
	{
		$numPhotos = 1; // Everyone has a profile picture
		$numPhotos += GetNumPhotosInAlbums($userid);
//jif ($userid == 537626338)//j
//j	return $numPhotos;//j
		$numPhotos += GetNumPhotosTagged($userid);
		return $numPhotos;
	}

	function IncludeAlbumsInDropDown($userid, $name, & $targetaid)
	{
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
			$output = '<option value="' . $aid . '" ' . $selected . '>' . $text . $num_photos . '</option>';
			echo $output;
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
			$output = '<option value="' . $aid . '" ' . $selected . '>' . $text . $num_photos . '</option>';
			echo $output;
		}

		// Add an entry for profile pictures
		$aid = 2;
		if ($targetaid == '') $targetaid = $aid;
		$bSelected = (!strcmp($aid, $targetaid));
		$selected = ($bSelected ? 'selected="selected"' : '');
		$text = $name . '\'s profile picture';
		$output = '<option value="' . $aid . '" ' . $selected . '>' . $text . '</option>';
		echo $output;
	}

	function GetPhotosViewTitle($firstName, $userid, $aid)
	{
		if ($aid == 1)
			return ''; // Photos in which the user has been tagged

		if ($aid == 2)
			return ''; // The user's Profile Picture

		return ' ' . $firstName . '\'s album'; // Photos from the user's album
	}

	function GetPhotosAsMobilizeLinks($userid, $aid)
	{
		global $defaultPicUrl;
		if ($aid == 1)
		{
			$photos = GetTaggedPhotosArray($userid);
			return GetPhotosArrayAsMobilizeLinks($photos);
		}

		if ($aid == 2)
		{
			$userinfo = GetUserInfoEx($userid, 'pic, pic_big');
			$url = $userinfo['pic_big'];
			$thumburl = $userinfo['pic'];
			if (!$url) $url = $defaultPicUrl;
			if (!$thumburl) $thumburl = $defaultPicUrl;
			return GetProfilePhotoAsMobilizeLink($url, $thumburl);
		}

		return GetPhotosArrayAsMobilizeLinks(GetAlbumPhotosArray($aid));
	}

	function GetProfilePhotoAsMobilizeLink($url, $thumburl)
	{
		$output = '<div>';
		$output .= GetPhotoUrlMobilizeLink($url, $thumburl);
		$output .= '</div>';
		return $output;
	}

	function GetPhotosArrayAsMobilizeLinks($photos)
	{
		$output = '<div>';
		foreach ($photos as $photo)
			$output .= GetPhotoMobilizeLink($photo);

		$output .= '</div>';
		return $output;
	}

	function GetPhotoMobilizeLink($photo)
	{
		// pid, aid, owner, src, src_big, src_small, link, caption, created
//j		$created = 'Created ' . StdDate($photo['created']);
//j		$owner = GetUserInfo($photo['owner'], 'name');
		$caption = $photo['caption'];
		$thumburl = $photo['src'];
		$url = $photo['src_big'];
		return GetPhotoUrlMobilizeLink($url, $thumburl, $caption);
	}

	function GetPhotoUrlMobilizeLink($url, $thumburl, $caption = '')
	{
		$output = '';
		$output .= '<span style="background-color:#E2E7EE; margin:2px; padding:2px; width:134px; height:134px; float:left; text-align:center; "';
		$output .= ' id="mxr-thumb" href="javascript:// Click to preview" onclick="SelectImage(this, \'' . $url . '\', \'' . $caption . '\')">';
//j		$output .= ' id="mxr-thumb" href="javascript:// Click to preview" onclick="PreviewImage(\'' . $url . '\', \'' . $caption . '\', null)">';
//j		$output .= ' id="mxr-thumb" href="' . GetMobilizeUrl($url) . '">';
		$output .= '<img border="0" alt="' . $caption . '" src="' . $thumburl . '" style="max-width: 130px;" />';
//j		$output .= '<br/>' . $caption;
		$output .= '</span>';
		
		return $output;
	}

	function ConvertTagsArrayToUserArray($tags, $useridToIgnore = null)
	{
		$outputarray = null;
		foreach ($tags as $tag)
		{
			$userid = $tag['subject'];
			if ($userid == $useridToIgnore)
				continue;
			$outputarray[$userid] = $userid;
		}

		return TransformArray($outputarray);
	}

	function GetPhotosOwnerArray($photos, $useridToIgnore = null)
	{
		$outputarray = null;
		foreach ($photos as $photo)
		{
			$userid = $photo['owner'];
			if ($userid == $useridToIgnore)
				continue;
			$outputarray[$userid] = $userid;
		}

		return TransformArray($outputarray);
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

	function GetTaggedByUserArray($userid)
	{
		global $facebook;
		$query = 'SELECT subject FROM photo_tag WHERE pid IN (SELECT pid FROM photo WHERE aid IN (SELECT aid FROM album WHERE owner=' . $userid . '))';
		$tags = $facebook->api_client->fql_query($query);
		return ConvertTagsArrayToUserArray($tags, $userid);
	}

	function GetWhoTaggedUserArray($userid)
	{
		global $facebook;
		global $bUseFqlQuery;
		if ($bUseFqlQuery)
		{
			$query = 'SELECT owner FROM photo WHERE pid IN (SELECT pid FROM photo_tag WHERE subject=' . $userid . ')';
			$photos = $facebook->api_client->fql_query($query);
		}
		else
			$photos = $facebook->api_client->photos_get($userid, null, null);

		return GetPhotosOwnerArray($photos, $userid);
	}

	function GetTaggedWithUserArray($userid)
	{
		global $facebook;
		global $bUseFqlQuery;
		if ($bUseFqlQuery)
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
//jreturn $outputarray;//j
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

	function SendStandardInvitation($to, $bInvitation = true)
	{
		global $canvasPageUrl;

		$typeword = '3Guppies Mobilizer';
		$body = '<fb:req-choice url="' . $canvasPageUrl . 'mobilize.php" label="Check out the 3Guppies Mobilizer" />Come use 3Guppies Mobilizer - it\'s the coolest thing since sliced bread.';
		$bOK = SendRequest($to, $typeword, $body, $bInvitation);
		return $bOK;
	}

	function SendStandardGift($user, $to)
	{
		global $appHomeUrl;
		global $canvasPageUrl;

		$giftImageUrl = $appHomeUrl . 'gift.jpg';
		$giftLinkUrl = $canvasPageUrl . 'mobilize.php';
		$giftName = 'A Gift Package';
		$title = ' sent a gift with <a href="' . $canvasPageUrl . 'mobilize.php">3Guppies Mobilizer</a>.';
		$body = '<fb:userlink uid="' . $user . '" /> sent <fb:userlink uid="' . $to . '" shownetwork="false" /> a ' . $giftName . ' with <a href="' . $canvasPageUrl . 'mobilize.php">3Guppies Mobilizer</a>. &nbsp;';
		$body .= 'Do you want to <a href="' . $canvasPageUrl . 'mobilize.php"> send a mobile gift</a> to a friend?';
		$bOK = SendMiniFeed($title, $body, $giftImageUrl, $giftLinkUrl);
		return $bOK;
	}
	
	function SendMiniFeed($title, $body, $imageUrl = null, $linkUrl = null)
	{
		// Publishes a Mini-Feed story to the loggedin user
		// and publishes News Feed stories to the friends of that user
		// $title is limited to 60 characters (excluding tags)
		// $body is limited to 200 characters (excluding tags)
		// Included images must have an associated link
		// feed_publishActionOfUser sends are restricted to 10 times per user in any 48 hour period

		global $facebook;
		$bOK = $facebook->api_client->feed_publishActionOfUser($title, $body, $imageUrl, $linkUrl);
		return $bOK;
	}

	function SendNewsFeed($title, $body)
	{
		// Publishes a News Feed story to the loggedin user
		// $title is limited to 60 characters (excluding tags)
		// $body is limited to 200 characters (excluding tags)
		// Included images must have an associated link
		// feed_publishStoryToUser sends restricted to once per user every 12 hours (Unlimited to app developer's newsfeed)

		global $facebook;
		$bOK = $facebook->api_client->feed_publishStoryToUser($title, $body);
		return $bOK;
	}

	function SendRequest($to, $typeword, $body, $bInvitation = true)
	{
		// Send a request or invitation to a set of users
		// You can send requests to users that have added the application without any confirmation,
		// or you can direct a user of your application to the URL returned by this function to
		// send requests to users who have not yet added your application
		// If a URL is returned, redirect the user to that URL to confirm sending of the request
		// $typeword // The type of request/invitation, as in the word "event" in "1 event invitation"
		// $body // FBML content of the request/invitation
		// $bInvitation // Whether to call this an "invitation" or a "request"

		global $facebook;
		global $canvasPageUrl;
		global $appHomeUrl;
		$image = $appHomeUrl . '3guppies_logo.gif'; // URL of an image to show beside the request; resized to be 100 pixels wide
		$result = $facebook->api_client->notifications_sendRequest($to, $typeword, $body, $image, $bInvitation);
		$url = $result;
		if (isset($url) && $url)
		{
			$facebook->redirect($url . '&canvas=1&next=invite.php');
			return true;
		}

		$bOK = ($result && $result != '');
		return $bOK;
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
		global $canvasPageUrl;
		$result = $facebook->api_client->notifications_send($to, $body, $emailbody);
		$url = $result;
		if (isset($url) && $url)
		{
			$facebook->redirect($url . '&canvas=1&next=invite.php');
			return true;
		}

		$bOK = ($result && $result != '');
		return $bOK;
	}
?>
