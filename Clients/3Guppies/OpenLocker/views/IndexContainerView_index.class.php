<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

require_once(dirname(__FILE__) . '/IndexView_index.class.php');

class IndexContainerView extends IndexView
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
        
        if( $request->getParameter( 'Upload' ) == 'true' && $ct == 'html' )
        {
            $renderer->setAttribute( 'openl_index_showUpload', true );
        }
        $renderer->setTemplate("IndexContainerView_index.$ct.tpl");
		if ($request->hasAttribute('MobilizeSuccess')) $renderer->setAttribute('MobilizeSuccess',$request->getAttribute('MobilizeSuccess'));

        $renderer->setAttribute('media_heade_contents',
            $request->getAttribute('media_heade_contents'));

        $contentType = $request->getAttribute('ContentType');

        $desc = 'You are able to see all the media in your Locker. Visitors will only be able to see those items you\'ve marked "public". Mouse over an item to get your toolbar, where you can edit your settings and get more details.';
        switch($contentType)
        {
        case 'Audio':
            $title = "My Ringtones";
            break;
        case 'Video':
            $title = "My Videos";
            break;
        case 'Picture':
            $title = "My Pictures";
            break;
        case 'All':
            $title = "All My Files";
            break;
        default:
            $title = 'My Locker';
            break;
        }

        $renderer->setAttribute('LockerTitle', $title);
        $renderer->setAttribute('LockerDescription', $desc);

        return $renderer;
    }
}

?>
