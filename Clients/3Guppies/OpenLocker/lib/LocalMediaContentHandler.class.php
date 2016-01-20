<?

class LocalMediaContentHandler extends ContentHandler
{
    function LocalMediaContentHandler(&$controller, $handler = null, $content = null)
    {
        $this->controller = $controller;
        $this->handler = $handler;
        $this->content = $content;
    }

    function validate()
    {
        if ( is_null($this->content) || empty($this->content) )
        {
            $this->setError('No media was specified');
            return FALSE;
        }

        $file_info = $this->getHandlerInfo();
        if ( !$file_info )
            return FALSE;

        $fname = call_user_func(array($file_info[0], 'getFullPath'), $file_info[1]);

        if ( !$fname || !file_exists($fname) || !@filesize($fname) )
        {
            $this->setError('Non-existent or empty file.');
            return FALSE;
        }
        return TRUE;
    }

    function getHandlerInfo()
    {
        $temp_content = explode('*', $this->content);
        if ( count($temp_content) != 2 )
        {
            $this->setError('Unknown content handler specified');
            return FALSE;
        }
        if ( ContentHandler::requireHandler($temp_content[0]) )
        {
            $temp_content[0] = $temp_content[0] . 'ContentHandler';
            return $temp_content;
        }
        else
        {
            $this->setError('Unknown content handler specified');
            return FALSE;
        }
    }

    // return the fileInfo data structure which contains basic file data like
    // path, size, type, etc
    function getContentData()
    {

        if ( is_null($this->fileInfo) )
        {
            $request = $this->controller->getRequest();
            $read_db =& $request->getAttribute('read_db');
            $MediaConverter = new MediaConverter(null, $read_db);

            $file_info = $this->getHandlerInfo();
            if ( !$file_info )
                return FALSE;

            $fname = call_user_func(array($file_info[0], 'getFullPath'), $file_info[1]);

            // if we have $fileInfo['basename'] && $fileInfo['extension'] use
            // $fileInfo['basename'] as second arg to GetMediaType
            $fileInfo = pathinfo($fname);

            // make sure we got a file
            if ( !file_exists($fname) || !@filesize($fname) )
            {
                $this->setError('Unable to retrieve the specified content');
                return NULL;
            }

            // try and determine a file name from the url
            $filename = $extension = null;
            if ( array_key_exists('basename', $fileInfo) && $fileInfo['basename'] &&
                    array_key_exists('extension', $fileInfo) && $fileInfo['extension'] )
            {
                $filename = $fileInfo['basename'];
                $extension = $fileInfo['extension'];
            }
            else // otherwise use a random file name
            {
                $filename = rand();
            }

            // try and determine the content type
            $mt = $this->_getMediaType($fname, $filename);

            $this->fileInfo['Personal'] = call_user_func(array($file_info[0], 'getScope'));

            // if we can't determine the media type bail out
            if ( !$mt )
            {
                if ( $this->fileInfo['Personal'] )
                {
                    if ( strstr($fname, '/tmp/') !== FALSE )
                        @unlink($fname);
                    if ( strstr($fname, PERSONAL_MEDIA_TEMP_BASE) !== FALSE )
                        @unlink($fname);
                }
                if($this->getError())
                {
                    $this->setError('Unable to determine file type or file type not compatible');
                }
                return NULL;
            }

            // set up the fileInfo array with all data we can gather
            $this->fileInfo['Size'] = @filesize($fname);
            $this->fileInfo['Path'] = $fname;
            $this->fileInfo['MimeType'] = $mt['mobileDeviceCapability_mime'];
            $this->fileInfo['CapabilityId'] = $mt['mobileDeviceCapability_id'];
            $this->fileInfo['Extension'] = $mt['mobileDeviceCapability_extension'];
            $this->fileInfo['PreviewUrl'] = PERSONAL_MEDIA_TEMP_URL . basename($fname);
            $this->fileInfo['RealFilename'] = "PostedFile*" . basename($fname);
            $this->fileInfo['Filename'] = $extension
                ? $filename
                : $filename . "{$this->fileInfo['Extension']}";

            $this->mediaHandler = new MediaHandler($controller, $this->fileInfo, $this->error);
        }

        return $this->fileInfo;
    }
}

?>
