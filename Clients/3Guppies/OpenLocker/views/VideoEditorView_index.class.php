<?php

require_once(dirname(__FILE__) . '/EditorBaseView_index.class.php');
// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * General Documentation Goes Here
 *
 * @author  Blake Matheny
 */

class VideoEditorView extends EditorBaseView
{

    /**
     * @brief Render the presentation.
     */
    function & execute (&$controller, &$request, &$user)
    {
        $renderer =& parent::execute($controller, $request, $user);

        $ct = $controller->getContentType();
        switch ( $ct )
        {
            case 'html':
            case 'xhtmlMobile':
            case 'wml':
            case 'async':
                break;
            default:
                $ct = 'html';
                break;
        }

        if( $request->hasAttribute('CannotDetectCarrier') &&
            $this->MobilizeParams->getParameter('MyPhone') == false )
        {
            require_once(dirname(__FILE__) . '/MobilizeView_input.class.php');
            $request->setError("Mobilize", "Sorry, we can't tell what cell phone company your friend uses, and are unable to send this video.");
            return MobilizeView::execute($controller, $request, $user);
        }
        else
        {
            $renderer->setTemplate("VideoEditorView_index.$ct.tpl");
        }

        if($renderer->getAttribute('OmniturePageName') == "Mobilization:Format Media")
        {
            $renderer->setAttribute('OmniturePageName', "Mobilization:Format Media:Video Editor");
        }

        return $renderer;
    }

}

?>
