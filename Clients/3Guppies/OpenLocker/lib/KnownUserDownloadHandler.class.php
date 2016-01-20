<?
require_once(dirname(__FILE__) . '/DownloadHandler.class.php');

/**
 * @brief The class handles the download of files for known users
 *
 * This type of handler is used only when a logged in user sends
 *   content (premium or personal) to their own phone.  We are
 *   trusting that the user has correctly set their phone model in
 *   their user preferences because we do not try to detect the
 *   the phone model that is hitting the download page when using
 *   this download handler.
 */
class KnownUserDownloadHandler extends DownloadHandler
{
    var $handler = 'KnownUser';
    var $linkData = null;

    /**
     * @brief Validate that we have enough metadata to generate the
     *   link information
     *
     * @note We must have the UserId and either a PersonalId or MediaId
     *
     * @return boolean
     */
    function validMessage()
    {
        // Make sure we have a userid and a personalid or mediaid
        return
            $this->hasParameter('UserId')
           &&
            ($this->hasParameter('PersonalId') || $this->hasParameter('MediaId'));
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
        else
        {
            return array('PhoneNumber'      =>  $this->getParameter('PhoneNumber'),
                         'UserId'           =>  $this->getParameter('UserId'),
                         'PersonalId'       =>  $this->getParameter('PersonalId'),
                         'MediaId'          =>  $this->getParameter('MediaId'),
                         'Sender'           =>  $this->getParameter('Sender'),
                         'CarrierId'        =>  $this->getParameter('CarrierId'),
                         'PhoneNumber'      =>  $this->getParameter('PhoneNumber'),
                         'MediaType'        =>  $this->getParameter('MediaType'),
                         'SenderUserName'   =>  $this->getParameter('SenderUserName'),
                         'SendEventId'      =>  $this->getParameter('SendEventId'),
                         'SourceType'       =>  $this->getParameter('SourceType'),
                         'FromPhoneNumber'  =>  $this->getParameter('FromPhoneNumber'),
                        );
        }
    }

    /**
     * @note This function should be called when sending the media, not upon download
     */
    function getMessageText()
    {
        $array = $this->getArrayFromParameters();
        return $this->getMessageTextHelper('KnownUser', $array);
    }

    function getAttachment()
    {
        if ( !$this->validMessage() )
            return array('Error' => 'Invalid Message');
        return $this->getLinkData(array());
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
    function getLinkData($_array)
    {
        // Request duration caching
        if( !is_null($this->linkData) && is_array($this->linkData) )
        {
            return $this->linkData;
        }

        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $user =& $controller->getUser();
        $is_active = true;
        $id = $uid = $personal = 0;

        // Set the parameters/member variable for this object
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

        // Determine if we send personal media or premium media
        $id = $this->getParameter('PersonalId');
        $userId = $this->getParameter('UserId');
        $read_db =& $request->getAttribute('read_db');
        $UserComm =& new UserComm(null, $read_db);
        $deviceId = $UserComm->getPhoneIdByUserId($userId);

        $ch = null;
        if ( $id )
        {
            $ch = new ContentHandler(Controller::getInstance(), 'PersonalMedia', $id);
            $personal = 1;
        }
        else
        {
            $id = $this->getParameter('MediaId');
            $ch = new ContentHandler(Controller::getInstance(), 'GlobalMedia', $id);
        }

        // Validate the content handler
        if ( !is_object($ch) || !method_exists($ch, 'validate') )
        {
            return array('Error' => 'Invalid ContentHandler');
        }

        // Check for initialization errors
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
        if ( !$data )
        {
            return array('Error' => 'Could not fetch handler data');
        }

        // fetch device info

        $uid = base_convert($userId,10,36);
        $id = base_convert($id, 10, 36);

        // If there is no filename set, we need to generate a random one
        if ( !array_key_exists('Filename', $data) || !$data['Filename'] )
        {
            $data['Filename'] = sprintf('cnd%d', rand(1,100));
        }

        $original_title = $data['Filename'];
        $extension = $data['Extension'];

        // Convert all non word characters to underscores and make sure
        //   that there is only one underscore in a row
        $title = preg_replace( '/[\W\[\]]/m' , '_' , $data['Filename']);
        $title = preg_replace( '/_{2,}/m', '_', $data['Filename']);

        // Does this phone have special download requirements?
        $BrowserInfo =& $user->getAttribute('BrowserInfo');
        if( !is_object($BrowserInfo) || !$BrowserInfo->isValid() )
        {
            $BrowserInfo =& HandsetDetect::getPhoneInfoByDeviceId($deviceId);
        }

        $file_format = null;
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

        // Look up the database info about the personal media or premium media
        //   to find the local path to the file and also its mime type
        // Also, generate the locker download url for the content
        $local_file = null;
        $mime_type = null;
        $locker_download_url = null;
        if ( $personal )
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
                    $mime_info = MediaConverter::GetContentInfoByExtension($ext_info['extension']);
                if ( isset($mime_info['content-type']) )
                    $mime_type = $mime_info['content-type'];
            }
        }
        else
        {
            $le = new LockerEntry($this->getParameter('UserId'),
                    null,
                    $this->getLockerDb(),
                    $this->getLockerDb());
            $entry = $le->getEntry($this->getParameter('UserId'), $this->getParameter('MediaId'));
            if ( !$entry || !is_array($entry) || !array_key_exists('lockerEntry_is_active', $entry) || $entry['lockerEntry_is_active'] == 0 )
                $is_active = false;
            else
            {
                $mime_info = array();
                $local_file = MediaConverter::GetFullPath($entry['lockerEntry_location'], false);
                $ext_info = MediaConverter::GetNameExtension($entry['lockerEntry_location']);
                if ( is_array($ext_info) && array_key_exists('extension', $ext_info) )
                    $mime_info = MediaConverter::GetContentInfoByExtension($ext_info['extension']);
                if ( isset($mime_info['content-type']) )
                    $mime_type = $mime_info['content-type'];

                if ( $mime_info )
                    $extension = '.' . $ext_info['extension'];
                else
                    $extension = $data['Extension'];

                $locker_download_url = MEDIA_URL
                    . ($personal ? 1 : 0)
                    . '/'
                    . $uid
                    . '/'
                    . $id
                    . '/'
                    . ($redirect ? 1 : 0)
                    . '/'
                    . $title
                    . '.'
                    . $extension
                    . (!in_array($extension,array('jad','jar')) ? $file_format : '');
            }

        }

        // If the content is active (downloadable), then we will set the
        //   DownloadUrl field in the return value.  The only was that the
        //   content will be inactive is if the media is premium and if the 
        //   content is unavailable or could not be found
        if ( $is_active )
        {
            if ( $locker_download_url )
            {
                $dl = $locker_download_url;
            }
            else
            {
                $dl = sprintf('%s%d/%s/%s/%d/%s%s%s',
                    MEDIA_URL,
                    $personal,
                    $uid,
                    $id,
                    $redirect,
                    $title,
                    $extension,
                    $file_format);
            }

            $pl = sprintf('%s%s/%s/%s',
                    MEDIA_URL,
                    $personal ? 'personal' : 'global',
                    basename($data['Path']),
                    $title);
        }
        else
        {
            // inactive, don't show links
            $dl = $pl = null;
        }

        // If we are downloading via wap, map this user agent to the deviceId
        //   set for the user whe sent this content.
        // Note: we are trusting that the user sent the content to themselves
        //   and correctly have their phone model set.
        $ct = $controller->getContentType();
        if( ($ct == 'wml' || $ct == 'xhtmlMobile') &&
            $this->getParameter('AccessCount') == 0 )
        {

            $write_db =& $request->getAttribute('write_db');
            if($deviceId && array_key_exists('HTTP_USER_AGENT', $_SERVER) )
            {
                $mua =& new MobileUserAgents(null, $write_db);
                $mua->user_id = $userId;
                $mua->mobileDevice_id = $deviceId;
                $mua->mobileUserAgent_string = $_SERVER['HTTP_USER_AGENT'];
                $mua->Log();
                $PhoneInfo =& HandsetDetect::getPhoneInfoByUserAgent($_SERVER['HTTP_USER_AGENT'], true);
                $user->setAttributeByRef('BrowserInfo', &$PhoneInfo);
            }
        }

        $this->linkInfo = array(
                'DownloadUrl'   =>  $dl,
                'Title'         =>  $original_title,
                'PreviewUrl'    =>  $pl,
                'Converted'     =>  $local_file,
                'MimeType'      =>  $mime_type,
                'Extension'     =>  $extension,
                );
        return $this->linkInfo;
    }
}

?>
