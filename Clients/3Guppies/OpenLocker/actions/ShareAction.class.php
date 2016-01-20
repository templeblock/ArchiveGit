<?php

require_once(dirname(__FILE__) . '/../lib/DownloadHandler.class.php');
require_once(dirname(__FILE__) . '/../../Misc/lib/RecipientHandler.class.php');

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief Share content with friends, family and the RIAA
 *
 * This action inherits all of the request parameters as the Misc/Contact
 * action. This action just sets up a message callback for the ContactAction
 * and forwards there. If an error occurs ($request->hasError('ContactView'))
 * VIEW_ERROR is returned, otherwise VIEW_SUCCESS is returned.
 *
 * At some point, VIEW_INPUT will take a list of phone numbers and emails, and
 * along with a PersonalId forward to Misc/Contact, but not yet.
 *
 * @author  Blake Matheny
 */

/**
 * NOTE: Do NOT set isSecure() to false without talking to Blake. Note to Blake,
 * pieces of this code rely on the ability to grab a user id.
 */

class ShareAction extends Action
{
    /**
     * @brief An ID that corresponds with personalMedia_id.
     *
     * The personalMedia_id that is to be shared with other people.
     */
    var $PersonalId     =   null;

    /**
     * @brief An ID that corresponds with media_id.
     *
     * This can only be used if Recipients is empty, since we do not allow
     * sharing with people other than yourself.
     */
    var $MediaId        =   null;

    /**
     * @brief Phone Number corresponding to the sender's phone number
     */
    var $FromNumber     =   null;

    /**
     * @brief Name corresponding to the sender's name
     */
    var $FromName       =   null;

    /**
     * @brief Array of info telling where mobilized content came from
     */
    var $SourceInfo     =   null;

    /**
     * @brief Array of phone numbers/email addresses to send content to
     */
    var $Recipients     =   null;

    /**
     * @brief Boolean whether we are sharing to ourselves
     */
    var $RecipientIsSelf =  false;

    /**
     * @brief DeviceId to override any conversions
     */
    var $RecipientDeviceId  =   null;

    /**
     * @brief Logged in user's user_id
     */
    var $UserId         =   null;

    /**
     * @brief Should we return a view? If true, then no view, if false, return
     *    a view
     */
    var $SimpleShare    =   false;
// +---------------------------------------------------------------------------+
// | Initialization, Execution and Validation                                  |  
// +---------------------------------------------------------------------------+


    /**
     * @brief Execute action initialization procedure.
     */
    function initialize (&$controller, &$request, &$user)
    {
        $this->SourceInfo = array();

        if($request->hasParameter('page'))
        {
            $urlInfo = @parse_url($request->getParameter('page'));
            if($urlInfo)
            {
                $scheme = array_key_exists('scheme', $urlInfo) ? $urlInfo['scheme'] : "http";
                $host = array_key_exists('host', $urlInfo) ? $urlInfo['host'] : "mixxer.com";

                $this->SourceInfo['Domain'] = $scheme . "://" . $host;
                $this->SourceInfo['Page'] = array_key_exists('path', $urlInfo) ? $urlInfo['path'] : "";
                $this->SourceInfo['Query'] = array_key_exists('query', $urlInfo) ? $urlInfo['query'] : "";
            }
        }

        if($request->hasParameter('PersonalId'))
        {
            $this->PersonalId = $request->getParameter('PersonalId');
            $this->SourceInfo['Source'] = SOURCE_TYPE_PERSONAL_MEDIA;
        }
        elseif($request->hasParameter('MediaId'))
        {
            $this->MediaId = $request->getParameter('MediaId');
            $this->SourceInfo['Source'] = SOURCE_TYPE_PREMIUM_MEDIA;
        }
        else
        {
            $request->setError('Share', 'No media specified');
        }

        if($request->hasAttribute('SimpleShare'))
        {
            $this->SimpleShare = true;
        }

        $UserCore =& $user->getAttribute('UserCore');
        if(is_object($UserCore))
        {
            $this->FromName = $request->getParameter('Subject');
            $this->UserId   = $UserCore->Uid();
            $UserComm =& $UserCore->Comm();
            if(is_object($UserComm))
            {
                $this->FromNumber = $UserComm->PhoneNumber();
            }
            else
            {
                $this->FromNumber = "1800649937";
            }
        }
        else
        {
            $this->FromName = "sharing";
            $this->FromNumber = "1800649937";
        }

        if($request->hasParameter('DeviceId'))
        {
            $this->RecipientDeviceId = $request->getParameter('DeviceId');
        }


        $recipients = array();
        if($request->hasParameter('Recipients'))
        {
            $recipients = explode(',', $request->getParameter('Recipients'));
        }
        else
        {
            $this->RecipientIsSelf = true;
            if(is_object($UserCore))
            {
                $UserComm =& $UserCore->Comm();
                if(is_object($UserComm))
                {
                    $recipients = array($UserComm->PhoneNumber());
                }
            }
        }
        $trimmedRecipients = array();
        foreach($recipients as $recipient)
        {
            $trimmedRecipient = trim($recipient);
            if(!empty($trimmedRecipient))
            {
                $trimmedRecipients[] = $trimmedRecipient;
            }
        }

        $rh = new Contact_RecipientHandler($trimmedRecipients);
        $this->Recipients = $rh->getRecipients();

        return TRUE;
    }

    /**
     * @brief Execute all business logic.
     */
    function execute (&$controller, &$request, &$user)
    {
        $model =& $controller->getModel('OpenLocker', 'OpenLocker');

        foreach($this->Recipients as $recipient)
        {
            if( $recipient->isEmail() )
            {
                $email = $recipient->getAddress();
                $from_email = $this->FromNumber . "@mixxer.com";
                if($model->sendMediaEmail(
                    $email,
                    $from_email,
                    $this->FromName,
                    $this->PersonalId,
                    $this->MediaId
                    ))
                {
                }
                else
                {
                    $request->setError('Share', $model->getError('SendMediaEmail'));
                }
            }
            elseif( $recipient->isPhoneNumber() )
            {
                $toPhone = $recipient->getAddress();
                if($model->sendMedia(
                    $toPhone,
                    $this->FromNumber,
                    $this->FromName,
                    $this->PersonalId,
                    $this->MediaId,
                    null,
                    $this->SourceInfo
                    ))
                {

                }
                else
                {
                    $request->setError('Share', $model->getError('SendMedia'));
                }
            }
            else
            {
                $request->setError('Share', 'Unknown Recipient Type');
            }
        }

        if($this->SimpleShare)
        {
            return VIEW_NONE;
        }

        if($request->hasError('Share'))
        {
            return VIEW_ERROR;
        }

        return VIEW_SUCCESS;
    }

    /**
     * @brief Validate the request as a whole.
     */
    function validate (&$controller, &$request, &$user)
    {
        $request_count = 0;

        if ( is_numeric($this->MediaId) && $this->MediaId > 0 )
            $request_count += 1;
        if ( is_numeric($this->PersonalId) && $this->PersonalId > 0 )
            $request_count += 1;

        if ( $request_count != 1 )
        {
            $request->setError('Share', 'Invalid API Usage; request count was ' . $request_count);
            return FALSE;
        }

        // Can not share global media with others
        if ( $this->MediaId && !$this->RecipientIsSelf )
        {
            $request->setError('Share', 'Can not share locked content with other users');
            return FALSE;
        }

        // Recipient is self or isn't media share
        if ( $this->PersonalId )
        {
            $pm = new PersonalMedia(
                    null,
                    Storage::GetLockerDb($request));
            $pm->PersonalMediaId($this->PersonalId);

            $entry = $pm->FindExacts();

            if ( !$entry || !is_array($entry) || empty($entry) || count($entry) != 1 )
            {
                $request->setError('Share',
                        sprintf('Could not find %d in personal files', $this->PersonalId));
                return FALSE;
            }
            $entry = array_shift($entry);

            // If no UserId, content must be unlocked
            // If UserId, content must be yours
            if ( $entry->UserId() != $this->UserId )
            {

                if ( $entry->AccessLevel() == 0 ) // FIXME: When 'Friends' is available
                {
                    $request->setError('Share',
                            sprintf('Tried to share %d but it is locked', $this->PersonalId));
                    return FALSE;
                }
            }
        }
        elseif ( $this->UserId && $this->MediaId )
        {
            // If MediaId, it must be in your locker and active
            $le = new LockerEntry($this->UserId,
                    null,
                    $request->getAttribute('read_db'),
                    Storage::GetLockerDb($request));

            $entry = $le->getEntry($this->UserId, $this->MediaId);

            if ( !$entry || !is_array($entry) || empty($entry) || $entry['lockerEntry_is_active'] == 0 )
            {
                $request->setError('Share',
                        sprintf('Could not find %d in locker', $this->MediaId));
                return FALSE;
            }

            //----------------------------------------------------------------+
            // Now make sure the user has permission to download this content |
            //----------------------------------------------------------------+
            $lockerSendDirectModel =& $controller->getModel('Downloads', 'LockerSendDirect');
            if(!$lockerSendDirectModel->validateRequest('MediaId')) {
                $request->setError('Share','Invalid request or media is not compatible with your phone');
                return FALSE;
            }
            $handsetModel =& $request->getAttribute('downl_locke_handsetModel');
            $mediaToHandsetInfo =& $handsetModel->findByMediaId($this->MediaId);
            if(!is_object($mediaToHandsetInfo)) {
                $request->setError('Share',
                    'Unknown error occured. Please contact Customer Service');
                return FALSE;
            }
            $mediaToHandsetInfo->setIsInLocker(true);
            if(!$lockerSendDirectModel->hasDownloadPermission($handsetModel)) {
                $request->setError('Share',
                    'Permission denied to download media');
                return FALSE;
            }
        }
        else
        {
            $request->setError('Share', 'Unknown general failure');
            return FALSE;
        }

        return TRUE;
    }


// +---------------------------------------------------------------------------+
// | ShareAction Methods                                                       |  
// +---------------------------------------------------------------------------+

    function & getRecipients(&$request)
    {
        $recipients = explode(",",$request->getParameter('Recipients'));
        $recipients = array_map(create_function('$a','return trim($a);'), $recipients);
        $recipients = array_unique($recipients);
        return $recipients;
    }

    function & getDownloadHandler()
    {
        // At some point we will want to add a resolveUser($info) call, which
        // returns known or unknown
        $dlh = null;
        if ( $this->RecipientIsSelf)// || $this->RegisteredPhoneNumber )
        {
            $dlh = DownloadHandlerFactory::get('KnownUser');
            $dlh->setParameter('UserId', $this->UserId);
        }
        else
        {
            $dlh = DownloadHandlerFactory::get('UnknownUser');
        }

        if ( $this->MediaId )
            $dlh->setParameter('MediaId', $this->MediaId);
        elseif ( $this->PersonalId )
            $dlh->setParameter('PersonalId', $this->PersonalId);
        else
            return NULL;

        return $dlh;
    }


// +---------------------------------------------------------------------------+
// | Overridden Mojavi Methods (Completed)                                     | 
// +---------------------------------------------------------------------------+


    /**
     * The code below specifies an action that requires a user to be logged in
     * to use, but no special privileges. The action returns VIEW_INPUT by
     * default, VIEW_ERROR if a validation error occurs, and VIEW_SUCCESS if
     * the execute() method is successfully processed.
     */

    /**
     * @brief Retrieve the default view.
     */
    function getDefaultView (&$controller, &$request, &$user)
    {
        return VIEW_INPUT;
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
        return REQ_GET | REQ_POST;
    }

    /**
     * @brief Handle a validation error.
     */
    function handleError (&$controller, &$request, &$user)
    {
        return VIEW_ERROR;
    }

    /**
     * @brief Determine if this action requires authentication.
     */
    function isSecure ()
    {
        return FALSE;
    }

    /**
     * @brief Register individual parameter validators.
     */
    function registerValidators (&$validatorManager, &$controller, &$request, &$user)
    {
        // None
    }

}

?>
