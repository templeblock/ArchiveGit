<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * General Documentation Goes Here
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

        
        $authInstance = $request->getAttribute( 'openl_proxy_authInstance' );
        
        if( $authInstance )
        {
            $renderer->setAttribute( 'openl_proxy_authUserName', 
                $authInstance->getUserName() );
            $renderer->setAttribute( 'openl_proxy_authPassName', 
                $authInstance->getPassName() );
        }
        
        if( $returnToUrl = $request->getParameter( 'returnto' ) )
        {
            $renderer->setAttribute( 'openl_proxy_returnTo', $returnToUrl );
        }
        
        return $renderer;
    }

}

?>
