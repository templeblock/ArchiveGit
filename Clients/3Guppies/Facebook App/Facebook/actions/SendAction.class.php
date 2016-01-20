<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 3Guppies Inc                                           |
// +---------------------------------------------------------------------------+

/**
 * Send action for the Facebook application
 *
 *
 * @author  Jim McCurdy
 */
class SendAction extends Action
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
	 * Executes the Send Action
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
		
		// fetching facebook information
		$to = (int)Param('user');
		if (!$to)
			$to = (int)Param('id'); // From the invite form

		$nameRecipient = GetUserInfo($to, 'first_name');

		$inviteSelect = Param('inviteSelect'); // From the invite form
		if (!$to && $inviteSelect)
		{
			//dump($inviteSelect, false);
			foreach ($inviteSelect as $invitee)
			{
				$userid = (int)$invitee;
				if (!$to)
					$to = $userid;
				else
					$to .= ',' . $userid;

				$firstName = GetUserInfo($userid, 'first_name');
				if (!$nameRecipient)
					$nameRecipient = $firstName;
				else
					$nameRecipient .= ', ' . $firstName;

				//echo 'InviteeName ' . $firstName . '<br/>';
			}
		}

		$bMiniFeed = ParamIsSet('minifeed');
		$bNewsFeed = ParamIsSet('newsfeed');
		$bInvitation = ParamIsSet('invitation');
		$bRequest = ParamIsSet('request');
		$bNotification = ParamIsSet('notification');
		$bGift = ParamIsSet('gift');
		$bEmail = ParamIsSet('email');

		$command = '';
		$bOK = false;
		if ($bMiniFeed)
		{
			$command = 'a Mini-Feed story';
			$nameRecipient = 'yourself';
			$feedTitle = ' posted this Mini-Feed story with <a href="' . FACEBOOK_CANVAS_URL . 'Mobilize">' . FACEBOOK_APPNAME . '</a>.';
			$feedBody = 'These stories can be up to 200 characters long, <b>plus markup</b>.  The titles can only be 60 characters long, plus markup.';
			$imagePidOrUrl = null;
			$imageLinkUrl = FACEBOOK_CANVAS_URL . 'Mobilize';
			$bOK = SendMiniFeed($feedTitle, $feedBody, $imagePidOrUrl, $imageLinkUrl);
		}
		else
		if ($bNewsFeed)
		{
			$command = 'a News Feed story';
			$nameRecipient = 'yourself';
			$feedTitle = ' This News Feed story was posted by <a href="' . FACEBOOK_CANVAS_URL . 'Mobilize">' . FACEBOOK_APPNAME . '</a>.';
			$feedBody = 'These stories can be up to 200 characters long, <b>plus markup</b>.  The titles can only be 60 characters long, plus markup.';
			$imagePidOrUrl = null;
			$imageLinkUrl = FACEBOOK_CANVAS_URL . 'Mobilize';
			$bOK = SendNewsFeed($feedTitle, $feedBody, $imagePidOrUrl, $imageLinkUrl);
		}
		else
		if ($bNotification)
		{
			$command = 'a notification';
			if ($bEmail)
				$command .= ' with an email';

			$body = ' wants you to know that you can "mobilize" your Facebook photos with ' . FACEBOOK_APPNAME . '.  Add the <a href="' . FACEBOOK_CANVAS_URL . 'Mobilize">' . FACEBOOK_APPNAME . '</a> to your Facebook profile so you don\'t miss out.';
			$emailbody = ($bEmail ? $body : null);
			$bOK = SendNotification($to, $body, $emailbody);
		}
		else
		if ($bInvitation || $bRequest)
		{
			$command = ($bInvitation ? 'an invitation' : 'a request');
			$bOK = SendStandardInvitation($to);
		}
		else
		if ($bGift)
		{
			$command = 'a gift';
			$bOK = SendStandardGift($facebookUser, $to);
		}
		
		// set the request attributes for presenting by the view
		$request->setAttribute('bOK', $bOK);
		$request->setAttribute('command', $command);
		$request->setAttribute('nameRecipient', $nameRecipient);
		
		// returning index view because this is the only view for the Send page
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
