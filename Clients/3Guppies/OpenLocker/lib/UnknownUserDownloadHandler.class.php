<?
require_once(dirname(__FILE__) . '/DownloadHandler.class.php');

/**
 * @brief This class handles the download of files for unknown users
 *
 * This type of handler is used if the user is anonymous (not logged in)
 *   or if the user is logged in and is sending to a phone number different
 *   than their own.
 *
 * When using this download hander, we try and figure out what kind of phone
 *   is downloading the content via its HTTP_USER_AGENT.  We may have to
 *   convert the media for this user again.
 */
class UnknownUserDownloadHandler extends DownloadHandler
{
    var $handler = 'UnknownUser';
    var $linkData = null;

    /**
     * @brief Validate that we have enough meta data to generate the link
     *   information
     *
     * @note We must have the PersonalId
     *
     * @return boolean
     */
    function validMessage()
    {
        // If downloading to a phone, and user is unknown, only allow personal id
        // If sharing via email, make sure we have personal or media id
        return
            (
             ($this->hasParameter('PhoneNumber') && $this->hasParameter('CarrierId'))
                &&
              $this->hasParameter('PersonalId')
            )
            ||
            (
             ($this->hasParameter('PersonalId') || $this->hasParameter('MediaId'))
                &&
              $this->hasParameter('Email')
            );
    }

    /**
     * @brief Returns a normalized set of parameters/meta data for this object
     *
     * @return array([key] => [value])
     */
    function getArrayFromParameters()
    {
        if ( !$this->validMessage() )
        {
            return array();
        }
        elseif ( $this->hasParameter('Email') )
        {
            return array('Email'        => $this->getParameter('Email'),
                         'PersonalId'   =>  $this->getParameter('PersonalId'),
                         'MediaId'      =>  $this->getParameter('MediaId'),
                         'Sender'       =>  $this->getParameter('Sender'),
                         'SenderUserName'   =>  $this->getParameter('SenderUserName'));
        }
        elseif ( $this->hasParameter('PhoneNumber') && $this->hasParameter('CarrierId') )
        {
            return array(   'PhoneNumber'       =>  $this->getParameter('PhoneNumber'),
                            'CarrierId'         =>  $this->getParameter('CarrierId'),
                            'PersonalId'        =>  $this->getParameter('PersonalId'),
                            'Sender'            =>  $this->getParameter('Sender'),
                            'SenderUserName'    =>  $this->getParameter('SenderUserName'),
                            'MediaType'         =>  $this->getParameter('MediaType'),
                            'SendEventId'       =>  $this->getParameter('SendEventId'),
                            'SourceType'        =>  $this->getParameter('SourceType'),
                            'FromPhoneNumber'   =>  $this->getParameter('FromPhoneNumber')
                        );
        }
    }

    /**
     * @note This function should be called only when sending the media,
     *   not upon download
     */
    function getMessageText()
    {
        $array = $this->getArrayFromParameters();
        return $this->getMessageTextHelper('UnknownUser', $array);
    }

    function getAttachment()
    {
        if ( !$this->validMessage() )
            return array('Error' => 'Invalid Message');
        return $this->getLinkData(array(),true);
    }

    /**
     * @brief Returns information about the download link
     *
     * @note This function should be called upon download, not when sending the media
     *
     * @param $_array - array - meta data that helps build the download link
     *
     * @return array
     */
    function getLinkData($_array, $_attachment = false)
    {
        // Request duration caching
        if( !is_null($this->linkData) && is_array($this->linkData) )
        {
            return $this->linkData;
        }

        $controller =& Controller::getInstance();
        $user =& $controller->getUser();

        $preview_only = (!$_attachment && $controller->getContentType() == 'html');

        $personal = 'global';

        // Set the parameters/member variables for this object.
        //   Remember, this object extends the Message class
        foreach ( $_array AS $key => $value )
        {
            $this->setParameter($key, $value);
        }

        // Validate that we have enough information to proceed
        if ( !$this->validMessage() )
        {
            return array('Error' => 'Invalid Error');
        }

        // Grab the current variables that we have
        $personal_id = $this->getParameter('PersonalId');
        $media_id = $this->getParameter('MediaId');
        $email = $this->getParameter('Email');
        $phone_number = $this->getParameter('PhoneNumber');
        $carrier_id = $this->getParameter('CarrierId');
        $sender = $this->getParameter('Sender');
        $senderUsername = $this->getParameter('SenderUserName');

        // Fetch a content handler and recover any edit options we can from
        //   the converted filename
        $ch = null;
        $editOptions = array();
        if ( $personal_id )
        {
            $ch = new ContentHandler($controller, 'PersonalMedia', $personal_id);
            $personal = 'personal';
            $mediaModel =& $controller->getModel('Media','Media');
            $pix =& $mediaModel->getPixByPersonalMediaId($personal_id);

            $mediaInfo =& $pix->getMediaInfo();
            $convertedFilename = $mediaInfo->getLocation();
            $editOptions = MediaConverter::getEditOptionsFromFilename($convertedFilename);
        }
        else
        {
            $ch = new ContentHandler($controller, 'GlobalMedia', $media_id);
        }

        // Validate the content handler
        if ( !is_object($ch) || !method_exists($ch, 'validate') )
        {
            return array('Error' => 'Invalid ContentHandler');
        }

        // Check for content handler initialization errors
        if ( $ch->getError() )
        {
            return array('Error' => $ch->getError());
        }

        // Validate the content within the content handler
        if ( !$ch->validate() )
        {
            return array('Error' => $ch->getError());
        }

        // Fetch information about the content
        $data = $ch->getContentData();

        // Fetch the phone information about the phone accessing this page
        $BrowserInfo = $user->getAttribute('BrowserInfo');
        if ( !$preview_only )
        {
            //$editOptions['idSizeDX'] = $BrowserInfo->getDeviceWidth();
            //$editOptions['idSizeDY'] = $BrowserInfo->getDeviceHeight();

            $postVars = array(
                'SendNotice'    =>  0,
                'ToPhone'       =>  $this->getParameter('PhoneNumber'),
                'FromPhone'     =>  $this->getParameter('PhoneNumber'),
                'FromName'      =>  $this->getParameter('SenderUserName'),
                'Reconvert'     =>  1,
                'PersonalId'    =>  $this->getParameter('PersonalId'));

            // Reconvert if needed
            if ( !$ch->convertContent($editOptions, $postVars) )
            {
                return array('Error' => $ch->getError(),
                    'MediaInfo' => $ch->handler->fetchedInfo);
            }
        }

        // Refetch information about the content
        $data = $ch->getContentData();

        if ( !$data )
        {
            return array('Error' => 'Could not fetch handler data');
        }

        // If there is no filename set, we need to generate a random one
        if ( !array_key_exists('Filename', $data) || !$data['Filename'] )
        {
            $data['Filename'] = sprintf('cnd%d', rand(1,100));
        }

        $original_title = $data['Filename'];

        // Figuring out the correct extension is important! Sprint phone
        //   require that the file extensions match the mime type!
        $extension = "";
        if( array_key_exists('ConvertedTo', $data) &&
            $BrowserInfo->hasCapabilityId($data['ConvertedTo']) )
        {
            $extension = $BrowserInfo->allCapabilities[$data['ConvertedTo']]['extension'];
        }
        elseif(array_key_exists('Converted', $data))
        {
            $convertedFilenameInfo = MediaConverter::GetNameExtension($data['Converted']);
            $extension = "." . $convertedFilenameInfo['extension'];
        }
        elseif(array_key_exists('Extension', $data))
        {
            $extension = $data['Extension'];
        }

        // Convert all not word characters to underscores and make sure
        //   that there is only one underscore in a row
        $title = preg_replace( '/[\W\[\]]/m' , '_' , $data['Filename']);
        $title = preg_replace( '/_{2,}/m', '_', $data['Filename']);

        // Does this phone have special download requirements?
        $redirect = 0;
        $file_format = "";
        $local_file = $mime_type = null;
        if ( !$preview_only )
        {
            $redirect = $BrowserInfo->hasCapabilityId(21) ? 1 : 0;

            // gcd
            if( $BrowserInfo->hasCapabilityId(5) )
            {
                $file_format = '.gcd';
            }
            // mcd
            elseif ( $BrowserInfo->hasCapabilityId(25) )
            {
                $file_format = '.mcd';
            }
            // OMA-FL
            elseif ( $BrowserInfo->hasCapabilityId(34) )
            {
                $file_format = '.dm';
            }

            $mime_info = MediaConverter::GetContentInfoByExtension($extension);
            if( array_key_exists('content-type', $mime_info) )
            {
                $mime_type = $mime_info['content-type'];
            }
            $local_file = MediaConverter::GetFullPath($data['Converted'], $personal ? true : false);

            // Sprint phone require that the file name + extensions be 25
            //   characters or less
            $end_length = strlen($extension.$file_format);
            $fullfile = substr($title,0,25-$end_length) . $extension . $file_format;
            $dl = sprintf('%s%s/%s/%s',
                    MEDIA_URL,
                    $personal,
                    $data['Converted'],
                    $fullfile
                    );
        }
        else
        {
            $dl = $data['PreviewUrl'];
        }

        if ( $_attachment && $personal == 'personal' )
        {
            $info = PersonalMediaContentHandler::getInfoByPmid($this->getParameter('PersonalId'), $redirect);
            if ( $info && array_key_exists('locker_download_url', $info) )
            {
                $mime_info = array();
                // TODO: account for jad/jar extension
                $locker_download_url = $info['locker_download_url'] . $file_format;
                $local_file = MediaConverter::GetFullPath($info['personalMedia_location'], true);
                $ext_info = MediaConverter::GetNameExtension($info['personalMedia_location']);

                if ( is_array($ext_info) && array_key_exists('extension', $ext_info) )
                {
                    $mime_info = MediaConverter::GetContentInfoByExtension($ext_info['extension']);
                }

                if ( array_key_exists('content-type', $mime_info) )
                {
                    $mime_type = $mime_info['content-type'];
                }

                if($locker_download_url)
                {
                    $dl = $locker_download_url;
                }
            }
        }
        $this->linkData = array(
                'DownloadUrl'   =>  $dl,
                'Title'         =>  $original_title,
                'PreviewUrl'    =>  $data['PreviewUrl'],
                'Converted'     =>  $local_file,
                'MimeType'      =>  $mime_type,
                'Preexisting'   =>  array_key_exists('Preexisting', $data) ? $data['Preexisting'] : "",
                'JobId'         =>  array_key_exists('JobId', $data) ? $data['JobId'] : null,
                'Extension'     =>  $extension,
                );
        return $this->linkData;
    }
}

?>
