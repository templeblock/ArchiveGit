<?

class PostedFileContentHandler extends ContentHandler
{
    function PostedFileContentHandler(&$controller, $handler = null, $content = null)
    {
        $this->controller = $controller;
        $this->handler = $handler;
        $this->content = $content;
    }

    function getUploadError($error_code)
    {
        switch ($error_code) {
            case UPLOAD_ERR_OK:
                return '';
            case UPLOAD_ERR_INI_SIZE:
                return("The uploaded file exceeds the upload_max_filesize directive (".ini_get("upload_max_filesize").") in php.ini.");
            case UPLOAD_ERR_FORM_SIZE:
                return("The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form.");
            case UPLOAD_ERR_PARTIAL:
                return("The uploaded file was only partially uploaded.");
            case UPLOAD_ERR_NO_FILE:
                return("No file was uploaded.");
            case UPLOAD_ERR_NO_TMP_DIR:
                return("Missing a temporary folder.");
            case UPLOAD_ERR_CANT_WRITE:
                return("Failed to write file to disk");
            default:
                return("Unknown File Error");
        }
    }

    function validate()
    {
        if ( is_null($this->content) || empty($this->content) || !is_array($this->content) )
        {
            $this->setError('No content was uploaded');
            return FALSE;
        }

        if ( $this->content['error'] != UPLOAD_ERR_OK )
        {
            $this->setError($this->getUploadError($this->content['error']));
            return FALSE;
        }
        $fname = $this->content['tmp_name'];
        $oursize = @filesize($fname);
        if ( !file_exists($fname) || !$oursize || $oursize != $this->content['size'] )
        {
            $this->setError('Non-existent or empty file.');
            return FALSE;
        }
        return TRUE;
    }

    // return the fileInfo data structure which contains basic file data like
    // path, size, type, etc
    function getContentData()
    {
        if ( is_null($this->fileInfo) )
        {
            $request = $this->controller->getRequest();

            $fname = $this->content['tmp_name'];

            // if we have $fileInfo['basename'] && $fileInfo['extension'] use
            // $fileInfo['basename'] as second arg to GetMediaType
            $fileInfo = pathinfo($this->content['name']);

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

            // if we can't determine the media type bail out
            if ( !$mt )
            {
                unlink($fname);
                if(!$this->getError())
                {
                    $this->setError('Unable to determine file type or file type not compatible');
                }
                return NULL;
            }

            $newfname = sprintf("%s%s%s",
                    PERSONAL_MEDIA_TEMP_BASE,
                    md5_file($fname),
                    $mt['mobileDeviceCapability_extension']);

            if ( !move_uploaded_file($fname, $newfname) )
            {
                unlink($fname);
                $this->setError('Unable to move file to temporary storage');
                return NULL;
            }

            // FIXME: This is broken. We lose the 'Filename' when we transfer
            // this stuff. Maintain in session or request object?

            // set up the fileInfo array with all data we can gather
            $this->fileInfo['Size'] = filesize($newfname);
            $this->fileInfo['Path'] = $newfname;
            $this->fileInfo['MimeType'] = $mt['mobileDeviceCapability_mime'];
            $this->fileInfo['CapabilityId'] = $mt['mobileDeviceCapability_id'];
            $this->fileInfo['Extension'] = $mt['mobileDeviceCapability_extension'];
            $this->fileInfo['PreviewUrl'] = str_replace(PERSONAL_MEDIA_TEMP_BASE,
                    URL_BASE . 'freshmeat/',
                    $newfname);
            $this->fileInfo['Personal'] = true;
            $this->fileInfo['RealFilename'] = 'PostedFile*' . basename($newfname);
            $this->fileInfo['Filename'] = $extension
                                        ? $filename
                                        : $filename . "{$this->fileInfo['Extension']}";

            $this->mediaHandler = new MediaHandler($controller, $this->fileInfo, $this->error);
        }
        return $this->fileInfo;
    }

    function getFullPath($file)
    {
        $fullfile = PERSONAL_MEDIA_TEMP_BASE . $file;
        return $fullfile;
    }
}


?>
