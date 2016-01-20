<?php

class SendNoticeView extends View
{
    function execute(&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');
        $renderer->setTemplate('SendNoticeView_error.html.tpl');

        if($request->hasErrors())
        {
            $errors = $request->getErrors();
            foreach($errors as $key => $value)
            {
                $renderer->setAttribute("SendNoticeError", $value);
            }
        }

        return $renderer;
    }
}

?>
