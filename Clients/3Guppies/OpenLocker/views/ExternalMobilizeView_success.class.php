<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * General Documentation Goes Here
 *
 * @author  Eric Malone
 */

class ExternalMobilizeView extends View
{

    /**
     * @brief Render the presentation.
     *
     * @return Renderer A Renderer instance.
     */
    function & execute (&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');

        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');

        $ct = $controller->getContentType();

        $renderer->setTemplate("ExternalMobilizeView_success.$ct.tpl");

        $widgetCode = $request->getAttribute( 'openl_exter_widgetcode' );
        $renderer->setAttribute( 'openl_exter_widgetcode', $widgetCode );
        

        
        $contents = $request->getAttribute( 'openl_exter_contents' );
        $domain   = $request->getAttribute( 'openl_exter_domain' );
        
        // cleanup for the send over to client
        $contents = preg_replace( "/[\s\n\r]+/", " ", $contents );
        $contents = str_replace( "'", "\'", $contents );
        
        $renderer->setAttribute( 'openl_exter_domain', $domain );
        $renderer->setAttribute( 'openl_exter_contents', $contents );
        
        
        $renderer->setAttribute( 'openl_exter_scrapeUrl', 
            $request->getAttribute( 'openl_exter_scrapeUrl' ) );
        
        
        $renderer->setAttribute( 'openl_exter_prettyUrl', 
            $request->getAttribute( 'openl_exter_prettyUrl' ) );
        
        
        $renderer->setAttribute( 'openl_exter_myspaceUserName', 
            $request->getAttribute( 'openl_exter_myspaceUserName' ) );
        
        $renderer->setAttribute( 'openl_exter_myspaceAvatarUrl', 
            $request->getAttribute( 'openl_exter_myspaceAvatarUrl' ) );
        
        $renderer->setAttribute( 'openl_exter_myspaceLoginError', 
            $request->getAttribute( 'openl_exter_myspaceLoginError' ) );
        
        $request->setAttribute('SF_Nav_Selected','Mobilizer');
        
        // any server scraped elements
        if( $serverEls = $request->getAttribute( 'openl_exter_serverEls' ) )
            $renderer->setAttribute( 'openl_exter_serverEls', $serverEls );
        
        
        $renderer->setAttribute( 'openl_exter_profileIsPrivate',
            $request->getAttribute( 'openl_exter_profileIsPrivate' ) );
        
        return $renderer;
    }
}
?>
