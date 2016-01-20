<?

class MediaHandler
{
    // Object of type Controller
    var $controller     =   null;

    // FileInfo array(Size,Path,MimeType,CapabilityId,Extension,Filename)
    var $fileInfo       =   null;

    // Object of type MediaHandler
    var $handler        =   null;

    // reference to error from ContentHandler
    var $error          =   null;

    function MediaHandler(&$controller, &$fileInfo, &$error)
    {
        $this->controller =& $controller;
        $this->fileInfo =& $fileInfo;
        $this->error =& $error;
        $this->setHandler();
    }

    function hasEditor()
    {
        return $this->handler->hasEditor();
    }

    function getEditor()
    {
        return $this->handler->getEditor();
    }

    function noConvert($_from, $_to, $_extension, $_personal)
    {
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $MC = new MediaConverter(null, $request->getAttribute('read_db'));

        $Original = $MC->NoConvert(
                $_from,
                $_to.$_extension,
                $_personal
                );

        //cleanup temporary files
        if ( strstr($_from, '/tmp/') !== FALSE )
            @unlink($_from);
        if ( strstr($_from, PERSONAL_MEDIA_TEMP_BASE) !== FALSE )
            @unlink($_from);

        return $Original;
    }

    /**
     * @brief Converts the media according to the EditOptions specified
     *
     * @pre $this->fileInfo['Personal'] = whether this is personal content
     * @pre $this->fileInfo['Path'] = original file location
     *
     * @return boolean whether we are able to send the conversion request
     */
    function convertContent($edit_options = array(), $post_vars = array(),
        $_isMyPhone = true)
    {
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $user =& $controller->getUser();
        $read_db =& $request->getAttribute('read_db');
        
        // Sanity check - does the file we are converting with exist?
        $from_file = $this->fileInfo['Path'];
        if ( !file_exists($from_file) )
        {
            $this->error = 'Unable to find file.';
            return false;
        }

        $is_personal = $this->fileInfo['Personal'];
        $md5 = md5_file($this->fileInfo['Path']);

        // Condition where user select do not format for my phone
        if ( array_key_exists('Global.NoFormat', $edit_options) )
        {
            // FIXME:
            $this->error = 'Global.NoFormat is not supported at this time.';
            return false;
        }

        // Find a conversion for the user's phone
        $from_media_type = $this->fileInfo['CapabilityId'];


        if(array_key_exists('DeviceId',$post_vars)) {
            $post_device_id = $post_vars['DeviceId'];
            $to_media_type = $this->getConversionTo($from_media_type,$_isMyPhone,$post_device_id);
        } else {
            $to_media_type = $this->getConversionTo($from_media_type);
        }
            
        if ( !$to_media_type )
        {
            $this->error = 'No suitable conversion found.';
            return false;
        }

        // Prepare conversion parameters
        list($device_width,$device_height) = 
            $this->getWidthHeight($edit_options);

        $cmds = $this->getCommands($edit_options);

        $filename_addon = '';
        $tempfile_addon = '-temp';
        if(array_key_exists('filename_addon', $cmds))
        {
            $filename_addon = $cmds['filename_addon'];
        }
        if(array_key_exists('tempfile_addon', $cmds))
        {
            $tempfile_addon = $cmds['tempfile_addon'];
        }

        $MC = new MediaConverter(null, $read_db);

        // Make the conversion request
        $conversionInfo = $MC->NewConvert($from_file, $from_media_type, $to_media_type,
            $is_personal, $device_width, $device_height, $cmds,
            $post_vars, $filename_addon, $tempfile_addon);
        if(!$conversionInfo)
        {
            $this->error = $MC->_error;
            return false;
        }
        $this->fileInfo = array_merge($this->fileInfo,$conversionInfo);
        $this->fileInfo['ConvertedTo'] = $to_media_type;

        return true;
    }

    /**
     * @brief Tells what the converted file name will be given the edit options
     *
     * @return boolean
     */
    function convertedFileAlreadyExisted()
    {
        if( array_key_exists('Preexisting', $this->fileInfo) )
        {
            return $this->fileInfo['Preexisting'];
        }

        return false;
    }

    // MUST OVERRIDE IN CHILD. Return array(0,0) if not applicable
    function getWidthHeight($_options = array())
    {
        return $this->handler->getWidthHeight($_options);
    }

    function getCommands($_options = array())
    {
        return $this->handler->getCommands($_options);
    }

    /**
     * @brief Finds the mobileDeviceCapability_id to convert the file to
     *
     * @param $_from - int - mobileDeviceCapability_id (media type) of the file
     *
     * @param $_deviceId - int - mobileDevice_id we are converting for
     *
     * @return int - 0 if no conversion available or an error occurred
     *               mobileDeviceCapability_id if conversion found
     */
    function getConversionTo($_from, $_isMyPhone = false, $_deviceId = null )
    {
        $media_from_type = array($_from);
        $controller =& Controller::getInstance();
        $user =& $controller->getUser();
        $request =& $controller->getRequest();
        $read_db =& $request->getAttribute('read_db');

        require_once(dirname(__FILE__) . '/../../Downloads/config/Categories.inc.php');
        global $_DEFAULT_DEVICE;
        $phone_capabilities = array_keys($_DEFAULT_DEVICE);
        $Device = $user->getAttribute('DeviceInfo');
        if ( is_object($Device) && $_isMyPhone )
        {
            $phone_capabilities = array_keys($Device->mCapabilities);
        }
        elseif(!is_null($_deviceId))
        {
            $PhoneInfo = HandsetDetect::getPhoneInfoByDeviceId($_deviceId);
            if($PhoneInfo->isValid())
            {
                $phone_capabilities = array_keys($PhoneInfo->getCapabilities());
            }
        }
        elseif($request->hasAttribute('MobilizeParams'))
        {
            $MobilizeParams =& $request->getAttribute('MobilizeParams');
            if($MobilizeParams)
            {
                $EditOptions =& $MobilizeParams->getParameter('EditOptions');
                if($EditOptions)
                {
                    if(array_key_exists('deviceId', $EditOptions))
                    {
                        $deviceId = $EditOptions['deviceId'];
                        if($deviceId > 0)
                        {
                            $PhoneInfo = HandsetDetect::getPhoneInfoByDeviceId($deviceId);
                            $phone_capabilities = array_keys($PhoneInfo->getCapabilities());
                        }
                    }
                }
            }
        }
        elseif($user->hasAttribute('BrowserInfo'))
        {
            $BrowserInfo =& $user->getAttribute('BrowserInfo');
            if($BrowserInfo->isValid())
            {
                $phone_capabilities = array_keys($BrowserInfo->getCapabilities());
            }
        }

        // Get conversions
        $MediaConversion    = new MediaConversion(null, $read_db);
        $media_to_types     = $MediaConversion->GetAllConversions($media_from_type, true, true);

        if(!is_array($media_to_types))
        {
            return 0;
        }

        $media_to_type = NULL;
        if(is_array($media_to_types) && is_array($phone_capabilities))
        {
            for ($i = 0; $i < count($media_to_types); $i++)
            {
                $to = $media_to_types[$i]['to'];
                $from = $media_to_types[$i]['from'];
                $cmd = $media_to_types[$i]['cmd'];

                for ($j = 0; $j < count($to); $j++)
                {
                    if ($cmd[$j] && in_array($to[$j], $phone_capabilities))
                    {
                        $media_from_type = $from[$j];
                        $media_to_type = $to[$j];
                        break 2;
                    }
                }

                if (!is_null($media_to_type))
                    break;
            }
        }

        if ( $media_to_type )
            return $media_to_type;
        else
            return 0;
    }

    function setHandler()
    {
        $extension = $this->fileInfo['Extension'];
        $extension = str_replace('.','',$extension);
        $type =
            MediaConverter::GetContentInfoByExtension($extension);
        if ( is_array($type) && array_key_exists('generic-type', $type) )
        {
            $ctype  = null;
            switch ( $type['generic-type'] )
            {
                case 'Video':
                case 'Audio':
                case 'Picture':
                case 'Other':
                    $ctype = $type['generic-type'];
                    break;
                default:
                    $this->error = "Unknown generic type found";
                    $ctype = null;
                    break;
            }

            if ( $ctype )
            {
                $class = sprintf('%s%s', $ctype, 'MediaHandler');
                $file = sprintf('%s/%s%s', dirname(__FILE__), $class, '.class.php');
                if ( file_exists($file) )
                {
                    require_once($file);
                    $this->handler = new $class($this->controller, $this->fileInfo, $this->error);
                    if(!$this->handler->isValid())
                    {
                        // handler should have set error message
                        $this->handler = null;
                    }
                }
                else
                {
                    $this->error = "Non-existant media handler '$ctype'";
                }

            }
        }
        else
        {
            $this->error = "Unable to determine generic file type";
        }
    }

    function isValid()
    {
        return true;
    }
}

?>
