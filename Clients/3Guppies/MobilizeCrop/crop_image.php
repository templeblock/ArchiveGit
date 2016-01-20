<?

/*

This file should be used to crop an image
	Input to this file:
	$_GET['image_ref']
	$_GET['x']
	$_GET['y']
	$_GET['width']
	$_GET['height']
	$_GET['convertTo']
	$_GET['percentSize']

*/

define("IMAGE_MAGICK_PATH","/imagemagick/");

if(isset($_GET['image_ref']) && isset($_GET['x']) && isset($_GET['y']) && isset($_GET['x']) && isset($_GET['width']) && isset($_GET['convertTo'])){
		
	// Use Imagemagick(www.imagemagick.org), Image Alchemy(Alchemy)
	
	$x = $_GET['x'];
	$y = $_GET['y'];
	$width = $_GET['width'];
	$height = $_GET['height'];
	$image_ref = $_GET['image_ref'];
	$percentSize = $_GET['percentSize'];
	$convertTo = $_GET['convertTo'];
	
	$x = preg_replace("/[^0-9]/si","",$x);
	$y = preg_replace("/[^0-9]/si","",$y);
	$width = preg_replace("/[^0-9]/si","",$width);
	$height = preg_replace("/[^0-9]/si","",$height);
	$percentSize = preg_replace("/[^0-9]/si","",$percentSize);
	
	// You need to validate some of the variables above in case someone is calling this file directly from their browser and not from the crop script
	// This is some examples:
	$image_ref = str_replace("../","",$image_ref);
	if(substr($image_ref,0,1)=="/")exit;
	if($percentSize>200)$percentSize = 200;
	
	if(strlen($x) && strlen($y) && $width && $height && $percentSize){
	
		$convertParamAdd = "";
		if($percentSize!="100"){
			$convertParamAdd = " -resize ".$percentSize."x".$percentSize."%";
			$x = $x * ($percentSize/100);	
			$y = $y * ($percentSize/100);	
			$width = $width * ($percentSize/100);	
			$height = $height * ($percentSize/100);	
		}
		
		$destinationFile = "demo-images/nature_copy.$convertTo";	// Name of the converted file. 
		$convertString = IMAGE_MAGICK_PATH."convert $image_ref $convertParamAdd -crop ".$width."x".$height."+".$x."+".$y." $destinationFile";
		exec($convertString);
		echo "var w = window.open('$destinationFile','imageWin','width=".($width+30).",height=".($height+30).",resizable=yes');";
	}else{
		echo "alert('Error!');";
	}	
}


?>

