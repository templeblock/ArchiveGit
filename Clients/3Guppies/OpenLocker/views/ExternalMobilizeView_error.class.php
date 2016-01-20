<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * External Mobilize Error View
 *
 * @author  Eric Malone
 */

class ExternalMobilizeView extends View
{

    /**
     * @brief Render the error.
     *
     * @return Renderer A Renderer instance.
     */
    function & execute (&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');

        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');

        $ct = $controller->getContentType();

        $renderer->setTemplate("ExternalMobilizeView_error.$ct.tpl");
        
        
        // failed to retrieve url from the initial request
        if( $errorCode = $request->getError( 'Widget' ) )
        {
            $renderer->setAttribute( 'openl_exter_errorcode', $errorCode );
        }
        
        $request->setAttribute('SF_Nav_Selected','Mobilizer');
        
        return $renderer;
    }
}
?>
