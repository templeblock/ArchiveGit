<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 Mixxer.                                                |
// +---------------------------------------------------------------------------+

require( dirname( __FILE__ ) . '/../lib/ProxyAuthenticator.class.php' );

/**
 * A proxy authentication form for sites like myspace. Will store the 
 * authentication cookie within the session
 *
 * @code
 * 
 *
 *
 *
 *
 * @author  Eric Malone
 */
class ProxyAuthAction extends Action
{
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
        return true;
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
        $rawUrl = $destUrl = (string) $request->getParameter( 'url' );

        /* Url fixup for lazy users */
        
        // Add http:// if url == 'www.foo.com/'
        if( strpos( $destUrl, 'http' ) === false )
            $destUrl = 'http://' . $destUrl;
        
        
        // Add a path if url == 'http://www.foo.com' (7 offest to skip http://)
        if( strpos( $destUrl, '/', 7 ) === false )
            $destUrl .= '/';
           
        // finally some validation
        if( strpos( $destUrl, '.' ) === false || 
            !$urlInfo = @parse_url( $destUrl ) )
        {
            $request->setError( 'url', 'The url entered is invalid' );
            $request->setAttribute( 'openl_proxy_url', $rawUrl );
            
            return VIEW_ERROR;
        }
        
        
        // factory us a proxy authenticator
        $domains = explode( '.', $urlInfo[ 'host' ] );
        $tld = array_pop( $domains );
        $secLD = array_pop( $domains );
        
        
        
        $proxyAuthInstance = ProxyAuthenticator::Factory( $user, $secLD . '.' . $tld );

        
        /* Process the request */
        
        
        // authenticate
        if( $request->hasParameter( 'authUsername' ) && 
            $request->hasParameter( 'authPassword' ) )
        {
            $username = (string) $request->getParameter( 'authUsername' );
            $password = (string) $request->getParameter( 'authPassword' );
            
            if( !$username || !$password )
            {
                $request->setError( 'name/pass', 'You must enter a username and password' );
                return VIEW_ERROR;
            }
            
            $proxyAuthInstance->initialize( $username, $password );
            
            $result = $proxyAuthInstance->execute();
            
            if( $returnToUrl = $request->getParameter( 'returnTo' ) )
            {
                // Specific for ExternalMobilize.... Remove this or update the
                // action to account for failure to authenticate
                if( !$result )
                {
                    $authCookies = $user->getAttribute( 'ProxyAuthCookies' );
                    
                    $authCookies[ $secLD . '.' . $tld ] = '';
                    $user->setAttribute( 'ProxyAuthCookies', $authCookies );
                }
                
                header( 'location: ' . $returnToUrl );
                exit( 'Please return to <a href="' . $returnToUrl . '">your original location</a>' );
            }
            
            else if( !$result )
            {
                $request->setError( 'name/pass', 'The authentication failed' );
                return VIEW_ERROR;
            }
            else
            {
                return VIEW_SUCCESS;
            }
        }
        
        else
        {
            $request->setAttributeByRef( 'openl_proxy_authInstance', $this->_proxyAuthInstance );
            
            return VIEW_INPUT;
        }
        
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

        return VIEW_INPUT;
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

        return REQ_GET | REQ_POST;

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
        // $validatorManager->setRequired('url', TRUE, 'You must enter a url to access');
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
