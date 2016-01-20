<?php

define('SEND_NOTICE_CODE', 'asdf');

/**
 * @brief Hidden class that will send media to a phone number
 *
 * @note This action takes a parameter called 'Code' which will be
 *   hard coded into the code on both ends.
 */
class SendNoticeAction extends Action
{
//    var $Code       =   null;
    var $UserId     =   null;
    var $PersonalId =   null;
    var $MediaId    =   null;
    var $ToPhone    =   null;
    var $FromPhone  =   null;
    var $FromName   =   null;
    var $CarrierId  =   null;
    var $SourceInfo =   null;
    var $DeviceId   =   null;
    var $MediaType  =   null;
    var $SendNotice =   null;
    var $ConvertedLocation = null;
    var $Status		=	null;
    var $JobID		=	null;
    var $Reconvert  =   null;

    function initialize(&$controller, &$request, &$user)
    {
        $request->setParameter('NoCruft', 1);
		$this->JobID = $request->getParameter('JobID');
        $this->Reconvert = $request->getParameter('Reconvert');
        $this->Status = $request->getParameter('Status');
//		$this->Code = $request->getParameter('Code');
        $this->UserId = $request->getParameter('UserId');
        $this->PersonalId = $request->getParameter('PersonalId');
        $this->MediaId = $request->getParameter('MediaId');
        $this->ToPhone = $request->getParameter('ToPhone');
        $this->FromPhone = $request->getParameter('FromPhone');
        $this->FromName = $request->getParameter('FromName');
        $this->CarrierId = $request->getParameter('CarrierId');
        $this->DeviceId = $request->getParameter('DeviceId');
        $this->MediaType = $request->getParameter('MediaType');
        $this->SendNotice = $request->getParameter('SendNotice');
        $this->ConvertedLocation = $request->getParameter('ConvertedLocation');
        $this->OriginalLocation = $request->getParameter('OriginalLocation');

        $this->SourceInfo = array(
            'Domain'    =>  $request->getParameter('SourceDomain'),
            'Page'      =>  $request->getParameter('SourcePage'),
            'Query'     =>  $request->getParameter('SourceQuery'),
            'Affiliate' =>  $request->getParameter('SourceAffiliate'),
            'Source'    =>  $request->getParameter('SourceType')
            );
        return true;
    }


    function execute(&$controller, &$request, &$user)
    {
        if($this->Status == "Success")
        {
            if($this->Reconvert)
            {
                $finalFileLocation = MediaConverter::GetFullPath(
                    basename($this->ConvertedLocation), true);
                $finalOriginalLocation = MediaConverter::GetFullPath(
                    basename($this->OriginalLocation), true);
                if(!$this->moveFile($request, $this->ConvertedLocation, $finalFileLocation))
                {
                    $request->setError('UpdateLocker', 'Error moving file');
                    return VIEW_ERROR;
                }
                if(!$this->moveFile($request, $this->OriginalLocation, $finalOriginalLocation))
                {
                    $request->setError('UpdateLocker', 'Error moving file');
                    return VIEW_ERROR;
                }
            }
            // Update personalMedia or lockerEntries with converted file info
            elseif($this->PersonalId)
            {
                $locker_db =& Storage::GetLockerDb($request);

                $finalFileLocation = MediaConverter::GetFullPath(
                    basename($this->ConvertedLocation), true);
                $finalOriginalLocation = MediaConverter::GetFullPath(
                    basename($this->OriginalLocation), true);

                $filesize = 0;
                if(file_exists($this->ConvertedLocation)) {
                    $filesize = filesize($this->ConvertedLocation);
                } else {

                }

                // If the userid-converted location already exists, we will
                //  get a duplicate key error.  We still need to send notice
                //  so use the old media id
                $PersonalMedia =& new PersonalMedia(null, $locker_db);
                $PersonalMedia->UserId($this->UserId);
                $PersonalMedia->Location(basename($this->ConvertedLocation));
                $matches = $PersonalMedia->FindExacts();

                if(sizeof($matches) > 0)
                {
                    $match = array_shift($matches);
                    $this->PersonalId = $match->PersonalMediaId();
                    $PersonalMedia->PersonalMediaId($this->PersonalId);
                }
                else
                {
                    $PersonalMedia->PersonalMediaId($this->PersonalId);
                }
                if($request->hasParameter('mobilizeTitle'))
                {
                    $PersonalMedia->Name($request->getParameter('mobilizeTitle'));
                }
                $PersonalMedia->Type($this->MediaType);
                $PersonalMedia->Original(basename($this->OriginalLocation));
                $PersonalMedia->FileSize($filesize);
                if(!$PersonalMedia->Commit())
                {
                    $request->setError('UpdateLocker', 'Error updating personalMedia');
                    //return VIEW_ERROR;
                }

                if(!$this->moveFile($request, $this->ConvertedLocation, $finalFileLocation))
                {
                    $request->setError('UpdateLocker', 'Error moving file');
                    //return VIEW_ERROR;
                }

                if(!$this->moveFile($request, $this->OriginalLocation, $finalOriginalLocation))
                {
                    $request->setError('UpdateLocker', 'Error moving file');
                    //return VIEW_ERROR;
                }

            }
            elseif($this->MediaId)
            {
/*
                $write_db =& Storage::GetWriteDb($request);
                $LockerEntry =& new LockerEntry(null, $write_db);
                $LockerEntry->UserId($this->UserId);
                $LockerEntry->MediaId($this->MediaId);
                if(!$LockerEntry->Commit())
                {
                    $request->setError('UpdateLocker', 'Error updating lockerEntry');
                    //return VIEW_ERROR;
                }
*/
                $finalFileLocation = MediaConverter::GetFullPath(
                    basename($this->ConvertedLocation), false);
                $finalOriginalLocation = MediaConverter::GetFullPath(
                    basename($this->OriginalLocation), false);

                if(!$this->moveFile($request, $this->ConvertedLocation, $finalFileLocation))
                {
                    $request->setError('UpdateLocker', 'Error moving file');
                    //return VIEW_ERROR;
                }

                if(!$this->moveFile($request, $this->OriginalLocation, $finalOriginalLocation))
                {
                    $request->setError('UpdateLocker', 'Error moving file');
                    //return VIEW_ERROR;
                }

            }

            // Send a text message with content
            if($this->SendNotice)
            {
                $model =& $controller->getModel('OpenLocker', 'OpenLocker');
                if( $model->sendMedia(
                    $this->ToPhone,
                    $this->FromPhone,
                    $this->FromName,
                    $this->PersonalId,
                    $this->MediaId,
                    $this->CarrierId,
                    $this->SourceInfo,
                    $this->DeviceId))
                {
                    //return VIEW_SUCCESS;
                }
                else
                {
                    $request->setError('SendMedia', $model->getError('SendMedia'));
                    //return VIEW_ERROR;
                }
            }
            //return VIEW_SUCCESS;
        }
        else
        {
            // handle transcode error
        }
    }

    function moveFile(&$request, $currentPath, $newPath)
    {
        if( !file_exists($newPath) )
        {
            if( !file_exists($currentPath) )
            {
                // file to move does not exists
                $request->setError('MoveFile', 'The source file does not exist');
                return false;
            }
            else
            {
                if( !copy($currentPath, $newPath) )
                {
                    unlink($currentPath);
                    $request->setError('MoveFile', 'Error moving file');
                    return false;
                }
                return true;
            }
        }
        else
        {
            // file already exists at the new path
            return true;
        }
    }

    function getRequestMethods()
    {
        return REQ_POST | REQ_GET;
    }

    function handleError(&$controller, &$request, &$user)
    {
        return VIEW_ERROR;
    }

    function validate(&$controller, &$request, &$user)
    {
        // SendNoticeAction is publically accessible so we need to send a 
        //   private code to validate the request
        //if($this->Code != SEND_NOTICE_CODE)
        //{
        //    $request->setError('Code', 'Invalid Code');
        //    return false;
        //}

        return true;
    }
}

?>
