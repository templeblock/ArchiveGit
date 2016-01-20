<?php

require_once("UrlUtil.class.php");

/**
 * @brief Base class that will transform a url into an actual media url
 *
 * @note UrlFilter performs a null operation on the input url (no change)
 *
 * @note All classes that extend UrlFilter must exist in the file:
 *   [hostname with '.'s replaced by '_'s]UrlFilter.class.php 
 *      and be named:
 *   [hostname with '.'s replaced by '_'s]UrlFilter
 *
 * @author Jeff Ching
 *
 * @date 2006-11-14
 */
class UrlFilter
{

    /**
     * @brief string - stores the original url (unmodified throughout the 
     *   request)
     *
     * @private
     */
    var $_original_url = null;

    /**
     * @brief string - stores the filtered url
     *
     * @note This field is not filled until the first request for getFilteredUrl
     *   and is not recalculated unless the original url is changed
     *
     * @private
     */
    var $_filtered_url = null;

    /**
     * @brief array(string => string) - stores any error messages that may have
     *   occurred during url filtering
     *
     * @private
     */
    var $_errors = array();

    /**
     * Constructor
     *
     * @brief Creates the filter object
     *
     * @param $_url - string - the original url to filter
     *
     * @public
     */
    function UrlFilter($_url = null)
    {
        $this->_original_url = $_url;
    }

    /**
     * @brief The business logic of the url filter.  Override this function
     *   in any child classes
     *
     * @return string - the filtered url on success,
     *         null if the execution failed
     *
     * @public
     */
    function execute()
    {
        $this->_filtered_url = $this->_original_url;
        return $this->_filtered_url;
    }

    /**
     * @brief Gets and instance of a UrlFilter object.  Determines if a
     *   specialized handler exists (based on url) and returns an instance
     *   of the specialized filter if it does exist
     *
     * @param $_url - string - the original url
     *
     * @return UrlHandler object
     */
    function getInstance($_url)
    {
        $urlHost = UrlUtil::getHost($_url);
        $className = str_replace(".", "_", $urlHost) . "UrlFilter";
        $fileName = HOME_BASE . "modules/OpenLocker/lib/" . $className . ".class.php";
        // Look for a specialized handler
        if(file_exists($fileName))
        {
            require_once($className . ".class.php");
            return new $className($_url);
        }

        // No specialized handler found, return the default filter
        return new UrlFilter($_url);
    }

    /**
     * @brief Logic to decide if this filter can take action on the given url
     *
     * @param $_url - string - the url to check compatibility
     *
     * @note if $_url is left null, then $this->_original_url should be used
     *
     * @note Override this function in any child classes
     *
     * @return bool -   true, if this filter is compatible with the url,
     *                  false, otherwise
     *
     * @public
     */
    function isCompatible($_url = null)
    {
        return true;
    }

    /**
     * @brief Accessor for the filtered url.  Calls execute if the filtered
     *   url has not been set yet
     *
     * @return string - the filtered url
     *
     * @public
     */
    function getFilteredUrl()
    {
        if($this->_filtered_url === null)
        {
            $this->execute();
        }
        return $this->_filtered_url;
    }

    /**
     * @brief Settor for the original url.  Resets the filtered url so it is
     *   recalculated on the next getFilteredUrl()
     *
     * @param $_url - string - the new original url
     *
     * @public
     */
    function setOriginalUrl($_url)
    {
        $this->_original_url = $_url;
        $this->_filtered_url = null;
    }

    /**
     * @brief Accessor for the original url
     *
     * @return string - the original url
     *
     * @public
     */
    function getOriginalUrl()
    {
        return $this->_original_url;
    }

    /**
     * @brief Checks the expected contentType agains what the filtered url
     *   is pointing to
     *
     * @param $contentType - string
     *
     * @return bool - whether we matched the content type with the header 
     *   returned by fetching the filtered url
     */
    function verifyContentType($contentType = null)
    {
        if($contentType == null)
        {
            return false;
        }

        $headerInfo = UrlUtil::getHeader($this->getFilteredUrl());
        if(array_key_exists('Content-Type', $headerInfo) &&
            $headerInfo['Content-Type'] == $contentType)
        {
            return true;
        }
        return false;
    }

    /**
     * @brief Parses a url and returns an array of key => value pairs for
     *   the parameters of the url query
     *
     * @param $_url - string - the url to parse
     *
     * @return array( [param_name] => [param_value] )
     *
     * @public
     */
    function getParameters($_url)
    {
        $query = @parse_url($_url);
        $params = array();
        if(array_key_exists('query', $query))
        {
            $query = $query['query'];
            $queryParams = explode("&", $query);

            foreach($queryParams as $queryParam)
            {
                $paramValues = explode("=", $queryParam);
                $paramName = array_shift($paramValues);
                if(sizeof($paramValues) > 0)
                {
                    $paramValue = array_shift($paramValues);
                }
                else
                {
                    $paramValue = "";
                }
                $params[$paramName] = $paramValue;
            }
        }

        return $params;
    }

    /**
     * @brief Tells whether this UrlFilter has experienced an errors during
     *   a previous execution
     *
     * @return boolean
     *
     * @public
     */
    function hasErrors()
    {
        return sizeof($this->_errors) > 0;
    }

    /**
     * @brief Tells whether this UrlFilter has experienced an specified error
     *   during a previous execution
     *
     * @param $key - string - the error to check for
     *
     * @return boolean
     *
     * @public
     */
    function hasError($key)
    {
        return is_null($this->getError($key)) ? false : true;
    }

    /**
     * @brief Return an array of all the errors that occured during execution
     *
     * @return array(string => string)
     *
     * @public
     */
    function getErrors()
    {
        return $this->_errors;
    }

    /**
     * @brief Return the error message for a specific error
     *
     * @param $key - string - which error to return the message for
     *
     * @return null if no error exists, string - message if the error exists
     *
     * @public
     */
    function getError($key)
    {
        if(array_key_exists($key, $this->_errors))
        {
            return $this->_errors[$key];
        }
        return null;
    }

    /**
     * @brief Sets an error message
     *
     * @param $key - string - The type of error that occurred
     *
     * @param $message - string - Description of the error that occurred
     *
     * @public
     */
    function setError($key, $message)
    {
        $this->_errors[$key] = $message;
    }
}

?>
