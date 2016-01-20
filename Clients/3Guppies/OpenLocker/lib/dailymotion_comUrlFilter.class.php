<?php

require_once("UrlFilter.class.php");
require_once("UrlUtil.class.php");

/**
 * @brief Class that converts dailymotion.comm media handles and follows links
 *   to provide a direct link to the media so it can be easily mobilized
 *
 * @author Jeff Ching
 *
 * @date 2006-11-28
 */
class dailymotion_comUrlFilter extends UrlFilter
{
    var $_embed_regex = '#addVariable\(["\']url["\'],\s*["\']([^"\']+)["\']#i';

    /**
     * @brief Overrides the generic url filter to follow specific redirects for
     *   dailymotion content
     *
     * @public
     */
    function execute()
    {
        // Default url returned if we do not recognize the url format
        $this->_filtered_url = $this->_original_url;

        $stage = $this->_detectStage($this->_original_url);
        if($stage >= 0)
        {
            switch($stage)
            {
            case 2:
                $this->_filtered_url = $this->_swfToFlv($this->_original_url);
                break;
            case 1:
                $this->_filtered_url = $this->_embedToFlv($this->_original_url);
                break;
            default:
                $this->_filtered_url = $this->_scanPageForPlayer($this->_original_url);
                break;
            }
        }

        return $this->_filtered_url;
    }

    /**
     * @brief Takes a url for a webpage, looks for a flash video player and 
     *   returns the link to that video
     *
     * @param $_url - string
     *
     * @return string - flv location
     *
     * @private
     */
    function _scanPageForPlayer($_url)
    {
        // Make sure we don't do unecessary fetching for a page
        //   that doesn't apply
        $content_type = UrlUtil::getContentType($_url);
        $pos = strpos($content_type, "text/html");

        if($pos === false)
        {
            // this link is not a html page -> don't fetch it
        }
        else
        {
            $contents = UrlUtil::getPage($_url);
            $matches = array();
            preg_match($this->_embed_regex, $contents, $matches);

            // Found the flash player on the page
            if(sizeof($matches) > 0)
            {
                $_url = urldecode($matches[1]);
                
                // Recursively follow forward links or back out after
                //   following $maxForwards forwards
                $maxForwards = 10;
                $headerInfo = UrlUtil::getHeader($_url);
                while(array_key_exists("Location", $headerInfo) &&
                    $maxForwards > 0)
                {
                    $_url = $headerInfo['Location'];
                    $headerInfo = UrlUtil::getHeader($_url);
                    $maxForwards--;
                }
            }
        }
        return $_url;
    }

    /**
     * @brief Internal helper function that takes a embed url and converts
     *   to the flv file
     *
     * @param $_url - string
     *
     * @return string - flv url
     *
     * @private
     */
    function _embedToFlv($_url)
    {
        // expecting http://www.dailymotion.com/swf/[videoid]
        $swfUrl = UrlUtil::getForwardPage($_url);
        return $this->_swfToFlv($swfUrl);
    }

    /**
     * @brief Internal helper function that takes a url to the flashplayer
     *   with all its parameters inline
     *
     * @param $_url - string
     *
     * @return string - flv url
     *
     * @private
     */
    function _swfToFlv($_url)
    {
        // expecting http://www.dailymotion.com/flash/flvplayer.swf?rev=[#]&
        //   url=[url]&url8=[url8]&previewURL=[prevUrl]&autoStart=[#]&
        //   playerUrl=[playerUrl]
        $params = $this->getParameters($_url);
        $_url = urldecode($params['url']);
        $_url = UrlUtil::getForwardPage($_url);
        return $_url;
    }

    /**
     * @brief Determines what kind of dailymotion url this is
     *
     * @param $_url - string
     *
     * @return int - stage level (defaults to 0 - scan page for flash player)
     *
     * @private
     */
    function _detectStage($_url)
    {
        $stageRegex = array(
            2   =>  '#http://(www\.)?dailymotion\.com/flash/flvplayer\.swf?(.*)?url=(.*)#i',
            1   =>  '#http://(www\.)?dailymotion\.com/swf/(.*)#i',
            0   =>  '#http://(www\.)?dailymotion\.com(.*)?#i',
            );
        foreach($stageRegex as $stage => $regex)
        {
            $matches = array();
            preg_match($regex, $_url, $matches);
            if(sizeof($matches) > 0)
            {
                return $stage;
            }
        }

        return 0;
    }
}

?>

