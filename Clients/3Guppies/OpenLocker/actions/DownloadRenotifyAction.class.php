<?php

class DownloadRenotifyAction extends Action
{
    var $JobId;

    function validate(&$controller, &$request, &$user)
    {
        if( !$request->hasParameter('JobId') )
        {
            $request->setError('Renotify', 'No job specified');
            return false;
        }
        $this->JobId = $request->getParameter('JobId');

        return true;
    }

    function execute(&$controller, &$request, &$user)
    {
        $curl = new CURL();
        global $_LIVE_SERVERS;
        $curl->setCurlOptTimeout($_LIVE_SERVERS['curl_opt_timeout']);
        $curl->setCurlOptConnectionTimeout($_LIVE_SERVERS['curl_connect_timeout']);
        $curl->returnHeaders(false);

        $interceptUrl = $_LIVE_SERVERS['transcode_intercept'] . '?SendNotice=1&job_id=' . $this->JobId;

        $response = trim($curl->get($interceptUrl));

        switch($response)
        {
        case 'sent notice':
        case 'set send notice':
        case 'nothing to do':
            return VIEW_SUCCESS;
            break;

        case 'error sending notice':
        case 'error setting send notice':
        case 'no job id':
        default:
            $request->setError('Renotify', 'Bad response from transcode server');
            return VIEW_ERROR;
            break;
        }

        // should not get here
        return VIEW_ERROR;
    }

}

?>
