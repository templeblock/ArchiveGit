<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief Set the permissions on a piece of personal content
 *
 * This action is very simple. Specify a PersonalId, and a Sharing parameter,
 * and set the permission of the content. Only approved types can be shared,
 * which are currently just pictures.
 *
 * If the permission setting is successful, VIEW_SUCCESS is returned. If the
 * permission can not be set for whatever reason, VIEW_ERROR is returned with an
 * appropriate error message.
 *
 * @author  Blake Matheny
 */
class PermissionAction extends Action
{

    /**
     * NOTE: The following variables are available as request object parameters.
     */

    /**
     * @brief An ID that corresponds with personalMedia_id.
     *
     * The personalMedia_id of the content to set permissions on. The
     * corresponding request parameter is 'PersonalId', an integer.
     *
     * NOTE: If COUNT(personalMedia_original) > 1 then the original file is not
     * deleted. If COUNT(personalMedia_location) > 1 then the original file is
     * not deleted. See the deletePersonalMedia
     */
    var $PersonalId     =   0;

    /**
     * @brief The type of sharing to set for a piece of personal media.
     *
     * The type of sharing permission to set on the personal media id. Valid
     * share types are documented in the Shares member variable.
     * The default is 'Public'. The request parameter is 'Sharing', and is a
     * string.
     */
    var $Sharing        =   'Public';

    /**
     * NOTE: The follwoing are NOT available as request object parameters.
     */

    /**
     * @brief Valid share types.
     *
     * Below are a list of valid share types
     *  'Public'    - Other users may be able to view content
     *  'Private'   - Other users can not see content
     * I expect 'Protected' to become available with friends. The index in this
     * case is actually significant, since it relates to the SQL ID for the
     * privilege.
     */
    var $Shares         =   array('Private', 'Public');

    /**
     * @brief Valid content types to share.
     *
     * Below is an array containing a list of valid content types to be shared.
     * Add to this list to enabled sharing. See MediaConverter::GetTypeById
     */
    var $ValidTypes     =   array('Screensavers');

    /**
     * @brief ID of user using this action.
     */
    var $UserId         =   0;


// +---------------------------------------------------------------------------+
// | Initialization, Execution and Validation                                  |  
// +---------------------------------------------------------------------------+


    /**
     * @brief Execute action initialization procedure.
     */
    function initialize (&$controller, &$request, &$user)
    {
        if ( $request->hasParameter('PersonalId') )
        {
            $this->PersonalId = $request->getParameter('PersonalId');
        }

        $this->Sharing = $request->getParameter('Sharing', $this->Sharing);

        if ( $user->isAuthenticated() )
        {
            if ( $user->hasAttribute('UserCore') )
            {
                $uc =& $user->getAttribute('UserCore');
                if ( is_object($uc) )
                {
                    $this->UserId = $uc->Uid();
                }
            }
        }

        return TRUE;
    }

    /**
     * @brief Execute all business logic.
     */
    function execute (&$controller, &$request, &$user)
    {
        if ( $this->setShare($request) )
            return VIEW_SUCCESS;
        else
            return VIEW_ERROR;
    }

    /**
     * @brief Validate the request as a whole.
     */
    function validate (&$controller, &$request, &$user)
    {
        if ( !is_numeric($this->PersonalId) || $this->PersonalId < 1 )
        {
            $request->setError('Permission',
                    sprintf('Invalid PersonalId (%s) Specified', $this->PersonalId));
            return FALSE;
        }

        if ( !is_string($this->Sharing) || !in_array($this->Sharing, $this->Shares) )
        {
            $request->setError('Permission',
                    sprintf('Invalid share type (%s) specified', $this->Sharing));
            return FALSE;
        }

        if ( !is_numeric($this->UserId) || $this->UserId < 1 )
        {
            $request->setError('Permission', 'You must be logged in to use this');
            return FALSE;
        }

        return TRUE;
    }


// +---------------------------------------------------------------------------+
// | DeleteAction Methods                                                      |  
// +---------------------------------------------------------------------------+


    function setShare(&$request)
    {
        $pm = new PersonalMedia(
                null,
                Storage::GetLockerDb($request));
        $pm->PersonalMediaId($this->PersonalId);
        $pm->UserId($this->UserId);

        $entry = $pm->FindExacts();

        if ( !$entry || !is_array($entry) || empty($entry) || count($entry) != 1 )
        {
            $request->setError('Permission',
                    sprintf('Could not find %d in personal files', $this->PersonalId));
            return FALSE;
        }

        // entry is object of type PersonalMedia
        $entry = array_pop($entry);

        $type = MediaConverter::GetTypeById($entry->Type());
        if ( in_array($type, $this->ValidTypes) )
        {
            $pm = new PersonalMedia(
                    null,
                    Storage::GetLockerDb($request));
            $pm->PersonalMediaId($this->PersonalId);
            $pm->AccessLevel(array_search($this->Sharing, $this->Shares));
            if ( $pm->Commit() )
                return TRUE;
            else
            {
                $request->setError('Permission',
                            sprintf('Unable to set permission (%s) on file (%d)',
                            array_search($this->Sharing, $this->Shares),
                            $this->PersonalId));
                return FALSE;
            }
        }
        else
        {
            $request->setError('Permission',
                    sprintf('Can not modify permissions for %s', $type));
            return FALSE;
        }

        $request->setError('Permission', 'Unknown Share Error');
        return FALSE;
    }


// +---------------------------------------------------------------------------+
// | Overridden Mojavi Methods (Completed)                                     | 
// +---------------------------------------------------------------------------+


    /**
     * @brief Retrieve the default view.
     */
    function getDefaultView (&$controller, &$request, &$user)
    {
        return VIEW_ERROR;
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
        return REQ_GET | REQ_POST | REQ_NONE;
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
        return TRUE;
    }

    /**
     * @brief Register individual parameter validators.
     */
    function registerValidators (&$validatorManager, &$controller, &$request,
                                 &$user)
    {
        // NONE
    }

}

?>
