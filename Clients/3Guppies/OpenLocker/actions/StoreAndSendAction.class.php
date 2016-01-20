<?php
/**
 * @brief Class to store personalMedia and send it to specified people
 *
 * @author Jeff Ching
 *
 * @date 2007-02-13
 */
class StoreAndSendAction extends Action
{
    /**
     * @brief This Message object contains all the mobilize parameters and is
     *   aggregated over the course of the mobilize chain of command
     */
    var $MobilizeParams;

    /**
     * @brief Initialize our variables
     */
    function initialize(&$controller, &$request, &$user)
    {
        $this->MobilizeParams =& $request->getAttribute('MobilizeParams');
        return TRUE;
    }

    /**
     * @brief Business logic for this action:
     *
     * 1. Convert the content given the EditOptions
     * 2. Store the content
     * 3. Send notification to the recipients
     * 4. Update initial settings for the media
     * 5. Forward you to the correct place or show the correct template
     */
    function execute(&$controller, &$request, &$user)
    {
        $toPhone = $request->getParameter( 'toPhone' );
        $loggedIn_carrierId = -1;
        if( $user->isAuthenticated() ) 
        {
            $deviceInfo =& $user->getAttribute('DeviceInfo');
            if( is_object( $deviceInfo ) ) 
            {
                $loggedIn_carrierId = $deviceInfo->MobileCarrierId();
            }
        }
        $carrierId = ThirdPartyCarrierMappings::getCarrierId( $toPhone  );
        if($loggedIn_carrierId != -1 && $carrierId != $loggedIn_carrierId) 
        {
            $carrierId = $loggedIn_carrierId;
        }
        $carriers = new Carriers( null, $request->getAttribute( 'read_db' ) );
        if( in_array( $carrierId, $carriers->GetMMSCarrierIds() ) )
             $request->setAttribute( 'sendViaSMS', false );
        else
            $request->setAttribute( 'sendViaSMS', true );

        $PN = new PhoneNumber(null, $request->getAttribute('read_db'));
        $PN->Number($this->MobilizeParams->getParameter('ToPhoneNumber'));
        if(!$PN->canContact())
        {
            $request->setError('Mobilize', $PN->Number() . " is on the Do Not Contact List.");
            return array('OpenLocker', 'Mobilize', VIEW_INPUT);
        }

        $carrier_id = $this->MobilizeParams->getParameter('CarrierId');
        if( is_null($carrier_id) || !is_numeric($carrier_id) || $carrier_id < 0)
        {
            $request->setError('Mobilize', "We are unable to send content to your carrier at this time. We have recorded your request and will work to support this carrier as soon as we can.");
            return array('OpenLocker', 'Mobilize', VIEW_INPUT); 
        }


        $EditOptions = $this->MobilizeParams->getParameter('EditOptions');
        $ContentHandler =& $this->MobilizeParams->getParameter('ContentHandler');
        $SendNotice = $this->MobilizeParams->getParameter('SendNotice');
        $Recipients = $this->MobilizeParams->getParameter('Recipients');
        $MyPhone = $this->MobilizeParams->getParameter('MyPhone');

        $deviceId = $this->getDeviceId($user, $EditOptions);
        $sourceUrl = $this->MobilizeParams->getParameter('SourceUrl');
        $urlInfo = @parse_url($sourceUrl);
        $domain = array_key_exists('host', $urlInfo) ? $urlInfo['host'] : "";
        $path = array_key_exists('path', $urlInfo) ? $urlInfo['path'] : "";
        $query = array_key_exists('query', $urlInfo) ? $urlInfo['query'] : "";
        $sourceType = $this->MobilizeParams->getParameter('InputMethod');
        $affiliate = $this->MobilizeParams->getParameter('Affiliate');

        $PostVars = array(
            'SendNotice'            =>  $this->MobilizeParams->getParameter('SendNotice'),
            'ToPhone'               =>  $this->MobilizeParams->getParameter('ToPhoneNumber'),
            'FromPhone'             =>  $this->MobilizeParams->getParameter('FromPhoneNumber'),
            'FromName'              =>  $this->MobilizeParams->getParameter('FromName'),
            'CarrierId'             =>  $this->MobilizeParams->getParameter('CarrierId'),
            'SourceDomain'          =>  $domain,
            'SourcePage'            =>  $path,
            'SourceQuery'           =>  $query,
            'SourceAffiliate'       =>  $affiliate,
            'SourceType'            =>  $sourceType,
            'DeviceId'              =>  $deviceId
            );

        // Store the content and make the conversion request
        $personalMediaId = $ContentHandler->storeAndConvert($EditOptions, $PostVars);
        $fileInfo = $ContentHandler->fileInfo;
        if( is_array($fileInfo) &&
            array_key_exists('Preexisting', $fileInfo) &&
            $fileInfo['Preexisting'] )
        {
            // Need to send the content
            $actionChain =& new ActionChain();
            $actionChain->register('notify', 'OpenLocker', 'SendNotice');

            $request->setParameter('Status','Success');
            $request->setParameter('OriginalLocation',$fileInfo['Original']);
            $request->setParameter('ConvertedLocation',$fileInfo['Converted']);
            $request->setParameter('CarrierId',$PostVars['CarrierId']);
            $request->setParameter('DeviceId',$PostVars['DeviceId']);
            $request->setParameter('FromName',$PostVars['FromName']);
            $request->setParameter('FromPhone',$PostVars['FromPhone']);
            $request->setParameter('MediaType',$fileInfo['CapabilityId']);
            $request->setParameter('PersonalId',$personalMediaId);
            $request->setParameter('SendNotice',$PostVars['SendNotice']);
            $request->setParameter('SourceAffiliate',$affiliate);
            $request->setParameter('SourceDomain',$domain);
            $request->setParameter('SourcePage',$path);
            $request->setParameter('SourceQuery',$query);
            $request->setParameter('SourceType',$sourceType);
            $request->setParameter('ToPhone',$PostVars['ToPhone']);
            $request->setParameter('UserId',$fileInfo['UserId']);

            $oldNoCruft = $request->getParameter('NoCruft');
            $actionChain->execute($controller, $request, $user);
            $request->setParameter('NoCruft', $oldNoCruft);

        }

        if($personalMediaId === false)
        {
            $error = $ContentHandler->getError();
            $request->setError('Mobilize', $error ? $error : 'Unknown storage/conversion error');
            return array('OpenLocker', 'Mobilize', VIEW_INPUT);
        }

        $this->MobilizeParams->setParameter('PersonalMediaId', $personalMediaId);

        // Log the mobilization event
        $Content =& $this->MobilizeParams->getParameter('Content');
        $mediaType = array_key_exists('CapabilityId', $Content) ? 
            $Content['CapabilityId'] : 0;

        // Set initial settings for the stored content
        $title = $this->MobilizeParams->getParameter('Title');
        $tags = $this->MobilizeParams->getParameter('Tags');
        $privacy = $this->MobilizeParams->getParameter('Privacy');
        $avatar = $this->MobilizeParams->getParameter('Avatar');
        $lockerDb =& Storage::GetLockerDb($request);
        $mediaModel =& $controller->getModel('Media', 'Media');
        $pix =& $mediaModel->getPixByPersonalMediaId($personalMediaId);
        $mediaInfo =& $pix->getMediaInfo();
        $mediaInfo->setAccess($privacy);
        if($title)
        {
            $mediaInfo->setTitle($title);
            $mediaInfo->setFilename($title);
        }
        if($tags)
        {
            $tags = $mediaModel->normalizeTags($tags, true);
            $mediaInfo->setTags($tags);
        }
        $gallery =& MediaFactory::Get('gallery');
        if(!$gallery->updateMedia($mediaInfo, $lockerDb))
        {
            trigger_error("Could not update personal media details for new content.", E_USER_NOTICE);
        }
        
        if($user->isAuthenticated())
        {
            $userCore =& $user->getAttribute('UserCore');
            $userId = $userCore->Uid();
            if($avatar)
            {
                $mediaModel->setAvatar('personalMedia_id', $userId,
                    $personalMediaId, $userCore, $mediaInfo);
            }
        }

        return VIEW_SUCCESS;
    }

    function getDeviceId(&$user, &$EditOptions)
    {
        if($user->hasAttribute('DeviceInfo'))
        {
            $di =& $user->getAttribute('DeviceInfo');
            return $di->MobileDeviceId();
        }

        if( is_array($EditOptions) &&
            array_key_exists('deviceId', $EditOptions) )
        {
            return $EditOptions['deviceId'];
        }

        return 0;
    }
}

?>
