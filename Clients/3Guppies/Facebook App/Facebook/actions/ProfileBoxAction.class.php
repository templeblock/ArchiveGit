<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 3Guppies Inc                                           |
// +---------------------------------------------------------------------------+

// Because I was not allowed to change /3g/widget/FlashWidget.class.php, I made my code changes in a derived class
// All I am doing is passing in 2 optional arguments to the generateCode method: $templateName and $uniqueString
// $templateName allows the caller to control which *.tpl file is used to create the widget; the Facebook widget 
//		needs to use an FBML tag <fb:swf> to instantiate the SWF, and needs to be about 91.3% of the original size
// $uniqueStringis allows the caller to control the guid used in the creation of the $widgetId; the widgetId for
//		Facebook users should be uniquely tied to their Facebook userid so that, once provisioned, can be uniquely tied to a 3G account
// I would prefer to make these changes directly in /3g/widget/FlashWidget.class.php to avoid a duplication of code
// All lines changed from the original FlashWidget class are commented //j
class FlashWidget1 extends FlashWidget {
    function generateCode( $userId = null, $extcmp = null, $widgetId = null,
        $noAnchors = false, $useSwfObject = false, $state = WIDGET_PROVISION_NONE, $templateName = null, $uniqueString = null )
    {
        
        $widgetQueryString = '';
        $moblogQueryString = '';
        
        // In some cases we do pass 0 as a widget id. 0 is "null" but will pass
        // the is_numeric check. It'll bypass creating a new widget id
        if( $widgetId == null && !is_numeric( $widgetId ) )
        {
            $widgetId = $this->generateGuid( $extcmp, $uniqueString );
        }
        
        // user id specified, no need to generate a random guid for widget
        if( $userId )
        {
            $widgetQueryString = "g3_userid=" . $userId . "&g3_widgetid=";
            $moblogQueryString = "&userid=" . $userId;
        }
        else
        {
            $widgetQueryString = "g3_widgetid=";
            $moblogQueryString = "&widget_id=" . $widgetId;
        }
        
        $widgetQueryString .= $widgetId;
        
        // for use in the template, will need the fully qualified extcmp
        if( $extcmp )
            $extcmp = '&extcmp=' . $extcmp;
        
        // using renderer to generate the widget code from a template in 3g/
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $user    =& $controller->getUser();
        
        $renderer =& $request->getAttribute('SmartyRenderer');
        
        $renderer->setAttribute( 'userId', $userId );
        $renderer->setAttribute( 'extcmp', $extcmp );
        $renderer->setAttribute( 'widgetId', $widgetId );
        $renderer->setAttribute( 'widgetQueryString', $widgetQueryString );
        $renderer->setAttribute( 'moblogQueryString', $moblogQueryString );
        $renderer->setAttribute( 'state', $state);
        
//j I changed the block below
        if( $useSwfObject )
            AddHeaderFile::addFile( 'javascript/swfobject.js' );

        if( !$templateName )
            $templateName = ($useSwfObject ? 'swfobject.tpl' : 'default.tpl');
        $renderer->setTemplate( CUSTOM_LIB_DIR . 'templates/widget/flash/' . $templateName);
//j I changed the block above
        
        $code =& $renderer->fetchResult( &$controller, &$request, &$user );
        
        $renderer->clearResult();
        
        // replacing 'www' with 'ww' because myspace is blocking 'www.3guppies.com'
        $patts    = array( '#>[\s\n\r]+<#', '#www\.3guppies#' );
        $replaces = array( '><', 'ww.3guppies' );
        
        if( $noAnchors )
        {
            $patts[]    = '#href=".+?"( target="_blank")?#';
            $replaces[] = 'href="#"';
        }
        
        $code = preg_replace( $patts, $replaces, $code );
        
        return $code;
    }

    function generateGuid( $extcmp = null, $uniqueString = null )
    {
//j I changed the block below
        if( !$uniqueString )
            $uniqueString  = md5( microtime() . `uuidgen` . mt_rand() );
        return ( $extcmp ? $extcmp . '-' : '' ) . $uniqueString . '-' . time();
//j I changed the block above
    }
}

/**
 * ProfileBox action for the Facebook application
 *
 *
 * @author  Jim McCurdy
 */
class ProfileBoxAction extends Action
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
	 * Executes the ProfileBox Action
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

		// fetching facebook information
		$userid = Param('user');
		if (!$userid)
			$userid = Param('fb_sig_user');

		// Generate a widget based on the unique Facebook userid and a standard Facebook GUID
		$uniqueFacebookGuid = '0eecba107d484c33abcd9b694939853c';
		$uniqueString = $userid . substr($uniqueFacebookGuid, strlen($userid));
		$extcmp = 'FBPE10310';
		$templateName = 'facebook.tpl';
		$widget =& new FlashWidget1();
		$widgetCode = $widget->generateCode(null/*3gUserId*/, $extcmp, null/*widgetId*/, false/*noAnchors*/, false/*useSwfObject*/, WIDGET_PROVISION_NONE/*$state*/, $templateName, $uniqueString);

		$lastVisited = StdDate();
		$lastVisitedDaysAgo = TheDate(time(), true/*relative*/);

		// set the request attributes for presenting by the view
		$request->setParameter('NoCruft', 1);
//j		$request->setParameter('no_header_footer', 1);
		$request->setAttribute('userid', $userid);
		$request->setAttribute('lastVisited', $lastVisited);
		$request->setAttribute('lastVisitedDaysAgo', $lastVisitedDaysAgo);
		$request->setAttribute('widgetCode', $widgetCode);

		// returning index view because this is the only view for the ProfileBox page
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
