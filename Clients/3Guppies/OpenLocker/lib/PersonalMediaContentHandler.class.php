<?

/**
 * @brief This class handles retrieving and validating files that are already
 *   stored as PersonalMedia
 */
class PersonalMediaContentHandler extends ContentHandler
{

    function PersonalMediaContentHandler(&$controller, $handler = null, $content = null)
    {
        $this->controller = $controller;
        $this->handler = $handler;
        $this->content = $content;
    }

    /**
     * @brief Validate the content we are trying to fetch
     *
     * @return boolean
     */
    function validate()
    {
        if ( is_null($this->content) || empty($this->content) )
        {
            $this->setError('No media was specified');
            return FALSE;
        }

        if ( !is_numeric($this->content) )
        {
            $this->setError('Invalid Personal Media ID Specified');
            return FALSE;
        }

        $info = $this->getInfoByPmid($this->content);

        if ( !$info )
        {
            $this->setError('Could not find requested personal media');
            return FALSE;
        }

        // Both the original and the converted files must exist
        if ( !file_exists(MediaConverter::getFullPath($info['personalMedia_location'],true)) ||
             !file_exists(MediaConverter::getFullPath($info['personalMedia_original'],true)) )
        {
            $this->setError("Could not find local personal media " . $info['personalMedia_location']);
            return FALSE;
        }

        $this->fetchedInfo = $info;

        return TRUE;
    }

    /**
     * @brief Fetch information about personal media
     *
     * @note This info is cached for the lifetime of this object
     *
     * @param $_pmid - int - personal media id
     *
     * @param $_redirect - whether the phone supports redirects (used for the
     *   download link)
     *
     * @return array
     */
    function getInfoByPmid($_pmid, $_redirect = false)
    {
        $_pmid = intval($_pmid);
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $locker_db =& Storage::GetLockerDb($request);

        $cacheKey = "PersonalMediaInfo_$_pmid";
        if($request->hasAttribute($cacheKey))
        {
            return $request->getAttribute($cacheKey);
        }

        $PersonalMedia = new PersonalMedia(null, $locker_db);
        $info = $PersonalMedia->GetPersonalFilesInfoByMediaId($_pmid, $_redirect);

        if ( !$info || !is_array($info) || count($info) != 1 )
        {
            return FALSE;
        }

        $fetchedInfo = array_pop($info);
        $request->setAttribute($cacheKey, $fetchedInfo);
        return $fetchedInfo;
    }

    /**
     * @brief Returns the fileInfo data structure which contains basic file
     *   data like path, size, type, etc
     *
     * @note These values are cached for the duration of the request
     * @note The Converted, ConvertedTo, and Original values are set
     *   when the media is converted (in MediaConverter::convertContent)
     *
     * @return array
     */
    function getContentData()
    {
        if ( is_null($this->fileInfo) )
        {
            $request = $this->controller->getRequest();
            $locker_db =& Storage::GetLockerDb($request);
            $read_db =& $request->getAttribute('read_db');

            $MediaConverter = new MediaConverter(null, $read_db);

            $info = $this->getInfoByPmid($this->content);

            // set up the fileInfo array with all data we can gather
            $file = file_exists($MediaConverter->GetFullPath($info['personalMedia_original'],true))
                  ? $info['personalMedia_original']
                  : $info['personalMedia_location'];

            $this->fileInfo['Path'] =
                $MediaConverter->getFullPath($file,true);
            $this->fileInfo['Size'] = @filesize($this->fileInfo['Path']);
            $this->fileInfo['PreviewUrl'] = sprintf('%spersonal/%s/%s',
                    MEDIA_URL,
                    $file,
                    $file);
            $this->fileInfo['Personal'] = true;
            $this->fileInfo['RealFilename'] = 'PersonalMedia*' . basename($file);
            $this->fileInfo['Filename'] = $info['personalMedia_name'];

            // try and determine the content type
            $mt = $this->_getMediaType($this->fileInfo['Path'],$this->fileInfo['Filename']);

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

    /**
     * @static
     *
     * @brief Returns the full path to the media given the md5
     *
     * @param $file - string - md5 string + extension
     *
     * @return string - path to the personal media file
     */
    function getFullPath($file)
    {
        return MediaConverter::getFullPath($file,true);
    }

}

?>
