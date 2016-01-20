<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief Delete content from the locker
 *
 * The basic logic for this works as follows. A delete can not occur if:
 *  * The content is personal and not owned by you
 *  * The content is premium (paid)
 *
 * If a delete is successful, VIEW_SUCCESS is returned. If a delete can not
 * occur, VIEW_ERROR is returned with an appropriate message.
 *
 * @author  Blake Matheny
 */
class DeleteAction extends Action
{

    /**
     * NOTE: The following variables are available as request object parameters.
     */

    /**
     * @brief An ID that corresponds with personalMedia_id.
     *
     * This supports deleting a piece of personal media from your locker. You
     * may only delete content from your own locker. The corresponding request
     * parameter is 'PersonalId', an integer.
     *
     * NOTE: If COUNT(personalMedia_original) > 1 then the original file is not
     * deleted. If COUNT(personalMedia_location) > 1 then the original file is
     * not deleted. See the deletePersonalMedia method for more information.
     */
    var $PersonalId     =   0;

    /**
     * @brief An ID that corresponds with media_id.
     *
     * This supports deleting a piece of global media from your locker. You may
     * only delete content from your own locker. The corresponding request
     * parameter is 'MediaId', an integer.
     *
     * NOTE: Premium media can not be deleted. Media that has been download more
     * than 0 times will not be deleted, they will be disabled. No files are
     * removed during this operation.
     */
    var $MediaId        =   0;

    /**
     * NOTE: The follwoing variables are NOT available as request object
     * parameters.
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

        if ( $request->hasParameter('MediaId') )
        {
            $this->MediaId = $request->getParameter('MediaId');
        }

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
        if ( $this->delete($request) )
            return VIEW_SUCCESS;
        else
            return VIEW_ERROR;
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
            $request->setError('Delete', 'Invalid API Usage; request count was ' . $request_count);
            return FALSE;
        }

        if ( !is_numeric($this->UserId) || $this->UserId < 1 )
        {
            $request->setError('Delete', 'You must be logged in to use this');
            return FALSE;
        }

        return TRUE;
    }


// +---------------------------------------------------------------------------+
// | DeleteAction Methods                                                      |  
// +---------------------------------------------------------------------------+


    function delete(&$request)
    {
        if ( $this->MediaId > 0 )
            return $this->deleteGlobalMedia($request);
        else
            return $this->deletePersonalMedia($request);
    }

    /**
     * @brief Remove personal content
     *
     * This is much more complex then deleteGlobalMedia(). The algorithm for
     * deletion works as follows:
     *
     *  - Grab all info based on personal media id
     *      - False - Non-existant and return
     *      - Array - exists
     *  - Delete from personalMedia
     *  - If count(original) < 1
     *      Delete from personalFiles table
     *      Delete original from file system
     *  - If count(location) < 1
     *      Delete from file system
     *  - Delete from PersonalMediaDetails, PersonalMediaComments,
     *    FolksonomyToPersonalMedia and UsersToFolksonomy tables
     */
    function deletePersonalMedia(&$request)
    {
        // ticket 138602, if the media is my avatar, reset my
        //   avatar to be the default avatar
        $controller =& Controller::getInstance();
        $mediaModel =& $controller->getModel('Media', 'Media');
        $isMyAvatar = $mediaModel->isMyAvatar(
            'personalMedia_id',
            $this->PersonalId);
        if($isMyAvatar)
        {
            // set the user's avatar to be the default avatar
            $defaultAvatar = IMAGES_DIR . 'user-image-default-us.gif';
            $user =& $controller->getUser();
            $write_db =& $request->getAttribute('write_db');
            $userCore =& $user->getAttribute('UserCore');
            $userId = $userCore->Uid();
            $forum =& $userCore->Forum();
            $forum->Avatar($defaultAvatar);
            $userForum =& new UserForum(null, $write_db);
            $userForum->Uid($userId);
            $userForum->Avatar($defaultAvatar);
            if($userForum->Commit())
            {
                $userCore->Forum($forum);
            }
        }

        $pm = new PersonalMedia(
                null,
                Storage::GetLockerDb($request));
        $pm->PersonalMediaId($this->PersonalId);
        $pm->UserId($this->UserId);

        $entry = $pm->FindExacts();

        if ( !$entry || !is_array($entry) || empty($entry) || count($entry) != 1 )
        {
            $request->setError('Delete',
                    sprintf('Could not find %d in personal files', $this->PersonalId));
            return FALSE;
        }

        // entry is object of type PersonalMedia
        $entry = array_pop($entry);

        $entry->SetDb(Storage::GetLockerDb($request));

        if ( $entry->Remove() )
        {
            $original_count = 0;
            $location_count = 0;

            if ( $entry->Original() )
            {
                $original_count = $entry->countByOriginal($entry->Original());
            }

            if ( $original_count < 1 )
            {
                // delete from personalFiles table
                $pf = new PersonalFiles(null, Storage::GetLockerDb($request));
                $pf->Location($entry->Original());
                $pf->Remove();

                //delete all other files that begin with the md5hash prefix
                $original_file = MediaConverter::GetFullPath($entry->Original(), true);
                $pos = strrpos($original_file,".");
                $prefix_md5_hash = substr($original_file,0,$pos)."*";
                foreach (glob($prefix_md5_hash) as $x)
                {
                    //@unlink($x);
                }
            }

            if ( $entry->Location() )
            {
                $location_count = $entry->countByLocation($entry->Location());
            }

            if ( $location_count < 1 )
            {
                // delete from file system
                //@unlink(MediaConverter::GetFullPath($entry->Location(), true));

                // remove from the top ten list
                $topTen = new TopTen(null, $request->getAttribute('read_db'));
                $topTen->mType = 10010;
                $topTen->mMediaId = $this->PersonalId;
                $topTen->Remove();
            }

            // delete from PersonalMediaDetails, PersonalMediaComments,
            // FolksonomyToPersonalMedia and UsersToFolksonomy

            $pmd = new PersonalMediaDetails(null, Storage::GetLockerDb($request));
            $pmd->Id($this->PersonalId);
            $pmd->Remove();

            $pmc = new PersonalMediaComments(null, Storage::GetLockerDb($request));
            $pmc->MediaId($this->PersonalId);
            $pmc->Remove();

            $ftpmd = new FolksonomyToPersonalMedia(null,
                            null,
                            null,
                            Storage::GetLockerDb($request));
            $ftpmd->deleteByPersonalMediaId($this->PersonalId);

            $utf = new UsersToFolksonomy(null, null, null, Storage::GetLockerDb($request));
            $utf->deleteByPersonalMediaId($this->PersonalId);

            return TRUE;
        }
        else
        {
            $request->setError('Delete',
                    sprintf('Could not delete personalMedia_id %d', $this->PersonalId));
            return FALSE;
        }

        $request->setError('Delete', 'Unknown error in DeleteAction:PersonalId');
        return FALSE;
    }

    /**
     * @brief Remove content from the locker table
     *
     * If the entry has been downloaded, simple set the entry to inactive. If it
     * has not been downloaded, remove it from the lockers. If it has been
     * downloaded, set it to inactive.
     */
    function deleteGlobalMedia(&$request)
    {
        $le = new LockerEntry($this->UserId,
                null,
                $request->getAttribute('read_db'),
                Storage::GetLockerDb($request));

        $entry = $le->getEntry($this->UserId, $this->MediaId);

        if ( !$entry || !is_array($entry) || empty($entry) )
        {
            $request->setError('Delete',
                    sprintf('Could not find %d in locker', $this->MediaId));
            return FALSE;
        }

        if ( $entry['lockerEntry_is_active'] == 2 )
        {
            $request->setError('Delete', 'You can not delete premium content');
            return FALSE;
        }

        if ( $entry['lockerEntry_first_download_date'] == '0000-00-00 00:00:00' )
        {
            if ( !$le->realDelete($this->UserId, $this->MediaId) )
            {
                $request->setError('Delete',
                        sprintf('Unable to delete %d:%d', $this->UserId, $this->MediaId));
                return FALSE;
            }
            else
                return TRUE;
        }
        else
        {
            $le->MediaId($this->MediaId);
            $le->UserId($this->UserId);
            if ( !$le->Remove() )
            {
                $request->setError('Delete',
                        sprintf('Unable to remove %d:%d', $this->UserId, $this->MediaId));
                return FALSE;
            }
            else
                return TRUE;
        }

        $request->setError('Delete', 'Unknown error in DeleteAction:MediaId');
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
