<?php

require_once(dirname(__FILE__) . '/IndexAction.class.php');

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * This is a container for the Index action.
 */
class AudioContainerAction extends OpenLocker_IndexAction
{

    function initialize(&$controller, &$request, &$user)
    {
        if(parent::initialize($controller, $request, $user))
        {
            $this->ContentType = 'Audio';
            return true;
        }
        return false;
    }

}

?>
