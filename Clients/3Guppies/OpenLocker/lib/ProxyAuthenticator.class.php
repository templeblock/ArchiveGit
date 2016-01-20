<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 Mixxer.                                                |
// +---------------------------------------------------------------------------+

define( 'PROXY_AUTH_TYPE_TEXT',     1 );
define( 'PROXY_AUTH_TYPE_PASSWORD', 2 );
define( 'PROXY_AUTH_TYPE_CAPTCHA',  3 );

/**
 * An abstract class which performs the act of authenticating a user for the 
 * specified site. Will store their cookie information into the user session.
 *
 * This can also give the VIEW information about the human readable
 * names to use for authentication to other websites.
 *
 * Domain-specific subclasses MUST follow the 'domain_comProxyAuthenticator'
 * file and classname structure to be 'factoryable'
 *
 * Subclasses should be able to override the constructor/initialize functions
 * add additional login parameters such as tokens, phone numbers, etc. This 
 * class is NOT meant to be the be-all and end-all of authentication schemes. 
 * Resolving the method for captcha logins remains.
 * 
 * Subclassing
 * All concrete subclasses need to override
 * 1. _authUrl - the private variable that will be accessed in authentication
 * 2. _doExecute - the function call that performs the actual execution
 * 3. _verifyAuthSucceeded - the function call used to verify that the 
 *    authentication succeeded
 * 4. _verifyAuthSucceededUrl - (optionally) the url to be used in a request
 *    to verify that the authentication was successful
 *
 * @code
 *
 *     ///   Simple factory creation based on a domain name, and then execute   ///
 *     $proxyAuth = ProxyAuthenticator::Factory( $user, 'myspace.com' );
 *     if( !$proxyAuth )
 *         exit( 'site doesnt support authentication' );
 *     
 *     $proxyAuth->initialize( 'guy@gmail.com', 'mypassword' );
 *     $proxyAuth->execute();
 *     if( $proxyAuth->hasErrors() )
 *         exit( "Sorry, you cannot access that site" );
 *
 *     
 *     ///   Generating and running this authenticator    ///
 *     $proxyAuth = new ProxyAuthenticator( $user, 'guy@gmail.com', 'mypassword' );
 *     if( !$proxyAuth->execute() )
 *         exit( implode( '<br>', $proxyAuth->getErrors() ) );
 *
 *
 *     ///   retrieval and usage of the user's myspace.com authentication cookie
 *            on a different page after running this object   ///
 *     $cookie = $user->GetAttribute( 'ProxyAuthCookies' );
 *     if( isset( $cookie[ 'myspace.com' ] ) )
 *         curl_setopt( $some_curl_handle, CURLOPT_COOKIE, $cookies[ 'myspace.com' ] );
 * @endcode
 *
 * @abstract
 *
 * @author  Eric Malone
 */
class ProxyAuthenticator
{
    /**
     * @var Object reference to the user object for storing cookie session info
     */
    var $_user;
    
    
    /**
     * @var string the destination domain we are authenticating to
     */
    var $_domain;
    
    
    /**
     * @var string filename cookie jar for these cookies
     */
    var $_cookieJar;
    
    
    /**
     * @var resource curl handle for sending out the authentication request
     */
    var $_curlHandle;
    
    
    /**
     * This variable MUST be overwritten by child class
     *
     * @var string the authentication url that the authentication request will
     * be sent to
     *
     * @abstract
     */
    var $_authUrl;
    
    
    /**
     * @var string the url to access to test if authentication has succeeded
     *
     * @abstract
     */
    var $_verifyAuthSucceededUrl;
    
    
    /**
     * @var string user agent to use when sending out the request
     */
    var $_userAgent;
    
    
    /**
     * @var string the form name to use for the user field of authentication
     */
    var $_userName;
    
    
    /**
     * @var string username / email used to authenticate the user
     */
    var $_userVal;
    
    /**
     * @var string the form name to use for the password field of authentication
     */
    var $_passName;
    
    
    /**
     * @var string password value used to authenticate the user
     */
    var $_passVal;
    
    
    /**
     * @var string the encoded POST string that will be sent to authenticate a user
     */
    var $_postString;
    
    
    /**
     * @var string the cookies set by the authentication process
     */
    var $_authCookieStr;
    
    
    /**
     * @var string the response contents from an authentication request
     */
    var $_responseContents;
    
    
    /**
     * @var boolean whether the authentication was successful
     */
    var $_success;
    
    
    /**
     * @var array contains string errors
     */
    var $_errors;
    
    
    /**
     * Constructor
     *
     * @param User $user The current user object to get/store authentication cookies
     *        to/from
     * @param string $domain the domain for which to authenticate
     * @param string $userVal the value of the 'username' or 'email' to login as
     * @param string $passVal the value of the password field for authentication
     * @param string $userName the 'username' name value for the particular site
     * @param string $passName the 'password' name value for the particular site
     */
    function ProxyAuthenticator( &$user, $domain, $userVal = '', $passVal = '', 
                                 $userName = 'email', $passName = 'password' )
    {
        $this->_user = $user;
        $this->_domain = (string) $domain;
        $this->_userVal  = (string) $userVal;
        $this->_userName = (string) $userName;
        
        $this->_passVal  = (string) $passVal;
        $this->_passName = (string) $passName;
        
        $this->_success = false;
        $this->_responseContents = '';
        $this->_authCookieStr = '';
        $this->_errors = array();
        $this->_authUrl = '';
        $this->_verifyAuthSucceededUrl = '';
        $this->_curlHandle = null;
        
        $this->_userAgent = !empty( $_SERVER[ 'HTTP_USER_AGENT' ] ) ? 
                $_SERVER[ 'HTTP_USER_AGENT' ] : 
                "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3";
        
        
        // attempt to get existing cookie data
        $cookieData = $this->_user->getAttribute( 'ProxyAuthCookies' );
        
        if( isset( $cookieData[ $this->_domain ] ) && 
            !empty( $cookieData[ $this->_domain ] ) )
        {
            $this->_authCookieStr = $cookieData[ $this->_domain ];
        }
        
        
        do {
            $this->_cookieJar = '/tmp/authjar-' . mt_rand( 1, 100000 );
            
        } while( file_exists( $this->_cookieJar ) );
    }

    
    /**
     * Factory method for child classes. Since authentication is domain specific
     * we are factory-ing based on domain
     *
     * @static
     *
     * @param User $user Reference to the user object which the cookie will be 
     *        added to (globally)
     * @param string $domain destination domain to perform authentication for
     */
    function Factory( &$user, $domain )
    {
        $cleanDomain = str_replace( '.', '_', $domain );
        
        $classname = $cleanDomain . 'ProxyAuthenticator';
        
        $filename = $cleanDomain . 'ProxyAuthenticator.class.php';
        $fullpath = dirname( __FILE__ ) . "/" . $filename;
        

        if( file_exists( $fullpath  ) )
        {
            require( $fullpath );
            return new $classname( $user );
        }
        else
        {
            return false;
        }
    }
    
    
    /**
     * Performs actual execution of authentication
     *
     * @abstract
     */
    function _doExecute()
    {
        exit( '_doExecute parent' );
        return true;
    }
    
    
    /**
     * Validates that the user has been authenticated to the site
     *
     * @return boolean true if the user authenticated, false otherwise
     * @abstract
     */
    function _verifyAuthSucceeded()
    {
        exit( "_verifyAuthSucceeded parent" );
        return true;
    }
    
    
    /**
     * Creates the curl handle resource
     *
     *
     * @param boolean true if curl was initialized, false otherwise
     * @private
     */
    function _initializeCurl( $cookieJar = false )
    {
        if( is_resource( $this->_curlHandle ) )
            curl_close( $this->_curlHandle );
        
        if( !$this->_authUrl )
        {
            $this->_addError( 'No authentication url provided' );
            return false;
        }

        $this->_curlHandle = curl_init();
        
        if( !is_resource( $this->_curlHandle ) )
        {
            $this->_addError( 'Unable to make curl connection' );
            return false;
        }
        
        curl_setopt( $this->_curlHandle, CURLOPT_FOLLOWLOCATION, true );
        curl_setopt( $this->_curlHandle, CURLOPT_MAXREDIRS, 10 );
        
        if( $cookieJar )
            curl_setopt( $this->_curlHandle, CURLOPT_COOKIEJAR, $this->_cookieJar );
        
        curl_setopt( $this->_curlHandle, CURLOPT_USERAGENT, $this->_userAgent );
        curl_setopt( $this->_curlHandle, CURLOPT_TIMEOUT, 30 );
        curl_setopt( $this->_curlHandle, CURLOPT_HEADER, true );
        curl_setopt( $this->_curlHandle, CURLOPT_RETURNTRANSFER, true );

        if( !is_resource( $this->_curlHandle ) )
        {
            $this->_addError( 'Unable to make curl connection' );
            return false;
        }
        
        return true;
    }
    
    
    /**
     * Performs storage of the authentication cookie into the session
     *
     * @private
     */
    function _storeAuthCookieString()
    {
        $authCookies = $this->_user->GetAttribute( 'ProxyAuthCookies' );

        if( !$authCookies )
            $authCookies = array();
        
        $authCookies[ $this->_domain ] = $this->_authCookieStr;

        $this->_user->setAttribute( 'ProxyAuthCookies', $authCookies );
    }
    
    
    /**
     * Stores an empty value into this domain's cookie value. Signifies an
     * attempt but failure to authenticate
     *
     * @private
     */
    function _setAuthCookieFailure()
    {
        $authCookies = $this->_user->GetAttribute( 'ProxyAuthCookies' );

        if( !$authCookies )
            $authCookies = array();
        
        $authCookies[ $this->_domain ] = '';

        $this->_user->setAttribute( 'ProxyAuthCookies', $authCookies );
    }
    
    
    /**
     * Adds an error message into this object's error list
     *
     * @private
     */
    function _addError( $str )
    {
        $this->_errors[] = $str;
    }
    
    
    /**
     * Closes connections and finalizes this object for GC
     * @private
     */
    function _close()
    {
        if( is_resource( $this->_curlHandle ) )
            curl_close( $this->_curlHandle );
        
        @unlink( $this->_cookieJar );
    }
    
    
    function _getAuthCookieStr()
    {
        
        // need to close the cookie jar before being able to get contents from it
        if( is_resource( $this->_curlHandle ) )
            curl_close( $this->_curlHandle );
        
        $cookieJar = file_get_contents( $this->_cookieJar );

        $authCookieStr = '';
        
        $matches = array();
        preg_match_all( '#([^\t]*\t){5}([^\t]*)\t(.*)#', $cookieJar, $matches, PREG_SET_ORDER );

        foreach( $matches as $matchArr )
        {
            if( count( $matchArr ) != 4 || !$matchArr[ 2 ] || !$matchArr[ 3 ] )
                continue;
            
            $authCookieStr .= 
                $matchArr[2] . '=' . $matchArr[3] . '; ';
        }

        @unlink( $this->_cookieJar );
        
        return $authCookieStr;
    }
    
    /**
     * Called to perform the proxy authentication. Upon success, will store the
     * returned cookies as an array of the 'ProxyAuth' user privilege
     *
     * @return boolean true if the authentication process completed successfully,
     *         false otherwise
     */
    function execute()
    {
        if( !$this->_domain )
        {
            $this->_addError( 'Unable to determine domain' );
            $this->_success = false;
            return false;
        }
        
        
        if( $this->_authCookieStr )
        {
            // error already handled in _initializeCurl
            if( !$this->_initializeCurl() )
            {
                $this->_success = false;
                return false;
            }

            if( $this->_verifyAuthSucceeded() )
            {
                $this->_success = true;
                return true;
            }
            else
            {
                $this->_authCookieStr = '';
            }
        }

        // error already handled in _initializeCurl
        if( !$this->_initializeCurl( true ) )
        {
            $this->_success = false;
            return false;
        }
        
        else if( !$this->_userName || !$this->_passName || !$this->_userVal || !$this->_passVal )
        {
            $this->_addError( 'User and password values must be set' );
            $this->_success = false;
            return false;
        }
        
        
        $this->_postString = rawurlencode( $this->_userName ) . '=' . 
                             rawurlencode( $this->_userVal ) . '&' .
                             rawurlencode( $this->_passName ) . '=' .
                             rawurlencode( $this->_passVal );

        if( !$this->_doExecute() )
        {
            $this->_close();
            
            $this->_addError( 'authentication failed in _doExecute' );
            $this->_success = false;
            $this->_setAuthCookieFailure();
            return false;
        }
        
        $this->_authCookieStr = $this->_getAuthCookieStr();
        
        
        if( !$this->_verifyAuthSucceeded() )
        {
            $this->_authCookieStr = '';
            $this->_close();
            
            $this->_addError( 'authentication failed in _verifyAuthSucceeded' );
            $this->_success = false;
            $this->_setAuthCookieFailure();
            return false;
        }

        $this->_storeAuthCookieString();
        
        $this->_close();
        
        $this->_success = true;

        return true;
    }
    
    
    function isAuthenticated()
    {
        if( !$this->_initializeCurl() )
        {
            $this->_addError( 'Unable to initialize curl' );
            return false;
        }
        
        return $this->_verifyAuthSucceeded();
    }
    
    
    /**
     * Object initialization
     */
    function initialize( $userVal, $passVal )
    {
        $this->_userVal = (string) $userVal;
        $this->_passVal = (string) $passVal;
    }
    
    
    function removeAuthCookie()
    {
        // remove this object's cookie
        $this->_authCookieStr = '';
        
        // remove the session's cookie
        $cookieData = $this->_user->getAttribute( 'ProxyAuthCookies' );
        
        if( isset( $cookieData[ $this->_domain ] ) && 
            !empty( $cookieData[ $this->_domain ] ) )
        {
            $cookieData[ $this->_domain ] = '';
        }
        
        $this->_user->setAttribute( 'ProxyAuthCookies', $cookieData );
        
    }
    
    
    /**
     * Determines if this object has any errors
     * @return boolean true if errors exist, false otherwise
     */
    function hasErrors()
    {
        return count( $this->_errors ) ? true : false;
    }
    
    
    /**
     * Returns errors occurred in authentication
     * @return array errors
     */
    function getErrors()
    {
        return $this->_errors;
    }
    
    
    /**
     * returns the name value for the site's 'user name'. Some sites may use
     * 'email' while others use 'username'
     * @return string
     */
    function getUserName()
    {
        return $this->_userName;
    }
    
    
    /**
     * returns the name value for the site's 'password' field
     * @return string
     */
    function getPassName()
    {
        return $this->_passName;
    }
    
    
    /**
     * sets the user value
     * @param string the email address or username to be used in authentication
     */
    function setUserVal( $val )
    {
        $this->_userVal = (string) $val;
    }
    

    /**
     * sets the password value
     * @param string the email address or username to be used in authentication
     */
    function setPassVal( $val )
    {
        $this->_passVal = (string) $val;
    }
    
    
    /**
     * Returns whether the authentication was successful. Also queriable from
     * ->execute
     * @return boolean true if authentication successful, false otherwise
     */
    function getAuthSuccess()
    {
        return $this->_success;
    }
    
    
    /**
     * Upon successful authentication, returns the user's cookie string
     * @return string if successful, user's session cookie string. Empty string
     *         otherwise
     */
    function getAuthCookieStr()
    {
        return $this->_authCookieStr;
        
    }
}
?>
