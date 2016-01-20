<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * An abstract class used to 
 *
 * @code
 * 
 * 
 * @endcode
 *
 * @abstract
 *
 * @author  Eric Malone
 */
class myspace_comProxyAuthenticator extends ProxyAuthenticator
{
    /**
     * @var string the pretty url of the profile, captured when validating
     *      authentication
     */
    var $_prettyUrl;
    
    function myspace_comProxyAuthenticator( &$user, $email = '', $password = '' )
    {
        parent::ProxyAuthenticator( $user, 'myspace.com', $email, $password );
        
        $this->_authUrl = 'http://login.myspace.com/index.cfm?fuseaction=login.process';
        $this->_verifyAuthSucceededUrl = 'http://home.myspace.com/index.cfm?fuseaction=user';
        $this->_userName = 'email';
        $this->_passName = 'password';
        
        $this->_domain = 'myspace.com';
        
        $this->_prettyUrl = '';
    }
    
    
    /**
     * Returns the pretty url of the profile if the user was successfully
     * authenticated
     */
    function getPrettyUrl()
    {
        return $this->_prettyUrl;
    }
    
    /**
     * @link Originally source from 
     * http://www.harrymaugans.com/2007/02/07/myspace-login-with-php-and-curl/
     */
    function _doExecute()
    {
        if( !is_resource( $this->_curlHandle ) )
        {
            $this->_addError( 'curl not valid in _doExecute' );
            return false;
        }
        
        // get the login token
        curl_setopt( $this->_curlHandle, CURLOPT_URL, "http://www.myspace.com" );
        
        $this->_responseContents = curl_exec( $this->_curlHandle );

        if( $err = curl_error( $this->_curlHandle ) )
        {
            $this->_addError( 'Error in first request to MySpace: ' . $err . 
                "<br><pre>" . print_r( curl_getinfo( $this->_curlHandle ), true ) . "</pre>" );
            $this->_authSuccess = false;
            return false;
        }
        
        $token = array();
        
        preg_match( '#MyToken=([^"]+)"#', $this->_responseContents, $token );

        if( !$token )
        {
            $this->_addError( 'token not available for authentication' );
            $this->_authSuccess = false;
            return false;
        }
        
        $token = $token[1];

        // POST to myspace. Redirects will be handled automatically
        curl_setopt( $this->_curlHandle, CURLOPT_URL, $this->_authUrl . "&MyToken={$token}" );
        curl_setopt( $this->_curlHandle, CURLOPT_REFERER, "http://www.myspace.com/" );
        curl_setopt( $this->_curlHandle, CURLOPT_HTTPHEADER, 
            array( "Content-Type: application/x-www-form-urlencoded" ) );
        curl_setopt( $this->_curlHandle, CURLOPT_POST, true );

        curl_setopt( $this->_curlHandle, CURLOPT_POSTFIELDS, 
            $this->_postString . 
            '&ctl00%24Main%24SplashDisplay%24login%24loginbutton.x=38&ctl00%24' .
            'Main%24SplashDisplay%24login%24loginbutton.y=15' );
        
        $this->_responseContents = curl_exec( $this->_curlHandle );

        $returnVal = false;
        
        if( $err = curl_error( $this->_curlHandle ) )
        {
            $this->_addError( 'Error in second request in _doExecute: ' . $err .
                "<br><pre>" . print_r( curl_getinfo( $this->_curlHandle ), true ) . "</pre>" );
        }
        else
        {
            $returnVal = true;
        }

        return $returnVal;
    }
    
    function _verifyAuthSucceeded()
    {
        if( !$this->_initializeCurl() )
            return false;
        
        
        if( !$this->_authCookieStr )
        {
            $this->_addError( 'no cookie string to work from in verifying authentication' );
            return false;
        }
        
        curl_setopt( $this->_curlHandle, CURLOPT_URL, $this->_verifyAuthSucceededUrl );
        
        curl_setopt( $this->_curlHandle, CURLOPT_HTTPGET, true );

        curl_setopt( $this->_curlHandle, CURLOPT_COOKIE, $this->_authCookieStr );
        
        $resp = curl_exec( $this->_curlHandle );
        
        
        if( strpos( $resp, 'Hello,' ) !== false )
        {
            $match = array();
            if( preg_match( '#<strong>My URL</strong><br[^>]*>[\s\n]+([^<]+)<br#', $resp, $match ) )
            {
                $this->_prettyUrl = $match[1];
            }
            
            return true;
        }
        else
        {
            return false;
        }
    }
}
