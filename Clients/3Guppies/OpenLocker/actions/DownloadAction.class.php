<?php

require_once(dirname(__FILE__) . '/../lib/DownloadHandler.class.php');

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * @brief Faciliate downloads via tiny URL
 *
 * This action takes a single parameter, 'Tiny', and based on that takes care
 * of deciphering the meaning, presenting the appropriate layout and data,
 * doing file conversions, handset detection and a host of other things.
 *
 * @author  Blake Matheny
 */

/**
 * NOTE: Do NOT set isSecure() to TRUE without talking to Blake
 */
class DownloadAction extends Action
{
    /**
     * NOTE: The following variables are available as request object parameters.
     */

    /**
     * @brief The ID of a url to fetch data for.
     *
     * Tiny is the only parameter available for this action, and it is a
     * string. This ID is used to look up data from lockers.downloadUrls which
     * maps to a PHP data structure. This data is then used to create download
     * urls, do device detection, and do media conversions.
     */
    var $Tiny           =   null;

    /**
     * NOTE: The follwoing variables are NOT available as request object
     * parameters.
     */
    var $UserId         =   0;
    var $MediaId        =   0;
    var $PersonalId     =   0;
    var $PhoneNumber    =   0;
    var $CarrierId      =   0;

    /**
     * Specific handler data, set in validate
     */
    var $DownloadData       =   null; // A row from downloadUrls
    var $DownloadHandler    =   null; // An object of type DownloadHandler
    var $DownloadDecoded    =   null; // An array of info, decoded by the handler

// +---------------------------------------------------------------------------+
// | Initialization, Execution and Validation                                  |  
// +---------------------------------------------------------------------------+


    /**
     * @brief Execute action initialization procedure.
     */
    function initialize (&$controller, &$request, &$user)
    {
        $this->Tiny = $request->getParameter('Tiny');
        return TRUE;
    }

    /**
     * @brief Execute all business logic.
     */
    function execute (&$controller, &$request, &$user)
    {
        // ticket 132364   https://bugs.mixxer.com/index.html?q=132364
        // Email recipients will be redirected over to invitation code landing page, so we can
        // register them!!!
        if ((NULL !== $this->DownloadDecoded) && array_key_exists('Email', $this->DownloadDecoded) )
        {
           $invitationCodeLandingPage = $controller->genUrl(array(
                            'module' => 'Checkout',
                            'action' => 'InvitationCode',
                            'invitation_code' => $this->Tiny));
           $controller->redirect($invitationCodeLandingPage);
           return VIEW_NONE;
        }

        $this->DownloadDecoded['Tiny'] = $this->Tiny;
        $this->DownloadDecoded['AccessCount'] = $this->DownloadData['downloadUrl_access_count'];

        $downloadData = $this->DownloadHandler->getLinkData($this->DownloadDecoded);
        $downloadData = array_merge($downloadData, $this->DownloadDecoded);

        $request->setAttribute('OpenLocker.Download.Data', $downloadData);

        $BrowserInfo = $user->getAttribute('BrowserInfo');
        if( is_object($BrowserInfo) && $BrowserInfo->isValid() &&
            array_key_exists('PhoneNumber', $this->DownloadDecoded) )
        {
            $deviceId = $BrowserInfo->getDeviceId();
            $write_db =& $request->getAttribute('write_db');
            $PhoneNumber = new PhoneNumber(null, $write_db);
            $PhoneNumber->Number($this->DownloadDecoded['PhoneNumber']);
            $PhoneNumber->DeviceId($deviceId);
            $PhoneNumber->DeviceLastUpdate(date("Y-m-d H:i:s"));
            $PhoneNumber->AddOrUpdate();
        }

        // Handle errors thrown by getLinkData
        if( array_key_exists('Error', $downloadData) )
        {
            $request->setError('OpenLocker.Download', $downloadData['Error']);
            return VIEW_ERROR;
        }

        // Handle error where downlaod url is not set
        if( empty($downloadData['DownloadUrl']) )
        {
            $request->setError('OpenLocker.Download', "We're sorry, the file you are attempting to download is no longer available.");
            return VIEW_ERROR;
        }

        // Anonymous download and the media is not ready for this phone
        if( array_key_exists('Preexisting', $downloadData) &&
            !$downloadData['Preexisting'] )
        {
            $request->setAttribute('JobId', $downloadData['JobId']);
            $request->setAttribute('Tiny', $this->Tiny);
            return array('OpenLocker','DownloadWait',VIEW_INDEX);
        }

        if( $controller->getContentType() == 'xhtmlMobile' && !empty($downloadData['DownloadUrl']) )
        {
            $controller->redirect($downloadData['DownloadUrl']);
        }

        return VIEW_SUCCESS;
    }

    /**
     * @brief Validate the request as a whole.
     */
    function validate (&$controller, &$request, &$user)
    {
        if ( empty($this->Tiny) )
        {
            $request->setError('OpenLocker.Download', 'Tiny URL not specified');
            return FALSE;
        }

        // Get the phone model info
        //$BrowserInfo =& $user->getAttribute('BrowserInfo');
        //if($controller->getContentType() != "html" && !$BrowserInfo->isValid())
        //{
        //    $request->setError('OpenLocker.Download', 'Unable to detect phone model');
        //    return FALSE;
        //}

        $dlh = new DownloadHandler();
        if ( $dlh )
        {
            // Fetch info from the downloadUrls table
            //   increases the download count
            $info = $dlh->getById($this->Tiny);

            // Validate the tiny url
            if ( !$info || !is_array($info) || !array_key_exists('downloadUrl_handler', $info) )
            {
                // The tiny url is NOT found in the downloadUrls table
                $request->setError('OpenLocker.Download', 'Unable to find url');
                return FALSE;
            }
            else
            {
                // The tiny url is found in the downloadUrls table
                $this->DownloadData = $info;
                $this->DownloadHandler = $info['downloadUrl_handler'];

                // Fetch the correct type of download handler (KnownUser vs. UnknownUser)
                $dlh = DownloadHandlerFactory::get($this->DownloadHandler);

                // DownloadHandler instantiation problem
                if ( !$dlh )
                {
                    $request->setError('OpenLocker.Download',
                            sprintf('Invalid handler (%s) specified', $this->DownloadHandler));
                    return FALSE;
                }

                $this->DownloadHandler = $dlh;
                $this->DownloadDecoded = $dlh->getUnencoded($this->DownloadData['downloadUrl_data']);

                // Check for bad data in the downloadUrl_data column.  Maybe error with 
                //   the meta data encoding process?
                if ( !$this->DownloadDecoded )
                {
                    $request->setError('OpenLocker.Download', 'Unable to decode handler data');
                    return FALSE;
                }
                return TRUE;
            }
        }
        else
        {
            $request->setError('OpenLocker.Download', 'Unable to initialize handler');
            return FALSE;
        }

        // Shouldn't get here (not returning in some condition)
        $request->setError('OpenLocker.Download', 'Unknown general failure');
        return FALSE;
    }


// +---------------------------------------------------------------------------+
// | ShareAction Methods                                                       |  
// +---------------------------------------------------------------------------+


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
    function registerValidators (&$validatorManager, &$controller, &$request,
                                 &$user)
    {
        // NONE
    }
}

?>
