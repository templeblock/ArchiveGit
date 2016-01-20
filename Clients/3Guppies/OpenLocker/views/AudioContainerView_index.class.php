<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

require_once(dirname(__FILE__) . '/IndexView_index.class.php');

class AudioContainerView extends IndexView
{
    function & execute (&$controller, &$request, &$user)
    {
        $renderer =& parent::execute($controller, $request, $user);
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
        $renderer->setTemplate("AudioContainerView_index.$ct.tpl");
		if ($request->hasAttribute('MobilizeSuccess')) $renderer->setAttribute('MobilizeSuccess',$request->getAttribute('MobilizeSuccess'));

        $renderer->setAttribute('media_heade_contents',
            $request->getAttribute('media_heade_contents'));

        return $renderer;
    }
}

?>
