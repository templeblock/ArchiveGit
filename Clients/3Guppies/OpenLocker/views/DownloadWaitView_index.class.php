<?php

class DownloadWaitView extends View
{
    function execute(&$controller, &$request, &$user)
    {
        $renderer =& $request->getAttribute('SmartyRenderer');
        $renderer->setTemplateDir($controller->getModuleDir() . 'templates/');
        $ct = $controller->getContentType();
        switch($ct)
        {
        case 'html':
        case 'wml':
        case 'xhtmlMobile':
            break;
        default:
            $ct = 'html';
            break;
        }
        $renderer->setTemplate("DownloadWait.$ct.tpl");

        $refreshUrl = URL_BASE . 't/' . $request->getAttribute('Tiny');

        // automatically refresh the page every 2 seconds checking
        //   for transcoding completion
        if($ct == 'xhtmlMobile')
        {
            $controller->redirect($refreshUrl, 4);
        }

        $jobId = $request->getAttribute('JobId');
        $retextUrl = $controller->genUrl(array(
            'module'    =>  'OpenLocker',
            'action'    =>  'DownloadRenotify')) . '?JobId=' . $jobId;

        $renderer->setAttribute('RefreshUrl', $refreshUrl);
        $renderer->setAttribute('RetextUrl', $retextUrl);

        $data = $request->getAttribute('OpenLocker.Download.Data');
        $sendEventId = array_key_exists('SendEventId', $data) ? $data['SendEventId'] : 0;

        $BrowserInfo =& $user->getAttribute('BrowserInfo');
        $deviceId = 0;
        if( is_object($BrowserInfo) && $BrowserInfo->isValid() )
        {
            $deviceId = $BrowserInfo->getDeviceId();
        }

        $Event =& new MobilizeDownloadEvent(null,Storage::getMessagingDb($request));
        $Event->SendEventId($sendEventId);
        $Event->Status(2);
        $Event->DeviceId($deviceId);
        $Event->Attempt(1);

        // Increments the download attempt count if it already exists
        $Event->AddOrCommit();

        return $renderer;
    }
}

?>
