<?php

class DownloadRenotifyView extends View
{
    function execute(&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->getTemplateDir($controller->getModuleDir() . 'templates/');
        $ct = $controller->getContentType();
        switch($ct)
        {
        case 'html':
        case 'xhtmlMobile':
        case 'wml':
            break;
        default:
            $ct = 'html';
            break;
        }
        $renderer->setTemplate("RenotifyView_error.$ct.tpl");

        $renderer->setAttribute('Error', $request->getError('Renotify'));

        return $renderer;
    }

}

?>
