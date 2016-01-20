<?

class AudioMediaHandler extends MediaHandler
{
    function AudioMediaHandler(&$controller, &$fileInfo, &$error)
    {
        $this->controller =& $controller;
        $this->fileInfo =& $fileInfo;
        $this->error =& $error;
    }

    function hasEditor()
    {
        return TRUE;
    }

    function getEditor()
    {
        return array('OpenLocker', 'AudioEditor', VIEW_INDEX);
    }

    function getWidthHeight($_options = array())
    {
        return array(0,0);
    }

    function normalizeOptions($_options = array())
    {
        $start = $end = 0;
        $divisor = 1;

        if ( array_key_exists('Ringtone.TimeFormat', $_options) )
        {
            if ( $_options['Ringtone.TimeFormat'] == 'milliseconds' )
                $divisor = 1000;
        }

        if ( array_key_exists('Ringtone.Start', $_options) )
            $start = round(intval($_options['Ringtone.Start']) / $divisor);
        else
            $start = 0;

        if ( array_key_exists('Ringtone.End', $_options) )
            $end = round(intval($_options['Ringtone.End']) / $divisor);
        else
            $end = 30;

        if ( ($start < 0) || ($start >= $end) || (($end - $start) > 32) || ($end > 600) )
        {
            $start = 0;
            $end = 30;
        }

        $_options['Ringtone.Start'] = $start;
        $_options['Ringtone.End'] = $end;

        return $_options;
    }

    function getCommands($_options = array())
    {
        $_options = $this->normalizeOptions($_options);

        if ( $this->fileInfo['CapabilityId'] == 15 )
            $cmd0 = 'NOTRIM';
        else
            $cmd0 = '';

        $cmd1 = $_options['Ringtone.Start'];
        $cmd2 = $_options['Ringtone.End'];
        if(is_numeric($cmd1) && is_numeric($cmd2))
        {
            $cmd4 = $cmd2 - $cmd1;
        }
        else
        {
            $cmd4 = 30;
        }

        $filename_addon = "-$cmd1-$cmd2";
        $tempfile_addon = "-trim-$cmd1-$cmd2";

        if ( array_key_exists('Ringtone.IncreaseVolume', $_options) )
            $cmd3 = 'vol 1.4';
        else
            $cmd3 = '';

        return array(
            'cmd0'              =>  $cmd0,
            'cmd1'              =>  $cmd1,
            'cmd2'              =>  $cmd2,
            'cmd3'              =>  $cmd3,
            'cmd4'              =>  $cmd4,
            'filename_addon'    =>  $filename_addon,
            'tempfile_addon'    =>  $tempfile_addon,
            );
    }

    function isValid()
    {
        /* OLD CODE FOR DISALLOWING ANONYMOUS MOBILIZE OF AUDIO FILES
        $controller =& Controller::getInstance();
        $user =& $controller->getUser();

        if($user->hasPrivilege('Anonymous User'))
        {
            $this->error = 'You must be logged in to mobilize audio files.';
            return false;
        }
        */
        return true;
    }
}

?>
