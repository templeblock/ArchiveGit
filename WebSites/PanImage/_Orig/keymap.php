<?php
/******************************************************************************
 *
 * Copyright DM Solutions Group Inc 2005.  All rights reserved.
 *
 * kaMap! keymap generator
 *
 * This file is called from the kaMap! module using XMLHttpRequest.  The result
 * is a URL to the generated keymap image.
 * 
 * $Id: keymap.php,v 1.2 2005/04/29 19:29:17 pspencer Exp $
 *
 *****************************************************************************/
include_once('config.php');

if (!extension_loaded('MapScript'))
{
    dl( $szPHPMapScriptModule );
}

$oMap = ms_newMapObj($szMapFile);
$oMap->setSize($_REQUEST['vpw'], $_REQUEST['vph']);

$inchesPerUnit = array(1, 12, 63360.0, 39.3701, 39370.1, 4374754);
$cellSize = $_REQUEST['scale']/($oMap->resolution*$inchesPerUnit[$oMap->units]);


$centerX = $_REQUEST['vpx'] * $cellSize;
$centerY = -1*$_REQUEST['vpy'] * $cellSize;

$minX = $centerX - $oMap->width*$cellSize/2;
$minY = $centerY - $oMap->height*$cellSize/2;
$maxX = $centerX + $oMap->width*$cellSize/2;
$maxY = $centerY + $oMap->height*$cellSize/2;

$oMap->setExtent( $minX, $minY, $maxX, $maxY );

$oImg = $oMap->drawReferenceMap();

$szURL = $oImg->saveWebImage();
echo $szURL;
?>
