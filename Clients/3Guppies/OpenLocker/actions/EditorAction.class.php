<?php
/**
 * @brief Action to deal with displaying and dealing with edit options.
 *
 * @author Jeff Ching
 *
 * @date 2007-02-13
 */
class EditorAction extends Action
{
    /**
     * @brief This Message object contains all the mobilize parameters and is
     *   aggregated over the course of the mobilize chain of command
     */
    var $MobilizeParams = null;

    /**
     * @brief Content of $EditOptions[], an array.
     *
     * The EditOptions array allows for a variety of edit options to be passed
     * in which are context sensitive (where the context is the media type).
     * This allows for new editors to be dropped in place for content types. The
     * Request parameter is 'EditOptions[]' which PHP treats as an array.
     *
     * Known options are documented below, or with the editor:
     *  Global.NoFormat     -   Do not apply any formatting to the file.
     *                          Boolean. Supercedes other options.
     *  Ringtone.Start      -   Integer. Time of ringtone start.
     *  Ringtone.End        -   Integer. Time of ringtone end.
     *  Image.Resize        -   Original or Stretch. Original maintains aspect
     *                          ratio and stretch forces the image to fit the
     *                          exact dimensions of the display.
     *
     *  idCropIX            -   These options are used with the cropper.
     *  idCropIY                Image.Resize is still supported though.
     *  idCropDX
     *  idCropDY
     *  idSizeDX
     *  idSizeDY
     *  idCropOccur         -   Boolean. Whether to use the -crop command at 
     *                          all.
     *  deviceId            -   Integer. Device to convert file for if the user
     *                          is anonymous or does not have DeviceInfo set
     *  deviceWidth         -   Integer. Screen width of phone if the user is
     *                          anonymous or does not have DeviceInfo set
     *  deviceHeight        -   Integer. Screen height of phone if the user is
     *                          anonymous or does not have DeviceInfo set
     *
     * Note: If you are passing EditOptions[] via a GET request, you should do
     * the following: urlencode(base64_encode(serialize($array))) and pass that
     * in as EditOptions. This will accomplish the same thing.
     */
    var $EditOptions = array();

    /**
     * @brief Allow the editing to be skipped.
     *
     * The EditPassThru option allows the editor to be bypassed, in which
     * case auto selections apply. This is mainly useful for third party
     * integration. The Request parameter is 'EditPassThru'.
     */
    var $EditPassThru = false;

    /**
     * @brief Should the user receive an SMS notification or not?
     *
     * If this is set, the user will receive an SMS notification of their
     * successful Add. The request Parameter is 'SendNotice', a boolean. By
     * default this is true, and user text messaging preferences are respected.
     */
    var $SendNotice     =   true;

    /**
     * @brief Title for the personalMedia when stored
     *
     * This request parameter is 'mobilizeTitle'
     */
    var $Title          =   null;

    /**
     * @brief Tags for the personalMedia when stored
     *
     * This request parameter is 'mobilizeTags'
     */
    var $Tags           =   null;

    /**
     * @brief Whether or not the personalMedia will be public when stored
     *
     * This request parameter is 'mobilizePrivacy'.  0 = private, 1 = public
     */
    var $Privacy        =   null;

    /**
     * @brief Whether to set the new personalMedia as the user's avatar after
     *   it is stored
     *
     * This request parameter is 'mobilizeAvatar'.  It should only be set from
     * the picture editor.
     */
    var $Avatar         =   null;

    var $CarrierId      =   null;

    var $DeviceId       =   null;

    /**
     * @brief Initialize our variables
     */
    function initialize(&$controller, &$request, &$user)
    {
        $this->MobilizeParams =& $request->getAttribute('MobilizeParams');
        $read_db =& $request->getAttribute('read_db');

        $eot = $request->getParameter('EditOptions', array());
        if ( is_string($eot) )
        {   
            $t = @base64_decode($eot);
            if ( !$t )
            {   
                @mail('bmatheny@mixxerinc.com', 'Mobilize EditOptions Error b64', $eot . "\r\n\r\n" . $_SERVER['HTTP_REFERER']);
            }
            else
            {   
                $t = @unserialize($t);
                if ( !$t )
                {   
                    @mail('bmatheny@mixxerinc.com', 'Mobilize EditOptions Error uns', $eot . "\r\n\r\n" . $_SERVER['HTTP_REFERER']);
                }
                else
                    $this->EditOptions = $t;
            }
        }
        else
            $this->EditOptions = $eot;
        if ( !is_array($this->EditOptions) )
            $this->EditOptions = array();

        $this->EditPassThru =   $request->getParameter('EditPassThru', false);
        
        $title = $this->MobilizeParams->getParameter('Title');
        if( $title )
            $this->Title = $title;
        else if( $request->hasParameter( 'Title' ) )
            $this->Title = $request->getParameter( 'Title' );
        else
            $this->Title =   $request->getParameter('mobilizeTitle', 
                $this->MobilizeParams->getParameter('Filename'));

        if( trim($this->Title) == "")
        {
            $this->Title = "(no title)";
        }

        $this->Tags         =   $request->getParameter('mobilizeTags');
        $this->Avatar       =   $request->getParameter('mobilizeAvatar');

        if($user->isAuthenticated())
        {
            if($this->MobilizeParams->getParameter('MyPhone') && 
               $request->hasParameter('DontSendNotice') &&
               $request->getParameter('DontSendNotice'))
            {
                $this->SendNotice = false;
            }
            else
            {
                $this->SendNotice = true;
            }
            $this->Privacy = $request->getParameter('mobilizePrivacy');
        }
        else
        {
            $this->SendNotice = true;
            $this->Privacy = 0;
        }

        $carrier = $request->getParameter('carrier');
        if(!is_null($carrier))
        {
            $this->CarrierId = $carrier;
        }
        else
        {
            $this->CarrierId = ThirdPartyCarrierMappings::getCarrierId(
                $this->MobilizeParams->getParameter('ToPhoneNumber'));
            if(is_null($this->CarrierId))
            {
                // Cannot detect phone number's carrier so don't sent to them
                $recipients =& $this->MobilizeParams->getParameter('Recipients');
                if($recipients[0] == $this->MobilizeParams->getParameter('ToPhoneNumber')) 
                {
                    $temp = array_shift($recipients);
                }
                $request->setAttribute('CannotDetectCarrier', true);
            }
        }

        // If the user is anonymous, try and guess the phone number
        if( !$user->isAuthenticated() )
        {
            if( is_array($this->EditOptions) && 
                array_key_exists('deviceId', $this->EditOptions) &&
                $this->EditOptions['deviceId'] > 0 )
            {
                $this->DeviceId = $this->EditOptions['deviceId'];
            }
            else
            {
                $number = $this->MobilizeParams->getParameter('ToPhoneNumber');
                $PhoneNumber =& new PhoneNumber(null, $read_db);
                $PhoneNumber->Number($number);
                $results = $PhoneNumber->FindExacts();
                if(sizeof($results) > 0)
                {
                    $PhoneNumber = array_shift($results);
                    $this->DeviceId = $PhoneNumber->DeviceId();
                    $this->EditOptions['deviceId'] = $this->DeviceId;
                }
            }
        }

        $this->MobilizeParams->setParameter('EditOptions', $this->EditOptions);
        $this->MobilizeParams->setParameter('EditPassThru', $this->EditPassThru);
        $this->MobilizeParams->setParameter('Title', $this->Title);
        $this->MobilizeParams->setParameter('Tags', $this->Tags);
        $this->MobilizeParams->setParameter('Privacy', $this->Privacy);
        $this->MobilizeParams->setParameter('Avatar', $this->Avatar);
        $this->MobilizeParams->setParameter('SendNotice', $this->SendNotice);
        $this->MobilizeParams->setParameter('CarrierId', $this->CarrierId);
        $this->MobilizeParams->setParameter('DeviceId', $this->DeviceId);

        return TRUE;
    }

    function execute(&$controller, &$request, &$user)
    {
        $read_db =& $request->getAttribute('read_db');

        $ContentHandler =& $this->MobilizeParams->getParameter('ContentHandler');
        $EditPassThru =& $this->MobilizeParams->getParameter('EditPassThru');
        $EditOptions =& $this->MobilizeParams->getParameter('EditOptions');

        $hasSeenCaptcha = $user->hasAttribute('CaptchaInfo');
        $validCaptcha = false;
        $CaptchaInfo = false;

        if(!$user->isAuthenticated())
        {
            $carrier = $request->getParameter('carrier');
            // Did the user suggest a phone model to us?
            if($carrier == -2)
            {
                $carrierName = $request->getParameter('suggestCarrier');
                $phoneName = $request->getParameter('suggestPhone');
                $this->_logPhoneSuggestion($carrier, $carrierName, $phoneName);

                $request->setError('Mobilize', 'Sorry, we do not support your carrier at this time.  Thank you for your input and we will work to support your suggested carrier and phone model.');
                return array('OpenLocker', 'Mobilize', VIEW_INPUT);
            }

            // Validate the captcha
            $CaptchaInfo =& $user->getAttribute('CaptchaInfo');
            if(!is_null($CaptchaInfo))
            {
                if($CaptchaInfo['valid'] > 0)
                {
                    $validCaptcha = true;
                }
                else
                {
                    // need to validate
                    $userInput = $request->getParameter('captchaValue');
                    if($userInput)
                    {
                        $seed = $CaptchaInfo['seed'];
                        if(Captcha::isMatch($seed, $userInput))
                        {
                            // valid user input -> we won't ask again for 5-10 requests
                            $CaptchaInfo['valid'] = rand(5,10);
                            $validCaptcha = true;
                        }
                        else
                        {
                            // invalid user input
                            $request->setAttribute('showCaptcha', true);
                            $request->setError('Captcha',
                                'The captcha you entered is incorrect. Please try again.');
                        }
                    }
                    else
                    {
                        // no user input
                        $request->setAttribute('showCaptcha', true);
                        if(!is_null($userInput))
                        {
                            $request->setError('Captcha', 'Please enter the captcha code.');
                        }
                    }
                }
            }
            else
            {
                // No captcha info (user has not yet seen a captcha)
                $request->setAttribute('showCaptcha', true);
                $validCaptcha = true;
            }
            // End validate captcha

            // Do we need to ask for the handset info?
            $PhoneNumber = $this->MobilizeParams->getParameter('ToPhoneNumber');

            // Cannot detect the recipient's carrier, must prompt them
            $Carriers =& new Carriers(null, $read_db);
            $request->setAttribute('PromptCarrier', true);
            $CarrierInfo = $Carriers->GetAllCarriers();
            $request->setAttribute('CarrierInfo', $CarrierInfo);

            if($this->CarrierId)
            {
                $Devices =& new Devices(null, $read_db);
                $Devices->MobileCarrierId($this->CarrierId);
                $DeviceInfo = $Devices->GetAllDeviceInfoByCarrierId();
                $request->setAttribute('DeviceInfo', $DeviceInfo);
            }
        }
        else
        {
            // No captcha required for logged in users
            $hasSeenCaptcha = true;
            $validCaptcha = true;
        }

        // Conditions to proceed to store and send
        if((array_key_exists('Global.NoFormat', $EditOptions) || 
            $EditPassThru || 
            !$ContentHandler->hasEditor()) && 
            $hasSeenCaptcha && 
            $validCaptcha )
        {
            if( array_key_exists('deviceId', $EditOptions) &&
                $EditOptions['deviceId'] == -2)
            {
                $phoneName = $request->getParameter('suggestPhone');
                $this->_logPhoneSuggestion($this->CarrierId, "", $phoneName);
            }

            if($CaptchaInfo) $CaptchaInfo['valid']--;
            $controller->forward('OpenLocker', 'StoreAndSend');
            return VIEW_NONE;
        }

        // Show the interim editor pages
        return $ContentHandler->getEditor();
    }

    function validate(&$controller, &$request, &$user)
    {
        return true;
    }

    function _logPhoneSuggestion($carrier_id, $carrier_name, $device_name)
    {
        $controller =& Controller::getInstance();
        $request =& $controller->getRequest();
        $read_db =& $request->getAttribute('read_db');
        $PhoneSuggestion =& new PhoneSuggestion(null, $read_db);

        if($carrier_id > 0)
        {
            $Carriers =& new Carriers(null, $read_db);
            $Carriers->MobileCarrierId($carrier_id);
            $carrier_name = $Carriers->GetCarrierNameById();
        }

        $PhoneSuggestion->User(0);
        $PhoneSuggestion->Carrier($carrier_name);
        $PhoneSuggestion->Model($device_name);
        $PhoneSuggestion->Date(time());
        return $PhoneSuggestion->Add();
    }
}

?>
