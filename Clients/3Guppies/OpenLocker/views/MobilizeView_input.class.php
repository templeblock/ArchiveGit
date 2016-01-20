<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2006 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * General Documentation Goes Here
 *
 * @author  Blake Matheny
 */

class MobilizeView extends View
{

    /**
     * @brief Render the presentation.
     *
     * @return Renderer A Renderer instance.
     */
    function & execute (&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');

        $ct = $controller->getContentType();
        switch ( $ct )
        {
            case 'html':
            case 'xhtmlMobile':
            case 'wml':
            case 'async':
                break;
            default:
                $ct = 'html';
                break;
        }

        // Figure out where we came from for omniture page and channel name
        $prev = $user->getAttribute('trackurls_prev');
        $prev = str_replace(URL_BASE . 'ringtones/', '', $prev);
        $prevArray = explode('/', $prev);
        $count    = sizeof($prevArray);
        $values = array();
        for ($i = 0; $i < $count; $i++)
        {
            if ($count > ($i + 1))
            {
                $values[$prevArray[$i]] =& $prevArray[++$i];
            }
        }

        $pageName = "Members";
        
        if( array_key_exists('module', $values) && 
            $values['module'] == "Downloads")
        {
            switch($values['action'])
            {
            case "RingtoneLanding":
                $pageName = "Ringtones";
                break;
            case "PictureLanding":
                $pageName = "Pictures";
                break;
            case "VideoLanding":
                $pageName = "Videos";
                break;
            default:
                break;
            }
        }

        $pageName = "Upload:" . $pageName;
        $prop2 = "";

        if($ct != 'async')
        {
            // Get header information for this page
            $userCore =& $user->getAttribute('UserCore');
            if(is_object($userCore))
            {
                $userId = $userCore->Uid();
            }
            else
            {
                $userId = null;
            }
            $actionChain =& new ActionChain();
            $actionChain->register('header', 'Media', 'Header');
            $request->setParameter('media_heade_userId', $userId);
            $request->setParameter('media_heade_active',
                $request->getParameter('media_heade_active'));
            $actionChain->execute($controller, $request, $user);
            $headerContents = $actionChain->fetchResult('header');
            $renderer->setAttribute('media_heade_contents', $headerContents);
        }
 
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');

        // special case where user has landed at the mobilize from external site
        // using the mobilizer widget
        if( $request->hasErrors() && $ct == 'async' )
        {
            $renderer->setTemplate("MobilizeView_error_async.html.tpl");
        }
        else if( $request->hasErrors() && $request->getParameter( 'Url' ) !== null &&
            $request->getParameter( 'extcmp' ) !== null )
        {
            $renderer->setTemplate("MobilizeView_error.$ct.tpl");
        }
        else
        {
            $renderer->setTemplate("MobilizeView_input.$ct.tpl");
        }
        
        if ( $request->hasError('Mobilize') )
            $renderer->setAttribute('MobilizeError', $request->getError('Mobilize'));
        elseif ( $request->hasAttribute('MobilizeSuccess') )
            $renderer->setAttribute('MobilizeSuccess',true);

        if($request->hasParameter('success'))
        {
            $renderer->setAttribute('MobilizeError', 'You have successfully mobilized the selected media');
        }

        if($user->hasPrivilege('Anonymous User'))
        {
            $renderer->setAttribute('IsAnonymous', true);
        }

        // this error is part of the mobilization process
        if( $request->hasError( 'Mobilize' ) )
        {
            $error = $request->getError( 'Mobilize' );
            
            if( strpos( $error, 'do not support your carrier' ) )
            {
                $renderer->setAttribute('OmniturePageName', 
                    'Mobilization:Error:Carrier Not Supported Final');
                $renderer->setAttribute('OmnitureChannel', 'Mobilization');
                $renderer->setAttribute('OmnitureProp2', $prop2);
            }
            else if( strpos( $error, 'determine generic file type' ) )
            {
                $renderer->setAttribute('OmniturePageName', 
                    'Mobilization:Error:File Type Not Supported');
                $renderer->setAttribute('OmnitureChannel', 'Mobilization');
                $renderer->setAttribute('OmnitureProp2', $prop2);
            }
        }
        else
        {
            $renderer->setAttribute('OmniturePageName', $pageName);
            $renderer->setAttribute('OmnitureChannel', 'Upload');
            $renderer->setAttribute('OmnitureProp2', $prop2);
        }
        
        return $renderer;
    }

}

?>
