<?

/**
TODO
If this is to be a replacement for LockerSend, it must do much more than simply
put content in the locker. We need to take care of determining if content is
free or not, decrementing credits, dealing with compatibility, etc. For now I am
going to remove GlobalMedia as an option for a ContentHandler since I do not
have time to finish this right now. Once this is completed,
ContentHandler::storeLockerContent() must also be finished.
 */
class GlobalMediaContentHandler extends ContentHandler
{
    // Database retrieved file info
    var $fetchedInfo    =   null;

    function GlobalMediaContentHandler(&$controller, $handler = null, $content = null)
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

        if ( !is_numeric($this->content) )
        {
            $this->setError('Invalid Media ID Specified');
            return FALSE;
        }

        $info = $this->getInfoByMid($this->content);

        if ( !$info )
        {
            $this->setError('Could not find requested media');
            return FALSE;
        }

        if (!file_exists(MediaConverter::getFullPath($info['media_approved'])))
        {
            $this->setError('Could not find local media');
            return FALSE;
        }

        $this->fetchedInfo = $info;

        return TRUE;
    }

    function getInfoByMid($_pmid)
    {
        $_pmid = intval($_pmid);
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $read_db =& $request->getAttribute('read_db');

        $Media = new Media(null, $read_db);
        $info = $Media->GetDetailsByMid($this->content,
                                        'media_approved = 1',
                                        ADODB_CACHE_HOUR,
                                        NULL,
                                        TRUE);

        if ( !$info )
            return FALSE;
        else
        {
            $tmp = $Media->TranslateGetDetailsByMid($info);
            return $tmp;
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

            if ( is_null($this->fetchedInfo) )
            {
                $this->fetchedInfo = $this->getInfoByMid($this->content);
                if ( !$this->fetchedInfo )
                    return NULL;
            }

            $info = $this->fetchedInfo;

            // set up the fileInfo array with all data we can gather

            $file = $info['media_location'];

            $this->fileInfo['Path'] = $MediaConverter->getFullPath($file);
            $this->fileInfo['Size'] = @filesize($this->fileInfo['Path']);
            $pv = $info['media_preview_image'];
            if ( empty($pv) )
            {
                $pv = sprintf('%s%s/%s/%s/%s', CDN_CNAME, 'media', $file{0}, $file{1}, $file);
            }
            else
            {
               $pv = sprintf('%s%s/%s/%s/%s', CDN_CNAME, 'media', $pv{0}, $pv{1}, $pv);
            }
            $this->fileInfo['PreviewUrl'] = $pv;
            $this->fileInfo['Personal'] = false;
            $this->fileInfo['RealFilename'] = 'GlobalMedia*' . basename($file);
            $this->fileInfo['Filename'] = $info['media_filename'];

            // try and determine the content type
            $mt = $this->_getMediaType($this->fileInfo['Path'], $this->fileInfo['Filename']);

            // if we can't determine the media type bail out
            if ( !$mt )
            {
                if($this->getError())
                {
                    $this->setError('Unable to determine file type or file type not compatible');
                }
                return NULL;
            }

            $this->fileInfo['MimeType'] = $mt['mobileDeviceCapability_mime'];
            $this->fileInfo['CapabilityId'] = $mt['mobileDeviceCapability_id'];
            $this->fileInfo['Extension'] = $mt['mobileDeviceCapability_extension'];

            $this->mediaHandler = new MediaHandler($controller, $this->fileInfo, $this->error);
        }
        return $this->fileInfo;
    }

    function getScope()
    {
        return FALSE;
    }

    function getFullPath($file)
    {
        return MediaConverter::getFullPath($file,false);
    }

}

?>
