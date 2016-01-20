<?php

require_once(HOME_BASE.'opt/util/EventLog.class.php');

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief This page displays the splash page that a user sees before downloading
 *   their content.
 *
 * @note For the 3 content types:
 *    html:         We a preview image of the content
 *    wml:          We show a splash page.  The user must click on the 
 *                    download link
 *    xhtmlMobile:  We show a splash page.  The phone's browser is automatically
 *                    redirected to the download link.  Alternatively, the user
 *                    can click and follow the download link shown.
 *
 * @author  Blake Matheny
 */
class DownloadView extends View
{

    /**
     * @brief Render the presentation.
     */
    function & execute (&$controller, &$request, &$user)
    {

        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');
        $ct = $controller->getContentType();
        switch ( $ct )
        {
            case 'html':
            case 'xhtmlMobile':
            case 'wml':
                break;
            default:
                $ct = 'html';
                break;
        }
        $renderer->setTemplate("DownloadView_index.$ct.tpl");

        // Message object
        $data =& $request->getAttribute('OpenLocker.Download.Data');

        if ( $request->hasError('OpenLocker.Download') )
        {
            $renderer->setAttribute('DownloadError', $request->getError('OpenLocker.Download'));
            return $renderer;
        }

        // DownloadHandler object
        $handler =& $data->getParameter('Handler');

        // DownloadUrl info
        $downloadUrlInfo = $data->getParameter('Data');

        // Array of meta data
        $d = $data->getParameter('Decoded');
        $d['Tiny'] = $data->getParameter('Tiny');
        $d['AccessCount'] = $downloadUrlInfo['downloadUrl_access_count'];

        // Fetches all the content data.  For the UnknownUserDownloadHandler,
        //   we may need convert the content after we detected the phone model
        //   that is accessing this page
        $downloadData = $handler->getLinkData($d);

        $renderer->setAttribute('DownloadData', $downloadData);

        // Did an error occur while figuring out the link data
        //   Could be a conversion problem if using the UnknownUserDownloadHandler
        $downloadStatus = 1;
        $downloadError = null;
        if ( array_key_exists('Error', $downloadData) )
        {
            $renderer->setAttribute('DownloadError', $downloadData['Error']);
            $downloadStatus = 0;
            $downloadError = $downloadData['Error'];
        }

        /****************************
         *  Log the download event  *
         ***************************/
        $read_db =& $request->getAttribute('read_db');

        $deviceId = 0;
        $BrowserInfo =& $user->getAttribute('BrowserInfo');
        if( is_object($BrowserInfo) && $BrowserInfo->isValid())
        {
            $deviceId = $BrowserInfo->getDeviceId();
        }

        $sendEventId = 0;
        if( array_key_exists('SendEventId', $d) )
        {
            $sendEventId = $d['SendEventId'];
        }

        $sourceType = SOURCE_TYPE_UNKNOWN;
        if( array_key_exists('SourceType', $d) )
        {
            $sourceType = $d['SourceType'];
        }

        $toPhoneNumber = 0;
        $recipientIsMember = 0;
        $sentToSelf = null;
        if( array_key_exists('PhoneNumber', $d) )
        {
            $toPhoneNumber = $d['PhoneNumber'];
            $PhoneNumber =& new PhoneNumber(null, $read_db);
            $PhoneNumber->Number($toPhoneNumber);
            $recipientIsMember = $PhoneNumber->isRegistered() ? 1 : 0;

            if( array_key_exists('FromPhoneNumber', $d) )
            {

                $fromPhoneNumber = $d['FromPhoneNumber'];
                if($toPhoneNumber == $fromPhoneNumber)
                {
                    $sentToSelf = 'Self';
                }
                else
                {
                    $sentToSelf = 'Other';
                }
            }
        }

        $mediaClass = 'Personal';
        $mediaId = null;
        if( array_key_exists('MediaId', $d) )
        {
            $mediaId = $d['MediaId'];
            if(!$mediaId)
            {
                $mediaId = null;
            }
        }

        $personalMediaId = null;
        if( array_key_exists('PersonalId', $d) )
        {
            $personalMediaId = $d['PersonalId'];
            if(!$personalMediaId)
            {
                $personalMediaId = null;
            }
        }

        $mediaGenericType = 'Other';
        if( array_key_exists('MediaType', $d) )
        {
            $mediaGenericType = $d['MediaType'];
            switch($mediaGenericType)
            {
            case "Pic":
            case "Picture":
                $mediaGenericType = "Picture";
                break;
            case "Video":
            case "Audio":
                break;
            default:
                $mediaGenericType = "Other";
                break;
            }
        }

        $carrierId = null;
        if( array_key_exists('CarrierId', $d) )
        {
            $carrierId = $d['CarrierId'];
        }
        else
        {
            $carrierId = ThirdPartyCarrierMappings::getCarrierId($toPhoneNumber);
        }
        

        $mediaFileExtension = null;
        if( array_key_exists('Extension', $downloadData) )
        {
            $mediaFileExtension = $downloadData['Extension'];
        }

        $Event =& new MobilizeDownloadEvent(null, Storage::getMessagingDb($request));
        $Event->SendEventId($sendEventId);
        $Event->Date(date("Y-m-d H:i:s"));
        $Event->Status($downloadStatus);
        $Event->Error($downloadError);
        $Event->SourceType($sourceType);
        $Event->RecipientPhone($toPhoneNumber);
        $Event->RecipientCarrier($carrierId);
        $Event->RecipientIsMember($recipientIsMember);
        $Event->DeviceId($deviceId);
        $Event->SentToSelf($sentToSelf);
        $Event->MediaId($mediaId);
        $Event->PersonalMediaId($personalMediaId);
        $Event->MediaClass($mediaClass);
        $Event->MediaGenericType($mediaGenericType);
        $Event->MediaFileExtension($mediaFileExtension);

        if($Event->Add())
        {

        }
        else
        {

        }

        $renderer->setAttribute('DownloadData', $downloadData);

        if ( $ct == 'xhtmlMobile' && !empty($downloadData['DownloadUrl']) )
            $controller->redirect($downloadData['DownloadUrl']);

        return $renderer;
    }

}

?>
