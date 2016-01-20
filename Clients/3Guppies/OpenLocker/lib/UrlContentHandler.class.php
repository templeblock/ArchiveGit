<?

require_once("UrlFilter.class.php");

class UrlContentHandler extends ContentHandler
{
    function UrlContentHandler(&$controller, $handler = null, $content = null)
    {
        $this->controller = $controller;
        $this->handler = $handler;
        $this->content = $content;
    }

    function validate()
    {
        if ( is_null($this->content) || empty($this->content) )
        {
            $this->setError('URL is empty');
            return FALSE;
        }
        $url = @parse_url($this->content);
        if ( !$url || !is_array($url) || !array_key_exists('scheme', $url) )
        {
            //trigger_error('Bad mobilize url: ' . $this->content, E_USER_NOTICE);
            $this->setError('Invalid Mobilize URL');
            return FALSE;
        }
        if ( $url['scheme'] != 'http' )
        {
            $this->setError("Invalid URI scheme {$url['scheme']}");
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
            $read_db =& $request->getAttribute('read_db');
            $MediaConverter = new MediaConverter(null, $read_db);

            // Do any specialized url filtering
            $urlFilter = UrlFilter::getInstance($this->content);
            $filteredUrl = $urlFilter->getFilteredUrl();

            // if we have $fileInfo['basename'] && $fileInfo['extension'] use
            // $fileInfo['basename'] as second arg to GetMediaType
            //$fileInfo = pathinfo($this->content);
            $fileInfo = pathinfo($filteredUrl);

            // TODO: add CURLOPT_REFERER
            // Fetch content with curl
            //$ch = curl_init($this->content);
            $ch = curl_init($filteredUrl);
            curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1) Gecko/20061010 Firefox/2.0');
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
            curl_setopt($ch, CURLOPT_HEADER, 0);
            $data = curl_exec($ch);
            curl_close($ch);

            // Check the connection to the database and reconnect if
            //   our connection has timed out
            mysql_ping($read_db->_connectionID);
            if(array_key_exists('ADODB_SESS_CONN', $GLOBALS))
            {
                $session_db =& $GLOBALS['ADODB_SESS_CONN'];
                mysql_ping($session_db->_connectionID);
            }

            // Write content out to temporary file
            $tmpfname = tempnam('/tmp', "UrlContentHandler_");
            $handle = fopen($tmpfname, "w");
            fwrite($handle,$data);
            fclose($handle);

            // make sure we got a file
            if ( !$data || count($data) <= 0 )
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
            $mt = $this->_getMediaType($tmpfname, $filename);


            // if we can't determine the media type bail out
            if ( !$mt )
            {
                unlink($tmpfname);
                if(!$this->getError())
                {
                    $this->setError('Unable to determine file type or file type not compatible');
                }
                return NULL;
            }

            $newfname = sprintf("%s%s%s",
                    PERSONAL_MEDIA_TEMP_BASE,
                    md5_file($tmpfname),
                    $mt['mobileDeviceCapability_extension']);

            // rename from tmp to freshmeat directory
            if ( !rename($tmpfname, $newfname) )
            {
                unlink($tmpfname);
                $this->setError('Unable to move file to temporary location');
                return NULL;
            }

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
            $this->fileInfo['RealFilename'] = 'Url*' . basename($newfname);
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
