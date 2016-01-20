<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 3Guppies Inc                                           |
// +---------------------------------------------------------------------------+

/**
 * Mobilize action for the Facebook application
 *
 *
 * @author  Jim McCurdy
 */
class MobilizeAction extends Action
{

	/**
	 * Initialize this action. It's a static page, so not much to do here.
	 *
	 * @param Controller A Controller instance.
	 * @param Request    A Request instance.
	 * @param User       A User instance.
	 *
	 * @return bool <b>TRUE</b>, if this action initializes successfully,
	 *              otherwise <b>FALSE</b>.
	 *
	 * @access public
	 * @since  2.0
	 */
	function initialize (&$controller, &$request, &$user)
	{
		return TRUE;
	}

	/**
	 * Executes the Mobilize Action
	 *
	 * <br/><br/>
	 *
	 * <note>
	 *     This method should never be called manually.
	 * </note>
	 *
	 * @param Controller A Controller instance.
	 * @param Request    A Request instance.
	 * @param User       A User instance.
	 *
	 * @return mixed A single string value describing the view or an indexed
	 *               array coinciding with the following list:
	 *
	 *               <ul>
	 *                   <li><b>1st index</b> - module name</li>
	 *                   <li><b>2nd index</b> - action name</li>
	 *                   <li><b>3rd index</b> - view name</li>
	 *               </ul>
	 *
	 * @access public
	 * @since  1.0
	 */
	function execute (&$controller, &$request, &$user)
	{
		global $facebook;

		// Standard setup code for every application "canvas" page
		$facebook->require_frame();
		$facebook->require_add();
		//$facebook->require_login(); // Redundant with require_add()

		// make sure not to use '$user' as the variable!
		$facebookUser = $facebook->get_loggedin_user();
		$facebookUserName = GetUserInfo($facebookUser, 'name');
		
		// fetching facebook information
		$userid = (int)Param('user');
		if (!$userid)
			$userid = (int)Param('id', $facebookUser); // From the profile-action link

		$aid = Param('aid');
		$bAdded = ParamIsSet('added');
		if ($bAdded)
			LogLine($facebookUser . ' added ' .  FACEBOOK_APPNAME . ' (' . $facebookUserName . ')');

		$fbmlActionLink = (!FACEBOOK_FISHTANK // Only add a profile action link for the production app 
			? '<fb:profile-action url="' . FACEBOOK_CANVAS_URL . 'Mobilize">' . FACEBOOK_APPNAME . '</fb:profile-action>'
			: '');
		$profileboxurl = FACEBOOK_CALLBACK_URL . 'ProfileBox?user=' . $facebookUser;
		$facebook->api_client->fbml_refreshRefUrl($profileboxurl);
		$fbmlRefUrl = '<fb:ref url="' . $profileboxurl . '" />';
		$facebook->api_client->profile_setFBML($fbmlActionLink . $fbmlRefUrl);
//j		$facebook->api_client->profile_setFBML(null);

		$userinfo = GetUserInfoEx($userid, 'name, first_name, pic_big, is_app_user, sex');
		$name = $userinfo['name'];
		$firstName = $userinfo['first_name'];
		$picUrl = $userinfo['pic_big'];
		if (!$picUrl) $picUrl = FACEBOOK_DEFAULT_PIC_URL;
		$picUrl .= '?pid=' . $userid;
		$bIsAppUser = $userinfo['is_app_user'];
		$sex = $userinfo['sex'];
		$hisHerTheir = ''; //j'their';
		if ($sex == 'male') $hisHerTheir = 'his';
		if ($sex == 'female') $hisHerTheir = 'her';
		$himHerThem = ''; //j 'them';
		if ($sex == 'male') $himHerThem = 'him';
		if ($sex == 'female') $himHerThem = 'her';
		$numAlbums = 0;
		$numPhotos = GetNumPhotos($userid, $numAlbums);
		$albumsForDropDown = GetAlbumsForDropDown($userid, $firstName, $aid);
		$arrUsers = GetViewUsersForMobilize(array($userid), false/*$bAddPhotoCount*/, $userid);

		$facebookSessionKey = (isset($facebook->fb_params['session_key']) ? $facebook->fb_params['session_key'] : '');
		$openlockerMobilizeUrl = FACEBOOK_OPENLOCKER_URL . 'Mobilize' .
			'?FacebookData=' . $facebookUser . ',' . $facebookSessionKey . 
			'&fromName=' . $facebookUserName .
			'&Url=';

		$usersAjaxUrl = FACEBOOK_CALLBACK_URL . 'UsersAjax' .
			'?fb_user=' . $facebookUser . 
			'&fb_session_key=' . $facebookSessionKey . 
			'&user=' . $userid .
			'&counts=';

		$thumbsAjaxUrl = FACEBOOK_CALLBACK_URL . 'ThumbsAjax' .
			'?fb_user=' . $facebookUser . 
			'&fb_session_key=' . $facebookSessionKey . 
			'&user=' . $userid .
			'&aid=';

		// set the request attributes for presenting by the view
		$request->setAttribute('userid', $userid);
		$request->setAttribute('name', $name);
		$request->setAttribute('firstName', $firstName);
		$request->setAttribute('aid', $aid);
		$request->setAttribute('picUrl', $picUrl);
		$request->setAttribute('bAdded', $bAdded);
		$request->setAttribute('bIsAppUser', $bIsAppUser);
		$request->setAttribute('hisHerTheir', $hisHerTheir);
		$request->setAttribute('himHerThem', $himHerThem);
		$request->setAttribute('openlockerMobilizeUrl', $openlockerMobilizeUrl);
		$request->setAttribute('usersAjaxUrl', $usersAjaxUrl);
		$request->setAttribute('thumbsAjaxUrl', $thumbsAjaxUrl);
		$request->setAttribute('albumsForDropDown', $albumsForDropDown);
		$request->setAttribute('arrUsers', $arrUsers);

		// returning index view because this is the only view for the Mobilize page
		return VIEW_INDEX;
	}

	/**
	 * Retrieve the default view.
	 *
	 * @param Controller A Controller instance.
	 * @param Request    A Request instance.
	 * @param User       A User instance.
	 *
	 * @return mixed <use method="execute"/>
	 *
	 * @access public
	 * @since  1.0
	 */
	function getDefaultView (&$controller, &$request, &$user)
	{
		return VIEW_INDEX;
	}

	/**
	 * Retrieve the privilege required to access this action.
	 *
	 * @param Controller A Controller instance.
	 * @param Request    A Request instance.
	 * @param User       A User instance.
	 *
	 * @return array An indexed array coinciding with the following list:
	 *
	 *               <ul>
	 *                   <li><b>1st index</b> - privilege name</li>
	 *                   <li><b>2nd index</b> - privilege namespace
	 *                       (optional)</li>
	 *               </ul>
	 *
	 * @access public
	 * @see    isSecure()
	 * @since  1.0
	 */
	function getPrivilege (&$controller, &$request, &$user)
	{
		return NULL;
	}

	/**
	 * Restricts requests to Facebook app requests
	 * @return int Constant for Facebook requests
	 */
	function getContentTypes()
	{
		return CONTENT_FACEBOOK;
	}
	
	/**
	 * Retrieve the HTTP request method(s) this action will serve.
	 *
	 * @return int A request method that is one of the following:
	 *
	 *             <ul>
	 *                 <li><b>REQ_GET</b> - serve GET requests</li>
	 *                 <li><b>REQ_POST</b> - serve POST requests</li>
	 *             </ul>
	 *
	 * @access public
	 * @since  1.0
	 */
	function getRequestMethods ()
	{
		return REQ_GET;
	}

	/**
	 * Handle a validation error.
	 *
	 * @param Controller A Controller instance.
	 * @param Request    A Request instance.
	 * @param User       A User instance.
	 *
	 * @return mixed <use method="execute"/>
	 *
	 * @access public
	 * @since  1.0
	 */
	function handleError (&$controller, &$request, &$user)
	{
		return VIEW_ERROR;
	}

	/**
	 * No actions in Facebook should need isSecure, since authentication is
	 * via Facebook
	 *
	 * @return bool <b>TRUE</b>, if this action requires authentication,
	 *              otherwise <b>FALSE</b>.
	 *
	 * @access public
	 * @since  1.0
	 */
	function isSecure ()
	{
		return FALSE;
	}

	/**
	 * Register individual parameter validators.
	 *
	 * <br/><br/>
	 *
	 * <note>
	 *     This method should never be called manually.
	 * </note>
	 *
	 * @param ValidatorManager A ValidatorManager instance.
	 * @param Controller       A Controller instance.
	 * @param Request          A Request instance.
	 * @param User             A User instance.
	 *
	 * @access public
	 * @since  1.0
	 */
	function registerValidators (&$validatorManager, &$controller, &$request, &$user)
	{
	}

	/**
	 * Validate the request as a whole.
	 *
	 * <br/><br/>
	 *
	 * <note>
	 *     This method should never be called manually.
	 * </note>
	 *
	 * @param Controller A Controller instance.
	 * @param Request    A Request instance.
	 * @param User       A User instance.
	 *
	 * @return bool <b>TRUE</b>, if validation completes successfully, otherwise
	 *              <b>FALSE</b>.
	 *
	 * @access public
	 * @since  1.0
	 */
	function validate (&$controller, &$request, &$user)
	{
		return TRUE;
	}
}

?>