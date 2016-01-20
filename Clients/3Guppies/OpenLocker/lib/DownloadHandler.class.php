<?

require_once('ContentHandler.class.php');

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

// @author Blake Matheny

// This is neccesarily simple in terms of an API

class DownloadHandlerFactory
{
    function get($_factory = 'KnownUser')
    {
        $handler = sprintf('%sDownloadHandler', $_factory);
        if ( class_exists($handler) )
        {
            return new $handler;
        }
        else
        {
            return NULL;
        }
    }
}

class DownloadHandler extends Message
{
    var $downloadUrl_id;

    function DownloadHandler() { }

    // return short url
    function getMessageText()
    {
        die('Override me; getMessageText()');
    }

    // return true or false
    function validMessage()
    {
        die("Override me; validMessage()");
    }

    function getAttachment()
    {
        die("Override me; getAttachment()");
    }

    // return an array to be encoded. should be uniform.
    function getArrayFromParameters()
    {
        die("Override me; getArrayByParameters()");
    }

    function getLinkData($_array)
    {
        die("Override me; getLinkData()");
    }

    /**
     * @brief Adds an entry to the downloadUrls and return the download link
     *
     * @param $_handler - string - KnownUser or UnknownUser
     *
     * @param $array - array - data to store in the downloadUrls table
     *
     * @return string - download url link
     */
    function getMessageTextHelper($_handler, $array)
    {
        if ( !$array )
            return NULL;

        $id = $this->getIdByChecksum($this->getChecksum($array));
        if ( $id )
        {
            $this->downloadUrl_id = $id;
            return $this->buildUrl($id);
        }
        else
        {
            $id = $this->Add($this->getChecksum($array), $_handler, $this->getEncoded($array));
            if ( $id )
            {
               $this->downloadUrl_id = $id;
               return $this->buildUrl($id);
            }
            else
                return NULL;
        }
    }

    /**
     * @static
     *
     * @brief Returns a reference to the Lockers database ADODB handle
     *
     * @return ADODB connection reference
     */
    function & getLockerDb()
    {
        $controller = $request = $locker_db = null;

        if ( class_exists('Controller') )
            $controller =& Controller::getInstance();
        else
            return NULL;

        if ( is_object($controller) )
            $request =& $controller->getRequest();
        else
            return NULL;

        $locker_db =& Storage::GetLockerDb($request);
        if ( !$locker_db )
            return NULL;
        else
            return $locker_db;
    }

    /**
     * @brief Returns the unserialized, base64 decoded version of the input
     *
     * @param $_data - string - serialized, base64 encoded mixed value
     *
     * @return mixed
     */
    function getUnencoded($_data)
    {
        return @unserialize(base64_decode($_data));
    }

    /**
     * @brief Returns the base64 encoded, serialized version of the input
     *
     * @param $_data - mixed
     *
     * @return string
     */
    function getEncoded($_data)
    {
        return base64_encode(@serialize($_data));
    }

    /**
     * @brief Returns the md5 of the base64 encoded, serialized version of the
     *   input
     *
     * @param $_data - mixed
     *
     * @return string - md5 string 
     */
    function getChecksum($_data)
    {
        return md5($this->getEncoded($_data));
    }

    /**
     * @brief Returns the tiny url key given an md5 checksum
     *
     * @param $_checksum - string
     *
     * @return string - tiny url key
     */
    function getIdByChecksum($_checksum)
    {
        $info = $this->getByChecksum($_checksum);
        if ( is_array($info) && array_key_exists('downloadUrl_id', $info) )
            return $info['downloadUrl_id'];
        else
            return NULL;
    }

    /**
     * @brief Returns a DownloadUrls object given an md5 checksum
     *
     * @param $_checksum - string
     *
     * @return DownloadUrls object
     */
    function getByChecksum($_checksum)
    {
        $locker_db =& $this->getLockerDb();
        if ( !$locker_db )
            return NULL;

        $durl = new DownloadUrls(null, $locker_db);
        return $durl->getByChecksum($_checksum);
    }

    /**
     * @brief Returns the raw data from downloadUrls table given the tiny url key
     *
     * @note This method will increase the download count!
     * 
     * @param $_id - string - tiny url key
     *
     * @return string - raw data (probably base64 encoded, serialized data)
     */
    function getDataById($_id)
    {
        $info = $this->getById($_id);
        if ( is_array($info) && array_key_exists('downloadUrl_data', $info) )
            return $info['downloadUrl_data'];
        else
            return NULL;
    }

    /**
     * @brief Returns a DownloadUrls object given its tiny url key
     *
     * @note The method will increase the download count!
     *
     * @param $_id - string - tiny url key
     *
     * @return DownloadUrls object
     */
    function getById($_id)
    {
        $locker_db =& $this->getLockerDb();
        if ( !$locker_db )
            return NULL;

        $durl = new DownloadUrls(null, $locker_db);
        return $durl->getById($_id);
    }

    /**
     * @brief Returns the tiny url key for this download handler
     *
     * @return string - tiny url key
     */
    function getId()
    {
       return $this->downloadUrl_id;
    }

    /**
     * @brief Creates a new DownloadUrls entry in the database
     *
     * @param $_checksum - FIXME
     *
     * @param $_handler - string - UnknownUser or KnownUser
     *
     * @param $_encoded - string - base64 encoded, serialized array of info
     *
     * @return string - tiny url key if successful,
     *         null otherwise
     */
    function Add($_checksum, $_handler, $_encoded)
    {
        $locker_db =& $this->getLockerDb();
        if ( !$locker_db )
            return NULL;

        $durl = new DownloadUrls(null, $locker_db);
        $durl->downloadUrl_checksum = $_checksum;
        $durl->downloadUrl_handler = $_handler;
        $durl->downloadUrl_data = $_encoded;

        if ( $durl->Add() )
        {
           $this->downloadUrl_id = $durl->downloadUrl_id;
            return $durl->downloadUrl_id;
        }
        else
            return NULL;
    }

    /**
     * @static
     *
     * @brief Generates a download link given a tiny url key
     *
     * @param $_id - string - tiny url key
     *
     * @return string - download url.  Example: http://mixxer.com/t/m1xx3r
     */
    function buildUrl($_id)
    {
        return str_replace('https://','http://',URL_BASE . 't/' . trim($_id));
    }

    /**
     * @brief Fetches the actual data from the downloadUrls table
     *
     * @param $_id - string - tiny url key
     *
     * @return mixed - probably an array
     */
    function getMessageDataFromText($_id)
    {
        $data = $this->getDataById($_id);
        if ( $data && is_array($data) )
            return $this->getUnencoded($data);
        else
            return array();
    }

    /**
     * @brief Fetch device info about the phone trying to use this 
     *    DownloadHandler
     *
     * @return int - deviceId
     */
    function & getDeviceInfo()
    {
        $controller =& Controller::getInstance();
        $user =& $controller->getUser();

        // Get the device id of the device that the user is using to
        //   access the site
        $BrowserInfo =& $user->getAttribute('BrowserInfo');
        if( $BrowserInfo )
        {
            return $BrowserInfo->getDeviceId();
        }
        return null;

    }
}

require_once(dirname(__FILE__) . '/UnknownUserDownloadHandler.class.php');
require_once(dirname(__FILE__) . '/KnownUserDownloadHandler.class.php');

?>
