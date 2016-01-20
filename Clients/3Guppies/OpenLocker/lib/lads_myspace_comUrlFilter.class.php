<?php

require_once("UrlFilter.class.php");
require_once("UrlUtil.class.php");

/**
 * @brief Class that converts myspace.com media handles to provide a
 * direct link to the media so it can be easily mobilized
 *
 * @author David Bartosh and Eric Malone
 *
 * @date 2007-04-04
 */

class lads_myspace_comUrlFilter extends UrlFilter
{
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
        $results = array();
        if ( preg_match( '#m=([\d]+)#', $this->_filtered_url, $results ) )
        {
            $mediaid = $results[1];
            $mediaidrev = strrev( $mediaid );
            $mediapath = str_pad( substr( $mediaid, 0, strlen( $mediaid ) - 5 ), 7, "0", STR_PAD_LEFT );
            $mediapath .=   "/" . 
                            $mediaidrev{0} .
                            $mediaidrev{1} .
                            "/" . 
                            $mediaidrev{2} .
                            $mediaidrev{3} .
                            "/" . $mediaid .
                            ".flv";
            $this->_filtered_url = "http://content.movies.myspace.com/" . $mediapath;
        }
        return $this->_filtered_url;
    }
}

?>

