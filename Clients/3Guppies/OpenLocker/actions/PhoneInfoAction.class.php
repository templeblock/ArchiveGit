<?php
/**
 * @brief Class to fetch information about a user's phone if we do not already
 *   have it.
 *
 * @author Jeff Ching
 *
 * @date 2007-02-13
 */
class PhoneInfoAction extends Action
{
    /**
     * @brief This Message object contains all the mobilize parameters and is
     *   aggregated over the course of the mobilize chain of command
     */
    var $MobilizeParams = null;

    /**
     * @brief Whether or not the user agrees to the terms and conditions
     *
     * If the user is logged in, then they already agreed to the terms and conditions
     * when they registered.  Otherwise, this value is passed in as this parameter
     *   $agreeTC
     */
    var $AgreeTC        = false;

    /**
     * @brief The addresses that we are sending the media to
     */
    var $Recipients     = array();

    /**
     * @brief The phone number that we are sending the media to
     *
     * If the user is logged in, then this value is the user's stored phone number
     * Otherwise, this value can be entered through the parameters
     *   $mobilizePhone1, $mobilizePhone2, $mobilizePhone3 OR
     *   $mobilizePhone
     */
    var $ToPhoneNumber  = null;

    /**
     * @brief The phone number that we are sending from
     *
     * If the user is logged in, then this value is the user's stored phone number
     * Otherwise, this value can be entered through the parameters
     *   $myPhone1, $myPhone2, $myPhone3 OR
     *   $myPhone
     */
    var $FromPhoneNumber = null;

    /**
     * @brief The name of the sender
     *
     * If the user is logged in, then this value is 'your locker'
     * Otherwise, this value can be entered through the parameter 
     *   $fromName
     */
    var $FromName       = null;

    
    /**
     * @brief boolean that tells us whether user is sending to self or not
     */
    var $MyPhone = false;
    
    
    /**
     * @brief Title for the personalMedia when stored
     *
     * This request parameter is 'mobilizeTitle'
     */
    var $Title =   null;
    
    
    /**
     * @brief Initialize our variables
     */
    function initialize(&$controller, &$request, &$user)
    {
        $this->MobilizeParams =& $request->getAttribute('MobilizeParams');

        // Is the user trying to login from the phone info page?
        if( $request->getParameter('loginViaUserName') ) 
          //  $request->getParameter('anonPassword') )
        {
            $userModel =& $controller->getModel('User', 'User');
            // Login the user, ignore other parameters on this page
            if($userModel->loginUser(
                $request->getParameter('anonUsername'),
                $request->getParameter('anonPassword')))
            {
                // user was logged in
            }
            else
            {
                // did not log user in
                $request->setError('Login', 'Username and/or password is incorrect');
            }
        }

        if($user->isAuthenticated())
        {
            $UserCore =& $user->getAttribute('UserCore');
            if(is_object($UserCore))
            {
                $UserComm =& $UserCore->Comm();
                if(is_object($UserComm))
                {
                    $this->ToPhoneNumber = $UserComm->PhoneNumber();
                    $this->FromPhoneNumber = $this->ToPhoneNumber;
                }
            }
        }

            // The number to send to
            if( $request->hasParameter('toPhone1') &&
                $request->hasParameter('toPhone2') &&
                $request->hasParameter('toPhone3'))
            {
                $phone = $request->getParameter('toPhone1') .
                    $request->getParameter('toPhone2') .
                    $request->getParameter('toPhone3');

                if( (strlen($phone) == 10) &&
                    is_numeric($phone) )
                {
                    $this->ToPhoneNumber = $phone;
                }
                else
                {
                    $request->setError('Phone', 'Invalid phone number.');
                }
            }
            else if($request->hasParameter('toPhone'))
            {
                $phone = $request->getParameter('toPhone');
                if( (strlen($phone) == 10) && is_numeric($phone) )
                {
                    $this->ToPhoneNumber = $phone;
                }
                else
                {
                    $request->setError('Phone', 'Invalid phone number.');
                }
            }
            elseif($user->hasAttribute('LastRecipient'))
            {
                $this->ToPhoneNumber = $user->getAttribute('LastRecipient');
            }
            elseif($user->hasAttribute('PhoneNumber'))
            {
                $this->ToPhoneNumber = $user->getAttribute('PhoneNumber');
            }

            // Default sender
            if(!$user->isAuthenticated())
            {
                $this->FromPhoneNumber = $this->ToPhoneNumber;
            }

            // Phone number being sent from
            if( $request->hasParameter('fromPhone1') &&
                $request->hasParameter('fromPhone2') &&
                $request->hasParameter('fromPhone3') ) 
            {
                $phone = $request->getParameter('fromPhone1') . 
                    $request->getParameter('fromPhone2') . 
                    $request->getParameter('fromPhone3');
                if( strlen($phone) == 10 &&
                    is_numeric($phone) )
                {
                    $this->FromPhoneNumber = $phone;
                }
            }
            elseif( $request->hasParameter('fromPhone') )
            {
                $phone = $request->getParameter('fromPhone');
                if( strlen($phone) == 10 &&
                    is_numeric($phone) )
                {
                    $this->FromPhoneNumber = $phone;
                }
            }
            elseif( $user->hasAttribute('PhoneNumber') )
            {
                $this->FromPhoneNumber = $user->getAttribute('PhoneNumber');
            }

            if( $request->getParameter('fromName') )
            {
                $this->FromName = $request->getParameter('fromName');
            }
            else
            {
                $this->FromName = $this->FromPhoneNumber;
            }

            // Does the user agree to the terms and conditions
            if($request->hasParameter('agreeTC'))
            {   
                $this->AgreeTC = $request->getParameter('agreeTC');
            }   

            if($request->hasParameter('myPhone'))
            {
                if($request->getParameter('myPhone') === "yes")
                {
                    $this->MyPhone = true;
                    // only $this->PhoneNumber is defined
                    if($request->hasParameter('rememberMyPhone1'))
                    {
                        $user->setAttribute('PhoneNumber', $this->ToPhoneNumber);
                        $user->setAttribute('LastRecipient', $this->ToPhoneNumber);
                    }
                    else
                    {
                        $user->removeAttribute('PhoneNumber');
                        $user->removeAttribute('LastRecipient');
                    }
                }
                elseif($request->getParameter('myPhone') === "no")
                {
                    $this->MyPhone = false;
                    // both $this->PhoneNumber and $this->MyPhone are defined
                    if($request->hasParameter('rememberMyPhone2'))
                    {
                        $user->setAttribute('PhoneNumber', $this->FromPhoneNumber);
                        $user->setAttribute('LastRecipient', $this->ToPhoneNumber);
                    }
                    else
                    {
                        $user->removeAttribute('PhoneNumber');
                        $user->removeAttribute('LastRecipient');
                    }
                }
            }
            elseif($this->FromPhoneNumber == $this->ToPhoneNumber)
            {
                $this->MyPhone = true;
            }
        //}

        $this->Recipients = array();
        if($request->hasParameter('Recipients'))
        {
            $recipientList = explode(',', $request->getParameter('Recipients'));
            foreach($recipientList as $recipient)
            {
                if(!empty($recipient))
                {
                    $this->Recipients[] = trim($recipient);
                }
            }
        }
        if(empty($this->Recipients))
        {
            array_push($this->Recipients, $this->ToPhoneNumber);
        }

        $this->Filename = $request->getParameter('Filename');

        if( $request->hasParameter( 'Title' ) )
            $this->Title = $request->getParameter( 'Title', '' );
        else
            $this->Title = $request->getParameter( 'mobilizeTitle', '' );
        
        
        // Store the parameters for future actions
        $this->MobilizeParams->setParameter('ToPhoneNumber',
            $this->ToPhoneNumber);
        $this->MobilizeParams->setParameter('FromPhoneNumber',
            $this->FromPhoneNumber);
        $this->MobilizeParams->setParameter('FromName',
            $this->FromName);
        $this->MobilizeParams->setParameter('AgreeTC',
            $this->AgreeTC);
        $this->MobilizeParams->setParameter('Recipients',
            $this->Recipients);
        $this->MobilizeParams->setParameter('MyPhone',
            $this->MyPhone);
        $this->MobilizeParams->setParameter('Title',
            $this->Title);
        
        return TRUE;
    }

    /**
     * @brief Execute the business logic for this action
     */
    function execute(&$controller, &$request, &$user)
    {
        if( ($this->ToPhoneNumber && $this->AgreeTC ) || 
            ($user->isAuthenticated() && !$this->MobilizeParams->getParameter('IsShareable')) )
        {
            $controller->forward('OpenLocker', 'Editor');
            return VIEW_NONE;
        }

        return VIEW_INPUT;
    }


    // If there is a validation error, send the user back to the Mobilize input page 
    function handleError(&$controller, &$request, &$user)
    {
        return array('OpenLocker', 'Mobilize', VIEW_INPUT);
    }

    // Make sure that we are only forwarded to by OpenLocker::MobilizeAction
    //   This action cannot be called directly
    function validate(&$controller, &$request, &$user)
    {
        if( ( $request->getParameter('module') != 'OpenLocker' ||
              $request->getParameter('action') != 'Mobilize' 
            ) && 
            ( $controller->getRequestModule() != 'OpenLocker' &&
              $controller->getRequestAction() != 'DelegateMobilize'
            )
          )
        {
            $request->setError('Mobilize', 'Invalid API Usage');
            return false;
        }

        // ToPhoneNumber and FromPhoneNumber are validated in initialize()

        return true;
    }

    function registerValidators(&$validatorManager, &$controller, &$request,
        $user)
    {
    }

    // Any user can execute this method
    function isSecure()
    {
        return false;
    }

}

?>
