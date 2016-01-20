<?php
/******************************************************************************
 *
 * Copyright DM Solutions Group Inc 2005.  All rights reserved.
 *
 * kaMap! initialization file
 *
 * This file is called from the kaMap! module using XMLHttpRequest.  The result
 * is evaluated as javascript and should initialize all the kaMap! variables
 * required to request the current map file (as specified in the configuration
 * file).
 * 
 * $Id: init.php,v 1.5.2.1 2005/05/16 21:33:07 pspencer Exp $
 *
 *****************************************************************************/
include_once( 'config.php' );

/*
 * passed the following:
 *
 * vpw - viewport width
 * vph - viewport height
 *
 * need to calculate the following:
 *
 * nScale
 * tileWidth
 * tileHeight
 * nLayers
 * aScales
 * aszLayers - eventually
 * 
 * nCurrentTop
 * nCurrentLeft
 * xOffset
 * yOffset
 */
 
if (!extension_loaded('MapScript'))
{
    dl( $szPHPMapScriptModule );
}

$oMap = ms_newMapObj($szMapFile);
if (isset($_REQUEST['vpw']) && is_numeric($_REQUEST['vpw']))
{
    $w = $_REQUEST['vpw'] + 2*$tileWidth;
}
else
{
    $w = $oMap->width;
}

if (isset($_REQUEST['vph']) && is_numeric($_REQUEST['vph']))
{
    $h = $_REQUEST['vph'] + 2*$tileHeight;
}
else
{
    $h = $oMap->height;
}
$oMap->setSize( $w, $h );

$minX = $oMap->extent->minx;
$minY = $oMap->extent->miny;
$maxX = $oMap->extent->maxx;
$maxY = $oMap->extent->maxy;

$nScale = $oMap->scale;

if ($nScale > $anScales[0])
    $nScale = $anScales[0];
    
//find closest valid scale (that is larger than this one)
foreach($anScales as $theScale)
{
    if ($nScale < $theScale)
    {
        $nScale = $theScale;
        break;
    }
}

//now have to zoom to this scale
$cX = ($minX + $maxX) / 2;
$cY = ($minY + $maxY) / 2;
$inchesPerUnit = array(1, 12, 63360.0, 39.3701, 39370.1, 4374754);
$cellSize = $nScale/($oMap->resolution*$inchesPerUnit[$oMap->units]);
$minX = $cX - ($w/2)*$cellSize;
$minY = $cY - ($h/2)*$cellSize;
$maxX = $cX + ($w/2)*$cellSize;
$maxY = $cY + ($h/2)*$cellSize;
$oMap->setExtent( $minX, $minY, $maxX, $maxY );    

$pixelWidth = ($maxX - $minX)/($oMap->width);
$pixelHeight = ($maxY - $minY)/($oMap->height);

$pixelLeft = $minX/$pixelWidth;
$pixelTop = -1*$maxY/$pixelHeight;

//clamp to a tile
$nCurrentTop = (round($pixelTop/$tileHeight +0.5) * $tileHeight); // - $tileHeight;
$nCurrentLeft = (round($pixelLeft/$tileWidth +0.5) * $tileWidth); // - $tileWidth;

$szResult = '/* leave this here */';
$szResult .= "aScales=new Array(".implode(",", $anScales).");";
$szResult .= "nScale=$nScale;";
$szResult .= "nCurrentLeft=$nCurrentLeft;";
$szResult .= "nCurrentTop=$nCurrentTop;";
$szResult .= "tileWidth=$tileWidth;";
$szResult .= "tileHeight=$tileHeight;";
$szResult .= "currentMap='$szMap';";

$szResult .= "aMapFiles = new Array();";
$szGroups = "aGroups = new Array();";
foreach($aszMapFiles as $key => $aszMapFile)
{
    $szResult .= "aMapFiles['$key']='".$aszMapFile[0]."';";
    $oMap = ms_newMapObj( $aszMapFile[1] );
    $aGroups = $oMap->getAllGroupNames();
    $szGroups .= "aGroups['$key'] = new Array('".(implode("','", $aGroups))."');";
}
$szResult .= $szGroups;
echo $szResult;
exit;
?>
