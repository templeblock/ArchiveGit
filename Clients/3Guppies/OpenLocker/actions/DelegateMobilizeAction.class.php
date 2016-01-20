<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * Delegates the generic mobilize url to the specific mobilize action
 *
 * @author  Eric Malone
 */
class DelegateMobilizeAction extends Action
{
    
    /**
     * @var object internal reference to the controller
     */
    var $_controller;
    
    
    /**
     * @var object internal reference to the request
     */
    var $_request;
    
    /**
     * Execute action initialization procedure.
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
        $this->_controller = $controller;
        $this->_request = $request;
        
        return TRUE;

    }
    
    
    /**
     * Execute all business logic.
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

        /********************************************
         src can be any of the following. Or if it is left off, we will see if 
         either the request parameter 'url' or 'surl' are used. If nothing else,
         then fallback to the widget.
         
         The firefox (and future IE) plugins have two possible outcomes. Either
         the widget (as specified by request parameter 'surl') or the regular
         upload mobilize (as specified by the request parameter 'url')
         
         mobiwidg – Mobilization widget
         ieplugin – IE plugin
         ffplugin – Firefox plugin
         upload – Uploading (either anonymously or logged in) on Mixxer’s website
         mixweb – Mixxer’s website, including mobilization initiated from the gallery and locker, but not upload
         mixwap – n/a today, but reserved for mixxer’s wap site
         txtme – formerly known as the SMS sender
         forward – n/a today, just reserving for the future
         reply – ditto
        ********************************************/
        
        $src = $request->getParameter( 'src' );

        switch( $src )
        {
            case 'mobiwidg':
                $this->forwardWidget();
                break;
            
            case 'ieplugin':
            case 'ffplugin':
                $this->determineForward();
                break;
                
            case 'upload':
                $this->forwardMobilize();
                break;
                
            default:
                $this->forwardWidget();
                break;
        }
        
        return VIEW_NONE;

    }

    /**
     * Forward to the widget page using the parameter 'surl', the HTTP_REFERER
     * or nothing
     */
    function forwardWidget()
    {
        if( $this->_request->hasParameter( 'surl' ) )
            $scrapeUrl = $this->_request->getParameter( 'surl' );
        
        // in case the regular url is used
        else if( $this->_request->hasParameter( 'url' ) )
            $scrapeUrl = $this->_request->getParameter( 'url' );
        
        else if( isset( $_SERVER[ 'HTTP_REFERER' ] ) )
            $scrapeUrl = $_SERVER[ 'HTTP_REFERER' ];
        else
            $scrapeUrl = null;
            
        if( $scrapeUrl )
            $scrapeUrl = rawurlencode( $scrapeUrl );
        
        $extcmp = $this->_request->getParameter( 'extcmp' );
        
        $redirect = $this->_controller->genUrl( 
                array( 'module' => 'OpenLocker', 
                       'action' => 'ExternalMobilize' ) );
            
        $redirect .= $extcmp ? '?extcmp=' . $extcmp . '&' : '?';

        $redirect .= 'sUrl=' . $scrapeUrl;
        
        header( 'location: ' . $redirect );
        exit();
    }
    
    /**
     * forward user to the mobilize action
     */
    function forwardMobilize()
    {
        $mobilizeUrl = null;
        
        if( $this->_request->hasParameter( 'url' ) )
            $mobilizeUrl = $this->_request->getParameter( 'url' );
        
        if( $mobilizeUrl )
            $this->_request->setParameter( 'Url', $mobilizeUrl );
        
        $this->_controller->forward( 'OpenLocker', 'Mobilize' );
    }
    
    /**
     * Based on either the 'url' or 'surl' request parameters, forward to a 
     * particular mobilize action
     */
    function determineForward()
    {
        if( $this->_request->hasParameter( 'url' ) )
            $this->forwardMobilize();
        else
            $this->forwardWidget();
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
     * Determine if this action requires authentication.
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
    function registerValidators (&$validatorManager, &$controller, &$request,
                                 &$user)
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
