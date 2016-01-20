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
        $renderer->setTemplate("DeleteView_success.$ct.tpl");

        // Decide where to forward the user to after successful delete
        $mediaType = $request->getParameter('openl_delet_type');
        $forwardUrl = "";
        switch($mediaType)
        {
        case 'audio':
            $forwardUrl = $controller->genUrl(array(
                'module' => 'OpenLocker',
                'action' => 'AudioContainer',
                'media_heade_active' => 'My+Audio+Library'));
            break;
        case 'image':
            $forwardUrl = $controller->genUrl(array(
                'module' => 'Media',
                'action' => 'Gallery'));
            break;
        case 'video':
            $forwardUrl = $controller->genUrl(array(
                'module' => 'OpenLocker',
                'action' => 'IndexContainer',
                'ContentType' => 'Video'));
            break;
        case 'locker':
        default:
            $forwardUrl = $controller->genUrl(array(
                'module' => 'OpenLocker',
                'action' => 'IndexContainer'));
            break;
        }

        $renderer->setAttribute('openl_delet_forwardUrl', $forwardUrl);

        return $renderer;
    }

}

?>
