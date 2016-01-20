<?

// A content handler handles content in a very generic way, which isn't specific
// to a mime type. The media handler deals with mime types.

require_once(dirname(__FILE__) . '/' . 'MediaHandler.class.php');

class ContentHandler
{
    // Controller object
    var     $controller     =   null;

    // Object in this class, string in child classes
    var     $handler        =   null;

    // Request specified content
    var     $content        =   null;

    // String containing an error
    var     $error          =   null;

    // non media specific file info; path, size, etc
    // $fileInfo['Handler'] might be VIdeoHandler, PictureHandler, MusicHandler,
    var     $fileInfo       =   null;

    var $mediaHandler       =   null;

    // boolean reflecting whether the end user should be receiving a notification message upon delivery of content
    var $sendNotice         =   null;

    // MUST OVERRIDE IN CHILD
    function ContentHandler(&$controller, $handler = null, $content = null)
    {
        $this->controller =& $controller;
        $this->content =& $content;
        $this->setHandler($handler);
    }

    function setFileInfo(&$fileInfo)
    {
        $this->fileInfo =& $fileInfo;
    }

    // CAN NOT BE CALLED BY CHILD
    function setHandler($handler)
    {
        $class = sprintf('%s%s', $handler, 'ContentHandler');
        if ( $this->requireHandler($handler) )
        {
            $c = new $class($this->controller, $handler, $this->content);
            $c->setFileInfo($this->fileInfo);
            $this->handler =& $c;
        }
        else
        {
            $this->setError('Non-existant content handler '.$handler);
        }
    }

    // given a file, return the full path to that file. False if a problem.
    function getFullPath($file)
    {
        die("Must be overridden by child");
    }

    // return TRUE if private, FALSE if public. Override if public.
    function getScope()
    {
        return TRUE;
    }

    function requireHandler($handler)
    {
        $class = sprintf('%s%s', $handler, 'ContentHandler');
        $file = sprintf('%s/%s%s', dirname(__FILE__), $class, '.class.php');
        if ( file_exists($file) )
        {
            require_once($file);
            return TRUE;
        }
        else
            return FALSE;
    }

    function setError($error)
    {
        $this->error = $error;
    }

    // CAN NOT BE CALLED BY CHILD
    function getError()
    {
        if ( !is_object($this->handler) )
            return $this->error;
        else
            return $this->handler->error;
    }

    function validate()
    {
        if ( is_object($this->handler) ) {
            return $this->handler->validate();
        }
        else
            return FALSE;
    }

    // retrieve all needed info: file type, size, path, etc
    function getContentData()
    {
        if ( is_object($this->handler) )
        {
            $cd =& $this->handler->getContentData();
            if (NULL !== $cd)
               $cd['SendNotice'] = $this->getNotification() ? 1:0;
            return $cd;
        }
        else
            return NULL;
    }

    // is an editor available for the media type?
    function hasEditor()
    {
        if ( is_object($this->handler->mediaHandler) )
            return $this->handler->mediaHandler->hasEditor();
        else
            return FALSE;
    }

    // what module/action/view handles editing?
    function getEditor()
    {
        if ( is_object($this->handler->mediaHandler) )
            return $this->handler->mediaHandler->getEditor();
        else
            return array();
    }

    // is delivery of this content intended to trigger a notification message to the end user?
    function getNotification()
    {
       return $this->sendNotice;
    }

    // set field which drives content delivery notification message to the end user
    function setNotification($sendNotice = null)
    {
       $this->sendNotice = $sendNotice;
    }

    function storeAndConvert($_options = array(), $post_vars = array())
    {
        $controller =& Controller::getInstance();
        $user =& $controller->getUser();
        $model =& $controller->getModel('OpenLocker', 'OpenLocker');

        $userId = 1; // default for anonymous
        if($user->isAuthenticated())
        {
            $UserCore =& $user->getAttribute('UserCore');
            if(is_object($UserCore))
            {
                $userId = $UserCore->Uid();
            }
        }

        $this->fileInfo['Converted'] = basename($this->fileInfo['Path']) . '?';

        $personalMediaId = $model->storePersonalMedia($userId, $this->fileInfo);
        $post_vars['UserId'] = $userId;
        $post_vars['PersonalId'] = $personalMediaId;

        if($this->convertContent($_options, $post_vars))
        {
            $this->fileInfo['UserId'] = $userId;
            return $personalMediaId;
        }
        else
        {
            return false;
        }
    }

    // convert the content for my phone
    function convertContent($_options = array(), $post_vars = array(), $_isMyPhone = true)
    {
        if ( is_object($this->handler->mediaHandler) )
        {
            $converted = $this->handler->mediaHandler->convertContent(
                $_options, $post_vars, $_isMyPhone);
            return $converted;
        }
        else
            return false;
    }

    /**
     * @brief Tells whether the converted file name already existed
     *
     * @return boolean
     */
    function convertedFileAlreadyExisted()
    {
        if ( is_object($this->handler->mediaHandler) )
        {
            return $this->handler->mediaHandler->convertedFileAlreadyExisted();
        }
        else
        {
            return false;
        }
    }

    // store the content on the file system and in the database
    function storeContent($userId = null)
    {
        if ( $this->fileInfo['Personal'] )
        {
            return $this->storePersonalContent($userId);
        }
        else
        {
            return $this->storeLockerContent();
        }
    }

    function storePersonalContent($userId = null)
    {
        $controller =& Controller::getInstance();
        $user =& $controller->getUser();
        $request =& $controller->getRequest();

        $locker_db =& Storage::GetLockerDb($request);
        $UserCore =& $user->getAttribute('UserCore');

        if(!is_null($userId))
        {
            
        }
        elseif(is_object($UserCore))
        {
            $userId = $UserCore->Uid();
        }
        else
        {
            // Anonymous
            $userId = 1;
        }

        if ( !is_object($locker_db) )
        {
            $this->setError('Error connecting to database.');
            return FALSE;
        }

        $convertedLocation = MediaConverter::getFullPath($this->fileInfo['Converted'], true);

        $PersonalMedia = new PersonalMedia(null, $locker_db);
        $PersonalMedia->UserId($userId);
        $PersonalMedia->Location($this->fileInfo['Converted']);
        $PersonalMedia->DateAdded(date("Y-m-d H:i:s",time()));
        $PersonalMedia->FileSize(
                file_exists($convertedLocation) ?
                filesize($convertedLocation) : 0);

        // If the userid/converted location is already in the locker, do not add it again
        if($PersonalMedia->isInLocker())
        {
            $PersonalMedia->AccessLevel(1);
            $PersonalMedia->Commit();
            return $PersonalMedia->PersonalMediaId();
        }

        $convertedPath = MediaConverter::getFullPath($this->fileInfo['Converted'],true);

        $PersonalMedia->PersonalMediaId(0);
        $PersonalMedia->Name($this->fileInfo['Filename']);
        $PersonalMedia->Type($this->fileInfo['ConvertedTo']);
        $PersonalMedia->AccessLevel(1);
        $PersonalMedia->Original($this->fileInfo['Original']);

        if(!$PersonalMedia->Add())
        {
            $this->setError('There was an error storing your file.');
            return FALSE;
        }

        //GET INSERT ID FOR FORWARD
        $insertId = $PersonalMedia->LastInsertId();

        $originalPath = MediaConverter::getFullPath($this->fileInfo['Original'], true);

        //ADD FILE INSERT TO HISTORY OF ORIGINAL FILES
        $files = new PersonalFiles (null, $locker_db);
        $files->Location($this->fileInfo['Original']);
        $files->Size(file_exists($originalPath) ? filesize($originalPath) : 0);
        $files->Type($this->fileInfo['CapabilityId']);
        $files->DateCreated(date("Y-m-d H:i:s",time()));
        $files->Add();

        return $insertId;
    }

    function storeLockerContent()
    {
        // See note at the top of GlobalMediaContentHandler.class.php
        return TRUE;
    }

    /**
     * @brief Wrapper to fetch the media type of files
     */
    function _getMediaType($path,$filename)
    {
        $request = $this->controller->getRequest();
        $MediaConverter = new MediaConverter(null, $request->getAttribute('read_db'));
        $mt = $MediaConverter->getMediaType($path,$filename,true,true);

        if(!$mt)
        {
            return null;
        }

        // Animated gif fix (check to see if the gif is animated using identify)
        if(array_key_exists(32,$mt))
        {
            $output = `identify $path`;
            if(sizeof(explode("\n", $output)) > 2)
            {
                return $mt[32];
            }
        }

        // DRM'ed wma file checking
        if(array_key_exists(84,$mt))
        {
            $command = HOME_BASE . 'assets/bin/wma-drm-check.sh';
            $output = trim(`$command $path`);

            if($output == "1")
            {
                return $mt[84];
            }
            else
            {
                $this->setError('Sorry, this file may contain DRM protection so we are unable to use this file.');
                return null;
            }
        }

        // DRM'ed aac file checking (m4a vs m4p)
        if(array_key_exists(31,$mt))
        {
            $command = HOME_BASE . 'assets/bin/aac-drm-check.sh';
            $output = trim(`$command $path`);

            if($output == "1")
            {
                return $mt[31];
            }
            else
            {
                $this->setError('Sorry, this file may contain DRM protection so we are unable to use this file.');
                return null;
            }
        }

        // application/octet-stream file detection
        if(array_key_exists(85,$mt))
        {
            $command = 'file -ib -m ' . HOME_BASE . 'assets/lib/mymagic.mime';
            $output = trim(`$command $path`);
            if($output == 'audio/x-aac')
            {
                return $mt[85];
            }
        }

        if(sizeof($mt) < 1)
        {
            $this->setError('Could not determine media type.');
            return null;
        }

        // take the first match we find
        $mt = array_shift($mt);
        return $mt;
    }
}

?>
