<?php

require_once("UrlFilter.class.php");
require_once("UrlUtil.class.php");

/**
 * @brief Class that converts GoogleVideo media handles and follows links
 *   to provide a direct link to the media so it can be easily mobilized
 *
 * @author Jeff Ching
 *
 * @date 2006-11-28
 */
class video_google_comUrlFilter extends UrlFilter
{
    /**
     * @brief The regular expression for the embedded google flash player
     *   within a page
     *
     * @private
     */
    var $_embed_regex = "#/googleplayer.swf\?&?videoUrl\\\\u003d([^\\\\]*)\\\\u003d#i";

    /**
     * @brief Overrides the generic url filter to follow redirects for 
     *   GoogleVideo content
     *
     * @public
     */
    function execute()
    {
        $this->_filtered_url = $this->_original_url;

        $stage = $this->_detectStage($this->_original_url);
        switch($stage)
        {
        case 1:
            $this->_filtered_url = $this->_swfToFlv($this->_original_url);
            break;
        default:
            $this->_filtered_url = $this->_scanPageForPlayer($this->_original_url);
            break;
        }

        return $this->_filtered_url;
    }

    /**
     * @brief Internal function to determine where to start processing the
     *   url
     *
     * @param $_url - string
     *
     * @return int - starting stage, defaults to 0 (scan page for flash
     *   player)
     *
     * @private
     */
    function _detectStage($_url)
    {
        $stages = array(
            1   =>  '#/googleplayer.swf\?&?docId=[^&]+#i'
            );

        foreach($stages as $stage => $regex)
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

    /**
     * @brief Converts an embeded flashplayer url (from myspace created code)
     *   into the link to the flv file
     *
     * @param $_url - string
     *
     * @return string - flv url
     *
     * @private
     */
    function _swfToFlv($_url)
    {
        // expecting /googleplayer.swf?docId=[docid]
        $params = $this->getParameters($_url);
        $page = "http://video.google.com/videoplay?docid=" . $params['docId'] . "&hl=en";
        return $this->_scanPageForPlayer($page);
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
}

?>
