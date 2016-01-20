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
        // initialization of renderer
        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');
        
        $ct = $controller->getContentType();
        $renderer->setTemplate("ExternalMobilizeView_input.$ct.tpl");

        // determining the source of the scrape
        $src = $request->getParameter('src');
        switch($src)
        {
            case 'ffplugin':
            case 'ieplugin':
            case 'mobiwidg':
                break;
            default:
                $src = 'mobiwidg';
        }

        $renderer->setAttribute( 'openl_exter_src', $src );

        $renderer->setAttribute( 'openl_exter_scrapeUrl', 
            $request->getAttribute( 'openl_exter_scrapeUrl' ) );
        
        
        $renderer->setAttribute( 'openl_exter_widgetcode', 
            $request->getAttribute( 'openl_exter_widgetcode' ) );
        
        /* the extcmp is stored so that during the mobilization process we 
           have an idea of where the user came from. 
           NOTE: This is not used in the Mobilize url as 'extcmp' instead it is
           NOTE: used as 'ext_cmp' only for our tracking purposes. This value is
           NOTE: not sent to Omniture as an extcmp, which would be bad. */
        if($request->hasParameter('extcmp'))
        {
            $renderer->setAttribute( 'openl_exter_extcmp', $request->getParameter('extcmp') );
        }

        $request->setAttribute('SF_Nav_Selected','Mobilizer');
        
        $extcmp = $request->getParameter( 'extcmp' );
        if( $extcmp )
        {
            if( strpos( $extcmp, 'FFX' ) !== false )
                $renderer->setAttribute( 'openl_exter_ffPlugin', true );
            else if( strpos( $extcmp, 'IEX' ) !== false )
                $renderer->setAttribute( 'openl_exter_iePlugin', true );
        }
        
        return $renderer;
    }
}
?>
