<?php

class PhoneInfoView extends View
{

    function execute(&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');
        $ct = $controller->getContentType();
        switch($ct)
        {
        case 'wml':
        case 'xhtmlMobile':
        case 'html':
        case 'async':
            break;
        default:
            $ct = 'html';
            break;
        }
        $renderer->setTemplate("PhoneInfoView_input.$ct.tpl");

        $MobilizeParams =& $request->getAttribute('MobilizeParams');
        $Content =& $MobilizeParams->getParameter('Content');
        $pageName = "Mobilization:Enter Recipient";
        $channel = "Mobilization";
        $prop2 = "";

        $isShareable = true;
        // We will not allow users to "share" mp3s if anonymous
        if( array_key_exists('CapabilityId', $Content) &&
            $Content['CapabilityId'] == 26 )
        {
            $isShareable = false;
        }
        $renderer->setAttribute('isShareable', $isShareable);

        // default and error values for the to phone number
        $ToPhoneNumber = $MobilizeParams->getParameter('ToPhoneNumber');
        $renderer->setAttribute('mobilizePhone1', substr($ToPhoneNumber, 0, 3));
        $renderer->setAttribute('mobilizePhone2', substr($ToPhoneNumber, 3, 3));
        $renderer->setAttribute('mobilizePhone3', substr($ToPhoneNumber, 6, 4));

        // default and error values for the from phone number
        $FromPhoneNumber = $MobilizeParams->getParameter('FromPhoneNumber');
        if($user->isAuthenticated())
        {
            $UserCore =& $user->getAttribute('UserCore');
            if(is_object($UserCore))
            {
                $UserComm =& $UserCore->Comm();
                if(is_object($UserComm))
                {
                    $FromPhoneNumber = $UserComm->PhoneNumber();
                }
            }
        }
        $renderer->setAttribute('myPhone1', substr($FromPhoneNumber, 0, 3));
        $renderer->setAttribute('myPhone2', substr($FromPhoneNumber, 3, 3));
        $renderer->setAttribute('myPhone3', substr($FromPhoneNumber, 6, 4));

        // were the remember my phone options previously checked?
        $rememberMyPhone1 = $user->hasAttribute('PhoneNumber');
        $rememberMyPhone2 = 
            $user->hasAttribute('LastRecipient') && 
            $user->hasAttribute('PhoneNumber') && 
            $user->getAttribute('LastRecipient') != $user->getAttribute('PhoneNumber');
        $renderer->setAttribute('rememberMyPhone1', $rememberMyPhone1);
        $renderer->setAttribute('rememberMyPhone2', $rememberMyPhone2);
        $renderer->setAttribute('myPhone', !$rememberMyPhone2);

        // MobilizeParams hidden form field values
        $renderer->setAttribute('Filename', 
            $MobilizeParams->getParameter('Filename'));
        $renderer->setAttribute('ContentInfo', $Content);
        $renderer->setAttribute('InputType', 
            $MobilizeParams->getParameter('InputType'));
        $renderer->setAttribute('InputValue',
            $MobilizeParams->getParameter('InputValue'));
        $renderer->setAttribute('InputMethod',
            $MobilizeParams->getParameter('InputMethod'));
        $renderer->setAttribute('Affiliate',
            $MobilizeParams->getParameter('Affiliate'));
        $renderer->setAttribute('Title',
            $MobilizeParams->getParameter('Title'));
        
        if($request->hasError('Login'))
        {
            $pageName = "Mobilization:Error:Login";
            $renderer->setAttribute('LoginError', $request->getError('Login'));
        } 
        elseif($request->hasError('Phone'))
        {
            $pageName = "Mobilization:Error:Malformed Phone Number";
            $renderer->setAttribute('PhoneError', $request->getError('Phone'));
        }
        $renderer->setAttribute('SourceUrl',
            $MobilizeParams->getParameter('SourceUrl'));


        $renderer->setAttribute('OmniturePageName', $pageName);
        $renderer->setAttribute('OmnitureChannel', $channel);
        $renderer->setAttribute('OmnitureProp2', $prop2);

        return $renderer;
    }

}

?>
