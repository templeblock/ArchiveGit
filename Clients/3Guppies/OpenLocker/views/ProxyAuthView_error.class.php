<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * This Error view uses the input template, since this is a simple operation
 *
 * <br/><br/>
 *
 * <note>
 *     Notes here.
 * </note>
 *
 * @author  Eric Malone
 */

class ProxyAuthView extends View
{

    /**
     * Render the presentation.
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
     * @return Renderer A Renderer instance.
     *
     * @access public
     * @since  1.0
     */
    function & execute (&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');

        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');

        $ct = $controller->getContentType();

        $renderer->setTemplate("ProxyAuthView_input.$ct.tpl");

        if( $request->hasErrors() )
        {
            $errors = $request->getErrors();
            
            if( isset( $errors[ 'url' ] ) )
            {
                $renderer->setAttribute( 'openl_proxy_showUrl', true );
            }
            
            if( $url = $request->getAttribute( 'openl_proxy_url' ) )
                $renderer->setAttribute( 'openl_proxy_url', $url );
            
            $renderer->setAttribute( 'errors', $errors );
        }
        
        return $renderer;
    }

}

?>
