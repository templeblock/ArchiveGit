<?php

class StoreAndSendView extends View
{

    function execute(&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');
        $ct = $controller->getContentType();
        switch($ct)
        {
        case 'async':
            $ct = 'async';
            break;
        case 'wml':
        case 'xhtmlMobile':
        case 'html':
        default:
            $ct = 'html';
            break;
        }
        $renderer->setTemplate("StoreAndSendView_success.$ct.tpl");

        $renderer->setAttribute("OmniturePageName", "Mobilization:Success");
        $renderer->setAttribute("OmnitureChannel", "Mobilization");

        // Generate contextual success links
        $links = array();
        $MobilizeParams =& $request->getAttribute('MobilizeParams');
        
        $MyPhone = $MobilizeParams->getParameter('MyPhone');
        $Content = $MobilizeParams->getParameter('Content');
        $personalMediaId = $MobilizeParams->getParameter('PersonalMediaId');
        $title = $MobilizeParams->getParameter('Title');
        $inputMethod = $MobilizeParams->getParameter('InputMethod');
        $mediaType = "Unknown";
        if( array_key_exists('Extension', $Content) )
        {
            $mediaTypeInfo = MediaConverter::GetContentInfoByExtension($Content['Extension']);
            if( array_key_exists('generic-type', $mediaTypeInfo) )
            {
                $mediaType = $mediaTypeInfo['generic-type'];
            }
        }

        // Generate link urls
        $registerUrl = $controller->genUrl(array(
            'module'        =>  'Checkout',
            'action'        =>  'RegisterStart',
            'PersonalId'    =>  $personalMediaId
            ));
        $createWidgetUrl = URL_BASE . "widget";
        $homeUrl = URL_BASE . "home";
        $lockerUrl = $controller->genUrl(array(
            'module'        =>  'OpenLocker',
            'action'        =>  'IndexContainer'
            ));
        $lockerAudioUrl = $controller->genUrl(array(
            'module'        =>  'OpenLocker',
            'action'        =>  'IndexContainer',
            'ContentType'   =>  'Audio'
            ));
        $lockerPictureUrl = $controller->genUrl(array(
            'module'        =>  'OpenLocker',
            'action'        =>  'IndexContainer',
            'ContentType'   =>  'Picture'
            ));
        $lockerVideoUrl = $controller->genUrl(array(
            'module'        =>  'OpenLocker',
            'action'        =>  'IndexContainer',
            'ContentType'   =>  'Video'
            ));
        $mobilizeUrl = $controller->genUrl(array(
            'module'        =>  'OpenLocker',
            'action'        =>  'Mobilize'
            ));

        // Create general links to choose from
        $lockerSaveLink = array(
            'url'   =>  $registerUrl,
            'pre'   =>  "",
            'name'  =>  "Save \"$title\"",
            'post'  =>  " in a FREE locker you can access from anywhere.",
            );
        $getWidgetLinkA = array(
            'url'   =>  $createWidgetUrl,
            'pre'   =>  "Get your own ",
            'name'  =>  "Mobilizer widget or borwser plug-in.",
            'post'  =>  "",
            );
        $getWidgetLinkB = array(
            'url'   =>  $createWidgetUrl,
            'pre'   =>  "Get your own ",
            'name'  =>  "Mobilizer widget or Firefox browser plug-in.",
            'post'  =>  "",
            );
        $findFreeStuffLink = array(
            'url'   =>  $homeUrl,
            'pre'   =>  "",
            'name'  =>  "Find more",
            'post'  =>  " FREE stuff for your phone.",
            );
        $findCoolStuffLinkA = array(
            'url'   =>  $homeUrl,
            'pre'   =>  "",
            'name'  =>  "Find more",
            'post'  =>  " cool stuff for your phone.",
            );
        $findCoolStuffLinkB = array(
            'url'   =>  $homeUrl,
            'pre'   =>  "",
            'name'  =>  "Find more",
            'post'  =>  " cool stuff for your phone at Mixxer.",
            );
        $lockerLink = array(
            'url'   =>  $lockerUrl,
            'pre'   =>  "Go to your ",
            'name'  =>  "Mixxer locker",
            'post'  =>  " to see or edit file details.",
            );
        $lockerAudioLink = array(
            'url'   =>  $lockerAudioUrl,
            'pre'   =>  "",
            'name'  =>  "See all",
            'post'  =>  " ringtones in your locker.",
            );
        $lockerPictureLink = array(
            'url'   =>  $lockerPictureUrl,
            'pre'   =>  "",
            'name'  =>  "See all",
            'post'  =>  " pictures in your locker.",
            );
        $lockerVideoLink = array(
            'url'   =>  $lockerVideoUrl,
            'pre'   =>  "",
            'name'  =>  "See all",
            'post'  =>  " videos in your locker.",
            );
        $mobilizeLink = array(
            'id'    =>  'mobilizeAgain',
            'url'   =>  $mobilizeUrl,
            'pre'   =>  "",
            'name'  =>  "Mobilize another",
            'post'  =>  " media file.",
            );
        $makeRingtoneLink = array(
            'url'   =>  $mobilizeUrl,
            'pre'   =>  "",
            'name'  =>  "Make another ringtone.",
            'post'  =>  "",
            );
        $sendPictureLink = array(
            'url'   =>  $mobilizeUrl,
            'pre'   =>  "",
            'name'  =>  "Send another",
            'post'  =>  " picture to your phone or a friend's.",
            );
        $sendVideoLink = array(
            'url'   =>  $mobilizeUrl,
            'pre'   =>  "",
            'name'  =>  "Send another",
            'post'  =>  " video to your phone or a friend's.",
            );

        // Assign contextual links (media type, logged in, source)
        if($user->isAuthenticated())
        {
            if($MyPhone)
            {
                $links[] = $lockerLink;
            }
            else
            {
                switch($mediaType)
                {
                case "Audio":
                    $links[] = $lockerAudioLink;
                    break;
                case "Video":
                    $links[] = $lockerVideoLink;
                    break;
                case "Picture":
                default:
                    $links[] = $lockerPictureLink;
                    break;
                }
            }

            switch($inputMethod)
            {
            case SOURCE_TYPE_URL: // proxy for the plug-in
                //$links[] = $lockerLink;
                $links[] = $findFreeStuffLink;
                break;
            case SOURCE_TYPE_WIDGET:
                //$links[] = $lockerLink;
                $links[] = $getWidgetLinkB;
                $links[] = $mobilizeLink; // FIXME: javascript onclick event
                break;
            case SOURCE_TYPE_WIDGET_SINGLE:
                //$links[] = $lockerLink;
                $links[] = $getWidgetLinkA;
                $links[] = $findFreeStuffLink;
                break;
            case SOURCE_TYPE_UPLOAD:
            case SOURCE_TYPE_PERSONAL_MEDIA:
            case SOURCE_TYPE_PREMIUM_MEDIA:
            default:
                switch($mediaType)
                {
                case "Audio":
                    //$links[] = $lockerAudioLink;
                    $links[] = $makeRingtoneLink;
                    $links[] = $findCoolStuffLinkA;
                    break;
                case "Video":
                    //$links[] = $lockerVideoLink;
                    $links[] = $sendVideoLink;
                    $links[] = $findCoolStuffLinkA;
                    break;
                case "Picture":
                default:
                    //$links[] = $lockerPictureLink;
                    $links[] = $sendPictureLink;
                    $links[] = $findCoolStuffLinkA;
                    break;
                }
                break;
            }
        }
        else
        {
            switch($inputMethod)
            {
            case SOURCE_TYPE_URL: // proxy for the plug-in
                $links[] = $lockerSaveLink;
                $links[] = $findCoolStuffLinkB;
                break;
            case SOURCE_TYPE_WIDGET:
                $links[] = $lockerSaveLink;
                $links[] = $getWidgetLinkB;
                $links[] = $mobilizeLink; // FIXME: javascript onclick event
                break;
            case SOURCE_TYPE_WIDGET_SINGLE:
                $links[] = $lockerSaveLink;
                $links[] = $getWidgetLinkA;
                $links[] = $findFreeStuffLink;
                break;
            case SOURCE_TYPE_UPLOAD:
            case SOURCE_TYPE_PERSONAL_MEDIA:
            case SOURCE_TYPE_PREMIUM_MEDIA:
            default:
                switch($mediaType)
                {
                case "Audio":
                    $links[] = $lockerSaveLink;
                    $links[] = $makeRingtoneLink;
                    $links[] = $findCoolStuffLinkA;
                    break;
                case "Video":
                    $links[] = $lockerSaveLink;
                    $links[] = $sendVideoLink;
                    $links[] = $findCoolStuffLinkA;
                    break;
                case "Picture":
                default:
                    $links[] = $lockerSaveLink;
                    $links[] = $sendPictureLink;
                    $links[] = $findCoolStuffLinkA;
                    break;
                }
                break;
            }
        }

        $renderer->setAttribute("links", $links);

        $renderer->setAttribute('Title', $title);
        $renderer->setAttribute('ToPhoneNumber',
            $this->_formatPhoneNumber(
                $MobilizeParams->getParameter('ToPhoneNumber')));
        
        //Pass SMS/MMS detection results to template
        $renderer->setAttribute( 'SendViaSMS', $request->getAttribute( 'sendViaSMS' ) );

        $renderer->setAttribute( 'SendNotice', 
            $MobilizeParams->getParameter( 'SendNotice' ) );
        return $renderer;
    }

    function _formatPhoneNumber($phoneNumber)
    {
        if( !is_numeric($phoneNumber) ||
            strlen($phoneNumber) != 10)
        {
            return $phoneNumber;
        }

        return "(" . substr($phoneNumber, 0, 3) . ") " .
            substr($phoneNumber, 3, 3) . "-" . substr($phoneNumber, 6, 4);

    }


}

?>
