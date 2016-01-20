<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 3Guppies Inc                                           |
// +---------------------------------------------------------------------------+

/**
 * PostRemove action for the Facebook application
 *
 *
 * @author  Jim McCurdy
 */
class PostRemoveAction extends Action
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
	 * Executes the PostRemove Action
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
		// The following fields are sent to your Post-Remove URL in the form of a POST request.
		// The user uninstalling your application will not be redirected to this URL.
		// Facebook's servers send this request in the background. 

		// int		fb_sig_uninstall	Set to 1 to indicate the user is uninstalling your application
		// string	fb_sig_time			A UNIX timestamp indicating when the uninstall occurred (e.g. 1187756160.7131)
		// int		fb_sig_user			The uid of the person who is uninstalling your application (e.g. 609143784)
		// string	fb_sig_session_key	The session_key originally given to your application for the user who is uninstalling the application.
		// int		fb_sig_expires		The expires originally given to your application for the original session_key.
		// string	fb_sig_api_key		The api_key of your application that is being uninstalled.

		$userid = (int)Param('fb_sig_user');
		$bVerified = VerifySignature();
		LogLine($userid . ' removed ' .  FACEBOOK_APPNAME . ' (' . ($bVerified ? 'verified)' : 'NOT verified)'));

		// set the request attributes for presenting by the view
//j		$request->setParameter('NoCruft', 1);
		$request->setParameter('no_header_footer', 1);
		$request->setAttribute('userid', $userid);
		$request->setAttribute('bVerified', $bVerified);
		
		// returning index view because this is the only view for the PostRemove page
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
