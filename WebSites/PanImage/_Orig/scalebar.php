<?php
/******************************************************************************
 *
 * Copyright DM Solutions Group Inc 2005.  All rights reserved.
 *
 * kaMap! scalebar generator
 *
 * This file is called from the kaMap! module using XMLHttpRequest.  The result
 * is a URL to the generated scalebar image.
 * 
 * $Id: scalebar.php,v 1.1 2005/04/29 19:29:38 pspencer Exp $
 *
 *****************************************************************************/
include_once('config.php');

if (!extension_loaded('MapScript'))
{
    dl( $szPHPMapScriptModule );
}

$oMap = ms_newMapObj($szMapFile);
$oPoint = ms_newPointObj( );
$oPoint->setXY($oMap->width/2, $oMap->height/2 );
$oMap->zoomScale( $_REQUEST['scale'], $oPoint, $oMap->width, $oMap->height, $oMap->extent );
$oImg = $oMap->drawScalebar();

$szURL = $oImg->saveWebImage();
echo $szURL;
?>
