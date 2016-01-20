<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * Mobilize Success view. Typically, the user is forwarded to some kind of 
 * gallery when they successfully mobilize content. This view is used for
 * instances where the user has mobilized via an AJAX method
 *
 * @see ExternalMobilize.class.php
 * @author  Eric Malone
 */

class MobilizeView extends View
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
        switch ( $ct )
        {
            case 'html':
            case 'xhtmlMobile':
            case 'wml':
                break;
            case 'async':
                $ct = 'async';
                break;
            default:
                $ct = 'html';
                break;
        }
        
        $renderer->setTemplate("MobilizeView_success.$ct.tpl");
        
        
        
        return $renderer;
    }

}

?>
