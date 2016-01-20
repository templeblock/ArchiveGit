<?

class VideoMediaHandler extends MediaHandler
{
    function VideoMediaHandler(&$controller, &$fileInfo, &$error)
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
        return array('OpenLocker', 'VideoEditor', VIEW_INDEX);
    }

    /*
     * Using the H263 codec there are only 5 possible video sizes:
     * Valid sizes are 128x96, 176x144, 352x288, 704x576, and 1408x1152.
     * This is too bad because we may end up streching or smushing video.
     * 
     * The H.263+ codec (h263p) allows for any dimensions, however it did
     * not work on the first phone we tested (LG CU500).  
     */
    function getWidthHeight($_options = array(), $_isMyPhone = true)
    {
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $user =& $controller->getUser();
        
        $width = 128;
        $height = 96;

        if( !$_isMyPhone )
        {
            return array($width, $height);
        }

        $BrowserInfo =& $user->getAttribute('BrowserInfo');
        $device =& $user->getAttribute('DeviceInfo');
        if( is_object($device) )
        {
            if (is_object($device) )
            {
                $width = $device->MobileDeviceDisplayWidth();
                $height = $device->MobileDeviceDisplayHeight();
            }
        }
        elseif( is_object($BrowserInfo) && $BrowserInfo->isValid() )
        {
            $width = $BrowserInfo->getDeviceWidth();
            $height = $BrowserInfo->getDeviceHeight();
        }
        else
        {
            if(array_key_exists('deviceId', $_options) && 
                $_options['deviceId'] > 0)
            {
                $deviceId = $_options['deviceId'];
                $PhoneInfo = HandsetDetect::getPhoneInfoByDeviceId($deviceId);
                $width = $PhoneInfo->getDeviceWidth();
                $height = $PhoneInfo->getDeviceHeight();;
            }
        }
        if ( $width >= 1408 && $height >= 1152 ) 
        {
            return array(1408,1152);
        } 
        else if ( $width >= 704 && $height >= 576 )
        {
            return array(704,576);
        } 
        else if ( $width >= 352 && $height >= 288 )
        {
            return array(352,288);
        } 
        else if ( $width >= 176 && $height >= 144 )
        {
            return array(176,144);
        } 
        else
        {
            return array(128,96);
        }
    }

    function normalizeOptions($_options = array())
    {
        return $_options;
    }

    function getCommands($_options = array())
    {
        return array();
    }

    function isValid()
    {
        return true;
    }
}

?>
