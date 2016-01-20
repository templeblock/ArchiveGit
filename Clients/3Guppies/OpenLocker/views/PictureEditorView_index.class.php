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

class PictureEditorView extends EditorBaseView
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
        $renderer->setTemplate("PictureEditorView_index.$ct.tpl");

        // BEGIN: Configurations for the image cropper

        $Content =& $this->MobilizeParams->getParameter('Content');
        if( array_key_exists('CapabilityId', $Content) &&
            $Content['CapabilityId'] == 32 )
        {
            $renderer->setAttribute('NoCroppingAllowed', true);
        }


        // Get the dimensions of the original image
        $dimensions = MediaConverter::GetNewDimensions($Content['Path'],$this->width,$this->height);
        $renderer->setAttribute('Dimensions', $dimensions);

        if($renderer->getAttribute('OmniturePageName') == "Mobilization:Format Media")
        {
            $renderer->setAttribute('OmniturePageName', "Mobilization:Format Media:Picture Cropper");
        }

        // END: Configurations for the image cropper

        return $renderer;
    }

}

?>
