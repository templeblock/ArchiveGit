<?php

require_once(dirname(__FILE__) . '/../lib/ContentHandler.class.php');
require_once(HOME_BASE.'opt/util/EventLog.class.php');

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief This is the 'mobilizer', and allows putting content in a users locker.
 *
 * This is a general purpose mechanism for putting content into a user locker.
 * Eventually this should replace the LockerSend action in the Download module
 * but for now it simply replaces the user upload feature of the locker.
 *
 * This action follows the chain of command design pattern.  Each intermediate
 * action is responsible for initializing a validating its own parameters.
 *
 * All requests must come through the MobilizeAction.
 *
 * The actions in this chain are:
 *   MobilizeAction
 *   PhoneInfoAction
 *   EditorAction
 *   StoreAndSendAction
 *
 * By default, the input view is rendered in most cases. Another actions view is
 * rendered if the file was successfully uploaded and there is an intermediary
 * step required such as tagging, image resizing or video editing.
 *
 * This action supports sending the following to a locker: urls, a user file, a
 * personal file of another user. Only one of these may be specified or 
 * VIEW_INPUT will be returned with an appropriate error message.
 *
 * Regardless of which mechanism is used for supplying the file, the same basic
 * algorithm is applied to the content. This algorithm is documented inline in
 * the execute method.
 *
 * Notes:
 *  On success, $request->hasAttribute('MobilizeSuccess') is true
 *  On failure, $request->hasError('Mobilize') is true
 *
 * @author  Blake Matheny, Jeff Ching
 *
 * @date 2007-02-13
 */

class MobilizeAction extends Action
{

    /**
     * NOTE: Each of the variables below is an object that inherits from
     * ContentHandler. A ContentHandler knows how to validate, detect, fetch,
     * manipulate, convert and store content. A ContentHandler makes use of the
     * composite and factory patterns.
     */

    /**
     * @brief URL to fetch content from.
     *
     * If a Url is specified, fetch content from that url and put it in the
     * users locker. The corresponding Request parameter is 'Url', a url encoded
     * string. If passing in via javascript, be sure to use encodeURIComponent
     */
    var $Url        =   null;

    /**
     * @brief Content of $_FILES['userfile'], an array.
     *
     * For legacy purposes, the corresponding Request parameter is 'userfile'
     * and not 'UserFile'. Eventually we will need to migrate existing code to
     * use this new interface, including the mixer.
     */
    var $UserFile   =   null;

    /**
     * @brief An ID that corresponds with personalMedia_id.
     *
     * This supports putting the content from another users locker (public
     * content) into your locker. The content must be public. The Request
     * parameter is 'PersonalId', an integer.
     */
    var $PersonalId =   null;

    /**
     * @brief An ID that corresponds with media_id.
     *
     * This supports putting content from the global catalog into your locker.
     * The content must be public, available, and compatible. The request
     * parameter is 'MediaId', an integer.
     *
     * Note: See GlobalMediaContentHandler
     */
    var $MediaId    =   null;

    /**
     * @brief Temporary working file, probably being edited.
     *
     * After a file is acquired, if there is an editor available this variable
     * is used to inform the editor which file to work with. The file is
     * prepended by '/media/files/freshmeat'. The request parameter is TempFile.
     */
    var $TempFile   =   null;

    /**
     * @brief Filename is transient, so allow it to be captured
     *
     * When a file is uploaded and an intermediary step is required, the
     * filename is lost. This variable allows it to be retained.
     */
    var $Filename       =   null;

    /**
     * @brief Method of passing file handler to the next stage of Mobilization
     *
     * After the initial method of fetching a file, we want to keep a handle for
     * the file we are dealing with.  This variable tells how we will maintain
     * maintain the file handle
     *
     * Valid options are 'TempFile' and 'PersonalId'.  We do not allow 'Url' to
     * because we would fetch the media from the remote host multiple times 
     * (instead we download it the first time and pass it like an uploaded file)
     */
    var $InputType      =   null;

    /**
     * @brief File handle value
     */
    var $InputValue     =   null;

    /**
     * @brief How the content was originally specified:
     *
     * Options:
     *   SOURCE_TYPE_UNKNOWN
     *   SOURCE_TYPE_UPLOAD
     *   SOURCE_TYPE_URL
     *   SOURCE_TYPE_PERSONAL_MEDIA
     *   SOURCE_TYPE_PREMIUM_MEDIA
     *   SOURCE_TYPE_WIDGET
     */
    var $InputMethod    =   null;

    /**
     * @brief The source url of the content if using SOURCE_TYPE_URL or 
     *   SOURCE_TYPE_WIDGET
     */
    var $SourceUrl      =   null;

    /**
     * @brief This Message object contains all the mobilize parameters and
     *   is aggregated over the course of the mobilize chain of command
     *
     * Each action in the chain of command adds its own parameters so that all
     * the parameters are available for the subsequent actions.
     *
     * List of parameters:
     *   MobilizeAction:
     *     InputType
     *     InputValue
     *     Filename
     *   PhoneInfoAction:
     *     AgreeTC
     *     ToPhoneNumber
     *     FromPhoneNumber
     *     FromName
     *     MyPhone
     *   EditorAction:
     *     EditOptions
     *     EditPassThru
     *   StoreAndSendAction:
     *
     */
    var $MobilizeParams =   null;

    /**
     * @brief Boolean whether or not this media can be shared with a friend
     *
     * Currently, we only disallow sharing for mp3 files
     */
    var $IsShareable = true;

    /**
     * @brief Identifier for campaigns
     */
    var $Affiliate = 0;

// +---------------------------------------------------------------------------+
// | Initialization, Execution and Validation                                  |  
// +---------------------------------------------------------------------------+


    /**
     * @brief Initialize our variables.
     */
    function initialize (&$controller, &$request, &$user)
    {
        // All subsequent actions should have the 'Mobilizer' tab selected in
        //   the navigation bar
        $request->setAttribute('SF_Nav_Selected', 'Mobilizer');

        $this->SourceUrl = $request->hasParameter('page') ? 
            $request->getParameter('page') : 
            $request->getParameter('SourceUrl');

        // ContentHandler($controller, 'HandlerType', 'Content');
        if ( $request->hasParameter('Url') )
        {
            $Url = urldecode($request->getParameter('Url'));
            $this->InputType    =   'TempFile';
            $this->Url          =   new ContentHandler(&$controller, 'Url', $Url);
            if($controller->getContentType() == 'async')
            {
                $this->InputMethod  =   SOURCE_TYPE_WIDGET;
            }
            else
            {
                $this->InputMethod  =   SOURCE_TYPE_URL;
            }
            $this->SourceUrl = $request->hasParameter('page') ?
                $request->getParameter('page') : 
                $Url;
        }

        if (is_array($_FILES) && array_key_exists('userfile', $_FILES) && is_array($_FILES['userfile']))
        {
            $this->InputType    =   'TempFile';
            $this->UserFile     =   new ContentHandler(
                                            &$controller,
                                            'PostedFile',
                                            $_FILES['userfile']);
            $this->InputMethod  =   SOURCE_TYPE_UPLOAD;
        }

        if ( $request->hasParameter('PersonalId') )
        {
            $PersonalId = $request->getParameter('PersonalId');
            $this->InputType    =   'PersonalId';
            $this->InputValue   =   $PersonalId;
            $this->PersonalId   =   new ContentHandler(
                                            &$controller,
                                            'PersonalMedia',
                                            $PersonalId);
            $this->InputMethod  =   SOURCE_TYPE_PERSONAL_MEDIA;
        }

        /* See GlobalMediaContentHandler for why we don't support This->MediaId */

        if ( $request->hasParameter('TempFile') )
        {
            $TempFile = $request->getParameter('TempFile');
            $this->InputType    =   'TempFile';
            $this->InputValue   =   $TempFile;
            $this->TempFile     =   new ContentHandler(
                                            &$controller,
                                            'LocalMedia',
                                            $TempFile);
            $this->InputMethod  =   $request->getParameter('InputMethod');
        }

        if( $request->hasParameter('src') )
        {
            $src = $request->getParameter('src');
            switch($src)
            {
            case 'mobiwidg':
                $this->InputMethod = SOURCE_TYPE_WIDGET;
                break;
            case 'ffplugin':
                $this->InputMethod = SOURCE_TYPE_FFPLUGIN;
                break;
            case 'ieplugin':
                $this->InputMethod = SOURCE_TYPE_IEPLUGIN;
                break;
            default:
                break;
            }
        }

        $this->MobilizeParams =& new Message();
        $request->setAttributeByRef('MobilizeParams', &$this->MobilizeParams);

        if($request->hasParameter('Filename')){
            $this->Filename = $request->getParameter('Filename');
            $this->MobilizeParams->setParameter('Filename', $this->Filename);
        }
        $this->MobilizeParams->setParameter('InputMethod', $this->InputMethod);
        $this->MobilizeParams->setParameter('SourceUrl', $this->SourceUrl);

        return TRUE;
    }

    /**
     * @brief Execute all business logic.
     *
     * The basic algorithm works as follows:
     *
     *      Fetch content info
     *          Retrieve content
     *          Ensure content sanity (not validation)
     *          Determine content type
     *          Setup a MediaHandler based on content type
     *      If an editor is available and requested, display it
     *          The MediaHandler knows about editors
     *      Convert the content for the phone (if requested)
     *          The MediaHandler knows about content conversion
     *      Store the content on the file system and database
     *          The ContentHandler knows how to manipulate files and use the db
     *      Notify the user via SMS if they requested it
     *          The Share action knows how to handle this based on the ContentHandler
     *
     * On error set Mobilize error. On Success set MobilizeSuccess.
     */
    function execute (&$controller, &$request, &$user)
    {
        $ContentHandler =& $this->getContentMethod();
        $Content =& $ContentHandler->getContentData();
        
        // Upload form page
        if ( !$Content || !is_array($Content) || $ContentHandler->getError() )
        {
            $error = $ContentHandler->getError();
            $request->setError('Mobilize', $error ? $error : 'Unknown content error');
            return VIEW_INPUT;
        }

        if( is_null($this->InputValue) && 
            array_key_exists('RealFilename', $Content) )
        {
            $this->InputValue = $Content['RealFilename'];
        }
        if( is_null($this->Filename) )
        {
            $this->Filename = $Content['Filename'];
            $this->MobilizeParams->setParameter('Filename',
                $this->Filename);
        }

        if( array_key_exists('CapabilityId', $Content) )
        {
            if($Content['CapabilityId'] == 26)
            {
                $this->IsShareable = false;
            }
        }

        $this->MobilizeParams->setParameterByRef('ContentHandler',
            &$ContentHandler);
        $this->MobilizeParams->setParameterByRef('Content', 
            &$Content);
        $this->MobilizeParams->setParameter('IsShareable', $this->IsShareable);
        $this->MobilizeParams->setParameter('InputType', $this->InputType);
        $this->MobilizeParams->setParameter('InputValue', $this->InputValue);
        $this->MobilizeParams->setParameter('Affiliate', $this->Affiliate);

        $controller->forward('OpenLocker', 'PhoneInfo');
        return VIEW_NONE;
    }

    /**
     * @brief Validate the request as a whole.
     *
     * The requirements for validation are documented inline in the method
     * below.
     */
    function validate (&$controller, &$request, &$user)
    {
        if ( $request->hasError('Mobilize') )
            return FALSE;

        /**
         * 1. One of Url, UserFile, PersonalId, MediaId or TempFile MUST be
         *    specified.
         * 2. Only one of Url, UserFile, PersonalId, MediaId or TempFile may be
         *    specified at any one time.
         */
        $count = $this->contentMethodCount();
        if ( $count > 1 )
        {
            $request->setError('Mobilize',
                               'Invalid API Usage. Only one content method may be specified');
            return FALSE;
        }
        elseif ( $this->contentMethodCount() == 0 ) // First usage
        {
            return FALSE;
        }

        // Now make sure that the one which is used is valid.
        $contentMethod =& $this->getContentMethod();
        if ( !is_object($contentMethod) || !method_exists($contentMethod, 'validate') )
        {
            $request->setError('Mobilize', 'Unable to instantiate content handler');
            return FALSE;
        }

        // There may have been an error during instantiation, so check for it
        if ( $contentMethod->getError() )
        {
            $request->setError('Mobilize', $contentMethod->getError());
            return FALSE;
        }

        /**
         * 3. Ensure that the data that was submitted via the request is valid
         *    according to the content handler.
         */
        if ( !$contentMethod->validate() )
        {
            $request->setError('Mobilize', $contentMethod->getError());
            return FALSE;
        }

        // validate the page param
        if( $request->hasParameter('page') )
        {
            $this->SourceUrl = addslashes($request->getParameter('page'));
        }

        // validate the ext_cmp param
        if( $request->hasParameter('ext_cmp') )
        {
            $this->Affiliate = addslashes($request->getParameter('ext_cmp'));
        }
        elseif( $request->hasParameter('Affiliate') )
        {
            $this->Affiliate = addslashes($request->getParameter('Affiliate'));
        }

        return TRUE;
    }


// +---------------------------------------------------------------------------+
// | MobilizeAction Methods                                                    |  
// +---------------------------------------------------------------------------+

// Url, UserFile, PersonalId, MediaId, TempFile

    /**
     * @brief Returns the number of input methods used for this workflow
     */
    function contentMethodCount()
    {
        $count = 0;
        if ( is_object($this->Url) )
            $count += 1;
        if ( is_object($this->UserFile) )
            $count += 1;
        if ( is_object($this->PersonalId) )
            $count += 1;
        if ( is_object($this->MediaId) )
            $count += 1;
        if ( is_object($this->TempFile) )
            $count += 1;
        return $count;
    }

    /**
     * @brief Returns the ContentHandler that will be used for this workflow
     *
     * @return ContentHandler object,
     *         false if no input methods
     */
    function & getContentMethod()
    {
        static $method = FALSE;
        if ( !$method )
        {
            if ( is_object($this->Url) )
                $method =& $this->Url;
            elseif ( is_object($this->UserFile) )
                $method =& $this->UserFile;
            elseif ( is_object($this->PersonalId) )
                $method =& $this->PersonalId;
            elseif ( is_object($this->TempFile) )
                $method =& $this->TempFile;
        }
        return $method;
    }


// +---------------------------------------------------------------------------+
// | Overridden Mojavi Methods (Completed)                                     | 
// +---------------------------------------------------------------------------+


    /**
     * @brief Determine if this action requires authentication.
     *
     * This must not be changed. A user can not have a locker without being a
     * user.
     */
    function isSecure ()
    {
        return FALSE;
    }

    /**
     * @brief Retrieve the default view.
     *
     * Use the INPUT view by default, which should contain an upload box, etc.
     */
    function getDefaultView (&$controller, &$request, &$user)
    {
        return VIEW_INPUT;
    }

    /**
     * @brief Retrieve the privilege required to access this action.
     *
     * No special privileges are required, just that you be logged in which is
     * handled by isSecure.
     */
    function getPrivilege (&$controller, &$request, &$user)
    {
        return NULL;
    }

    /**
     * @brief Retrieve the HTTP request method(s) this action will serve.
     *
     * Assuming a GET or POST is made, and validation passes, the execute method
     * will be called.
     */
    function getRequestMethods ()
    {
        return REQ_GET | REQ_POST | REQ_NONE;
    }

    /**
     * @brief Handle a validation error.
     *
     * If a validation error occurs, which view is rendered? We render
     * VIEW_INPUT and that view will display an appropriate error message.
     *
     * The validation which failed MUST setError('Mobilize') in the Request
     * object.
     */
    function handleError (&$controller, &$request, &$user)
    {
        return VIEW_INPUT;
    }

    /**
     * @brief Register individual parameter validators.
     */
    function registerValidators (&$validatorManager, &$controller, &$request,
                                 &$user)
    {
        // Possibly no useful validators?
    }
}

?>
