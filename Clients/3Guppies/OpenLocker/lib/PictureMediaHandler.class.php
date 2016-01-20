<?

class PictureMediaHandler extends MediaHandler
{
    function PictureMediaHandler(&$controller, &$fileInfo, &$error)
    {
        $this->controller =& $controller;
        $this->fileInfo =& $fileInfo;
        $this->error =& $error;
    }

    function hasEditor()
    {
        return TRUE;
    }

    function getEditor()
    {
        return array('OpenLocker', 'PictureEditor', VIEW_INDEX);
    }

    function getWidthHeight($_options = array(), $_isMyPhone = true)
    {
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $user =& $controller->getUser();
        $_options = $this->normalizeOptions($_options);

        // idSizeDX and idSizeDY may not be set when putting newly mobilized
        //   content into a user's locker during registration
        $resizedWidth = 50;
        $resizedHeight = 50;

        if( !$_isMyPhone )
        {
            return array($resizedWidth, $resizedHeight);
        }

        if( array_key_exists('idSizeDX', $_options) &&
            array_key_exists('idSizeDY', $_options) )
        {
            // Mobilize workflow
            return array($_options['idSizeDX'], $_options['idSizeDY']);
        }

        // Download reconvert workflow
        $deviceWidth = 50;
        $deviceHeight = 50;

        $DeviceInfo =& $user->getAttribute('DeviceInfo');
        if( is_object($DeviceInfo) )
        {
            $deviceWidth = $DeviceInfo->MobileDeviceDisplayWidth();
            $deviceHeight = $DeviceInfo->MobileDeviceDisplayHeight();
        }
        elseif( array_key_exists('deviceId', $_options) &&
                $_options['deviceId'] > 0 )
        {
            $PhoneInfo = HandsetDetect::getPhoneInfoByDeviceId($_options['deviceId']);
            $deviceWidth = $PhoneInfo->getDeviceWidth();
            $deviceHeight = $PhoneInfo->getDeviceHeight();
        }
        else
        {
            $BrowserInfo =& $user->getAttribute('BrowserInfo');
            if( $BrowserInfo->isValid() )
            {
                $deviceWidth = $BrowserInfo->getDeviceWidth();
                $deviceHeight = $BrowserInfo->getDeviceHeight();
            }
        }

        $imageFile = $this->fileInfo['Path'];

        $dimensions = array();
        if( array_key_exists('idCropDX', $_options) &&
            array_key_exists('idCropDY', $_options) )
        {
            $dimensions = MediaConverter::scaleDimensions(
                $_options['idCropDX'], $_options['idCropDY'],
                $deviceWidth, $deviceHeight);
        }
        else
        {
            $dimensions = MediaConverter::GetNewDimensions(
                $imageFile,
                $deviceWidth,
                $deviceHeight
                );
        }

        switch($_options['Image.Resize'])
        {
        case 'Stretch':
        case 'FitScreen':
            return array(
                $dimensions['exact']['width'],
                $dimensions['exact']['height'],
                );
        case 'Freeform':
        default:
            return array(
                $dimensions['proportional']['width'],
                $dimensions['proportional']['height'],
                );
        }


        //Old and deprecated


        if( !array_key_exists('idSizeDX', $_options) ||
            !array_key_exists('idSizeDY', $_options) )
        {
            $DeviceInfo =& $user->getAttribute('DeviceInfo');
            if( is_object($DeviceInfo) )
            {
                $resizedWidth = $DeviceInfo->MobileDeviceDisplayWidth();
                $resizedHeight = $DeviceInfo->MobileDeviceDisplayHeight();
            }
        }
        else
        {
            $resizedWidth = $_options['idSizeDX'];
            $resizedHeight = $_options['idSizeDY'];
        }

        if( array_key_exists('idCropOccur', $_options) )
        {
            // idCropDX and idCropDY may not be set when using the 
            //   for UnknownUserDownloadHandler for uncropped pictures
            if( !array_key_exists('idCropDX', $_options) ||
                !array_key_exists('idCropDY', $_options) )
            {
                return array($resizedWidth, $resizedHeight);
            }
            $selectionWidth = $_options['idCropDX'];
            $selectionHeight = $_options['idCropDY'];

            if ( array_key_exists('Image.Resize', $_options ) )
            {
                $resizeOption = $_options['Image.Resize'];
                if( $resizeOption == "Freeform" )
                {
                    // If the image selection is smaller than the phone's screen
                    //   do not scale it, just grab the selection.
                    // Otherwise, scale the image down to fit in the phone's box
                    if( $selectionWidth <= $resizedWidth &&
                        $selectionHeight <= $resizedHeight )
                    {
                        return array($selectionWidth, $selectionHeight);
                    }
                    else
                    {
                        return array($resizedWidth, $resizedHeight);
                    }
                }
                else
                {
                    if( array_key_exists('deviceId', $_options) &&
                        $_options['deviceId'] > 0 )
                    {
                        $PhoneInfo = HandsetDetect::getPhoneInfoByDeviceId($_options['deviceId']);
                        $resizedWidth = $PhoneInfo->getDeviceWidth();
                        $resizedHeight = $PhoneInfo->getDeviceHeight();
                    }

                    return array($resizedWidth, $resizedHeight);
                }
            }
            return array($resizedWidth, $resizedHeight);
        }
        elseif( array_key_exists('deviceId', $_options) )
        {
            $PhoneInfo = HandsetDetect::getInfoByDeviceId($_options['deviceId']);
            $width = $PhoneInfo->getDeviceWidth();
            $height = $PhoneInfo->getDeviceHeight();
            return array($width, $height);
        }
        else
        {
            $BrowserInfo =& $user->getAttribute('BrowserInfo');
            if($BrowserInfo && is_object($BrowserInfo))
            {
                $resizedWidth = $BrowserInfo->getDeviceWidth();
                $resizedHeight = $BrowserInfo->getDeviceHeight();
            }

            if( array_key_exists('Image.Resize', $_options) &&
                $_options['Image.Resize'] == 'Freeform' )
            {
                //calculate the dimensions
                $dimensions = MediaConverter::GetNewDimensions(
                    $this->fileInfo['Path'],
                    $resizedWidth,
                    $resizedHeight
                    );

                return array($dimensions['proportional']['width'],
                    $dimensions['proportional']['height']);
            }
        }
        return array($resizedWidth,$resizedHeight);
    }

    function normalizeOptions($_options = array())
    {
        if ( array_key_exists('Image.Resize', $_options) )
        {
            switch ( $_options['Image.Resize'] )
            {
                case 'Freeform':
                case 'Stretch':
                case 'FitScreen':
                    break;
                default:
                    $_options['Image.Resize'] = 'Freeform';
                    break;
            }
        }
        else
            $_options['Image.Resize'] = 'Freeform';

        return $_options;
    }

    function getCommands($_options = array())
    {
        $_options = $this->normalizeOptions($_options);
        $cmd0 = '';
        $cmd1 = '';
        $cmd2 = '';
        $cmd3 = '';
        $cmd4 = '';
        $cmd5 = '';

        if(array_key_exists('idCropOccur', $_options) &&
            !intval($_options['idCropOccur']) )
        {
            $cmd1 = '';
        }
        else
        {
            $cmd1 = '-crop %dx%d+%d+%d%s';
            $keys = array('idCropIX', 'idCropIY', 'idCropDX', 'idCropDY');

            $idCropIX = $idCropIY = $idCropDX = $idCropDY = null;

            foreach ( $keys AS $anon_key => $key_value )
            {
                if ( array_key_exists($key_value, $_options) &&
                     intval($_options[$key_value]) >= 0 &&
                     preg_match('/^[0-9]+$/', $_options[$key_value]) )
                {
                    $$key_value = $_options[$key_value];
                }
            }

            $stretch = "";
            if( array_key_exists('Image.Resize', $_options) &&
                ($_options['Image.Resize'] == 'Stretch' || $_options['Image.Resize'] == 'FitScreen' ) )
            {
                $stretch = "!";
            }

            if ( !is_null($idCropIX) && !is_null($idCropIY) && !is_null($idCropDX) && !is_null($idCropDY) )
            {
                $cmd1 = sprintf($cmd1, $idCropDX, $idCropDY, $idCropIX, $idCropIY, $stretch);
                $cmd2 = $idCropDX;
                $cmd3 = $idCropDY;
                $cmd4 = $idCropIX;
                $cmd5 = $idCropIY;
            }
            else
            {
                $cmd1 = "";
            }

        }
        $filename_addon = str_replace('+', 'x', str_replace(' ', '-', $cmd1));

        return array(
            'cmd0'  =>  $cmd0,
            'cmd1'  =>  $cmd1,
            'cmd2'  =>  $cmd2,
            'cmd3'  =>  $cmd3,
            'cmd4'  =>  $cmd4,
            'cmd5'  =>  $cmd5,
            'filename_addon'    =>  $filename_addon,
            );

    }

    function isValid()
    {
        return true;
    }
}

?>
