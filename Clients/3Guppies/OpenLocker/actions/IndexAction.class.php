<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief This is the display of OpenLocker, used for modifying and listing.
 *
 * This action serves up a listing of content that a user has acquired. Please
 * see 'Open Locker V. 1', August 4, 2006 document to see the design
 * specification and wireframes. Most of the functionality beyond listing
 * (rename, edit, send, etc) are handled by other actions. Additionally, this
 * action will most likely be referenced by another action for AJAX purposes.
 *
 * @author  Blake Matheny
 */

/**
 * Notes:
 *  Failure messages can be found in $request->getError('Index'). 
 */

class OpenLocker_IndexAction extends Action
{
    /*******************************************************/
    /** The following are available as request parameters **/
    /*******************************************************/

    /**
     * @brief Available sorting methods.
     *
     * The array below outlines the available methods for sorting content, these
     * should, in the request object, be specified as SortType.Desc OR
     * SortType.Asc to Specify what attribute to use for sorting and what
     * direction to sort in. The array below is used almost specifically for
     * validation and documentation.
     */
    var $SortTypes      =   array('Name','DateAdded','Type','Sharing','Size');

    /**
     * @brief Selected sorting method.
     *
     * The SortBy parameter is used to specify how to sort the list. By default,
     * or if nothing else is satisfied, DateAdded.Desc is used. The
     * corresponding Request parameter is 'SortBy', a string. See SortTypes for
     * more specific detail on this parameter.
     */
    var $SortBy         =   'DateAdded.Desc';

    /**
     * @brief Available content types.
     *
     * The array below specifies the types that may be specified as values for
     * the 'ContentType' request parameter. The array below is used specifically
     * for validation and documentation.
     */
    var $ContentTypes   =   array('All','Audio','Picture','Video','Other','Help');

    /**
     * @brief Selected content type.
     * 
     * The ContentType parameter is used to specify what content should be
     * displayed to the user. By default, or if no other conditions are
     * satisfied, All is used as the content type. The corresponding Request
     * parameter is 'ContentType', a string. See ContentTypes for more specific
     * detail on the parameter.
     */
    var $ContentType    =   'All';

    /***********************************************************/
    /** The following are NOT available as request parameters **/
    /***********************************************************/

    /**
     * @brief Items per page when paging is used.
     */
    var $ItemsPerPage   =   20;

    /**
     * @brief Page we are on when paging is used.
     */
    var $locker_next_page   =  1;

    /**
     * @brief The number of items in the users locker
     */
    var $LockerCount    =   0;

    /**
     * @brief Number of bytes used for personal files
     */
    var $SpaceUsed      =   0;


// +---------------------------------------------------------------------------+
// | Initialization, Execution and Validation                                  |  
// +---------------------------------------------------------------------------+


    /**
     * @brief Execute action initialization procedure.
     */
    function initialize (&$controller, &$request, &$user)
    {
        $this->SortBy       = $request->getParameter('SortBy', $this->SortBy);
        $this->ContentType  = $request->getParameter('ContentType', $this->ContentType);
        $this->locker_next_page = $request->getParameter('locker_next_page', 1);

        $entries['SortBy'] =& $this->SortBy;
        $entries['ContentType'] =& $this->ContentType;
        $entries['LockerCount'] =& $this->LockerCount;
        $entries['SpaceRemaining'] = $this->getRemainingLockerSpace();
        $entries['Entries'] = array();

        $request->setAttributeByRef('OpenLockerIndexView', $entries);

        $request->setAttribute('HEADER_Title',
                'Cell Phone Downloads, Phone Downloads, Motorola Mobile Phone Tools Download, mixxer Downloads, Mobile Phone Downloads');
        $request->setAttribute('HEADER_Keywords',
                'cell phone downloads,phone downloads,motorola mobile phone tools download,mixxer downloads,mobile phone downloads,cell phone download,cell phone ring downloads,download cell phone games');
        $request->setAttribute('HEADER_Description',
                'Cell Phone Downloads at mixxer.com.  Your source for phone downloads, motorola cell phone tools downloads, mixxer downloads and mobile phone downloads');

        return TRUE;
    }

    /**
     * @brief Execute all business logic.
     */
    function execute (&$controller, &$request, &$user)
    {
        // Get header information for this page
        $userCore =& $user->getAttribute('UserCore');
        $userId = $userCore->Uid();
        $actionChain =& new ActionChain();
        $actionChain->register('header', 'Media', 'Header');
        $request->setParameter('media_heade_userId', $userId);
		$request->setParameter('media_heade_active',
		    $request->getParameter('media_heade_active'));
        $actionChain->execute($controller, $request, $user);
        $headerContents = $actionChain->fetchResult('header');
        $request->setAttribute('media_heade_contents', $headerContents);

        $entries = array();
        if ( $this->ContentType == 'Help' )
        {
            $entries = array('Grid' => array(), 'HelpText' => $this->getHelpText($request,$user));
        }
        else
        {
            $entries = $this->fetchLockerEntries($controller, $request, $user);
        }

        if ( !array_key_exists('Grid', $entries) || (count($entries['Grid']) <= 0 && $this->ContentType != 'Help') )
        {
            $request->setError('Index', 'No content found');
        }

        $entries = $this->translateEntriesForView($entries);

        $request->setAttribute('OpenLockerIndexView', $entries);

        $request->setAttribute('ContentType', $this->ContentType);

        return VIEW_INDEX;
    }

    /**
     * @brief Validate the request as a whole.
     *
     * Ensure valid sort and content types. Ensure that a user has locker
     * entries.
     */
    function validate (&$controller, &$request, &$user)
    {
        $sb = $this->SortBy;
        $sb = explode('.', $sb);
        if ( !is_array($sb) || count($sb) != 2 || !in_array($sb[0], $this->SortTypes) )
        {
            $request->setError('Index', "Invalid sort type requested'".$this->SortBy."'");
            $this->SortBy = 'DateAdded.Desc';
        }

        if ( !in_array($this->ContentType, $this->ContentTypes) )
        {
            $request->setError('Index', "Invalid content type requested'".$this->ContentType."'");
            $this->ContentType = 'All';
        }

        $user_id = $user->getAttribute('UserCore');
        if ( is_object($user_id) )
            $user_id = $user_id->Uid();
        else
        {
            $request->setError('Index', 'UserCore was uninitialized');
            return FALSE;
        }

        $le = new LockerEntry($user_id,
                                null,
                                $request->getAttribute('read_db'),
                                Storage::getLockerDb($request));

        $count = $le->getLockerUsage();
        if ( $count['Count'] > 0 )
        {
            $this->LockerCount = $count['Count'];
            $this->SpaceUsed = $count['Size'];
        }

        return TRUE;
    }


// +---------------------------------------------------------------------------+
// | IndexAction Methods                                                       |  
// +---------------------------------------------------------------------------+


    function getHelpText(&$request,&$user)
    {
        $DeviceInfo = $user->getAttribute('DeviceInfo');

        if ( !is_object($DeviceInfo) )
        {
            return NULL;
        }

        $help_id   = $DeviceInfo->DownloadHelpId();
        $help_text = NULL;
        if ( !is_null($help_id) && $help_id > 0 )
        {
            $Help      = new Help(NULL, $request->getAttribute('read_db'));
            $help_info = $Help->PopulateById($help_id);
            if ( $help_info )
            {
                $help_text = $Help->Text();
                $help_text = trim($help_text);
            }
        }

        if ( !empty($help_text) )
            return $help_text;
        else
        {
            // TODO: This isn't the right approach, now we're doing presentation
            // in the action. Ew.
            if ( $DeviceInfo->HasCapabilityName('noWapDownload') )
            {
                return '
								    <BR><BR><b>INSTRUCTIONS FOR DOWNLOADING</b><BR><BR>
								    <b>You must have a <a href="http://www.vzwpix.com/" target="_blank">vzwpix.com</a> account
								       before you can receive messages. It is free to sign up for vzwpix.com.</b><br/><br />
								    <b>A text message has just been sent to your phone:</b>
								    <ol>
									<li>View it</li>
									<li>There will be an attachment</li>
									<li>Save the attachment to your phone</li>
								    </ol>
								    <br><br><br>
                    ';
            }
            else
            {
                return '
										<b>Go to mixxer.com with your phone</b></br>
										<ol>
										<li>Login to your locker
									    <li>Once your locker loads up, select the category of your choice ( ringers, screensavers, etc. )<br>
									    <li>Select an item you would like to download and click on it.<br>
									    <li>Select YES when the unsafe content box pops up<br>
									    </ol>
                    ';
            }
            return NULL;
        }
    }

    // How much space is left for usage by this user?
    function getRemainingLockerSpace()
    {
        $space_allowed = 104857600;
        return Storage::ConvertBytes($space_allowed - abs($this->SpaceUsed));
    }

    /**
     * @brief Create a data structure that can be used by the view
     */
    function translateEntriesForView($entries)
    {
        $controller =& Controller::getInstance();
        $mediaModel =& $controller->getModel('Media', 'Media');
        $mediaInfo =& new MediaInfo();

        $Structure['Entries'] = array();
        if (array_key_exists('Header', $entries) && !empty($entries['Header']))
        {
            $Structure['Pager'] = $entries['Header'];
        }
        if (array_key_exists('HelpText', $entries) && !empty($entries['HelpText']))
        {
            $Structure['HelpText'] = $entries['HelpText'];
        }
        foreach ( $entries['Grid'] AS $anon_key => $array_value )
        {
            $tmp = array();

            $tmp['Title'] = $array_value['Title'];

            $tmp['Type'] = null;
            if ( $array_value['Type'] == 'Personal' )
                $tmp['Type'] = 'Unlicensed';
            elseif ( $array_value['Type'] == 'Premium' )
                $tmp['Type'] = 'Premium';
            else
                $tmp['Type'] = 'Licensed';

            $tmp['DateAdded'] = date('m/d/y', $array_value['DateAdded']);

            switch ( $array_value['ContentType'] )
            {
                case 'Video':
                    $tmp['ContentType'] = 'video';
                    break;
                case 'Screensavers':
                    $tmp['ContentType'] = 'picture';
                    break;
                case 'Ringtones':
                    $tmp['ContentType'] = 'audio';
                    break;
                default:
                    $tmp['ContentType'] = 'other';
                    break;
            }
            switch ( $array_value['AccessLevel'] )
            {
                case 0:
                    $tmp['Sharing'] = 'Private';
                    break;
                case 1:
                    $tmp['Sharing'] = 'Public';
                    break;
                default:
                    $tmp['Sharing'] = 'NA';
                    break;
            }
            $tmp['Preview'] = $array_value['Preview'];
            $tmp['Location'] = $array_value['Location'];
            $tmp['Size'] = str_replace(' ','',$array_value['FileSize']);
            $tmp['PreviewUrl'] = $array_value['PreviewUrl'];

            $tmp['DownloadUrl'] = $array_value['DownloadUrl'];
            $tmp['Id'] = $array_value['Id'];

            $mediaInfo->setLocation($tmp['Location']);
            $mediaInfo->setMediaId($tmp['Id']);
            $tmp['IsMyAvatar'] = $mediaModel->isMyAvatar('personalMediaId',
                $tmp['Id'], null, $mediaInfo);
            $tmp['isCropped'] = $mediaInfo->getIsCropped();
            $Structure['Entries'][$anon_key] = $tmp;
        }
        $Structure['LockerCount'] = $this->LockerCount;
        $Structure['SpaceRemaining'] = $this->getRemainingLockerSpace();
        $Structure['SortBy'] = $this->SortBy;
        $Structure['ContentType'] = $this->ContentType;

        return $Structure;
    }

    /**
     * @brief Retrieve locker entries from the database.
     *
     * Return an empty array if there is an error, and $request->setError
     * appropriately.
     */
    function fetchLockerEntries(&$controller, &$request, &$user)
    {
        $user_id = $user->getAttribute('UserCore');
        $di = $user->getAttribute('DeviceInfo');
        $file_format = null;
        $redirect = false;
        $pagingInfo = array('BaseUrl' => NULL, 'locker_next_page' => NULL, 'ItemsPerPage' => NULL);

        if ( is_object($user_id) )
            $user_id = $user_id->Uid();
        else
        {
            $request->setError('Index', 'UserCore was uninitialized');
            return FALSE;
        }

        if ( !is_object($di) )
        {
            $request->setError('Index', 'DeviceInfo was uninitialized' );
            return FALSE;
        }

        $le = new LockerEntry($user_id,
                                null,
                                $request->getAttribute('read_db'),
                                Storage::getLockerDb($request));

        if ( $this->usePaging($controller) )
        {
            $module = $request->getParameter('module') ? $request->getParameter('module') : 'OpenLocker';
            $action = $request->getParameter('action') ? $request->getParameter('action') : 'IndexContainer';

            $pagingInfo['BaseUrl'] = $controller->genUrl(array(
                            'module'        =>  $module,
                            'action'        =>  $action,
                            'SortBy'        =>  $this->SortBy,
                            'ContentType'   =>  $this->ContentType,
                        ));
            $pagingInfo['locker_next_page'] = $this->locker_next_page;
            $pagingInfo['ItemsPerPage'] = $this->ItemsPerPage;
        }

        if ( in_array($controller->getContentType(), array('wml','xhtmlMobile')) )
        {
            if ( $di->HasCapabilityName('gcd') )
            {
                $file_format = '.gcd';
            }
            elseif ( $di->HasCapabilityName('mcd') )
            {
                $file_format = '.mcd';
            }
            elseif ( $di->HasCapabilityName('OMA-FL') )
            {
                $file_format = '.dm';
            }
            $redirect = $di->HasCapabilityName('redirect');
        }

        $entries = $le->GetVisibleEntries(
                    $controller,
                    $request,
                    $user,
                    $user_id,
                    'Locker',
                    $this->_translateTypeToLimit(),
                    $pagingInfo['BaseUrl'],
                    $pagingInfo['locker_next_page'],
                    $pagingInfo['ItemsPerPage'],
                    $redirect,
                    $file_format,
                    false,
                    $this->SortBy
                );

        return $entries;
    }

    /**
     * @brief Translate our ContentTypes to a 'Limit' that can be used by GVE
     */
    function _translateTypeToLimit()
    {
        switch ( $this->ContentType )
        {
            case 'Audio':
                return 'Ringtones';
            case 'Picture':
                return 'Screensavers';
            case 'Video':
                return 'Video';
            case 'Other':
                return 'Executables';
            default:
                return 'All';
        }
    }

    /**
     * @brief Logic that determines whether to use paging
     */
    function usePaging(&$controller)
    {
        $paging = false;
        switch ( $controller->getContentType() )
        {
            case 'xhtmlMobile':
                $this->ItemsPerPage = 10;
                $paging = true;
                break;
            case 'wml':
                $this->ItemsPerPage = 5;
                $paging = true;
                break;
            default:
                $this->ItemsPerPage = 20;
				$paging = true;
                break;
        }
        return $paging;
    }

// +---------------------------------------------------------------------------+
// | Overridden Mojavi Methods (Completed)                                     | 
// +---------------------------------------------------------------------------+

    /**
     * @brief Retrieve the default view.
     */
    function getDefaultView (&$controller, &$request, &$user)
    {
        return VIEW_INDEX;
    }

    /**
     * @brief Retrieve the privilege required to access this action.
     */
    function getPrivilege (&$controller, &$request, &$user)
    {
        return NULL;
    }

    /**
     * @brief Retrieve the HTTP request method(s) this action will serve.
     */
    function getRequestMethods ()
    {
        return REQ_GET | REQ_NONE | REQ_POST;
    }

    /**
     * @brief Handle a validation error.
     */
    function handleError (&$controller, &$request, &$user)
    {
        return VIEW_INDEX;
    }

    /**
     * @brief Determine if this action requires authentication.
     */
    function isSecure ()
    {
        return TRUE;
    }

    /**
     * @brief Register individual parameter validators.
     */
    function registerValidators (&$validatorManager, &$controller, &$request,
                                 &$user)
    {
        // no validators
    }

}

?>
