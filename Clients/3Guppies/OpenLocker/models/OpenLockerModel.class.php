<?php

require_once(dirname(__FILE__) . '/../lib/DownloadHandler.class.php');

/**
 * @brief This model contains common functionality used in the OpenLocker module
 *
 * @author Jeff Ching
 *
 * @date 2007-02-14
 */
class OpenLockerModel extends Model
{
    /**
     * @brief Sends a media file to the supplied phone number
     *
     * @param $_to_phone - string/int - 10 digit phone number to send to
     *
     * @param $_from_phone - string/int - 10 digit phone number to send from
     *
     * @param $_from_name - string - the sender of the file
     *
     * @param $_personal_media_id - int - the personalMedia_id to send
     *
     * @param $_media_id - int - the media_id to send
     *
     * @param $_to_carrier - int - the carrier id to force for the phone number, 
     *    we will detect the carrier if this value is null or not set
     *
     * @param @_source_info - array - info about how the content was obtained
     *
     * @param @_device_id - int - the device that the content is sent to
     *
     * @return boolean - true if sending the message succeeds,
     *                   false if there is an error
     */
    function sendMedia($_to_phone, $_from_phone, $_from_name,
        $_personal_media_id, $_media_id, $_to_carrier = null, 
        $_source_info = array(), $_device_id = null)
    {
        
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $user =& $controller->getUser();

        $read_db =& $this->request->getAttribute('read_db');
        if(is_null($_from_name))
        {
            $_from_name = $_to_phone;
        }

        
        // Check the Do Not Contact List
        $PhoneNumber =& new PhoneNumber(null, $read_db);
        $PhoneNumber->Number($_to_phone);
        if(!$PhoneNumber->canContact())
        {
            $this->setError('SendMedia', 
                "$_to_phone is on the Do Not Contact list");
            return false;
        }
        

        // Get this person's carrier info
        $Carriers =& new Carriers(null, $read_db);
        if(!is_null($_to_carrier))
        {
            $carrier_id = $_to_carrier;
        }
        else
        {
            $carrier_id = ThirdPartyCarrierMappings::getCarrierId($_to_phone);
        }
        
        // Validate the carrier id
        if(is_null($carrier_id))
        {
            $this->setError('SendMedia', 'Unable to determine carrier.');
            return false;
        }
        if($carrier_id <= 0)
        {
            $this->setError('SendMedia', 'We are unable to send content to your carrier at this time. We have recorded your request and will work to support this carrier as soon as we can.');
            return false;
        }

        // Figure out what type of media this is
        $mediaType = $this->_getMediaType($_personal_media_id, $_media_id);
        $sendToSelf = true;
        
        $DeviceInfo =& $this->user->getAttribute('DeviceInfo');
        
        // Populate Message Parameters
        if($this->user->isAuthenticated())
        {
            // Logged-In User
            $UserCore =& $this->user->getAttribute('UserCore');
            $username = $UserCore->Uname();
            $user_id = $UserCore->Uid();
            
            if( $_to_phone == $_from_phone)
            {
                $dlh = DownloadHandlerFactory::get('KnownUser');
            }
            else
            {
                $sendToSelf = false;
                $dlh = DownloadHandlerFactory::get('UnknownUser');
            }

            if($_personal_media_id)
            {
                $dlh->setParameter('PersonalId', $_personal_media_id);
            }
            elseif($_media_id)
            {
                $dlh->setParameter('MediaId', $_media_id);
            }
            else
            {
                // should not get here
            }

            $dlh->setParameter('UserId', $user_id);
            $omafl = $DeviceInfo->hasCapabilityName('OMA-FL');
            $_device_id = $DeviceInfo->MobileDeviceId();
        }
        else
        {
            // Anonymous
            $dlh = DownloadHandlerFactory::get('UnknownUser');

            if($_personal_media_id)
            {
                $dlh->setParameter('PersonalId', $_personal_media_id);
            }
            elseif($_personal_media_id)
            {
                $dlh->setParameter('MediaId', $_media_id);
            }
            else
            {
                // should not get here
            }

            $username = 'noreply';
            if(!is_null($_from_name))
            {
                $username = $_from_name;
            }
            $user_id = 1;

            $omafl = false;
            if(!is_null($_device_id))
            {
                $PhoneInfo = HandsetDetect::getPhoneInfoByDeviceId($_device_id);
                if( $PhoneInfo->hasCapabilityId(34) )
                {
                    $omafl = true;
                }
            }

            if($_to_phone != $_from_phone)
            {
                $sendToSelf = false;
            }
        }

        $from = $username . '@mixxer.com';
        $subject = $mediaType . " from " . $username;
        $dlh->setParameter('Sender', $user_id);
        $dlh->setParameter('SenderUserName', $username);
        $dlh->setParameter('CarrierId', $carrier_id);
        $dlh->setParameter('PhoneNumber', $_to_phone);
        $dlh->setParameter('FromPhoneNumber', $_from_phone);
        $dlh->setParameter('MediaType', $mediaType);

        $deliveryType = null;

        // Create appropriate Message and send
        $mms_hosts = $Carriers->GetMMSHosts();
        $premiumMedia = false;
        if($_media_id)
        {
            $premiumMedia = true;
        }

        // Get a normalized device info object for the recipient's phone
        $RecipientDeviceInfo = null;
        if($sendToSelf && is_object($DeviceInfo))
        {
            $RecipientDeviceInfo =& $DeviceInfo;
        }
        elseif( !is_null($_device_id) )
        {
            $Devices = new Devices(null, $read_db);
            $Devices->MobileDeviceId($_device_id);
            if($Devices->GetAllDeviceInfo(0,0,null,true))
            {
                $RecipientDeviceInfo =& $Devices;
            }
        }

        // Note: If we do not know the recipient's device, then
        //   $RecipientDeviceInfo === NULL

        $recipientHasMMSHost = array_key_exists($carrier_id, $mms_hosts);

        // Default is whether the carrier is a MMS type or SMS type
        $wapDownloadOk = $recipientHasMMSHost ? false : true;

        // If a phone has a setting then use that to override the carrier's
        //   setting
        if( $recipientHasMMSHost && 
            $sendToSelf && 
            is_object($RecipientDeviceInfo) )
        {
            if(!$RecipientDeviceInfo->hasCapabilityName('noWapDownload'))
            {
                $wapDownloadOk = true;
            }
        }

        if( !$wapDownloadOk )
        {
            $email = $_to_phone . '@' . $mms_hosts[$carrier_id];
            $deliveryType = 'MMS';
        }
        else
        {
            $Carriers->MobileCarrierId($carrier_id);
            $sms_host = $Carriers->GetCarrierSmsHostById();
            
            $email = $_to_phone . '@' . $sms_host;
            $deliveryType = 'SMS';
        }
        
        
        
        ////////////////////////////////////////
        // BEGIN: Logging Mobilize Send Event //
        ////////////////////////////////////////
         
        // Normalize the $_source_info
        if(!array_key_exists('Domain', $_source_info))
        {
            $_source_info['Domain'] = "";
        }
        if(!array_key_exists('Page', $_source_info))
        {
            $_source_info['Page'] = "";
        }
        if(!array_key_exists('Query', $_source_info))
        {
            $_source_info['Query'] = "";
        }
        if(!array_key_exists('Affiliate', $_source_info))
        {
            $_source_info['Affiliate'] = "";
        }
        if(!array_key_exists('Source', $_source_info))
        {
            $_source_info['Source'] = SOURCE_TYPE_UNKNOWN;
        }

        // TODO: Make sure this works integrating with Do Not Contact
        $PhoneNumber =& new PhoneNumber(null, $read_db);
        $PhoneNumber->Number($_from_phone);
        $senderIsMember = $PhoneNumber->isRegistered();
        $PhoneNumber->Number($_to_phone);
        $recipientIsMember = $PhoneNumber->isRegistered();

        // Log the sent media
        $Event =& new MobilizeSendEvent(null, Storage::GetMessagingDb($request));
        $Event->ParentId(0);
        $Event->Date(date("Y-m-d H:i:s"));
        $Event->SenderPhone($_from_phone);
        $Event->SenderIsMember($senderIsMember ? 1 : 0);
        $Event->RecipientPhone($_to_phone);
        $Event->RecipientIsMember($recipientIsMember ? 1 : 0);
        $Event->RecipientCarrier($carrier_id);
        $Event->RecipientDevice($_device_id);
        $Event->SenderIsLoggedIn($user->isAuthenticated() ? 1 : 0);
        $Event->DeliveryMethod($deliveryType);
        $Event->SourceType($_source_info['Source']);
        $Event->SourceDomain($_source_info['Domain']);
        $Event->SourcePage($_source_info['Page']);
        $Event->SourceQuery($_source_info['Query']);
        $Event->DownloadUrl($dlh->downloadUrl_id);
        $Event->Affiliate($_source_info['Affiliate']);
        $Event->Status(1);
        if($_personal_media_id)
        {
            $Event->PersonalMediaId($_personal_media_id);
        }
        else
        {
            $Event->MediaId($_media_id);
        }

        if($Event->Add())
        {
            $dlh->setParameter('SendEventId', $Event->LastInsertId());
        }
        else
        {
        }

        //////////////////////////////////////
        // END: Logging Mobilize Send Event //
        //////////////////////////////////////

        $dlh->setParameter('SourceType', $_source_info['Source']);
        
        $mediaLink = '';
        $attachments = array();
        
        if($_personal_media_id != null)
        {
            $MediaModel =& $this->controller->getModel('Media', 'Media');
            $Pix =& $MediaModel->GetPixByPersonalMediaId($_personal_media_id);
            $MediaInfo =& $Pix->getMediaInfo();
            
            $mediaTitle = $MediaInfo->getFilename();
            $mediaType = $MediaInfo->getMediaTypeGroup();
            
            
            if($deliveryType == 'MMS')
            {
                array_push($attachments, MediaConverter::getFullPath($MediaInfo->getLocation(), true));
            }
            else
            {
                $mediaLink = $dlh->getMessageText();
            }
        }
        else
        {
            $mediaObject =& new Media(null, $read_db);
            $mediaDetails = $mediaObject->GetDetailsByMid($_media_id, 1, null, '');
            
            $mediaTitle = $mediaDetails['title'];
            $mediaType = MediaConverter::GetTypeById($mediaDetails['type']);
            
            
            if($deliveryType == 'MMS')
            {
                array_push($attachments, MediaConverter::getFullPath($mediaDetails['location'], false));
            }
            else
            {
                $mediaLink = $dlh->getMessageText();
            }
        }
        
        // Integrate new messaging
        $message = new MessageSender();
		
		$FromPhoneCarriers =& new Carriers(null, $read_db);
		$from_phone_carrier_id = ThirdPartyCarrierMappings::getCarrierId($_from_phone);
		$FromPhoneCarriers->MobileCarrierId($from_phone_carrier_id);
		$from_phone_sms_host = $FromPhoneCarriers->GetCarrierSmsHostById();
		$fromPhoneAddress = $_from_phone . '@' . $from_phone_sms_host;
		
        $toUserName = null;
        $toPhoneNumber = $_to_phone;
        $toEmailAddress = $email;
        $fromUsername = ($_from_name != '' ? $_from_name : $username);
        $fromPhoneNumber = $_from_phone;
        
        if(isset($UserCore))
        {
            $UserComm =& $UserCore->Comm();
            $fromEmail = $UserComm->Email();
        }
        else
        {
            $fromEmail = $from;
        }
        
        //$direction = '1';
        $sourceName = 'http';
        $protocolName = 'email';
        $messageTypeOverride = ($_media_id == null ? 'MobilizeTo' : 'PremiumTo' ) . ($sendToSelf ? 'Self' : 'Friend') . $deliveryType;
        $messageProtocolOverride = 'email';
        $processorID = '0';
        
        $inputParams = array
        (
            'TOUSERNAME' => $toUserName,
            'TOPHONE' => $toPhoneNumber,
            'TOADDRESS' => $toEmailAddress,
            'FROMUSERNAME' => $fromUsername,
            'FROMPHONE' => $fromPhoneNumber,
            'FROMADDRESS' => $fromEmail,
            'FROMPHONEADDRESS' => $fromPhoneAddress,
            'MEDIALINK' => $mediaLink,
            'MEDIATITLE' => $mediaTitle,
            'MEDIATYPE' => $mediaType
        );
        
        //$message->setParameter('direction', $direction);
        //$message->setParameter('from_addr', $fromEmail);
        $message->setParameter('to_addr_raw', $toEmailAddress);
        $message->setParameter('input_params', $inputParams);
        $message->setParameter('attachments', $attachments);
        $message->setParameter('source_name', $sourceName);
        $message->setParameter('protocol_name', $protocolName);
        $message->setParameter('message_type_override', $messageTypeOverride);
        $message->setParameter('message_protocol_override', $messageProtocolOverride);
        $message->setParameter('processor_id', $processorID);
        
        $messageSent = false;
        $messageSent = $message->send();
        
        
        // Send the media in an SMS or MMS
        if($messageSent == false)
        {
            if($message->hasError())
            {
                $error = '';
                
                foreach($message->errors as $currentError)
                {
                    $error .= $currentError . "<br />\n";
                }
                
                $this->setError('SendMedia', $error);
            }
            else
            {
                $this->setError('SendMedia', 'Unknown error in MessageSender');
            }
        
            // there was an error, roll back the mobilizeSendEvent
            $Event->Remove();
            return false;
        }
              
        return true;
    }

    /**
     * @private
     *
     * @brief Returns a string representation of the media type of the specified media
     *
     * @return string - media type
     */
    function _getMediaType($_personal_media_id, $_media_id)
    {
        if(!is_null($_personal_media_id))
        {
            $MediaModel =& $this->controller->getModel('Media', 'Media');
            $Pix =& $MediaModel->GetPixByPersonalMediaId($_personal_media_id);
            $MediaInfo =& $Pix->getMediaInfo();
            if( !is_object($MediaInfo) )
            {
                return 'Other';
            }

            $mediaType = MediaConverter::GetTypeById($MediaInfo->getMediaType());
            if($mediaType == 'Screensavers')
            {
                $mediaType = 'Pic';
            }
            elseif($mediaType == 'Ringtones')
            {
                $mediaType = 'Ringtone';
            }

            return $mediaType;
        }
        elseif(!is_null($_media_id))
        {
            $read_db =& $this->request->getAttribute('read_db');
            $media =& new Media(null, $read_db);
            $mediaDetails = $media->GetDetailsByMid($_media_id);
            $mediaType = MediaConverter::GetTypeById($mediaDetails['type']);
            if($mediaType == 'Ringtones')
            {
                $mediaType = 'Ringtone';
            }
            elseif($mediaType == 'Other')
            {
                $mediaType = 'Application';
            }
            return $mediaType;
        }
        else
        {
            $this->setError('MediaType', 'No media specified');
            return false;
        }
    }

    /**
     * @private
     *
     * @brief Returns the renderer content of a Smarty template
     *
     * @param $template - string - template name
     *
     * @param $dlh - DownloadHandler object - contains parameters for the template
     *
     * @return string - contents of the renderered template
     */
    function _renderSmsTemplate($template, &$dlh)
    {
        $renderer =& $this->request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir(HOME_BASE . 'modules/OpenLocker/templates/');
        $renderer->setTemplate($template);
        $renderer->setAttributeByRef('MessageCallback', &$dlh);
        $body = $renderer->fetchResult($this->controller, $this->request, 
            $this->user);
        return $body;
    }

    /**
     * @brief Send media to an email address via an attachment
     *
     * @param $_to_email - string - email address to send the media
     *
     * @param $_from_email - string - the return address that the recipient will
     *    see
     *
     * @param $_from_name - string - the name the the recipient will see
     *
     * @param $_personal_media_id - int - the personal media to attach
     *
     * @param $_media_id - int - the media to attach
     *
     * @return boolean - true if the message was sent (may not have been 
     *    received, but the message was sent)
     */
    function sendMediaEmail($_to_email, $_from_email, $_from_name, 
        $_personal_media_id = null, $_media_id = null)
    {
        
        $dlh = DownloadHandlerFactory::get('UnknownUser');
        $mediaType = $this->_getMediaType($_personal_media_id, $_media_id);
        if($_personal_media_id)
        {
            $dlh->setParameter('PersonalId', $_personal_media_id);
        }
        elseif($_media_id)
        {
            $dlh->setParameter('MediaId', $_media_id);
        }
        else
        {
            $this->setError('SendMediaEmail', 'No media specified');
            return false;
        }

        // Template parameters
        $dlh->setParameter('Sender', "Sender");
        $dlh->setParameter('SenderUsername', "SenderUsername");
        $dlh->setParameter('CarrierId', "CarrierId");
        $dlh->setParameter('PhoneNumber', "PhoneNumber");
        $dlh->setParameter('MediaType', "MediaType");

        $body = $this->_renderSmsTemplate('txt.tpl', $dlh);
        $subject = $mediaType . " from " . $_from_name;

        // Create an email message with attachments
        $message = new MMSPixMessage($_to_email, $_from_email, $subject, $body, 
            false);
        if($_personal_media_id)
        {
            $message->setLockerDb(Storage::getLockerDb($this->request));
            $message->addPersonalMediaId($_personal_media_id);
        }
        elseif($_media_id)
        {
            $message->setReadDb($this->request->getAttribute('read_db'));
            $message->addMediaId($_media_id);
        }

        // Try and send the email
        if($message->Send())
        {
            return true;
        }
        else
        {
            $this->setError('SendMediaEmail', 'Error sending message');
            return false;
        }
        
    }

    /**
     * @brief Method to put media content into a user's locker
     *
     * @param $userId - int - the user id whose locker we are inserting content
     *
     * @param $fileInfo - array - settings for the personal media entry
     *
     * @note This function does not move any files. You must handle this
     *   elsewhere.
     *
     * Required info in $fileInfo:
     *   - 'Filename',      defaults to ""
     *   - 'CapabilityId',  defaults to 0
     *   - 'Path',          defaults to ""
     *
     * Options for $fileInfo:
     *   - 'AccessLevel',   defaults to 1
     *   - 'Converted',     defaults to ""
     *   - 'ConvertedTo',   defaults to 0
     *
     * @return personalMedia_id if the insert succeeded,
     *         false otherwise
     */
    function storePersonalMedia($userId, $fileInfo = array())
    {
        if( !array_key_exists('Filename', $fileInfo) ||
            !array_key_exists('CapabilityId', $fileInfo) ||
            !array_key_exists('Path', $fileInfo) )
        {
            $this->setError('StorePersonalMedia', 'Not enough file info.');
            return false;
        }

        $currentLocation = $fileInfo['Path'];

        $locker_db =& Storage::GetLockerDb($this->request);

        $filename = $fileInfo['Filename'];
        $originalType = $fileInfo['CapabilityId'];
        $originalLocation = MediaConverter::getFullPath(basename($currentLocation), true);
        $originalFileSize = filesize($currentLocation);

        $convertedLocation = array_key_exists('Converted', $fileInfo) ? 
            $fileInfo['Converted'] : "";
        $fullConvertedLocation = MediaConverter::getFullPath($convertedLocation, true);
        $dateAdded = date("Y-m-d H:i:s");
        $convertedFileSize = file_exists($fullConvertedLocation) ?  
            filesize($fullConvertedLocation) : 0;
        $convertedType = array_key_exists('ConvertedTo', $fileInfo) ? 
            $fileInfo['ConvertedTo'] : 0;
        $accessLevel = array_key_exists('AccessLevel', $fileInfo) ?
            $fileInfo['AccessLevel'] : 1;

        // Insert into personalMedia table

        $PersonalMedia = new PersonalMedia(null, $locker_db);
        $PersonalMedia->PersonalMediaId(0);
        $PersonalMedia->UserId($userId);
        $PersonalMedia->Location($convertedLocation);
        $PersonalMedia->DateAdded(date("Y-m-d H:i:s",time()));
        $PersonalMedia->FileSize($convertedFileSize);

        // If the userid/converted location is already in the locker, do not add it again
        if($PersonalMedia->isInLocker())
        {
            $PersonalMedia->AccessLevel(1);
            $PersonalMedia->Commit();
            return $PersonalMedia->PersonalMediaId();
        }

        $PersonalMedia->Name($filename);
        $PersonalMedia->Type($convertedType);
        $PersonalMedia->AccessLevel($accessLevel);
        $PersonalMedia->Original($filename);


/*
        $PersonalMedia =& new PersonalMedia(null, $locker_db);
        $PersonalMedia->PersonalMediaId(0);
        $PersonalMedia->UserId($userId);
        $PersonalMedia->Name($filename);
        $PersonalMedia->Location($convertedLocation);
        $PersonalMedia->DateAdded($dateAdded);
        $PersonalMedia->FileSize($convertedFileSize);
        $PersonalMedia->Type($convertedType);
        $PersonalMedia->AccessLevel($accessLevel);
        $PersonalMedia->Original($filename);
*/
        if(!$PersonalMedia->Add())
        {
            $this->setError('StorePersonalMedia', 'There was an error storing your file.');
            return false;
        }

        $insertId = $PersonalMedia->LastInsertId();

        // Try and create a personalFiles entry.  If the file already exists,
        //   ignore the insert error
        $PersonalFiles =& new PersonalFiles(null, $locker_db);
        $PersonalFiles->Location($originalLocation);
        $PersonalFiles->Size($originalFileSize);
        $PersonalFiles->Type($originalType);
        $PersonalFiles->DateCreated($dateAdded);
        $PersonalFiles->Add();

        return $insertId;
    }

}

?>
