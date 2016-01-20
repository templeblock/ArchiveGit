<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * General Documentation Goes Here
 *
 * @author  Blake Matheny
 */
class PictureEditorAction extends Action
{
    function execute (&$controller, &$request, &$user)
    {
        return VIEW_INDEX;
    }

    /**
     * @brief Retrieve the default view.
     */
    function getDefaultView (&$controller, &$request, &$user)
    {
        return VIEW_INDEX;
    }

    /**
     * @brief Determine if this action requires authentication.
     */
    function isSecure ()
    {
        return TRUE;
    }
}

?>
