<?php

class EditorBaseView extends View
{

    var $MobilizeParams = null;

    var $width          = 150;
    var $height         = 150;

    function execute(&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');

        $this->MobilizeParams =& $request->getAttribute('MobilizeParams');
        $Content =& $this->MobilizeParams->getParameter('Content');

        $renderer->setAttribute('toPhone',
            $this->MobilizeParams->getParameter('ToPhoneNumber'));
        $renderer->setAttribute('fromPhone',
            $this->MobilizeParams->getParameter('FromPhoneNumber'));
        $renderer->setAttribute('ContentInfo', $Content);
        $renderer->setAttribute('myPhone', $this->MobilizeParams->getParameter('MyPhone'));
        $renderer->setAttribute('agreeTC', $this->MobilizeParams->getParameter('AgreeTC'));
        $renderer->setAttribute('fromName', $this->MobilizeParams->getParameter('FromName'));


        // Anonymous mobilize logic
        if(!$user->isAuthenticated())
        {
            // BEGIN: Captcha logic
            $renderer->setAttribute('showCaptcha',
                $request->getAttribute('showCaptcha'));
            if($request->hasError('Captcha'))
            {
                $renderer->setAttribute('CaptchaError', $request->getError('Captcha'));
                $renderer->setAttribute('status', 2);
            }
            else
            {
                $renderer->setAttribute('status', 1);
            }
            // END: Captcha logic

            // BEGIN: logic to populate selects for specifying handset model
            $promptCarrier = $request->getAttribute('PromptCarrier');
            $renderer->setAttribute('PromptCarrier', $promptCarrier);
            $renderer->setAttribute('carrierId',
                $this->MobilizeParams->getParameter('CarrierId'));
            $EditOptions = $this->MobilizeParams->getParameter('EditOptions');
            $deviceId = null;
            if(is_array($EditOptions) && array_key_exists('deviceId', $EditOptions))
            {
                $deviceId = $EditOptions['deviceId'];
                $PhoneInfo = HandsetDetect::getPhoneInfoByDeviceId($deviceId);
                $this->width = $PhoneInfo->getDeviceWidth();
                $this->height = $PhoneInfo->getDeviceHeight();
                $renderer->setAttribute('deviceId', $deviceId);
            }

            $CarrierInfo = $request->getAttribute('CarrierInfo');
            $renderer->setAttribute( 'CarrierInfo', $CarrierInfo );
    
            $DeviceInfo = $request->getAttribute('DeviceInfo');
            if($DeviceInfo)
            {
                //$deviceIds = array();
                //$deviceNames = array();
                $deviceIdNameMap = array();
                foreach($DeviceInfo as $info)
                {
                    $deviceIdNameMap[ $info['mobileDevice_id'] ] = 
                        $info['mobileDevice_model'];
                }

                $renderer->setAttribute( 'DeviceIdNameMap', $deviceIdNameMap );
                $renderer->setAttribute('DeviceInfo', $DeviceInfo);
            }

            // END: logic to populate selects for specifying handset model
        }

        // BEGIN: Logic to set the default title for the media
        $personalMediaId = $request->getParameter('PersonalId');
        if($personalMediaId)
        {
            // If this is a community picture already, get the title and tags
            //   to set the default values in the form
            $mediaModel =& $controller->getModel('Media', 'Media');
            $pix =& $mediaModel->getPixByPersonalMediaId($personalMediaId);
            $mediaInfo =& $pix->getMediaInfo();
            $title = $mediaInfo->getTitle();
            if ( strlen( $title ) > 30 )
            {
                $parts = explode ( '.', $title );
                $extension = array_pop ( $parts );
                $title = substr( implode ( '.', $parts ), 0, 29 - strlen( $extension ) ).'.'.$extension;
            }
            $tags = $mediaInfo->getTags();
            if(is_array($tags))
            {
                $tags = implode(", ", $tags);
            }
            $renderer->setAttribute('mobilizeTitle', $title);
            $renderer->setAttribute('mobilizeTags', $tags);
            $renderer->setAttribute('personalMediaId', $personalMediaId);
        }
        else
        {
            // Use the uploaded file's name as the default title
            // No defaults for the tags
            $filename = $this->MobilizeParams->getParameter('Filename');
            if(!$filename)
            {
                $filename = $Content['Filename'];
            }
            
            $title = $this->MobilizeParams->getParameter( 'Title' );
            
            // clean up the title
            if( $title )
            {
                // take all non-alnum chars and replace with a single '_'
                // then remove all '_' that end a title
                // then remove all '_' that start a title
                $patts = array( '#[^\w\d\.-]+#', '#_+(\..{1,})?$#', '#^_+#'  );
                $repls = array( '_', '\1', '' );
    
                $title = preg_replace( $patts, $repls, $title );
            }
            else
                $title = $filename;

            if( strlen( $title ) > 30 )
                $title = substr( $title, 0, 30 );
                
            if ( strlen( $filename ) > 30 )
            {
                $parts = explode ( '.', $filename );
                $extension = array_pop ( $parts );
                $filename = substr( implode ( '.', $parts ), 0, 29 - strlen( $extension ) ).'.'.$extension;
            }
            $renderer->setAttribute('Filename', $filename);
            $renderer->setAttribute('mobilizeTitle', $title);
        }
        // END: Logic to set the default title for the media

        // BEGIN: Configure device info
        $device =& $user->getAttribute('DeviceInfo');
        $model = "Phone";
        if ( is_object($device) )
        {
            $this->width = $device->MobileDeviceDisplayWidth();
            $this->height = $device->MobileDeviceDisplayHeight();
            $model = $device->MobileDeviceModel();
        }
        $renderer->setAttribute('DeviceDim', array('width' => $this->width, 'height' => $this->height));
        $renderer->setAttribute('DeviceModel', $model);
        // END: Configure device info

        if($request->hasAttribute('CannotDetectCarrier'))
        {
            $renderer->setAttribute('CannotDetectCarrier', 1);
        }

        $renderer->setAttribute('InputType',
            $this->MobilizeParams->getParameter('InputType'));
        $renderer->setAttribute('InputValue',
            $this->MobilizeParams->getParameter('InputValue'));
        $renderer->setAttribute('InputMethod',
            $this->MobilizeParams->getParameter('InputMethod'));
        $renderer->setAttribute('SourceUrl',
            $this->MobilizeParams->getParameter('SourceUrl'));
        $renderer->setAttribute('Recipients',
            implode(',',$this->MobilizeParams->getParameter('Recipients')));
        $renderer->setAttribute('MyPhone',
            $this->MobilizeParams->getParameter('MyPhone'));
        $renderer->setAttribute('Affiliate',
            $this->MobilizeParams->getParameter('Affiliate'));


        $pageName = "Mobilization:Format Media";
        $channel = "Mobilization";
        $prop2 = "";
        
        if($request->hasError('Captcha'))
        {
            $pageName = "Mobilization:Error:Captcha";
        }
        elseif($request->hasAttribute('CannotDetectCarrier'))
        {
            $pageName = "Mobilization:Error:Carrier Not Detected";
        }

        $renderer->setAttribute('OmniturePageName', $pageName);
        $renderer->setAttribute('OmnitureChannel', $channel);
        $renderer->setAttribute('OmnitureProp2', $prop2);

        return $renderer;
    }

}

?>
