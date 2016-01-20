<?php

/**
 * @brief Static utility wrapper class for curl to allow remote fetching of
 *   pages/resources from any website
 *
 * @author Jeff Ching
 *
 * @date 2006-11-16
 */
class UrlUtil 
{
    /**
     * @brief Static interface to getPage()
     *
     * @param $host - string - the host name (i.e http://[hostname]/index.php)
     *
     * @param $page - string - the page name (i.e http://mixxer.com/[pagename])
     *
     * @param $getHeader - [0,1] - whether or not to return the header with
     *   the page body
     *
     * @public
     */
    function getPageByHostAndPage($host, $page, $getHeader = 0)
    {
        return getPage("http://" . $host . $page, $getHeader);
    }

    /**
     * @brief Static function to fetch the contents of the given url, using curl
     *
     * @param $_url - string - the url of the page to fetch
     *
     * @param $getHeader - [0,1] - whether or not to return the header with
     *   the page body
     *
     * @return string - contents of the page
     *
     * @public
     */
    function getPage($_url, $getHeader = 0, $ignoreBody = 0)
    {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $_url);
        curl_setopt($ch, CURLOPT_USERAGENT, 'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1) Gecko/20061010 Firefox/2.0');
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_HEADER, $getHeader);
        curl_setopt($ch, CURLOPT_NOBODY, $ignoreBody);

        $contents = curl_exec($ch);

        curl_close($ch);

        return $contents;
    }

    /**
     * @brief Static funtion that gets the next url forwarded to (via http 
     *   redirect header) by following the given url
     *
     * @param $_url - string - the url to follow
     *
     * @return  original url if no redirect header, 
     *          new url otherwise
     *
     * @public
     */
    function getForwardPage($_url)
    {
        $headerValues = UrlUtil::getHeader($_url);
        if(array_key_exists("Location", $headerValues))
        {
            return $headerValues['Location'];
        }
        return $_url;
    }

    /**
     * @brief Static function that gets the content type of the file that the
     *   given url points to
     *
     * @param $_url - string
     *
     * @return string content type if it is set
     *         null otherwise
     *
     * @public
     */
    function getContentType($_url)
    {
        $headerValues = UrlUtil::getHeader($_url);
        if(array_key_exists('Content-Type', $headerValues))
        {
            return $headerValues['Content-Type'];
        }
        return null;
    }

    /**
     * @brief Static function that gets only the header's contents
     *
     * @param $_url - string
     *
     * @return array([header_param] => [header_value])
     *
     * @public
     */
    function getHeader($_url)
    {
        return UrlUtil::parseHeader(UrlUtil::getHeaderString($_url));
    }

    /**
     * @brief Static function that gets only the header's contents
     *
     * @param $_url - string
     *
     * @return string the header contents
     *
     * @public
     */
    function getHeaderString($_url)
    {
        return UrlUtil::getPage($_url, 1, 1);
    }

    /**
     * @brief Static function that parses the header string into an
     *   associative array of key => value pairs
     *
     * @param $_header_string - string - the header
     *
     * @return array([header_param] => [header_value])
     *
     * @public
     */
    function parseHeader($_header_string)
    {
        $result = array();
        $lines = explode("\n", $_header_string);
        foreach($lines as $line)
        {
            $pos = strpos($line, ":");
            if($pos !== false)
            {
                $key = substr($line, 0, $pos);
                $value = trim(substr($line, $pos + 1));
                $result[$key] = $value;
            }
        }
        return $result;
    }

    /**
     * @brief Returns the host name from a given url even if no protocol is set
     *
     * @param $_url - string - the url to parse
     *
     * @note If no protocol is set in the url, we add http:// and try again
     *
     * @return string the host name
     *
     * @public
     */
    function getHost($_url)
    {
        $urlInfo = @parse_url($_url);

        if(!isset($urlInfo['host']))
        {
            $urlInfo = @parse_url("http://" . $_url);
        }
        $host = str_replace("www.", "", $urlInfo['host']);
        return $host;
    }

}


?>
