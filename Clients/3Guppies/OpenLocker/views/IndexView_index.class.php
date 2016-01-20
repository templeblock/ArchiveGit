<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief Render a view for the locker
 *
 * @author  Blake Matheny
 */

class IndexView extends View
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
        $renderer->setTemplate("IndexView_index.$ct.tpl");

        $info = $request->getAttribute('OpenLockerIndexView');

        if ( $request->hasError('Index') || !is_array($info) )
        {
            $renderer->setAttribute('IndexError', $request->getError('Index'));
        }

        if ( array_key_exists('Pager', $info) ) // produces xhtml error?
        {
            $renderer->setAttribute('Pager', $info['Pager']);
        }

        if ( array_key_exists('HelpText', $info) )
        {
            $renderer->setAttribute('HelpText', $info['HelpText']);
        }

        $renderer->setAttribute('LockerCount', $info['LockerCount']);
        $renderer->setAttribute('SpaceRemaining', $info['SpaceRemaining']);
        $renderer->setAttribute('LockerEntries', $info['Entries']);

        $renderer->setAttribute('SortedBy', $info['SortBy']);
        $renderer->setAttribute('ContentType', $info['ContentType']);

        return $renderer;
    }

}

?>
