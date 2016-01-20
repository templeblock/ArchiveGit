<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief General Documentation Goes Here
 *
 * @author  Blake Matheny
 */

class ShareView extends View
{

    /**
     * @brief Render the presentation.
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
            default:
                $ct = 'html';
                break;
        }
        $renderer->setTemplate("ShareView_error.$ct.tpl");

        if ( $request->hasError('Share') )
            $renderer->setAttribute('ShareError', $request->getError('Share'));
        else
            $renderer->setAttribute('ShareError', 'Unknown general share error');

        if( $outputContext = $request->getParameter( 'OutputContext' ) )
            $renderer->setAttribute( 'openl_share_OutputContext', 
                $outputContext );
            
        return $renderer;
    }

}

?>
