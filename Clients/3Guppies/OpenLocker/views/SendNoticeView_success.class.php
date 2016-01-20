<?php

class SendNoticeView extends View
{
    function execute(&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');
        $renderer->setTemplate('SendNoticeView_success.html.tpl');


        return $renderer;
    }
}

?>
