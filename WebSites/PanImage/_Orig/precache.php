<?php
/******************************************************************************
 *
 * Copyright DM Solutions Group Inc 2005.  All rights reserved.
 *
 * kaMap! cache populator
 *
 * this is intended to be run from the command line and is used to
 * pre-generate cached images for a given map file.
 * 
 * $Id: precache.php,v 1.1 2005/05/17 13:33:26 pspencer Exp $
 *
 *****************************************************************************/
include( 'config.php' );
if (!extension_loaded('MapScript'))
{
    dl( $szPHPMapScriptModule );
}

set_time_limit( 0 );

$szKey = "hawaii";

$szURL = "http://sb5.dmsolutions.ca/kamap/tile.php?";

$aScales = $aszMapFiles[$szKey][2];
$oMap = ms_newMapObj( $aszMapFiles[$szKey][1] );

$mapWidth = $metaWidth * $tileWidth;
$mapHeight = $metaHeight * $tileHeight;

$oMap->setSize( $mapWidth, $metaHeight*$tileHeight );
$inchesPerUnit = array(1, 12, 63360.0, 39.3701, 39370.1, 4374754);
    
$dMinX = $oMap->extent->minx;
$dMaxX = $oMap->extent->maxx;
$dMinY = $oMap->extent->miny;
$dMaxY = $oMap->extent->maxy;

$nTotalTiles = 0;

$aScales = array( 25000, 12500 );

foreach($aScales as $scale)
{
    echo "processing scale $scale<BR>";
    
    $oMap->setExtent($dMinX, $dMinY, $dMaxX, $dMaxY);
    
    $cellSize = $scale/($oMap->resolution*$inchesPerUnit[$oMap->units]);
    
    $pixMinX = $oMap->extent->minx / $cellSize;
    $pixMaxX = $oMap->extent->maxx / $cellSize;
    $pixMinY = $oMap->extent->miny / $cellSize;
    $pixMaxY = $oMap->extent->maxy / $cellSize;
    
    echo "pix dimensions: $pixMinX, $pixMinY : $pixMaxX, $pixMaxY<BR>";
    
    $metaMinX = floor($pixMinX/($mapWidth)) * ($mapWidth);
    $metaMaxX = ceil($pixMaxX/($mapWidth)) * ($mapWidth);
    $metaMinY = floor($pixMinY/($mapHeight)) * ($mapHeight);
    $metaMaxY = ceil($pixMaxY/($mapHeight)) * ($mapHeight);
    
    echo "meta dimensions: $metaMinX, $metaMinY : $metaMaxX, $metaMaxY<BR>";
    
    $nWide = ($metaMaxX - $metaMinX)/$mapWidth;
    $nHigh = ($metaMaxY - $metaMinY)/$mapHeight;
    echo "meta tiles: $nWide x $nHigh = ".($nWide*$nHigh)." meta tiles and ".($nWide*$nHigh*$metaWidth*$metaHeight)." tiles<BR>";
    $nTotalTiles += $nWide*$nHigh*$metaWidth*$metaHeight;
    for ($j=0; $j <$nHigh; $j++)
    {
        for ($k=0; $k < $nWide; $k++)
        {
            file_get_contents("http://kamap:demo@sb5.dmsolutions.ca/kamap/tile.php?s=$scale&t=".($metaMinY + $j*$mapHeight)."&l=".($metaMinX + $k*$mapWidth)) ;
        }
    }
    echo "requested $nTotalTiles so far<BR>";
    flush();
}



?>
