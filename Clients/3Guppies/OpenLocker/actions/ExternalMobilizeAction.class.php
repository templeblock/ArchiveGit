<?php

require_once(dirname(__FILE__) . '/../lib/ProxyAuthenticator.class.php');
require_once(dirname(__FILE__) . '/../../Widget/lib/GenericScraper.class.php' );

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * these constants will be used client side in determining what action to take
 */


/**
 * Successful scrape. Contents will include the resulting html
 */
define( 'WIDGET_SUCCESS', 1 );


/**
 * will set if mixxer could not access the website, either via being blocked by
 * 404 or if the site is simply not accessible
 */
define( 'WIDGET_NOACCESS', 2 );


/**
 * if user didn't enter full url including http:// in the GET, will return 
 * VIEW_ERROR
 */
define( 'WIDGET_BADURL',  3 );


/**
 * no url was specified, either by the HTTP_REFERER or GET parameter
 */
define( 'WIDGET_NOURL', 4 );

/**
 * this error only used on the FE after scraping. Is used if absolutely NO media
 * was found on the page.
 */
define( 'WIDGET_NOMEDIA', 5 );

/**
 * this error only used on the FE. If request times out via javascript setTimeout,
 * will use this error
 */
define( 'WIDGET_TIMEOUT', 6 );


/**
 * if user attempts to mobilize a private profile on myspace we will show them 
 * the avatar and the ability to login
 */
define( 'WIDGET_MYSPACE_PRIVATE_PROFILE', 7 );


/**
 * if user entered a url for binary data by accident (music, an image, etc.)
 * then this code will be used. The message will be the actual url to go to
 */
define( 'WIDGET_REDIRECT', 8 );




/**
 * Corresponds to the Scraper (javascript) class's defined media type
 */
define( 'WIDGET_PICTURE', 0 );


/**
 * Corresponds to the Scraper (javascript) class's defined media type
 */
define( 'WIDGET_VIDEO', 1 );


/**
 * Corresponds to the Scraper (javascript) class's defined media type
 */
define( 'WIDGET_AUDIO', 2 );

/**
 * @brief External mobilize action widget.
 *
 * Users will be able to add the mobilizer widget link to any webpage (i.e.
 * myspace). There are two ways of reaching this action.
 * 1. Land here from the mobilizer widget link, which has the desired url in the
 *    query string OR from the HTTP_REFERER. Set the url as 
 *    a renderer parameter.
 * 2. Receive an Ajax request containing the url, in which this action will 
 *    fetch the url's contents and return them in the VIEW_SUCCESS.
 *
 *
 *
 * @author  Eric Malone
 */
class ExternalMobilizeAction extends Action
{
    /**
     * @var string the url that will be scraped. Determined in the REQ_GET 
     */
    var $_scrapeUrl;
    
    
    /**
     * @var int for use in REQ_POST, stores the widget error code if an error
     *      occurred. Need to output this code to user in the success template
     */
    var $_widgetCode;
    
    
    /**
     * @var string the response contents to be pushed as the result
     */
    var $_responseContents;
    
    
    /**
     * @var string authentication cookie string for accessing private content
     */
    var $_authCookieString;
    
    
    /**
     * @var array server-side parsed elements that can be embedded into the
     *      results
     */
    var $_serverEls;
    
    
    /**
     * @brief Execute action initialization procedure.
     */
    function initialize (&$controller, &$request, &$user)
    {
        $this->_widgetCode = 0;
        $this->_scrapeUrl = "";
        $this->_responseContents = "";
        $this->_serverEls = array();
        
        return TRUE;
    }

    /**
     * @brief Execute all business logic.
     */
    function execute (&$controller, &$request, &$user)
    {
        
        $request->setAttribute( 'openl_exter_scrapeUrl', $this->_scrapeUrl );
        
        $scraper = new GenericScraper( $this->_scrapeUrl, true );
        
        $request->setAttributeByRef( 'openl_exter_scraper', $scraper );
        
        // receiving request for the mobilize
        if( $request->getMethod() == REQ_POST  )
        {
            
            // attempt to authenticate the user given credentials
            $myspaceEmail = $request->getParameter( 'myspaceEmail' );
            $myspacePass = $request->getParameter( 'myspacePass' );
            
            if( $myspaceEmail && $myspacePass )
            {
                require( dirname(__FILE__) . '/../lib/myspace_comProxyAuthenticator.class.php');
                $myspaceAuth = new myspace_comProxyAuthenticator( 
                    $user, $myspaceEmail, $myspacePass );
                
                $myspaceAuth->execute();
            }
            
            
            // determine which scraper to use
            if( preg_match( '#profile\.myspace\.com|myspace\.com/[\w\d_]+$#', $this->_scrapeUrl ) )
            {
                require( dirname( __FILE__ ) . '/../../Widget/lib/MySpaceProfileScraper.class.php' );
                $scraper = new MySpaceProfileScraper( $user, $this->_scrapeUrl );
            }
            else
            {
                $scraper = new GenericScraper( $user, $this->_scrapeUrl );
            }
            
            $scraper->execute();
            
            if( $scraper->hasErrors() )
            {
                $errCodes = $scraper->getErrorCodes();

                if( ( $errCodes & SCRAPER_ERR_NOACCESS ) == SCRAPER_ERR_NOACCESS )
                {
                    $this->_widgetCode = WIDGET_NOACCESS;
                }
                else
                {
                    $this->_widgetCode = WIDGET_TIMEOUT;
                }
                
            }
            
            else if( !$scraper->responseIsText() )
            {
                $mobilizeUrl = $controller->genUrl( 
                    array( 
                        'module' => 'OpenLocker',
                        'action' => 'Mobilize'
                    ) ) . '?Url=' . $this->_scrapeUrl;
                
                if( $extcmp = $request->getParameter( 'extcmp' ) )
                    $mobilizeUrl .= '&extcmp=' . $extcmp;
                    
                $this->_widgetCode = WIDGET_REDIRECT;
                
                $request->setAttribute( 'openl_exter_contents', $mobilizeUrl );
            }
            
            else
            {
                $this->_responseContents = $scraper->getResponseText();
                $this->_widgetCode = WIDGET_SUCCESS;
    
                if( is_a( $scraper, 'MySpaceProfileScraper' ) )
                {
                    foreach( $scraper->getProfileEmbeddedVideos() as $video )
                    {
                        $this->_serverEls[] = array(
                            'type' => WIDGET_VIDEO,
                            'src'  => $video[ 'src' ],
                            'title' => $video[ 'title' ],
                            'flashAttrs' => $video[ 'flashAttrs' ]
                        );
                    }
                    
                    $request->setAttribute( 'openl_exter_prettyUrl', 
                        $scraper->getPrettyUrl() );
                    
                    $request->setAttribute( 'openl_exter_myspaceUserName', 
                        $scraper->getUserName() );
                    
                    $request->setAttribute( 'openl_exter_myspaceAvatarUrl', 
                        $scraper->getAvatarImgUrl() );
                    
                    if( $scraper->profileIsPrivate() )
                    {
                        // special handling of myspace private profile
                        $authCookies = $user->getAttribute( 'ProxyAuthCookies' );
    
                        
                        // at some point in the past, we've stored authentication
                        // cookie information for this profile. The fact that it
                        // still appears to be private says that the authentication
                        // likely failed
                        if( is_array( $authCookies ) && 
                            array_key_exists( $scraper->getDomain(), $authCookies ) )
                        {
                            $request->setAttribute( 'openl_exter_myspaceLoginError', true );
                        }
                        
                        $this->_widgetCode = WIDGET_MYSPACE_PRIVATE_PROFILE;
                    }
                    
                    // since user is likely authenticated to view this profile,
                    // the easiest way to determine if it is private is to
                    // perform a genric request on it
                    else
                    {
                        $scraper2 = new GenericScraper( $user, $this->_scrapeUrl );
                        $scraper2->execute( false );
                        
                        $response = $scraper2->getResponseText();
                        
                        
                        if( is_string( $response ) && 
                                strpos( $response, 'This profile is set to private' ) !== false )
                        {
                            $request->setAttribute( 'openl_exter_profileIsPrivate', true );
                        }
                        
                    }
                }
            }
            
            $request->setAttribute( 'openl_exter_widgetcode', $this->_widgetCode );
            $request->setAttribute( 'openl_exter_serverEls', $this->_serverEls );
            $request->setAttribute( 'openl_exter_contents', $this->_responseContents );
            
            return VIEW_SUCCESS;
        }
        else  // $request->getMethod() == REQ_GET
        {
            
            $binaryRegExp = '/(3gp|asf|avi|awb|bmp|dm|exe|flv|gcd|gif|jad|jar|jpg|m4a|mcd|mel|mid|mid|mov|mp3|mp4|mpg|ogg|ogm|pmd|png|qcp|qt|rm|smaf|swf|txt|wav|wmv)\s*$/i';
            
            if( preg_match( $binaryRegExp, $this->_scrapeUrl ) )
            {
                $mobilizeUrl = $controller->genUrl( 
                    array( 
                        'module' => 'OpenLocker',
                        'action' => 'Mobilize'
                    )
                ) . '?Url=' . rawurlencode( $this->_scrapeUrl );
                
                if( $extcmp = $request->getParameter( 'extcmp' ) )
                    $mobilizeUrl .= '&extcmp=' . $extcmp;
                    
                header( 'location: ' . $mobilizeUrl );
                exit( 'Please click <a href="' . $mobilizeUrl . '">here</a> to be redirected' );
            }
            
            $request->setAttribute( 'openl_exter_scrapeUrl', $this->_scrapeUrl );
            $request->setAttribute( 'openl_exter_widgetcode', $this->_widgetCode );
            
            return VIEW_INPUT;
        }
    }

    /**
     * @brief Validate the request as a whole.
     */
    function validate (&$controller, &$request, &$user)
    {
        $scrapeUrl = '';
        
        $reqMethod = $request->getMethod();
        
        // determine url
        if( $reqMethod == REQ_POST )
        {
            $scrapeUrl = $request->getParameter( 'sUrl' );
        }
        else if( $reqMethod == REQ_GET )
        {
            if( $request->hasParameter( 'sUrl' ) )
            {
                $scrapeUrl = $request->getParameter( 'sUrl' );
            }
            else if( !empty( $_SERVER[ 'HTTP_REFERER' ] ) )
            {
                $scrapeUrl = $_SERVER[ 'HTTP_REFERER' ];
            }
        }
        
        if( !$scrapeUrl )
        {
            $this->_widgetCode = WIDGET_NOURL;
        }
        else
        {
            $return = GenericScraper::VALIDATE_URL( $scrapeUrl );
                
            if( is_string( $return ) )
            {
                $this->_scrapeUrl = $return;
            }
            else
            {
                // At least for now, it's easiest just to tell the user that there
                // was no url. NO URL and BAD URL should be rolled into one
                $this->_widgetCode = WIDGET_NOURL;
            }
        }
        
        return true;
    }

    /**
     * @brief Retrieve the default view.
     */
    function getDefaultView (&$controller, &$request, &$user)
    {
        return VIEW_INPUT;
    }

    /**
     * @brief Retrieve the privilege required to access this action.
     */
    function getPrivilege (&$controller, &$request, &$user)
    {
        return NULL;
    }

    /**
     * @brief Retrieve the HTTP request method(s) this action will serve.
     */
    function getRequestMethods ()
    {
        return REQ_GET | REQ_POST;
    }

    /**
     * @brief Handle a validation error.
     */
    function handleError (&$controller, &$request, &$user)
    {
        return VIEW_ERROR;
    }

    /**
     * @brief Determine if this action requires authentication.
     */
    function isSecure ()
    {
        return FALSE;
    }

    /**
     * @brief Register individual parameter validators.
     */
    function registerValidators (&$validatorManager, &$controller, &$request,
                                 &$user)
    {
        // NONE
    }
}

?>
