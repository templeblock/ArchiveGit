<?php

require_once("UrlFilter.class.php");
require_once("UrlUtil.class.php");

/**
 * @brief Class that converts YouTube.com media handles and follows links to
 *   provide a direct link to the media so it can be easily mobilized
 *
 * @note There are certain videos that link to 
 *   http://youtube.com/get_video?video_id=[video_id] which are dead-ends
 *   This appears to be a temporal problem as other requests for the same
 *   urls return http://[server].youtube.com/get_video?video_id=[video_id].
 *
 *   If we get one of the problematic urls, we try to filter it again and look
 *   for a valid server.
 *
 * @author Jeff Ching
 *
 * @date 2006-11-14
 */
class youtube_comUrlFilter extends UrlFilter
{
    /**
     * @brief Overrides the generic url filter to follow redirects for YouTube
     *    content
     *
     * @public
     */
    function execute($tries = 20)
    {
        // Default url returned if we do not recognize the url format
        $this->_filtered_url = $this->_original_url;

        // Find the starting point to jump into the redirection flow
        $starting_stage = $this->_detectStage($this->_original_url);
        if($starting_stage >= 0)
        {
            switch($starting_stage)
            {
            case 0:
                $this->_filtered_url = $this->_cthruToFlv($this->_original_url);
                break;
            case 1:
                $this->_filtered_url = $this->_watchToFlv($this->_original_url);
                break;
            case 2:
                $this->_filtered_url = $this->_vToFlv($this->_original_url);
                break;
            case 3:
                $this->_filtered_url = $this->_swfToFlv($this->_original_url);
                break;
            case 4:
                $this->_filtered_url = $this->_getVideoToFlv($this->_original_url);
                break;
            case 5:
                $this->_filtered_url = $this->_original_url;
                break;
            default:
            }
        }

        // Is the result acceptable -> check for the temporal error
        if($this->_isBadFinishUrl($this->_filtered_url))
        {
            // Increment the error counts and store the bad urls 
            if($this->hasError('badUrlCount'))
            {
                $this->setError('badUrlCount', $this->getError('badUrlCount') + 1);
                $badUrls = $this->getError('badUrls');
                array_push($badUrls, $this->_filtered_url);
                $this->setError('badUrls', $badUrls);
            }
            else
            {
                $this->setError('badUrlCount', 1);
                $this->setError('badUrls', array($this->_filtered_url));
            }

            // we are at the try limit
            if($tries <= 1)
            {
                // Log in the instrumentation logger
                trigger_error("Reached maximum retries for youtube", E_USER_NOTICE);
                return $this->_filtered_url;
            }

            // try again from the start
            return $this->execute($tries - 1);
        }

        return $this->_filtered_url;

    }

    /**
     * @brief Identifies what kind of YouTube link and where to start processing
     *
     * @note    0: /cthru? page
     *          1: /watch? page
     *          2: /v/ page
     *          3: /get_video? page
     *          4: /.../...flv (Direct link)
     *
     * @param $_url - string - the YouTube URL
     *
     * @return int representing the stage we start at. -1 if we don't
     *   recognize the format
     *
     * @private
     */
    function _detectStage($_url)
    {
        $stages = array(0   =>  '#^(https?://)?(www\.)?youtube.com/cthru\?.*$#i',
                        1   =>  '#^(https?://)?(www\.)?youtube.com/watch\?.*$#i',
                        2   =>  '#^(https?://)?(www\.)?youtube.com/v/.*$#i',
                        3   =>  '#^(https?://)?(www\.)?youtube.com/[a-zA-Z0-9]+\.swf\?.*$#i',
                        4   =>  '#^(https?://)?([^/]*)?youtube.com/get_video\?.*#i',
                        5   =>  '#^(https?://)?.*\.flv#i');
        foreach($stages as $stage => $regexp)
        {
            if(preg_match($regexp, $_url) > 0)
            {
                return $stage;
            }
        }
        return -1;
    }

    /**
     * @brief Converts a url from the form youtube.com/cthru?... to the flv
     *   location
     *
     * @param $_url - string
     *
     * @return string - flv location
     *
     * @private
     */
    function _cthruToFlv($_url)
    {
        $vPage = UrlUtil::getForwardPage($_url);
        return $this->_watchToFlv($vPage);
    }

    /**
     * @brief Converts a url from the form youtube.com/watch?... to the flv
     *   location
     *
     * @param $_url - string
     *
     * @return string - flv location
     *
     * @private
     */
    function _watchToFlv($_url)
    {
        // Try the /v/[videoid] http forward
        $watchParams = $this->getParameters($_url);
        if(isset($watchParams['v']))
        {
            $vPage = "http://youtube.com/v/" . $watchParams['v'];
        }
        else
        {
            $vPage = $_url;
        }
        $swfPage = UrlUtil::getForwardPage($vPage);
        if($swfPage != $vPage)
        {
            // If there is a http redirect, use this shortcut
            return $this->_swfToFlv($swfPage);
        }

        // Otherwise, scan the watch page for the flash player
        $contents = UrlUtil::getPage($_url);
        $swfRegex = "#/[a-zA-Z0-9]+\.swf\?video_id=([^&]+)&.*t=([a-zA-Z0-9\-]+)#i";
        $matches = array();
        preg_match($swfRegex, $contents, $matches);
        if(sizeof($matches) > 0)
        {
            return $this->_swfToFlv("http://youtube.com" . $matches[0]);
        }

        $this->setError('watch', "Could not find the flash player on watch page and /v/[videoid] shortcut failed - url: $url");
        return $_url;
    }

    /**
     * @brief Converts a url from the form youtube.com/v/... to the flv
     *   location
     *
     * @param $_url - string
     *
     * @return string - flv location
     *
     * @private
     */
    function _vToFlv($_url)
    {
        $swfPage = UrlUtil::getForwardPage($_url);
        return $this->_swfToFlv($swfPage);
    }

    /**
     * @brief Converts a url from the form youtube.com/p.swf?... to the flv
     *   location
     *
     * @param $_url - string
     *
     * @return string - flv location
     *
     * @private
     */
    function _swfToFlv($_url)
    {
        $swfParams = $this->getParameters($_url);
        if(isset($swfParams['video_id']) && isset($swfParams['t']))
        {
            $getVideoPage = "http://youtube.com/get_video?video_id=" . 
                //$swfParams['video_id'] . "&l=60&t=" . $swfParams['t'];
                $swfParams['video_id'] . "&t=" . $swfParams['t'];
        }
        else
        {
            $this->setError('swf', "Invalid parameters for _swfToFlv url: $_url");
            $getVideoPage = $_url;
        }
        return $this->_getVideoToFlv($getVideoPage);
    }

    /**
     * @brief Converts a url from the form youtube.com/get_video?... to the flv
     *   location
     *
     * @param $_url - string
     *
     * @return string - flv location
     *
     * @private
     */
    function _getVideoToFlv($_url, $_tries = 3)
    {
        $flvPage = UrlUtil::getForwardPage($_url);
        return $flvPage;
    }

    /**
     * @brief Overrides the UrlFilter::isCompatible
     *
     * @note currently is not used
     *
     * @param $_url - string
     *
     * @return bool, true if we have a YouTube video handle,
     *               false otherwise
     */
    function isCompatible($_url = null)
    {
        return $this->_detectStage($_url) >= 0;
    }

    /**
     * @brief Detects an invalid finish url
     *
     * @param $_url - string
     *
     * @return boolean
     *  
     * @private
     */
    function _isBadFinishUrl($_url)
    {
        $problemRegex = "#^http://youtube\.com/get_video\?video_id=.*$#i";
        $matches = array();
        preg_match($problemRegex, $this->_filtered_url, $matches);
        if(sizeof($matches) > 0)
        {
            return true;
        }
        return false;
    }
}

?>
