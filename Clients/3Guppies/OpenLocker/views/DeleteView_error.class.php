<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief General Documentation Goes Here
 *
 * @author  Blake Matheny
 */

class DeleteView extends View
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
        $renderer->setTemplate("DeleteView_error.$ct.tpl");

        if ( $request->hasError('Delete') )
            $renderer->setAttribute('DeleteError', $request->getError('Delete'));
        else
            $renderer->setAttribute('DeleteError', 'General Error');

        return $renderer;
    }

}

?>
