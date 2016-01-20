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

class AudioEditorView extends EditorBaseView
{

    /**
     * @brief Render the presentation.
     */
    function & execute (&$controller, &$request, &$user)
    {
        $renderer =& parent::execute($controller, $request, $user);

        $ct = $controller->getContentType();
        
        switch ( $ct ){
            case 'html':
            case 'xhtmlMobile':
            case 'wml':
            case 'async':
                break;
            default:
                $ct = 'html';
                break;
        }
        
        $renderer->setTemplate("AudioEditorView_index.$ct.tpl");
        $Content =& $this->MobilizeParams->getParameter('Content');

        $track_in_seconds = 0;
        switch ( $Content['CapabilityId'] )
        {
            case 26:
                $editor = 'mp3';
                $track_in_seconds = MediaConverter::getMp3LengthInSeconds($Content['Path'], 60);
                break;
            default:
//j				$editor = 'default';
//j				$Content['CapabilityId'] = 26;
                $editor = 'mp3';
                $track_in_seconds = 0;
                break;
        }
        $renderer->setTemplate("AudioEditorView_index.$editor.$ct.tpl");
        $renderer->setAttribute('TrackInSeconds', $track_in_seconds);

        if($renderer->getAttribute('OmniturePageName') == "Mobilization:Format Media")
        {
            $renderer->setAttribute('OmniturePageName', "Mobilization:Format Media:Ringtone Cropper");
        }

        return $renderer;
    }

}

?>
