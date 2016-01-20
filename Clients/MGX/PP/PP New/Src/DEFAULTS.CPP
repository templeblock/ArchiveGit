//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "bsrutil.h"
#include "defaults.h"
#include "ppafx.h"

// Static prototypes
static void HandleBrushSettings( int idTool, BOOL fPut );

static FNAME szPreferenceFile;

LPTSTR BASED_CODE Private_Ini     = "PP.INI";
LPTSTR BASED_CODE MGX_Ini     	 = "MGX.INI";
LPTSTR BASED_CODE Empty_Str       = "";
LPTSTR BASED_CODE Mgxi_Str        = "Micrografx";
LPTSTR BASED_CODE BDrive_Str      = "B:\\";
LPTSTR BASED_CODE Intl_Str        = "intl";
LPTSTR BASED_CODE Measure_Str     = "iMeasure";
LPTSTR BASED_CODE Zeros3_Str      = "0,0,0";
LPTSTR BASED_CODE RGB_Str         = "255,255,255";
LPTSTR BASED_CODE NoDel_Str       = "NODELETE.PP4";
LPTSTR BASED_CODE FileSrch_Str    = "FILE SEARCH";
LPTSTR BASED_CODE Erase_Str       = "Erase";
LPTSTR BASED_CODE Paint_Str       = "Paint";
LPTSTR BASED_CODE PaintOn_Str     = "PaintOn";
LPTSTR BASED_CODE Spray_Str       = "Spray";
LPTSTR BASED_CODE Clone_Str       = "Clone";
LPTSTR BASED_CODE TPaint_Str      = "TexturePaint";
LPTSTR BASED_CODE Smear_Str       = "Smear";
LPTSTR BASED_CODE Sharpen_Str     = "Sharpen";
LPTSTR BASED_CODE Smooth_Str      = "Smooth";
LPTSTR BASED_CODE Lighten_Str     = "Lighten";
LPTSTR BASED_CODE Darken_Str      = "Darken";
LPTSTR BASED_CODE DrawLine_Str    = "DrawLine";
LPTSTR BASED_CODE DrawRect_Str    = "DrawRect";
LPTSTR BASED_CODE DrawFree_Str    = "DrawFree";
LPTSTR BASED_CODE StrokeMask_Str  = "StrokeMask";
LPTSTR BASED_CODE BrushShape_Str  = "BrushShape";
LPTSTR BASED_CODE BrushSize_Str   = "BrushSize";
LPTSTR BASED_CODE Opacity_Str     = "Opacity";
LPTSTR BASED_CODE SPressure_Str   = "StitchPressure";
LPTSTR BASED_CODE MergeMode_Str   = "MergeMode";
LPTSTR BASED_CODE Feather_Str     = "Feather";
LPTSTR BASED_CODE StyleName_Str   = "StyleName";
LPTSTR BASED_CODE MainMemMin_Str           = "MainMemMin";
LPTSTR BASED_CODE MainMemFactor_Str        = "MainMemFactor";
LPTSTR BASED_CODE LineArtAsGray_Str        = "LineArtAsGray";
LPTSTR BASED_CODE UseThreading_Str         = "UseThreading";
LPTSTR BASED_CODE ScratchpadWidth_Str      = "ScratchpadWidth";
LPTSTR BASED_CODE ScratchpadHeight_Str     = "ScratchpadHeight";
LPTSTR BASED_CODE IsScratchpadColor_Str    = "IsScratchpadColor";
LPTSTR BASED_CODE NoUndo_Str               = "NoUndo";
LPTSTR BASED_CODE UseApply_Str             = "UseApply";
LPTSTR BASED_CODE UndoObjects_Str          = "UndoObjects";
LPTSTR BASED_CODE UndoMasks_Str            = "UndoMasks";
LPTSTR BASED_CODE UseTablet_Str            = "UseTablet";
LPTSTR BASED_CODE MaskTint_Str             = "MaskTint";
LPTSTR BASED_CODE IndependentSettings_Str  = "IndependentSettings";
LPTSTR BASED_CODE Didot_Str                = "Didot";
LPTSTR BASED_CODE Units_Str                = "Units";
LPTSTR BASED_CODE UseRulers_Str            = "UseRulers";
LPTSTR BASED_CODE DisplayPixelValue_Str    = "DisplayPixelValue";
LPTSTR BASED_CODE UsePlugins_Str           = "UsePlugins";
LPTSTR BASED_CODE MultipleObjects_Str      = "MultipleObjects";
LPTSTR BASED_CODE UseMaskAndObjects_Str    = "UseMaskAndObjects";
LPTSTR BASED_CODE nRepeat_Str              = "nRepeat";
LPTSTR BASED_CODE fJournalMacro_Str        = "fJournalMacro";
LPTSTR BASED_CODE fBatchClose_Str          = "fBatchClose";
LPTSTR BASED_CODE BatchSave_Str            = "BatchSave";
LPTSTR BASED_CODE NewDepth_Str             = "NewDepth";
LPTSTR BASED_CODE NewResolution_Str        = "NewResolution";
LPTSTR BASED_CODE GammaLock_Str            = "GammaLock";
LPTSTR BASED_CODE GammaDisable_Str         = "GammaDisable";
LPTSTR BASED_CODE DoPicture_Str            = "DoPicture";
LPTSTR BASED_CODE DoBitmap_Str             = "DoBitmap";
LPTSTR BASED_CODE ScreenColor_Str          = "ScreenColor";
LPTSTR BASED_CODE xImage_Str               = "xImage";
LPTSTR BASED_CODE yImage_Str               = "yImage";
LPTSTR BASED_CODE Hints_Str                = "Hints";
LPTSTR BASED_CODE Save24BitWallpaper_Str   = "Save24BitWallpaper";
LPTSTR BASED_CODE UseObjectMarquee_Str     = "UseObjectMarquee";
LPTSTR BASED_CODE DefaultFileType_Str      = "DefaultFileType";
LPTSTR BASED_CODE UseFunctionThreading_Str = "UseFunctionThreading";
LPTSTR BASED_CODE NTBeta_Str               = "NTBeta";
LPTSTR BASED_CODE DoCommandList_Str        = "DoCommandList";
LPTSTR BASED_CODE UseShortCommandNames_Str = "UseShortCommandNames";
LPTSTR BASED_CODE UseFileForClipboard_Str  = "UseFileForClipboard";
LPTSTR BASED_CODE WriteFileOnPaste_Str     = "WriteFileOnPaste";
LPTSTR BASED_CODE UseProjectDirectory_Str  = "UseProjectDirectory";
LPTSTR BASED_CODE bPTRows_Str              = "bPTRows";
LPTSTR BASED_CODE PTNumRows_Str            = "PTNumRows";
LPTSTR BASED_CODE PTNumCols_Str            = "PTNumCols";
LPTSTR BASED_CODE Poster_bAuto_Str         = "Poster_bAuto";
LPTSTR BASED_CODE ContBrit_bAuto_Str       = "ContBrit_bAuto";
LPTSTR BASED_CODE Balance_bAuto_Str       = "Balance_bAuto";
LPTSTR BASED_CODE Map_bAuto_Str            = "Map_bAuto";
LPTSTR BASED_CODE Map_nPoints_Str          = "Map_nPoints";
LPTSTR BASED_CODE Map_bGrid_Str            = "Map_bGrid";
LPTSTR BASED_CODE Map_bPercent_Str         = "Map_bPercent";
LPTSTR BASED_CODE Map_bIntensity_Str       = "Map_bIntensity";
LPTSTR BASED_CODE Map_bCurves_Str          = "Map_bCurves";
LPTSTR BASED_CODE Map_CurveType_Str        = "Map_CurveType";
LPTSTR BASED_CODE HueShift_bAuto_Str       = "HueShift_bAuto";
LPTSTR BASED_CODE HueMap_bAuto_Str         = "HueMap_bAuto";
LPTSTR BASED_CODE Quarter_bAuto_Str        = "Quarter_bAuto";
LPTSTR BASED_CODE Quarter_HSacrifice_Str   = "Quarter_HSacrifice";
LPTSTR BASED_CODE Quarter_SSacrifice_Str   = "Quarter_SSacrifice";
LPTSTR BASED_CODE Quarter_bUseMidPer_Str   = "Quarter_bUseMidPer";
LPTSTR BASED_CODE Quarter_bPercent_Str     = "Quarter_bPercent";
LPTSTR BASED_CODE Type_Str                 = "Type";
LPTSTR BASED_CODE PrintNumeric_Str         = "PrintNumeric";
LPTSTR BASED_CODE OutputType_Str           = "OutputType";
LPTSTR BASED_CODE ScatterPrint_Str         = "ScatterPrint";
LPTSTR BASED_CODE Centered_Str             = "Centered";
LPTSTR BASED_CODE SepFlags_Str             = "SepFlags";
LPTSTR BASED_CODE TrimMarks_Str            = "TrimMarks";
LPTSTR BASED_CODE RegMarks_Str             = "RegMarks";
LPTSTR BASED_CODE Labels_Str               = "Labels";
LPTSTR BASED_CODE StepScale_Str            = "StepScale";
LPTSTR BASED_CODE Negative_Str             = "Negative";
LPTSTR BASED_CODE EmulsionDown_Str         = "EmulsionDown";
LPTSTR BASED_CODE BinaryPS_Str             = "BinaryPS";
LPTSTR BASED_CODE BinaryEPS_Str            = "BinaryEPS";
LPTSTR BASED_CODE PrintScaling_Str         = "PrintScaling";
LPTSTR BASED_CODE UsePrinterScreening_Str  = "UsePrinterScreening";
LPTSTR BASED_CODE LockAspect_Str           = "LockAspect";
LPTSTR BASED_CODE DoCalibration_Str        = "DoCalibration";
LPTSTR BASED_CODE ColorPreview_Str         = "ColorPreview";
LPTSTR BASED_CODE ScanLAInvert_Str         = "ScanLAInvert";
LPTSTR BASED_CODE BitsPerPixel_Str         = "BitsPerPixel";
LPTSTR BASED_CODE Resolution_Str           = "Resolution";
LPTSTR BASED_CODE ScanSpeed_Str            = "ScanSpeed";
LPTSTR BASED_CODE UseDMA_Str               = "UseDMA";
LPTSTR BASED_CODE Frameleft_Str            = "Frame.left";
LPTSTR BASED_CODE Frametop_Str             = "Frame.top";
LPTSTR BASED_CODE Frameright_Str           = "Frame.right";
LPTSTR BASED_CODE Framebottom_Str          = "Frame.bottom";
LPTSTR BASED_CODE SmartSize_Str            = "SmartSize";
LPTSTR BASED_CODE wFeatherEdge_Str         = "wFeatherEdge";
LPTSTR BASED_CODE FeatherEdgeSoft_Str      = "FeatherEdgeSoft";
LPTSTR BASED_CODE FeatherEdgeNormal_Str    = "FeatherEdgeNormal";
LPTSTR BASED_CODE FeatherEdgeHard_Str      = "FeatherEdgeHard";
LPTSTR BASED_CODE FeatherAmount_Str        = "FeatherAmount";
LPTSTR BASED_CODE FeatherInside_Str        = "FeatherInside";
LPTSTR BASED_CODE DeleteMask_Str           = "DeleteMask";
LPTSTR BASED_CODE ProbeMethod_Str          = "ProbeMethod";
LPTSTR BASED_CODE IfHidden_Str             = "IfHidden";
LPTSTR BASED_CODE ShieldMethod_Str         = "ShieldMethod";
LPTSTR BASED_CODE MaskShield_Str           = "MaskShield";
LPTSTR BASED_CODE Mask_Str                 = "Mask";
LPTSTR BASED_CODE ColorMask_Str            = "ColorMask";
LPTSTR BASED_CODE Include_Str              = "Include";
LPTSTR BASED_CODE On_Str                   = "On";
LPTSTR BASED_CODE MaskRange0_Str           = "MaskRange[0]";
LPTSTR BASED_CODE MaskRange1_Str           = "MaskRange[1]";
LPTSTR BASED_CODE MaskRange2_Str           = "MaskRange[2]";
LPTSTR BASED_CODE MaskRange3_Str           = "MaskRange[3]";
LPTSTR BASED_CODE MaskRange4_Str           = "MaskRange[4]";
LPTSTR BASED_CODE MaskRange5_Str           = "MaskRange[5]";
LPTSTR BASED_CODE MaskRange6_Str           = "MaskRange[6]";
LPTSTR BASED_CODE MaskRange7_Str           = "MaskRange[7]";
LPTSTR BASED_CODE AutoBrightness_Str       = "AutoBrightness";
LPTSTR BASED_CODE AutoBlend_Str            = "AutoBlend";
LPTSTR BASED_CODE Pressure_Str             = "Pressure";
LPTSTR BASED_CODE SmartStitch_Str          = "SmartStitch";
LPTSTR BASED_CODE iTransform_Str           = "iTransform";
LPTSTR BASED_CODE MiniType_Str             = "MiniType";
LPTSTR BASED_CODE fOptimize_Str            = "fOptimize";
LPTSTR BASED_CODE fScatter_Str             = "fScatter";
LPTSTR BASED_CODE fDither_Str              = "fDither";
LPTSTR BASED_CODE ConvertColors_Str        = "ConvertColors";
LPTSTR BASED_CODE StrokeTool_Str           = "StrokeTool";
LPTSTR BASED_CODE iShapeModes_Str          = "iShapeModes";
LPTSTR BASED_CODE DoMaskCircle_Str         = "DoMaskCircle";
LPTSTR BASED_CODE ConstrainRectAspect_Str  = "ConstrainRectAspect";
LPTSTR BASED_CODE ShapeUnits_Str           = "ShapeUnits";
LPTSTR BASED_CODE iFreeModes_Str           = "iFreeModes";
LPTSTR BASED_CODE AutoMask_Str             = "AutoMask";
LPTSTR BASED_CODE PointEdit_Str            = "PointEdit";
LPTSTR BASED_CODE idAutoColorModel_Str     = "idAutoColorModel";
LPTSTR BASED_CODE AutoSearchDepth_Str      = "AutoSearchDepth";
LPTSTR BASED_CODE AutoMinEdge_Str          = "AutoMinEdge";
LPTSTR BASED_CODE iPaintOnModes_Str        = "iPaintOnModes";
LPTSTR BASED_CODE PaintOnObject_Str        = "PaintOnObject";
LPTSTR BASED_CODE iWandModes_Str           = "iWandModes";
LPTSTR BASED_CODE WandRange_Str            = "WandRange";
LPTSTR BASED_CODE WandFade_Str             = "WandFade";
LPTSTR BASED_CODE idMagicColorModel_Str    = "idMagicColorModel";
LPTSTR BASED_CODE GrowAmount_Str           = "GrowAmount";
LPTSTR BASED_CODE TransformImage_Str       = "TransformImage";
LPTSTR BASED_CODE iTransformModes_Str      = "iTransformModes";
LPTSTR BASED_CODE PasteQuality_Str         = "PasteQuality";
LPTSTR BASED_CODE TransMode_Str            = "TransMode";
LPTSTR BASED_CODE TransformOpacity_Str     = "TransformOpacity";
LPTSTR BASED_CODE PointIModes_Str          = "PointIModes";
LPTSTR BASED_CODE PointAntialias_Str       = "PointAntialias";
LPTSTR BASED_CODE PointerFunction_Str      = "PointerFunction";
LPTSTR BASED_CODE MakePathTolerance_Str    = "MakePathTolerance";
LPTSTR BASED_CODE MakePathContinuity_Str	     = "MakePathContinuity";
LPTSTR BASED_CODE MakePathCurveTolerance_Str  = "MakePathCurveTolerance";
LPTSTR BASED_CODE MakePathType_Str         = "MakePathType";
LPTSTR BASED_CODE ConstrainCropAspect_Str  = "ConstrainCropAspect";
LPTSTR BASED_CODE CropUnits_Str            = "CropUnits";
LPTSTR BASED_CODE Gradient_Str             = "Gradient";
LPTSTR BASED_CODE RepeatCount_Str          = "RepeatCount";
LPTSTR BASED_CODE SoftTransition_Str       = "SoftTransition";
LPTSTR BASED_CODE VigColorModel_Str        = "VigColorModel";
LPTSTR BASED_CODE VigOpacity_Str           = "VigOpacity";
LPTSTR BASED_CODE Midpoint_Str             = "Midpoint";
LPTSTR BASED_CODE fHorzFlip_Str            = "fHorzFlip";
LPTSTR BASED_CODE fVertFlip_Str            = "fVertFlip";
LPTSTR BASED_CODE TextureOpacity_Str       = "TextureOpacity";
LPTSTR BASED_CODE FillOpacity_Str          = "FillOpacity";
LPTSTR BASED_CODE FloodRange_Str           = "FloodRange";
LPTSTR BASED_CODE FloodFade_Str            = "FloodFade";
LPTSTR BASED_CODE idFillColorModel_Str     = "idFillColorModel";
LPTSTR BASED_CODE FloodOpacity_Str         = "FloodOpacity";
LPTSTR BASED_CODE wSquareStyle_Str         = "wSquareStyle";
LPTSTR BASED_CODE DoDrawCircle_Str         = "DoDrawCircle";
LPTSTR BASED_CODE wFreeStyle_Str           = "wFreeStyle";
LPTSTR BASED_CODE ZoomOut_Str              = "ZoomOut";
LPTSTR BASED_CODE ZoomWindow_Str           = "ZoomWindow";
LPTSTR BASED_CODE FullScreen_Str           = "FullScreen";
LPTSTR BASED_CODE fReMap_Str               = "fReMap";
LPTSTR BASED_CODE fBubbleHintsOn_Str       = "fBubbleHintsOn";
LPTSTR BASED_CODE fStatusHintsOn_Str       = "fStatusHintsOn";
LPTSTR BASED_CODE fHintWindowOn_Str        = "fHintWindowOn";
LPTSTR BASED_CODE iModes_Str               = "iModes";
LPTSTR BASED_CODE FreeAntialias_Str            = "FreeAntialias";
LPTSTR BASED_CODE idColorModel_Str         = "idColorModel";
LPTSTR BASED_CODE Range_Str                = "Range";
LPTSTR BASED_CODE Constrain_Str            = "Constrain";
LPTSTR BASED_CODE DoCircle_Str             = "DoCircle";
LPTSTR BASED_CODE CommonUnits_Str          = "CommonUnits";
LPTSTR BASED_CODE AutoThumbnails_Str       = "AutoThumbnails";
LPTSTR BASED_CODE CreateThumbnailOnSave_Str = "CreateThumbnailOnSave";
LPTSTR BASED_CODE UseVolumeLabels_Str 			= "UseVolumeLabels";
LPTSTR BASED_CODE ConfirmOnDelFile_Str     = "ConfirmOnDelFile";
LPTSTR BASED_CODE ConfirmOnMoveFile_Str    = "ConfirmOnMoveFile";
LPTSTR BASED_CODE ConfirmOnCopyFile_Str    = "ConfirmOnCopyFile";
LPTSTR BASED_CODE ConfirmOnRenameFile_Str  = "ConfirmOnRenameFile";
LPTSTR BASED_CODE ConfirmOnAddFile_Str     = "ConfirmOnAddFile";
LPTSTR BASED_CODE ConfirmOnCreateDir_Str   = "ConfirmOnCreateDir";
LPTSTR BASED_CODE ConfirmOnRenameDir_Str   = "ConfirmOnRenameDir";
LPTSTR BASED_CODE ConfirmOnDeleteDir_Str   = "ConfirmOnDeleteDir";
LPTSTR BASED_CODE ConfirmOnRenameAlbum_Str = "ConfirmOnRenameAlbum";
LPTSTR BASED_CODE ConfirmOnDeleteAlbum_Str = "ConfirmOnDeleteAlbum";
LPTSTR BASED_CODE OKtoSavePath_Str         = "OKtoSavePath";
LPTSTR BASED_CODE bsrFileType_Str          = "bsrFileType";
LPTSTR BASED_CODE bsrImpFileType_Str       = "bsrImpFileType";
LPTSTR BASED_CODE ViewThumbnails_Str       = "ViewThumbnails";
LPTSTR BASED_CODE ViewAlbums_Str           = "ViewAlbums";
LPTSTR BASED_CODE PasteAutoThumbnails_Str  = "PasteAutoThumbnails";
LPTSTR BASED_CODE PasteThumbCreate_Str     = "PasteThumbCreate";
LPTSTR BASED_CODE PasteViewThumbnails_Str  = "PasteViewThumbnails";
LPTSTR BASED_CODE DoDCS_Str                = "DoDCS";
LPTSTR BASED_CODE PreviewBPP_Str           = "PreviewBPP";
LPTSTR BASED_CODE ClipPath_Str             = "ClipPath";
LPTSTR BASED_CODE CompositeBPP_Str         = "CompositeBPP";
LPTSTR BASED_CODE bEPSPromptUser_Str       = "bEPSPromptUser";
LPTSTR BASED_CODE bDCSPromptUser_Str       = "bDCSPromptUser";
LPTSTR BASED_CODE TIFFCompress_Str         = "TIFFCompress";
LPTSTR BASED_CODE TIFFDiff_Str             = "TIFFDiff";
LPTSTR BASED_CODE bTIFFPromptUser_Str      = "bTIFFPromptUser";
LPTSTR BASED_CODE TIFFbWritePT_Str         = "TIFFbWritePT";
LPTSTR BASED_CODE TIFFSaveMask_Str 			= "TIFFSaveMask";
LPTSTR BASED_CODE bTGAPromptUser_Str      = "bTGAPromptUser";
LPTSTR BASED_CODE TGASaveMask_Str 			= "TGASaveMask";
LPTSTR BASED_CODE bJPEGPromptUser_Str      = "bJPEGPromptUser";
LPTSTR BASED_CODE JPEGCompressionFactor_Str= "JPEGCompressionFactor";
LPTSTR BASED_CODE JPEGSubsampling_Str	   = "JPEGSubsampling";
LPTSTR BASED_CODE bPPFFPromptUser_Str      = "bPPFFPromptUser";
LPTSTR BASED_CODE PPFFCompress_Str         = "PPFFCompress";
LPTSTR BASED_CODE PPFFbWritePT_Str         = "PPFFbWritePT";
LPTSTR BASED_CODE bSaveMask_Str            = "bSaveMask";
LPTSTR BASED_CODE bSaveProxy_Str           = "bSaveProxy";
LPTSTR BASED_CODE bSaveCmdList_Str         = "bSaveCmdList";
LPTSTR BASED_CODE bSaveLink_Str         	= "bSaveLink";
LPTSTR BASED_CODE bAVIPromptUser_Str       = "bAVIPromptUser";
LPTSTR BASED_CODE iAVIPosType_Str          = "iAVIPosType";
LPTSTR BASED_CODE iAVISizingType_Str       = "iAVISizingType";
LPTSTR BASED_CODE AVIPosition_Str          = "AVIPosition";
LPTSTR BASED_CODE AVILastPosition_Str      = "AVILastPosition";
LPTSTR BASED_CODE DefaultDitherType_Str    = "DefaultDitherType";
LPTSTR BASED_CODE ConvertDitherType_Str    = "ConvertDitherType";
LPTSTR BASED_CODE ConvertType_Str          = "ConvertType";
LPTSTR BASED_CODE DitherType_Str           = "DitherType";
LPTSTR BASED_CODE TransformMergeMode_Str   = "TransformMergeMode";
LPTSTR BASED_CODE VigMergeMode_Str         = "VigMergeMode";
LPTSTR BASED_CODE TextureMergeMode_Str     = "TextureMergeMode";
LPTSTR BASED_CODE FillMergeMode_Str        = "FillMergeMode";
LPTSTR BASED_CODE FloodMergeMode_Str       = "FloodMergeMode";
LPTSTR BASED_CODE RamDisk_Str              = "RamDisk";
LPTSTR BASED_CODE PouchPath_Str            = "PouchPath";
LPTSTR BASED_CODE PluginPath1_Str          = "PluginPath1";
LPTSTR BASED_CODE PluginPath2_Str          = "PluginPath2";
LPTSTR BASED_CODE BatchAlbum_Str           = "BatchAlbum";
LPTSTR BASED_CODE BatchDir_Str             = "BatchDir";
LPTSTR BASED_CODE ColorMap_Str             = "ColorMap";
LPTSTR BASED_CODE Special_Str              = "Special";
LPTSTR BASED_CODE Scanner_Str              = "Scanner";
LPTSTR BASED_CODE ScanMap_Str              = "ScanMap";
LPTSTR BASED_CODE Printer_Str              = "Printer";
LPTSTR BASED_CODE PrintStyle_Str           = "PrintStyle";
LPTSTR BASED_CODE Clipboard_Str            = "Clipboard";
LPTSTR BASED_CODE CustomBrush_Str          = "CustomBrush";
LPTSTR BASED_CODE MainManifest_Str         = "MainManifest";
LPTSTR BASED_CODE SupManifest_Str          = "SupManifest";
LPTSTR BASED_CODE InstallerDisk_Str        = "InstallerDisk";
LPTSTR BASED_CODE ColorManager_Str         = "ColorManager";
LPTSTR BASED_CODE ColorManagerEnabled_Str  = "ColorManagerEnabled";
LPTSTR BASED_CODE Macro_Str                = "Macro";
LPTSTR BASED_CODE StartMacro_Str           = "StartMacro";
LPTSTR BASED_CODE Shape_Str                = "Shape";
LPTSTR BASED_CODE ConvertPalette_Str       = "ConvertPalette";
LPTSTR BASED_CODE TextureName_Str          = "TextureName";
LPTSTR BASED_CODE ThumbnailDir_Str         = "ThumbnailDir";
LPTSTR BASED_CODE DBFInfoDir_Str           = "DBFInfoDir";
LPTSTR BASED_CODE AlbumFileDir_Str         = "AlbumFileDir";
LPTSTR BASED_CODE DiskMarker_Str           = "DiskMarker";
LPTSTR BASED_CODE SearchAlbum_Str          = "SearchAlbum";
LPTSTR BASED_CODE LastAlbum_Str            = "LastAlbum";
LPTSTR BASED_CODE MaskColor0rgb_Str        = "MaskColor[0].rgb";
LPTSTR BASED_CODE MaskColor1rgb_Str        = "MaskColor[1].rgb";
LPTSTR BASED_CODE MaskColor2rgb_Str        = "MaskColor[2].rgb";
LPTSTR BASED_CODE MaskColor3rgb_Str        = "MaskColor[3].rgb";
LPTSTR BASED_CODE MaskColor4rgb_Str        = "MaskColor[4].rgb";
LPTSTR BASED_CODE MaskColor5rgb_Str        = "MaskColor[5].rgb";
LPTSTR BASED_CODE MaskColor6rgb_Str        = "MaskColor[6].rgb";
LPTSTR BASED_CODE MaskColor7rgb_Str        = "MaskColor[7].rgb";
LPTSTR BASED_CODE rgbBackground_Str        = "rgbBackground";
LPTSTR BASED_CODE ScreenWidth_Str          = "ScreenWidth";
LPTSTR BASED_CODE NewWidth_Str             = "NewWidth";
LPTSTR BASED_CODE NewHeight_Str            = "NewHeight";
LPTSTR BASED_CODE RGamma_Str               = "RGamma";
LPTSTR BASED_CODE GGamma_Str               = "GGamma";
LPTSTR BASED_CODE BGamma_Str               = "BGamma";
LPTSTR BASED_CODE OffsetX_Str              = "OffsetX";
LPTSTR BASED_CODE OffsetY_Str              = "OffsetY";
LPTSTR BASED_CODE Width_Str                = "Width";
LPTSTR BASED_CODE Height_Str               = "Height";
LPTSTR BASED_CODE ConstrainRectWidth_Str   = "ConstrainRectWidth";
LPTSTR BASED_CODE ConstrainRectHeight_Str  = "ConstrainRectHeight";
LPTSTR BASED_CODE ConstrainCropWidth_Str   = "ConstrainCropWidth";
LPTSTR BASED_CODE ConstrainCropHeight_Str  = "ConstrainCropHeight";
LPTSTR BASED_CODE ZoomDefaultSize_Str      = "ZoomDefaultSize";
LPTSTR BASED_CODE OneBitMask_Str           = "OneBitMask";
LPTSTR BASED_CODE PasteAtRealSize_Str      = "PasteAtRealSize";
LPTSTR BASED_CODE DisplayQuality_Str      = "Quality";
LPTSTR BASED_CODE UseGrayScaleIcons_Str    = "UseGrayScaleIcons";
LPTSTR BASED_CODE DisableAutoScroll_Str    = "DisableAutoScroll";
LPTSTR BASED_CODE DisableDither_Str    = "DisableDither";
LPTSTR BASED_CODE UseBrushEditColor_Str    = "UseBrushEditColor";
LPTSTR BASED_CODE fScanUseCMS_Str    		= "fScanUseCMS";
LPTSTR BASED_CODE fScanCalVisual_Str    		= "fScanCalVisual";
LPTSTR BASED_CODE fPrintCalVisual_Str    	= "fPrintCalVisual";
LPTSTR BASED_CODE TGACMYK_Str   	            = "TGACMYK";
LPTSTR BASED_CODE fPCDUseCMS_Str    			= "fPCDUseCMS";
LPTSTR BASED_CODE PackageDir_Str    			= "PackageDir";
LPTSTR BASED_CODE fUnlinkWintab_Str 			= "UnlinkWintab";
LPTSTR BASED_CODE fBatchChangeType_Str 		= "fBatchChangeType";
LPTSTR BASED_CODE BatchFileType_Str 			= "BatchFileType";
LPTSTR BASED_CODE NoHiResRebuild_Str 		= "NoHiResRebuild";
LPTSTR BASED_CODE WarnReadOnly_Str 		    = "WarnReadOnly";
#ifndef _DESIGNERFIX
LPTSTR BASED_CODE DesignerEditor_Str    	   	= "EditorInProgress";  //hack to determine if Designer is using us as an editor
#endif //_DESIGNERFIX
LPTSTR BASED_CODE GeneratePS_Str 		    = "GeneratePostScript";
#define PRIVATE_INI

// uncomment the line below to use registry for prefs
#define REGISTRY_INI

#define GetDefDitherType(N,INT) (N=(DITHER_TYPE)GetDefaultInt(#N,INT))
#define GetDefConvertType(N,INT) (N=(CONVERT_TYPE)GetDefaultInt(#N,INT))

static FNAME szIniPath;

// define PARSE_INMEMORY if want Preferences to be Loaded and Parsed in memory
//#define PARSE_INMEMORY  
static LPTSTR lpPreferences = NULL;

/************************************************************************/
LPCTSTR SetupPreferencePath()
/************************************************************************/
{
	LPTSTR lp;
	if ( !szPreferenceFile[0] )
 	{
		if ( !AstralStr( IDS_INIFILE, &lp ) )
			lp = Private_Ini;
		lstrcpy( szPreferenceFile, lp );
		GetIniPath( szPreferenceFile );
   	}
	return(szPreferenceFile);
}

/************************************************************************/
BOOL GetIniPath( LPTSTR lpIniFile )
/************************************************************************/
{
FNAME szFileName;
FNAME szIniName;
BOOL bRet;

if (*szIniPath)
	{
	lstrcpy(szIniName, szIniPath);
	lstrcat(szIniName, lpIniFile);
	lstrcpy(lpIniFile, szIniName);
	return(FileExists(lpIniFile));
	}

if ( GetWindowsDirectory( szFileName, sizeof(FNAME) ) > 2)
	{
	FixPath( szFileName );
	lstrcat( szFileName, MGX_Ini );
	if ( FileExists( szFileName ) )
		{
		if ( GetPrivateProfileString( Mgxi_Str, Private_Ini, Empty_Str,
					szIniName, sizeof(szIniName), szFileName ) > 2 )
			{
				FixPath( szIniName );
				lstrcat( szIniName,  lpIniFile);
				if ( bRet = FileExists( szIniName ) )
					goto Found;
			}
		}
	}

lstrcpy( szIniName, Control.ProgHome );
lstrcat( szIniName, lpIniFile );
if ( !(bRet = FileExists( szIniName )) )
	Message( IDS_INI_NOEXIST, Lowercase(szIniName) );

Found:
lstrcpy( lpIniFile, szIniName );

if ( bRet && !FilePermission( lpIniFile, 2 /*WRITE*/ ) )
	Message( IDS_INI_NOWRITE, Lowercase(lpIniFile) );

// Set the network variable if the INI and EXE are not in the same place
lstrcpy( szFileName, lpIniFile );
stripfile( szFileName );
FixPath( szFileName );
lstrcpy(szIniPath, szFileName);

Control.bNetworkInstall = !StringsEqual( Control.ProgHome, szFileName );
return( bRet );
}

/************************************************************************/
int GetDefaultInt( LPTSTR lpKeyName, int nDefault )
/************************************************************************/
{
#if defined(REGISTRY_INI)

return((int)PictPubApp.GetProfileInt(ID_SETTINGS, stripstruct(lpKeyName), nDefault));

#elif defined(PRIVATE_INI)
LPTSTR lp;
STRING tmp;

if ( lp = FindPreference( stripstruct(lpKeyName) ) )
	{
	lstrcpy( tmp, lp );
	return( atoi( tmp ) );
	}
else return( GetPrivateProfileInt( ID_APPL, stripstruct(lpKeyName),
	nDefault, szPreferenceFile ) );

#else
return( GetProfileInt( ID_APPL, stripstruct(lpKeyName), nDefault ) );
#endif
}

/************************************************************************/
WORD GetDefaultString( LPTSTR lpKeyName, LPTSTR lpDefault, LPTSTR lpReturnedString,
			int nSize)
/************************************************************************/
{
#if defined(REGISTRY_INI)

CString szReturn = PictPubApp.GetProfileString(ID_SETTINGS, stripstruct(lpKeyName), lpDefault);
lstrcpy(lpReturnedString, szReturn);
return(szReturn.GetLength());

#elif defined(PRIVATE_INI)
LPTSTR lp;
int length;

if ( lp = FindPreference( stripstruct(lpKeyName) ) )
	{
	length = min( lstrlen(lp), nSize-1 );
	copy( (unsigned char *)lp, (unsigned char *)lpReturnedString, length );
	*(lpReturnedString+length) = '\0';
	return( length+1 );
	}
else return( GetPrivateProfileString( ID_APPL, stripstruct(lpKeyName),
	lpDefault, lpReturnedString, nSize, szPreferenceFile ) );

#else
return( GetProfileString( ID_APPL, stripstruct(lpKeyName), lpDefault,
	lpReturnedString, nSize ) );
#endif
}

/************************************************************************/
BOOL PutDefaultInt( LPTSTR lpKeyName, int nDefault )
/************************************************************************/
{
#if defined(REGISTRY_INI)
return(PictPubApp.WriteProfileInt(ID_SETTINGS, stripstruct(lpKeyName), nDefault));
#else
char tmp[40];

return( PutDefaultString( lpKeyName, itoa(nDefault,tmp,10) ) );
#endif
}

/************************************************************************/
BOOL PutDefaultString( LPTSTR lpKeyName, LPTSTR LPTSTRing)
/************************************************************************/
{
#if defined(REGISTRY_INI)
return(PictPubApp.WriteProfileString(ID_SETTINGS, stripstruct(lpKeyName), LPTSTRing));
#elif defined(PRIVATE_INI)
LPTSTR lp;

return( WritePrivateProfileString( ID_APPL, stripstruct(lpKeyName),
	LPTSTRing, szPreferenceFile ));
#else
return( WriteProfileString( ID_APPL, stripstruct(lpKeyName), LPTSTRing ));
#endif
}

/************************************************************************/
BOOL PutPrivateInt( LPTSTR lpszProfile, LPTSTR lpszSection, LPTSTR lpszKey, int nDefault )
/************************************************************************/
{
    char tmp[40];

    return( PutPrivateString( lpszProfile, lpszSection, lpszKey, itoa(nDefault,tmp,10) ) );
}

/************************************************************************/
BOOL PutPrivateString( LPTSTR lpszProfile, LPTSTR lpszSection, LPTSTR lpszKey, LPTSTR lpszString )
/************************************************************************/
{
    return( WritePrivateProfileString( lpszSection, stripstruct(lpszKey),
	    lpszString, lpszProfile ));
}

/************************************************************************/
void LoadAllPreferences( BOOL fMemOnly )
/************************************************************************/
{
LPTSTR lp;
int i, English, Didot;
STRING tmp;
char t[50];

if (fMemOnly) // memory related info only
	{
	// Memory Group Preferences
    GetDefaultString(RamDisk_Str,Control.ProgHome,Control.RamDisk,sizeof(Control.RamDisk));
	FixPath( Lowercase( Control.RamDisk ) );
    Control.MainMemMin = GetDefaultInt(MainMemMin_Str, 1536);
    Control.MainMemFactor=GetDefaultInt(MainMemFactor_Str, 75);
	if (  Control.MainMemFactor < 10 )
		Control.MainMemFactor = 10;
	if (  Control.MainMemFactor > 100 )
		Control.MainMemFactor = 100;
    Control.LineArtAsGray = GetDefaultInt(LineArtAsGray_Str, NO);
    Control.UseThreading=GetDefaultInt(UseThreading_Str, YES);
#ifdef WIN32
	if (Control.WinVer == WV_WIN32S)
		Control.UseThreading = NO;
#else
	Control.UseThreading = NO;
#endif

	FrameSettings( Control.RamDisk, Control.MainMemMin, Control.MainMemFactor);

	Control.UseGrayScaleIcons=GetDefaultInt(UseGrayScaleIcons_Str,NO);

	// Hint Settings
	Hints.fBubbleHintsOn=GetDefaultInt(fBubbleHintsOn_Str,YES);
	Hints.fStatusHintsOn=GetDefaultInt(fStatusHintsOn_Str,YES);
	Hints.fHintWindowOn=GetDefaultInt(fHintWindowOn_Str,YES);

    // Unload Wintab DLL
    Control.bUnlinkWintab=GetDefaultInt(fUnlinkWintab_Str,NO);

	return;
	}

AstralCursor( IDC_WAIT );

#ifdef PARSE_INMEMORY
lpPreferences = LoadPreferences( szPreferenceFile );
#endif //PARSE_INMEMORY

// Retouch global
RetouchBrushCurrent = NO_BRUSH;

// Recall List
Control.RecallCount = 0;
for ( i=0; i<MAX_RECALL; i++ )
	{
	GetDefaultString( itoa(i,tmp,10), Empty_Str,
		Control.RecallImage[i], sizeof(Control.RecallImage[i]) );
	if ( *Control.RecallImage[i] )
		Control.RecallCount++;
	}

// Scratchpad Group Preferences
Control.ScratchpadWidth=GetDefaultInt(ScratchpadWidth_Str,100);
Control.ScratchpadHeight=GetDefaultInt(ScratchpadHeight_Str,200);
Control.IsScratchpadColor=GetDefaultInt(IsScratchpadColor_Str,YES);

// Undo Group Preferences
Control.NoUndo=GetDefaultInt(NoUndo_Str,FALSE);
Control.UseApply=GetDefaultInt(UseApply_Str,NO);
if( Control.Retail )
    Control.UndoObjects=GetDefaultInt(UndoObjects_Str,NO);
else
    Control.UndoObjects=NO;

Control.UndoMasks=GetDefaultInt(UndoMasks_Str,YES);

// Miscellaneous Group Preferences
PictPubApp.GetLocalProfileString(PouchPath_Str,Control.ProgHome,Control.PouchPath,sizeof(Control.PouchPath));
FixPath( Lowercase( Control.PouchPath ) );
Control.UseTablet=GetDefaultInt(UseTablet_Str,0);
Control.MaskTint=GetDefaultInt(MaskTint_Str,0); // Red
Control.IndependentSettings=GetDefaultInt(IndependentSettings_Str,YES);
Control.DisableAutoScroll=GetDefaultInt(DisableAutoScroll_Str,NO);

if( Control.Retail )
    Control.bUseCMS=GetDefaultInt(ColorManagerEnabled_Str,NO);
else
    Control.bUseCMS = NO;

Vignette.DisableDither=GetDefaultInt(DisableDither_Str,NO);

// Units Group Preferences
English = GetProfileInt( Intl_Str, Measure_Str, 1 ); // 1 = English
Didot=GetDefaultInt(Didot_Str,-1);
if ( Didot >= 0 ) // If Didot is overridden...
	Control.Points = ( !Didot ? 723 : 676 );
else	Control.Points = ( English ? 723 : 676 );
Control.Units=GetDefaultInt(Units_Str,!English);
Control.Units += IDC_PREF_UNITINCHES;
GetDefaultString(ScreenWidth_Str,FixedAscii(DBL2FIX(9.0), t, 4),t,sizeof(t));
Control.ScreenWidth=AsciiFixed(t);
View.UseRulers=GetDefaultInt(UseRulers_Str,NO);
Control.DisplayPixelValue=GetDefaultInt(DisplayPixelValue_Str,NO);
SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITFIRST), Control.UnitRes, Control.Points);

// Plugin Group Preferences
GetDefaultString(PluginPath1_Str,Empty_Str,Control.PluginPath1,sizeof(Control.PluginPath1));
FixPath( Lowercase( Control.PluginPath1 ) );
GetDefaultString(PluginPath2_Str,Empty_Str,Control.PluginPath2,sizeof(Control.PluginPath2));
FixPath( Lowercase( Control.PluginPath2 ) );
Control.UsePlugins=GetDefaultInt(UsePlugins_Str,NO);

// Photo CD CMS option.
if( Control.Retail )
    Control.fPCDUseCMS=GetDefaultInt(fPCDUseCMS_Str,NO);
else
    Control.fPCDUseCMS = NO;

// Object Group Preferences
if( Control.Retail )
    Control.MultipleObjects=GetDefaultInt(MultipleObjects_Str,YES);
else
    Control.MultipleObjects=NO;

if( Control.Retail )
    Control.UseMaskAndObjects=GetDefaultInt(UseMaskAndObjects_Str,YES);
else
    Control.UseMaskAndObjects=NO;

// Macro
Macro.nRepeat=GetDefaultInt(nRepeat_Str,1);
Macro.fJournalMacro=GetDefaultInt(fJournalMacro_Str,NO);
Macro.fBatchClose=GetDefaultInt(fBatchClose_Str,YES);
Macro.BatchSave=GetDefaultInt(BatchSave_Str,IDC_NOSAVE-IDC_BATCHSAVEFIRST);
Macro.BatchSave += IDC_BATCHSAVEFIRST;
GetDefaultString(BatchAlbum_Str,Empty_Str,Macro.BatchAlbum,sizeof(Macro.BatchAlbum));
GetDefaultString(BatchDir_Str,Empty_Str,Macro.BatchDir,sizeof(Macro.BatchDir));
Macro.fBatchChangeType = GetDefaultInt(fBatchChangeType_Str,FALSE);
Macro.BatchFileType = GetDefaultInt(BatchFileType_Str,IDN_TIFF-IDN_FIRSTFILETYPE);
Macro.BatchFileType += IDN_FIRSTFILETYPE;

// New Dialog
Control.NewDepth=GetDefaultInt(NewDepth_Str,1);
GetDefaultString(NewWidth_Str,FixedAscii(DBL2FIX(4.0), t, 4),t,sizeof(t)); 
Control.NewWidth=AsciiFixed(t);
GetDefaultString(NewHeight_Str,FixedAscii(DBL2FIX(5.0), t, 4),t,sizeof(t));
Control.NewHeight=AsciiFixed(t);
Control.NewResolution=GetDefaultInt(NewResolution_Str,100);

// Monitor Gamma Dialog
Control.GammaLock=GetDefaultInt(GammaLock_Str,NO);

// Clipboard settings
Control.DoPicture=GetDefaultInt(DoPicture_Str,NO);
Control.DoBitmap=GetDefaultInt(DoBitmap_Str,YES);

// Miscellaneous
Control.ScreenColor=GetDefaultInt(ScreenColor_Str,YES);
GetDefaultString(rgbBackground_Str,Zeros3_Str,t,sizeof(t));
AsciiRGB(t,&Control.rgbBackground);
Control.xImage=GetDefaultInt(xImage_Str,4);
Control.yImage=GetDefaultInt(yImage_Str,4);
Control.Hints=GetDefaultInt(Hints_Str,YES);
Control.Save24BitWallpaper=GetDefaultInt(Save24BitWallpaper_Str,FALSE);

if( Control.Retail )
    Control.UseObjectMarquee=GetDefaultInt(UseObjectMarquee_Str,YES);
else
    Control.UseObjectMarquee=NO;

Control.DefaultFileType=GetDefaultInt(DefaultFileType_Str,IDN_TIFF-IDN_FIRSTFILETYPE); // TIFF
Control.DefaultFileType += IDN_FIRSTFILETYPE;
GetDefDitherType( Control.DefaultDitherType, DT_SCATTER );
Control.UseFunctionThreading=GetDefaultInt(UseFunctionThreading_Str,YES);
Control.NTBeta=GetDefaultInt(NTBeta_Str,NO);
if( Control.Retail )
    Control.DoCommandList=GetDefaultInt(DoCommandList_Str,YES);
else
    Control.DoCommandList=NO;

Control.UseShortCommandNames=GetDefaultInt(UseShortCommandNames_Str,YES);
Control.UseFileForClipboard=GetDefaultInt(UseFileForClipboard_Str,NO);
Control.WriteFileOnPaste=GetDefaultInt(WriteFileOnPaste_Str,YES);
//Control.UseProjectDirectory=GetDefaultInt(UseProjectDirectory_Str,YES);
Control.UseProjectDirectory=FALSE;
Control.UseBrushEditColor = GetDefaultInt( UseBrushEditColor_Str, YES );
Control.TGACMYK = GetDefaultInt( TGACMYK_Str, 0 );
Control.bNoHiResRebuild=GetDefaultInt(NoHiResRebuild_Str,NO);

// Browser info
LoadBrowserPrefs();
LoadBrowserState();
Browser.bPTRows=GetDefaultInt(bPTRows_Str,YES);
Browser.PTNumRows=GetDefaultInt(PTNumRows_Str,3);
Browser.PTNumCols=GetDefaultInt(PTNumCols_Str,3);

// File Save Options
LoadSaveOptions();

// Mapping Group Preferences
Mapping.Map_bAuto=GetDefaultInt(Map_bAuto_Str,YES);
Mapping.Map_nPoints=GetDefaultInt(Map_nPoints_Str,3);
Mapping.Map_bGrid=GetDefaultInt(Map_bGrid_Str,YES);
Mapping.Map_bPercent=GetDefaultInt(Map_bPercent_Str,YES);
Mapping.Map_bIntensity=GetDefaultInt(Map_bIntensity_Str,NO);
Mapping.Map_bCurves=GetDefaultInt(Map_bCurves_Str,YES);
Mapping.Map_CurveType=GetDefaultInt(Map_CurveType_Str,SMOOTH_NORMAL);
Mapping.HueShift_bAuto=GetDefaultInt(HueShift_bAuto_Str,YES);
Mapping.HueMap_bAuto=GetDefaultInt(HueMap_bAuto_Str,YES);
Mapping.Poster_bAuto=GetDefaultInt  (Poster_bAuto_Str,YES);
Mapping.ContBrit_bAuto=GetDefaultInt(ContBrit_bAuto_Str,YES);
Mapping.Balance_bAuto=GetDefaultInt(Balance_bAuto_Str,YES);
Mapping.Quarter_bAuto=GetDefaultInt(Quarter_bAuto_Str,YES);
Mapping.Quarter_HSacrifice=GetDefaultInt(Quarter_HSacrifice_Str,5);
Mapping.Quarter_SSacrifice=GetDefaultInt(Quarter_SSacrifice_Str,5);
Mapping.Quarter_bUseMidPer=GetDefaultInt(Quarter_bUseMidPer_Str,YES);
Mapping.Quarter_bPercent=GetDefaultInt(Quarter_bPercent_Str,NO);


// Extended Names
GetDefaultString(ColorMap_Str,Empty_Str,Names.ColorMap,sizeof(Names.ColorMap));
GetDefaultString(Special_Str,Empty_Str,Names.Special,sizeof(Names.Special));
GetDefaultString(Mask_Str,Empty_Str,Names.Mask,sizeof(Names.Mask));
GetDefaultString(Scanner_Str,Empty_Str,Names.Scanner,sizeof(Names.Scanner));
GetDefaultString(ScanMap_Str,Empty_Str,Names.ScanMap,sizeof(Names.ScanMap));
GetDefaultString(Printer_Str,Empty_Str,Names.Printer,sizeof(Names.Printer));
GetDefaultString(PrintStyle_Str,Empty_Str,Names.PrintStyle,sizeof(Names.PrintStyle));
GetDefaultString(Clipboard_Str,Empty_Str,Names.Clipboard,sizeof(Names.Clipboard));
GetDefaultString(CustomBrush_Str,Empty_Str,Names.CustomBrush,sizeof(Names.CustomBrush));
GetDefaultString(MainManifest_Str,BDrive_Str,Names.MainManifest,sizeof(Names.MainManifest));
GetDefaultString(SupManifest_Str,BDrive_Str,Names.SupManifest,sizeof(Names.SupManifest));
GetDefaultString(InstallerDisk_Str,BDrive_Str,Names.InstallerDisk,sizeof(Names.InstallerDisk));
GetDefaultString(ColorManager_Str,Empty_Str,Names.ColorManager,sizeof(Names.ColorManager));
GetDefaultString(Macro_Str,Empty_Str,Names.Macro,sizeof(Names.Macro));

if( Control.Retail )
    GetDefaultString(StartMacro_Str,Empty_Str,Names.StartMacro,sizeof(Names.StartMacro));
else
    Names.StartMacro[0] = '\0';

GetDefaultString(Shape_Str,Empty_Str,Names.Shape,sizeof(Names.Shape));
GetDefaultString(PackageDir_Str,Empty_Str,Names.szPackageDir,sizeof(Names.szPackageDir));

// Print Style
LoadPrintStyle( Names.PrintStyle );

// Print Dialog
Page.Type=GetDefaultInt(Type_Str,1);
Page.Type += IDC_PRINTER_IS_MONO;
Page.PrintNumeric=GetDefaultInt(PrintNumeric_Str,NO);
Page.OutputType=GetDefaultInt(OutputType_Str,0);
Page.OutputType += IDC_PRINT_GRAY;
Page.ScatterPrint=GetDefaultInt(ScatterPrint_Str,NO);
Page.Centered=GetDefaultInt(Centered_Str,YES);
GetDefaultString(OffsetX_Str,FixedAscii(DBL2FIX(0.0), t, 4),t,sizeof(t)); 
Page.OffsetX=AsciiFixed(t);
GetDefaultString(OffsetY_Str,FixedAscii(DBL2FIX(0.0), t, 4),t,sizeof(t));
Page.OffsetY=AsciiFixed(t);
Page.SepFlags=GetDefaultInt(SepFlags_Str,0);
Page.TrimMarks=GetDefaultInt(TrimMarks_Str,NO);
Page.RegMarks=GetDefaultInt(RegMarks_Str,NO);
Page.Labels=GetDefaultInt(Labels_Str,NO);
Page.StepScale=GetDefaultInt(StepScale_Str,NO);
Page.Negative=GetDefaultInt(Negative_Str,NO);
Page.EmulsionDown=GetDefaultInt(EmulsionDown_Str,NO);
Page.BinaryPS=GetDefaultInt(BinaryPS_Str,NO); // Binary Postscript
Page.BinaryEPS=GetDefaultInt(BinaryEPS_Str,NO); // Binary Encapsulated Postscript
Page.PrintScaling=GetDefaultInt(PrintScaling_Str,YES); // Do print time image scaling
Page.UsePrinterScreening=GetDefaultInt(UsePrinterScreening_Str,YES); // Use GDI Printer Screening
Control.fPrintCalVisual=GetDefaultInt(fPrintCalVisual_Str,NO); // use visual scanner calibration
Page.GeneratePS=GetDefaultInt(GeneratePS_Str,YES);

// Scanner Setup/Scan Dialogs
Scans.LockAspect=GetDefaultInt(LockAspect_Str,NO);
Scans.DoCalibration=GetDefaultInt(DoCalibration_Str,NO);
Scans.ColorPreview=GetDefaultInt(ColorPreview_Str,NO);
LoadScanCalibrate(Names.ScanMap);
Scans.ScanLAInvert=GetDefaultInt(ScanLAInvert_Str,NO); // invert line art scans

if( Control.Retail )
    Scans.fScanUseCMS=GetDefaultInt(fScanUseCMS_Str,NO); // use CMS for scanning
else
    Scans.fScanUseCMS = NO;

Scans.fScanCalVisual=GetDefaultInt(fScanCalVisual_Str,NO); // use visual scanner calibration

//GetDefHex( SCinit.PortAddr, 0x278 );
GetDefHex( Control.ScanMapMode, 0);
//SCatt.BitsPerPixel=GetDefaultInt(BitsPerPixel_Str,8);
//SCatt.Resolution=GetDefaultInt(Resolution_Str,150);
//SCatt.ScanSpeed=GetDefaultInt(ScanSpeed_Str,5);
//SCatt.UseDMA=GetDefaultInt(UseDMA_Str,YES);
//SCatt.Frame.left=GetDefaultInt(Frameleft_Str,1000);
//SCatt.Frame.top=GetDefaultInt(Frametop_Str,1000);
//SCatt.Frame.right=GetDefaultInt(Frameright_Str,7000);
//SCatt.Frame.bottom=GetDefaultInt(Framebottom_Str,9000);

// Size Dialog
Edit.SmartSize=GetDefaultInt(SmartSize_Str,NO);

// Feather Mask/Object Dialog
Mask.wFeatherEdge=GetDefaultInt(wFeatherEdge_Str,1);
Mask.FeatherEdgeSoft=GetDefaultInt(FeatherEdgeSoft_Str,1);
Mask.FeatherEdgeNormal=GetDefaultInt(FeatherEdgeNormal_Str,50);
Mask.FeatherEdgeHard=GetDefaultInt(FeatherEdgeHard_Str,100);
Mask.FeatherAmount=GetDefaultInt(FeatherAmount_Str,3);
Mask.FeatherInside=GetDefaultInt(FeatherInside_Str,YES);

// Load Mask Dialog
Mask.DeleteMask=GetDefaultInt(DeleteMask_Str,YES);

Mask.OneBitMask=GetDefaultInt(OneBitMask_Str,1);

// Color Shield
ColorMask.ProbeMethod=GetDefaultInt(ProbeMethod_Str,0);
ColorMask.ProbeMethod += IDC_PROBEPOINT;
ColorMask.IfHidden=GetDefaultInt(IfHidden_Str,YES);
ColorMask.ShieldMethod=GetDefaultInt(ShieldMethod_Str,1);
ColorMask.ShieldMethod += IDC_PROBEPOINT;
ColorMask.MaskShield=GetDefaultInt(MaskShield_Str,0); // Which is selected for the tool
ColorMask.Mask=GetDefaultInt(ColorMask_Str,0); // What's active
ColorMask.Include=GetDefaultInt(Include_Str,YES);
ColorMask.On=GetDefaultInt(On_Str,NO);
// do from 0 to NUM_COLORMASKS
ColorMask.MaskRange[0]=GetDefaultInt(MaskRange0_Str,10);
ColorMask.MaskRange[1]=GetDefaultInt(MaskRange1_Str,10);
ColorMask.MaskRange[2]=GetDefaultInt(MaskRange2_Str,10);
ColorMask.MaskRange[3]=GetDefaultInt(MaskRange3_Str,10);
ColorMask.MaskRange[4]=GetDefaultInt(MaskRange4_Str,10);
ColorMask.MaskRange[5]=GetDefaultInt(MaskRange5_Str,10);
ColorMask.MaskRange[6]=GetDefaultInt(MaskRange6_Str,10);
ColorMask.MaskRange[7]=GetDefaultInt(MaskRange7_Str,10);
// do from 0 to NUM_COLORMASKS
GetDefaultString(MaskColor0rgb_Str,RGB_Str,t,sizeof(t)); AsciiRGB(t,&ColorMask.MaskColor[0].rgb);
GetDefaultString(MaskColor1rgb_Str,RGB_Str,t,sizeof(t)); AsciiRGB(t,&ColorMask.MaskColor[1].rgb);
GetDefaultString(MaskColor2rgb_Str,RGB_Str,t,sizeof(t)); AsciiRGB(t,&ColorMask.MaskColor[2].rgb);
GetDefaultString(MaskColor3rgb_Str,RGB_Str,t,sizeof(t)); AsciiRGB(t,&ColorMask.MaskColor[3].rgb);
GetDefaultString(MaskColor4rgb_Str,RGB_Str,t,sizeof(t)); AsciiRGB(t,&ColorMask.MaskColor[4].rgb);
GetDefaultString(MaskColor5rgb_Str,RGB_Str,t,sizeof(t)); AsciiRGB(t,&ColorMask.MaskColor[5].rgb);
GetDefaultString(MaskColor6rgb_Str,RGB_Str,t,sizeof(t)); AsciiRGB(t,&ColorMask.MaskColor[6].rgb);
GetDefaultString(MaskColor7rgb_Str,RGB_Str,t,sizeof(t)); AsciiRGB(t,&ColorMask.MaskColor[7].rgb);

// Stitch settings
Stitch.AutoBrightness=GetDefaultInt(AutoBrightness_Str,YES);
Stitch.AutoBlend=GetDefaultInt(AutoBlend_Str,YES);
Stitch.Pressure=GetDefaultInt(SPressure_Str,0);
Stitch.Pressure = bound(Stitch.Pressure, 0, 255);
Stitch.SmartStitch=GetDefaultInt(SmartStitch_Str,0);
Stitch.iTransform=GetDefaultInt(iTransform_Str,0);

// Convert settings
Convert.MiniType=GetDefaultInt(MiniType_Str,0);
Convert.MiniType += IDC_MINI256;
Convert.fOptimize=GetDefaultInt(fOptimize_Str,YES);
Convert.fScatter=GetDefaultInt(fScatter_Str,NO);
Convert.fDither=GetDefaultInt(fDither_Str,YES);
GetDefDitherType( Convert.ConvertDitherType, DT_SCATTER);
GetDefConvertType( Convert.ConvertType, CT_OPTIMIZED);
GetDefaultString(ConvertPalette_Str,Empty_Str,Convert.ConvertPalette,sizeof(Convert.ConvertPalette));
Convert.ConvertColors=GetDefaultInt(ConvertColors_Str,256);

// Stroke Mask Dialog
Mask.StrokeTool=GetDefaultInt(StrokeTool_Str,0);
HandleBrushSettings( IDC_STROKEMASK, NO );
Mask.StrokeTool += IDC_STROKEFIRST;

// Selector Tool

// Shape Mask Tool
Mask.iShapeModes=GetDefaultInt(iShapeModes_Str,SHAPE_DEFAULT);
Mask.DoMaskCircle=GetDefaultInt(DoMaskCircle_Str,0);
Mask.ConstrainRectAspect=GetDefaultInt(ConstrainRectAspect_Str,0);
Mask.ConstrainRectAspect += IDC_ASPECTFIRST;
GetDefaultString(ConstrainRectWidth_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t));
Mask.ConstrainRectWidth=AsciiFixed(t);
GetDefaultString(ConstrainRectHeight_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t));
Mask.ConstrainRectHeight=AsciiFixed(t);
Mask.ShapeUnits=GetDefaultInt(ShapeUnits_Str,!English);
Mask.ShapeUnits += IDC_PREF_UNITINCHES;

// Freehand Mask Tool
Mask.FreeAntialias=GetDefaultInt(FreeAntialias_Str,NO);
Mask.iFreeModes=GetDefaultInt(iFreeModes_Str,SHAPE_DEFAULT);
Mask.AutoMask=GetDefaultInt(AutoMask_Str,NO);
Mask.PointEdit=GetDefaultInt(PointEdit_Str,NO);
Mask.idAutoColorModel=GetDefaultInt(idAutoColorModel_Str,NO);
Mask.AutoSearchDepth=GetDefaultInt(AutoSearchDepth_Str,3);
Mask.AutoMinEdge=GetDefaultInt(AutoMinEdge_Str,3);

// PaintOn Mask Tool
Mask.iPaintOnModes=GetDefaultInt(iPaintOnModes_Str,SHAPE_DEFAULT);
Mask.PaintOnObject=GetDefaultInt(PaintOnObject_Str,NO);
HandleBrushSettings( IDC_PAINTON, NO );

// Magic Wand Mask Tool
Mask.iWandModes=GetDefaultInt(iWandModes_Str,SHAPE_DEFAULT);
Mask.WandRange=GetDefaultInt(WandRange_Str,15);
Mask.WandFade=GetDefaultInt(WandFade_Str,0);
Mask.idMagicColorModel=GetDefaultInt(idMagicColorModel_Str,0);
Mask.GrowAmount=GetDefaultInt(GrowAmount_Str,1);

// Mask Transformer Tool
Mask.TransformImage=GetDefaultInt(TransformImage_Str,1);
Mask.TransformImage = Mask.TransformImage + IDC_TRANSFUNCTIONFIRST;

// Object Transformer Tool
Mask.iTransformModes=GetDefaultInt(iTransformModes_Str,SHAPE_DEFAULT);
Mask.PasteQuality=GetDefaultInt(PasteQuality_Str,YES);
Mask.TransMode=GetDefaultInt(TransMode_Str,0);
Mask.TransMode += IDC_TRANSMODEFIRST;
Mask.TransformOpacity=GetDefaultInt(TransformOpacity_Str,255);
Mask.TransformOpacity = bound( Mask.TransformOpacity, 0, 255 );
GetDefMergeMode( Mask.TransformMergeMode, MM_NORMAL );
Mask.PasteAtRealSize=GetDefaultInt(PasteAtRealSize_Str,NO);
Mask.DisplayQuality=GetDefaultInt(DisplayQuality_Str,NO);

// Mask Pointer Editor Tool
Mask.PointIModes=GetDefaultInt(PointIModes_Str,SHAPE_DEFAULT);
Mask.PointAntialias=GetDefaultInt(PointAntialias_Str,NO);
Mask.PointerFunction=GetDefaultInt(PointerFunction_Str,0);
Mask.PointerFunction += IDC_MOVEPOINTS;
Mask.MakePathTolerance=GetDefaultInt(MakePathTolerance_Str,2);
Mask.MakePathContinuity=GetDefaultInt(MakePathContinuity_Str,20);
Mask.MakePathCurveTolerance=GetDefaultInt(MakePathCurveTolerance_Str,1);
Mask.MakePathType=GetDefaultInt(MakePathType_Str,1)+IDC_PATH_LINES;

// Crop Tool
Mask.ConstrainCropAspect=GetDefaultInt(ConstrainCropAspect_Str,0);
Mask.ConstrainCropAspect += IDC_ASPECTFIRST;
GetDefaultString(ConstrainCropWidth_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t));
Mask.ConstrainCropWidth=AsciiFixed(t);
GetDefaultString(ConstrainCropHeight_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t));
Mask.ConstrainCropHeight=AsciiFixed(t);
Mask.CropUnits=GetDefaultInt(CropUnits_Str,!English);
Mask.CropUnits += IDC_PREF_UNITINCHES;

// Paint Tool
HandleBrushSettings( IDC_PAINT, NO );

// Airbrush Tool
HandleBrushSettings( IDC_SPRAY, NO );

// Clone Tool
HandleBrushSettings( IDC_CLONE, NO );

// Texture Tool
HandleBrushSettings( IDC_TEXTURE, NO );

// Smear Tool
HandleBrushSettings( IDC_SMEAR, NO );

// Eraser Tool
HandleBrushSettings( IDC_ERASER, NO );

// Sharpen Tool 
HandleBrushSettings( IDC_SHARP, NO );

// Smooth Tool
HandleBrushSettings( IDC_SMOOTH, NO );

// Lighten Tool
HandleBrushSettings( IDC_LIGHTEN, NO );

// Darken Tool
HandleBrushSettings( IDC_DARKEN, NO );

// Gradient Tool
Vignette.Gradient=GetDefaultInt(Gradient_Str,0);
Vignette.Gradient += IDC_VIGLINEAR;
Vignette.RepeatCount=GetDefaultInt(RepeatCount_Str,1);
Vignette.SoftTransition=GetDefaultInt(SoftTransition_Str,NO);
Vignette.VigColorModel=GetDefaultInt(VigColorModel_Str,0);
Vignette.VigOpacity=GetDefaultInt(VigOpacity_Str,255);
GetDefMergeMode( Vignette.VigMergeMode, MM_NORMAL );
Vignette.Midpoint=GetDefaultInt(Midpoint_Str,128);

// Texture Fill Tool
GetDefaultString(TextureName_Str,Empty_Str,Texture.TextureName,sizeof(Texture.TextureName));
Texture.fHorzFlip=GetDefaultInt(fHorzFlip_Str,NO);
Texture.fVertFlip=GetDefaultInt(fVertFlip_Str,NO);
Texture.TextureOpacity=GetDefaultInt(TextureOpacity_Str,255);
GetDefMergeMode( Texture.TextureMergeMode, MM_NORMAL );

// Tint Fill Tool
Fill.FillOpacity=GetDefaultInt(FillOpacity_Str,255);
GetDefMergeMode( Fill.FillMergeMode, MM_NORMAL );

// Magic Fill Tool
Fill.FloodRange=GetDefaultInt(FloodRange_Str,2);
Fill.FloodFade=GetDefaultInt(FloodFade_Str,0);
Fill.idFillColorModel=GetDefaultInt(idFillColorModel_Str,0);
Fill.FloodOpacity=GetDefaultInt(FloodOpacity_Str,255);
GetDefMergeMode( Fill.FloodMergeMode, MM_NORMAL );

// Shape Draw Tool
Draw.wSquareStyle=GetDefaultInt(wSquareStyle_Str,0);
Draw.wSquareStyle += IDC_SQUEDGE;
Draw.DoDrawCircle=GetDefaultInt(DoDrawCircle_Str,NO);
HandleBrushSettings( IDC_DRAWSQUARE, NO );

// Freehand Draw Tool
Draw.wFreeStyle=GetDefaultInt(wFreeStyle_Str,0);
HandleBrushSettings( IDC_DRAWFREE, NO );

// Pencil Draw Tool
HandleBrushSettings( IDC_PENCIL, NO );

// Custom View Tool
View.ZoomOut=GetDefaultInt(ZoomOut_Str,NO);
View.ZoomWindow=GetDefaultInt(ZoomWindow_Str,NO);
GetDefaultString(ZoomDefaultSize_Str,FixedAscii(DBL2FIX(.25), t, 4),t,sizeof(t));
View.ZoomDefaultSize=AsciiFixed(t);
View.FullScreen=GetDefaultInt(FullScreen_Str,NO);

// Text Tool
TextForum.Load();
// Palette Edit Dialog
PaletteSettings.fReMap=GetDefaultInt(fReMap_Str,TRUE);
GetDefDitherType( PaletteSettings.DitherType, DT_SCATTER);
	   
// Load Common Tool Settings
Common.iModes=GetDefaultInt(iModes_Str,SHAPE_DEFAULT);
Common.Opacity=GetDefaultInt(Opacity_Str,255);
GetDefMergeMode( Common.MergeMode, MM_NORMAL );
Common.idColorModel=GetDefaultInt(idColorModel_Str,0);
Common.Range=GetDefaultInt(Range_Str,2);
GetDefaultString(Width_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t));
Common.Width=AsciiFixed(t);
GetDefaultString(Height_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t)); 
Common.Height=AsciiFixed(t);
Common.Constrain=GetDefaultInt(Constrain_Str,0);
Common.Constrain += IDC_ASPECTFIRST;
Common.BrushShape=GetDefaultInt(BrushShape_Str,0);
Common.BrushShape += IDC_BRUSHCIRCLE;
Common.BrushSize=GetDefaultInt(BrushSize_Str,15);
Common.Pressure=GetDefaultInt(Pressure_Str,64);
Common.Feather=GetDefaultInt(Feather_Str,100);
Common.DoCircle=GetDefaultInt(DoCircle_Str,NO);
Common.CommonUnits=GetDefaultInt(CommonUnits_Str,!English);
Common.CommonUnits += IDC_PREF_UNITINCHES;

SetCommonSettings();

LoadFileLocations();
AstralCursor( NULL );

#ifdef PARSE_INMEMORY
    if (lpPreferences)
    {
        FreeUp(lpPreferences);
        lpPreferences = NULL;
    }
#endif //PARSE_INMEMORY

}

/************************************************************************/
LPTSTR LoadPreferences( LPTSTR lpFileName )
/************************************************************************/
{
int ifh;
LPTSTR lp, lpDefaults, lpNew;
BYTE c;
long lCount, lBufferSize;
BOOL fCopyingName;

// Load the defaults file into a block of memory
// comprised of pairs of strings - keyname and value

/* open the defaults file */
if ( ( ifh = _lopen( (const char *)lpFileName, OF_READ)) < 0 )
	return( NULL );

lCount = 8192;
if ( !(lpDefaults = (LPTSTR)Alloc( lCount )) )
	return( NULL );

// Read the entire file into the buffer lpDefaults
lCount = _lread( ifh, lpDefaults, (WORD)lCount );

lp = lpDefaults;
lpNew = lp;
fCopyingName = YES;
while ( --lCount >= 0 )
	{ // Break the memory block up into pairs of strings
	c = *lp++;
	if ( fCopyingName )
		{
		if ( c == '[' )
			{ // Skip over any [Names] in brackets
			while ( --lCount >= 0 )
				{
				if ( *lp++ == ']' )
					break;
				}
			continue;
			}
		if ( c == '=' )
			{ // When we hit an equal sign, we change modes
			fCopyingName = NO;
			c = '\0';
			}
		else
		if ( c < ' ' || c > 128 ) // Skip over any NULLs and non-ascii
			continue;
		else
		if ( c == ' ' ) // Skip over any spaces
			continue;
		c = tolower(c);
		}
	else
	if ( !fCopyingName )
		{
		if ( c == '\r' || c == '\n' )
			{ // When we hit a "return", we change modes
			fCopyingName = YES;
			c = '\0';
			}
		else
		if ( c < ' ' || c > 128 ) // Skip over any NULLs and non-ascii
			continue;
		}
	*lpNew++ = c;
	}

*lpNew++ = '\0'; // Add a few nulls to terminate
*lpNew++ = '\0';
*lpNew++ = '\0';

_lclose( ifh );
return( lpDefaults );
}

/************************************************************************/
LPTSTR FindPreference( LPTSTR lpKeyName )
/************************************************************************/
{
LPTSTR lp;

if ( !(lp = lpPreferences) )
	return( NULL );
while ( *lp )
	{
	if ( StringsEqual( (LPTSTR)lp, (LPTSTR)lpKeyName ) )
		return( lp + lstrlen((LPTSTR)lp) + 1 );
	lp = lp + lstrlen((LPTSTR)lp) + 1;
	lp = lp + lstrlen((LPTSTR)lp) + 1;
	}

return( NULL );
}

/************************************************************************/
void SavePreferences()
/************************************************************************/
{
}


/************************************************************************/
void SaveMiscPreferences()
/************************************************************************/
{
LPTSTR lp;
char t[40];

AstralCursor( IDC_WAIT );
 
// Recall List
SaveRecallList();

// Clipboard
PutDefaultInt( DoPicture_Str, Control.DoPicture );
PutDefaultInt( DoBitmap_Str, Control.DoBitmap );

// Macro
PutDefaultInt( nRepeat_Str, Macro.nRepeat);
//PutDefaultInt( Macro.fJournalMacro, Macro.fJournalMacro );
PutDefaultInt( fBatchClose_Str, Macro.fBatchClose );
PutDefaultInt( BatchSave_Str, Macro.BatchSave-IDC_BATCHSAVEFIRST );
PutDefaultString( BatchAlbum_Str, Macro.BatchAlbum );
PutDefaultString( BatchDir_Str, Macro.BatchDir );
PutDefaultInt(fBatchChangeType_Str, Macro.fBatchChangeType);
PutDefaultInt(BatchFileType_Str, Macro.BatchFileType-IDN_FIRSTFILETYPE);

// New Dialog
PutDefaultInt( NewDepth_Str, Control.NewDepth );
PutDefaultString(NewWidth_Str,FixedAscii(Control.NewWidth,t,-1));
PutDefaultString(NewHeight_Str,FixedAscii(Control.NewHeight,t,-1));
PutDefaultInt( NewResolution_Str, Control.NewResolution );

// Miscellaneous
PutDefaultString(rgbBackground_Str,RGBAscii(&Control.rgbBackground,t));
PutDefaultInt( Save24BitWallpaper_Str, Control.Save24BitWallpaper );
PutDefaultInt( DefaultDitherType_Str, Control.DefaultDitherType );
PutDefaultInt( DoCommandList_Str, Control.DoCommandList );
PutDefaultInt( UseShortCommandNames_Str, Control.UseShortCommandNames );
PutDefaultInt( UseFileForClipboard_Str, Control.UseFileForClipboard );
PutDefaultInt( WriteFileOnPaste_Str, Control.WriteFileOnPaste );
//PutDefaultInt( UseProjectDirectory_Str, Control.UseProjectDirectory);
PutDefaultInt( UseBrushEditColor_Str, Control.UseBrushEditColor );
PutDefaultInt( NoHiResRebuild_Str, Control.bNoHiResRebuild );

// PhotoCD CMS option.
PutDefaultInt( fPCDUseCMS_Str, Control.fPCDUseCMS );

// Browser Dialog
SaveBrowserState();
PutDefaultInt(bPTRows_Str,   Browser.bPTRows );
PutDefaultInt(PTNumRows_Str, Browser.PTNumRows );
PutDefaultInt(PTNumCols_Str, Browser.PTNumCols );
PutDefaultInt( WarnReadOnly_Str, Browser.WarnReadOnly );

// Monitor Gamma Dialog
PutDefaultInt( GammaLock_Str, Control.GammaLock );

// Mapping Group Preferences
PutDefaultInt( Map_bAuto_Str, Mapping.Map_bAuto);
PutDefaultInt( Map_nPoints_Str, Mapping.Map_nPoints);
PutDefaultInt( Map_bGrid_Str, Mapping.Map_bGrid);
PutDefaultInt( Map_bPercent_Str, Mapping.Map_bPercent);
PutDefaultInt( Map_bIntensity_Str, Mapping.Map_bIntensity);
PutDefaultInt( Map_bCurves_Str, Mapping.Map_bCurves);
PutDefaultInt( Map_CurveType_Str, Mapping.Map_CurveType);
PutDefaultInt( HueShift_bAuto_Str, Mapping.HueShift_bAuto);
PutDefaultInt( HueMap_bAuto_Str, Mapping.HueMap_bAuto);
PutDefaultInt( Poster_bAuto_Str,   Mapping.Poster_bAuto);
PutDefaultInt( ContBrit_bAuto_Str, Mapping.ContBrit_bAuto);
PutDefaultInt( Balance_bAuto_Str, Mapping.Balance_bAuto);
PutDefaultInt( Quarter_bAuto_Str, Mapping.Quarter_bAuto);
PutDefaultInt( Quarter_HSacrifice_Str, Mapping.Quarter_HSacrifice);
PutDefaultInt( Quarter_SSacrifice_Str, Mapping.Quarter_SSacrifice);
PutDefaultInt( Quarter_bUseMidPer_Str, Mapping.Quarter_bUseMidPer);
PutDefaultInt( Quarter_bPercent_Str, Mapping.Quarter_bPercent);

// Extended Names
PutDefaultString( ColorMap_Str, Names.ColorMap );
PutDefaultString( Mask_Str, Names.Mask );
PutDefaultString( Scanner_Str, Names. Scanner );
PutDefaultString( ScanMap_Str, Names.ScanMap );
PutDefaultString( Printer_Str, Names.Printer );
PutDefaultString( PrintStyle_Str, Names.PrintStyle );
PutDefaultString( Clipboard_Str, Names.Clipboard );
PutDefaultString( CustomBrush_Str, Names.CustomBrush );
PutDefaultString( Special_Str, Names.Special );
PutDefaultString( MainManifest_Str, Names.MainManifest);
PutDefaultString( SupManifest_Str, Names.SupManifest);
PutDefaultString( InstallerDisk_Str, Names.InstallerDisk);
PutDefaultString( ColorManager_Str, Names.ColorManager);
PutDefaultString( Shape_Str, Names.Shape );
PutDefaultString( PackageDir_Str, Names.szPackageDir );

// Print Dialog
PutDefaultInt( Type_Str, Page.Type - IDC_PRINTER_IS_MONO );
PutDefaultInt( PrintNumeric_Str, Page.PrintNumeric );
PutDefaultInt( OutputType_Str, Page.OutputType - IDC_PRINT_GRAY );
PutDefaultInt( ScatterPrint_Str, Page.ScatterPrint );
PutDefaultInt( Centered_Str, Page.Centered );
PutDefaultString(OffsetX_Str,FixedAscii(Page.OffsetX,t,-1));
PutDefaultString(OffsetY_Str,FixedAscii(Page.OffsetY,t,-1));
PutDefaultInt( SepFlags_Str, Page.SepFlags );
PutDefaultInt( TrimMarks_Str, Page.TrimMarks );
PutDefaultInt( RegMarks_Str, Page.RegMarks );
PutDefaultInt( Labels_Str, Page.Labels );
PutDefaultInt( StepScale_Str, Page.StepScale );
PutDefaultInt( Negative_Str, Page.Negative );
PutDefaultInt( EmulsionDown_Str, Page.EmulsionDown );
PutDefaultInt( BinaryPS_Str, Page.BinaryPS );
//PutDefaultInt( Page.BinaryEPS, Page.BinaryEPS );
PutDefaultInt( PrintScaling_Str, Page.PrintScaling);
PutDefaultInt( UsePrinterScreening_Str, Page.UsePrinterScreening);
PutDefaultInt( fPrintCalVisual_Str, Control.fPrintCalVisual );
PutDefaultInt( GeneratePS_Str, Page.GeneratePS );

// Scan and Scan Setup Dialogs
PutDefaultInt( LockAspect_Str, Scans.LockAspect );
PutDefaultInt( DoCalibration_Str, Scans.DoCalibration );
PutDefaultInt( ColorPreview_Str, Scans.ColorPreview );
//PutDefHex( SCinit.PortAddr, SCinit.PortAddr );
//PutDefHex( Control.ScanMapMode, Control.ScanMapMode );
//PutDefaultInt( BitsPerPixel_Str, SCatt.BitsPerPixel );
//PutDefaultInt( Resolution_Str, SCatt.Resolution );
//PutDefaultInt( ScanSpeed_Str, SCatt.ScanSpeed );
//PutDefaultInt( UseDMA_Str, SCatt.UseDMA );
//PutDefaultInt( Frameleft_Str, SCatt.Frame.left );
//PutDefaultInt( Frametop_Str, SCatt.Frame.top );
//PutDefaultInt( Frameright_Str, SCatt.Frame.right );
//PutDefaultInt( Framebottom_Str, SCatt.Frame.bottom );
PutDefaultInt( fScanUseCMS_Str, Scans.fScanUseCMS );
PutDefaultInt( fScanCalVisual_Str, Scans.fScanCalVisual );

// Size Dialog
PutDefaultInt( SmartSize_Str, Edit.SmartSize );

// Convert settings
PutDefaultInt( MiniType_Str, Convert.MiniType-IDC_MINI256);
PutDefaultInt( fOptimize_Str, Convert.fOptimize );
PutDefaultInt( fScatter_Str, Convert.fScatter );
PutDefaultInt( fDither_Str, Convert.fDither );
PutDefaultInt( ConvertDitherType_Str, Convert.ConvertDitherType);
PutDefaultInt( ConvertType_Str, Convert.ConvertType);
PutDefaultString( ConvertPalette_Str, Convert.ConvertPalette );
PutDefaultInt( ConvertColors_Str, Convert.ConvertColors );

// Feather Mask/Alpha Dialogs
PutDefaultInt( wFeatherEdge_Str,  Mask.wFeatherEdge );
PutDefaultInt( FeatherAmount_Str, Mask.FeatherAmount );
PutDefaultInt( FeatherInside_Str, Mask.FeatherInside );
PutDefaultInt( DisplayQuality_Str, Mask.DisplayQuality );

// Load Mask Dialog
PutDefaultInt (DeleteMask_Str, Mask.DeleteMask );

PutDefaultInt (OneBitMask_Str, Mask.OneBitMask );

// Palette Edit Dialog
PutDefaultInt( fReMap_Str, PaletteSettings.fReMap);
PutDefaultInt( DitherType_Str, PaletteSettings.DitherType);

// Save Hint Settings
PutDefaultInt( fBubbleHintsOn_Str, Hints.fBubbleHintsOn );
PutDefaultInt( fStatusHintsOn_Str, Hints.fStatusHintsOn );
PutDefaultInt( fHintWindowOn_Str,  Hints.fHintWindowOn );

// Save Common Tool Settings
PutDefaultInt( iModes_Str, Common.iModes );
PutDefaultInt( Opacity_Str, Common.Opacity );
PutDefaultInt( MergeMode_Str, Common.MergeMode );
PutDefaultInt( idColorModel_Str, Common.idColorModel );
PutDefaultInt( Range_Str, Common.Range );
PutDefaultString(Width_Str,FixedAscii(Common.Width,t,-1));
PutDefaultString(Height_Str,FixedAscii(Common.Height,t,-1));
PutDefaultInt( Constrain_Str, Common.Constrain-IDC_ASPECTFIRST );
PutDefaultInt( BrushShape_Str, Common.BrushShape-IDC_BRUSHCIRCLE );
PutDefaultInt( BrushSize_Str, Common.BrushSize );
PutDefaultInt( Pressure_Str, Common.Pressure );
PutDefaultInt( Feather_Str, Common.Feather );
PutDefaultInt( DoCircle_Str, Common.DoCircle );
PutDefaultInt( CommonUnits_Str, Common.CommonUnits - IDC_PREF_UNITINCHES );

AstralCursor( NULL );
}


/***********************************************************************/
void SaveRecallList()
/***********************************************************************/
{
char tmp[10];
int i;

for (i = 0; i < MAX_RECALL; ++i)
	{
	if (i >= Control.RecallCount)
		PutDefaultString( itoa(i,tmp,10), Empty_Str );
	else	PutDefaultString( itoa(i,tmp,10), Control.RecallImage[i] );
	}
}


/************************************************************************/
LPTSTR stripstruct( LPTSTR LPTSTRing )
/************************************************************************/
{
LPTSTR lp;

lp = LPTSTRing;
while ( *lp )
	{
	if ( *lp++ == '.' )
		return( lp );
	}
return( LPTSTRing );
}


/************************************************************************/
BOOL SaveToolPreferences( int idTool )
/************************************************************************/
{
LPTSTR lp;
STRING szString;
char t[40];

if (idTool != IDC_STROKEMASK)
	{
	if ( AstralStr( IDC_TOOLNAMESTART+idTool, &lp ) )
		lstrcpy( szString, lp );
	else	szString[0] = '\0';
	if ( AstralOKCancel( IDS_OKTOSAVEPREF, (LPTSTR)szString ) == IDCANCEL )
		return( NO );
	}

switch ( idTool )
   {
   case 0:
	break;
	case IDC_SELECTOR:
	break;
   case IDC_SQUARE:
	PutDefaultInt( iShapeModes_Str, Mask.iShapeModes );
	PutDefaultInt( DoMaskCircle_Str, Mask.DoMaskCircle );
	PutDefaultInt( ConstrainRectAspect_Str,
			Mask.ConstrainRectAspect-IDC_ASPECTFIRST );
   PutDefaultString(ConstrainRectWidth_Str,FixedAscii(Mask.ConstrainRectWidth,t,-1));
   PutDefaultString(ConstrainRectHeight_Str,FixedAscii(Mask.ConstrainRectHeight,t,-1));
	PutDefaultInt( ShapeUnits_Str, Mask.ShapeUnits - IDC_PREF_UNITINCHES );
	break;
   case IDC_FREE:
	PutDefaultInt( FreeAntialias_Str, Mask.FreeAntialias );
	PutDefaultInt( iFreeModes_Str, Mask.iFreeModes );
	PutDefaultInt( AutoMask_Str, Mask.AutoMask );
	PutDefaultInt( PointEdit_Str, Mask.PointEdit );
	PutDefaultInt( idAutoColorModel_Str, Mask.idAutoColorModel );
	PutDefaultInt( AutoSearchDepth_Str, Mask.AutoSearchDepth );
	PutDefaultInt( AutoMinEdge_Str, Mask.AutoMinEdge );
	break;
	case IDC_PAINTON:
	PutDefaultInt(iPaintOnModes_Str, Mask.iPaintOnModes );
	PutDefaultInt(PaintOnObject_Str, Mask.PaintOnObject );
	HandleBrushSettings(idTool, YES);
	break;
   case IDC_MAGICWAND:
	PutDefaultInt( iWandModes_Str, Mask.iWandModes );
	PutDefaultInt( WandRange_Str, Mask.WandRange);
	PutDefaultInt( WandFade_Str, Mask.WandFade);
	PutDefaultInt( idMagicColorModel_Str, Mask.idMagicColorModel );
	PutDefaultInt( GrowAmount_Str, Mask.GrowAmount );
	break;
   case IDC_TRANSFORMER:
	PutDefaultInt( TransformImage_Str, Mask.TransformImage-IDC_TRANSFUNCTIONFIRST);
	break;
	case IDC_OBJECTTRANSFORM:
//	case IDC_MASKTRANSFORM:
//	case IDC_SELECTORTRANSFORM:
	PutDefaultInt( iTransformModes_Str, Mask.iTransformModes ); 
	PutDefaultInt( PasteQuality_Str, Mask.PasteQuality );
	PutDefaultInt( TransMode_Str, Mask.TransMode-IDC_TRANSMODEFIRST );
	PutDefaultInt( TransformOpacity_Str, Mask.TransformOpacity );
	PutDefaultInt( TransformMergeMode_Str, Mask.TransformMergeMode );
	break;
   case IDC_POINTER:
	PutDefaultInt( PointIModes_Str, Mask.PointIModes ); 
	PutDefaultInt( PointAntialias_Str, Mask.PointAntialias ); 
	PutDefaultInt( PointerFunction_Str, Mask.PointerFunction - IDC_MOVEPOINTS );
	PutDefaultInt( MakePathTolerance_Str, Mask.MakePathTolerance);
	PutDefaultInt( MakePathContinuity_Str, 		Mask.MakePathContinuity);
	PutDefaultInt( MakePathCurveTolerance_Str, 	Mask.MakePathCurveTolerance);
	PutDefaultInt( MakePathType_Str, Mask.MakePathType - IDC_PATH_LINES);
	break;
   case IDC_CROP:
	PutDefaultInt( ConstrainCropAspect_Str,
			              Mask.ConstrainCropAspect-IDC_ASPECTFIRST );
   PutDefaultString(ConstrainCropWidth_Str,FixedAscii(Mask.ConstrainCropWidth,t,-1));
   PutDefaultString(ConstrainCropHeight_Str,FixedAscii(Mask.ConstrainCropHeight,t,-1));
	PutDefaultInt( CropUnits_Str, Mask.CropUnits - IDC_PREF_UNITINCHES );
	break;
   case IDC_PAINT:
   case IDC_SPRAY:
   case IDC_CLONE:
   case IDC_TEXTURE:
   case IDC_SMEAR:
   case IDC_ERASER:
   case IDC_SHARP:
   case IDC_SMOOTH:
   case IDC_LIGHTEN:
   case IDC_DARKEN:
	PutDefaultString( CustomBrush_Str, Names.CustomBrush );
	HandleBrushSettings(idTool, YES);
	break;
   case IDC_VIGNETTE:
	PutDefaultInt( Gradient_Str, Vignette.Gradient - IDC_VIGLINEAR );
	PutDefaultInt( RepeatCount_Str, Vignette.RepeatCount );
	PutDefaultInt( SoftTransition_Str, Vignette.SoftTransition );
	PutDefaultInt( VigColorModel_Str, Vignette.VigColorModel );
	PutDefaultInt( VigOpacity_Str, Vignette.VigOpacity );
	PutDefaultInt( VigMergeMode_Str, Vignette.VigMergeMode );
	PutDefaultInt( Midpoint_Str, Vignette.Midpoint );
	break;
   case IDC_TEXTUREFILL:
	PutDefaultString( TextureName_Str, Texture.TextureName );
	PutDefaultInt( fHorzFlip_Str, Texture.fHorzFlip );
	PutDefaultInt( fVertFlip_Str, Texture.fVertFlip );
	PutDefaultInt( TextureOpacity_Str, Texture.TextureOpacity );
	PutDefaultInt( TextureMergeMode_Str, Texture.TextureMergeMode );
	break;
   case IDC_TINTFILL:
	PutDefaultInt( FillOpacity_Str, Fill.FillOpacity );
	PutDefaultInt( FillMergeMode_Str, Fill.FillMergeMode );
	break;
   case IDC_FLOOD:
	PutDefaultInt( FloodRange_Str, Fill.FloodRange );
	PutDefaultInt( FloodFade_Str, Fill.FloodFade );
	PutDefaultInt( idFillColorModel_Str, Fill.idFillColorModel);
	PutDefaultInt( FloodOpacity_Str, Fill.FloodOpacity );
	PutDefaultInt( FloodMergeMode_Str, Fill.FloodMergeMode );
	break;
   case IDC_DRAWSQUARE:
   case IDC_DRAWFREE:
   case IDC_PENCIL:
	if ( idTool == IDC_DRAWSQUARE )
		{
		PutDefaultInt( wSquareStyle_Str, Draw.wSquareStyle-IDC_SQUEDGE);
		PutDefaultInt( DoDrawCircle_Str, Draw.DoDrawCircle );
		}
	if ( idTool == IDC_DRAWFREE )
		PutDefaultInt( wFreeStyle_Str, Draw.wFreeStyle );
	PutDefaultString( CustomBrush_Str, Names.CustomBrush );
	HandleBrushSettings(idTool, YES);
	break;
   case IDC_CUSTOMVIEW:
	PutDefaultInt( ZoomOut_Str, View.ZoomOut );
	PutDefaultInt( ZoomWindow_Str, View.ZoomWindow );
	break;
   case IDC_TEXT:
   	TextForum.Save();
	break;
   case IDC_PROBE:
	PutDefaultInt( ProbeMethod_Str, ColorMask.ProbeMethod - IDC_PROBEPOINT );
	break;
	case IDC_STROKEMASK:
	PutDefaultInt( StrokeTool_Str, Mask.StrokeTool-IDC_STROKEFIRST );
	PutDefaultString( CustomBrush_Str, Names.CustomBrush );
	HandleBrushSettings(idTool, YES);
	break;
   case IDC_SHIELD:
	PutDefaultInt( MaskShield_Str, ColorMask.MaskShield );
	PutDefaultInt( ColorMask_Str, ColorMask.Mask );
	PutDefaultInt( Include_Str, ColorMask.Include );
	PutDefaultInt( On_Str, ColorMask.On );
	PutDefaultInt( ShieldMethod_Str,
		ColorMask.ShieldMethod - IDC_PROBEPOINT );
	PutDefaultInt( IfHidden_Str, ColorMask.IfHidden );
	// do from 0 to NUM_COLORMASKS
	PutDefaultInt( MaskRange0_Str, ColorMask.MaskRange[0] );
	PutDefaultInt( MaskRange1_Str, ColorMask.MaskRange[1] );
	PutDefaultInt( MaskRange2_Str, ColorMask.MaskRange[2] );
	PutDefaultInt( MaskRange3_Str, ColorMask.MaskRange[3] );
	PutDefaultInt( MaskRange4_Str, ColorMask.MaskRange[4] );
	PutDefaultInt( MaskRange5_Str, ColorMask.MaskRange[5] );
	PutDefaultInt( MaskRange6_Str, ColorMask.MaskRange[6] );
	PutDefaultInt( MaskRange7_Str, ColorMask.MaskRange[7] );
	// do from 0 to NUM_COLORMASKS
   PutDefaultString(MaskColor0rgb_Str,RGBAscii(&ColorMask.MaskColor[0].rgb,t));
   PutDefaultString(MaskColor1rgb_Str,RGBAscii(&ColorMask.MaskColor[1].rgb,t));
   PutDefaultString(MaskColor2rgb_Str,RGBAscii(&ColorMask.MaskColor[2].rgb,t));
   PutDefaultString(MaskColor3rgb_Str,RGBAscii(&ColorMask.MaskColor[3].rgb,t));
   PutDefaultString(MaskColor4rgb_Str,RGBAscii(&ColorMask.MaskColor[4].rgb,t));
   PutDefaultString(MaskColor5rgb_Str,RGBAscii(&ColorMask.MaskColor[5].rgb,t));
   PutDefaultString(MaskColor6rgb_Str,RGBAscii(&ColorMask.MaskColor[6].rgb,t));
   PutDefaultString(MaskColor7rgb_Str,RGBAscii(&ColorMask.MaskColor[7].rgb,t));

	break;
   case IDC_STITCH:
	PutDefaultInt( AutoBrightness_Str, Stitch.AutoBrightness );
	PutDefaultInt( AutoBlend_Str, Stitch.AutoBlend );
	PutDefaultInt( SPressure_Str, Stitch.Pressure-IDC_PRESSLIGHT );
	PutDefaultInt( SmartStitch_Str, Stitch.SmartStitch);
	PutDefaultInt( iTransform_Str, Stitch.iTransform);
	break;
   }
return( YES );
}


/***********************************************************************/
void SaveBrowserPrefs()
/***********************************************************************/
{
	PutDefaultInt(OKtoSavePath_Str, Browser.OKtoSavePath );
	PictPubApp.WriteLocalProfileFileName(ThumbnailDir_Str, Browser.ThumbnailDir);	
	PictPubApp.WriteLocalProfileFileName(DBFInfoDir_Str,Browser.DBFInfoDir);  	
	PictPubApp.WriteLocalProfileFileName(AlbumFileDir_Str,Browser.AlbumFileDir);	
	PutDefaultString(SearchAlbum_Str,Browser.SearchAlbum);	
	PutDefaultInt(AutoThumbnails_Str,Browser.AutoThumbnails);		
	PutDefaultInt(CreateThumbnailOnSave_Str,Browser.CreateThumbnailOnSave);		
	PutDefaultInt(ConfirmOnDelFile_Str,Browser.ConfirmOnDelFile);
	PutDefaultInt(ConfirmOnMoveFile_Str,Browser.ConfirmOnMoveFile);
	PutDefaultInt(ConfirmOnCopyFile_Str,Browser.ConfirmOnCopyFile);
	PutDefaultInt(ConfirmOnRenameFile_Str,Browser.ConfirmOnRenameFile);
	PutDefaultInt(ConfirmOnAddFile_Str,Browser.ConfirmOnAddFile);
	PutDefaultInt(ConfirmOnCreateDir_Str,Browser.ConfirmOnCreateDir);
	PutDefaultInt(ConfirmOnRenameDir_Str,Browser.ConfirmOnRenameDir);
	PutDefaultInt(ConfirmOnDeleteDir_Str,Browser.ConfirmOnDeleteDir);
	PutDefaultInt(ConfirmOnRenameAlbum_Str,Browser.ConfirmOnRenameAlbum);
	PutDefaultInt(ConfirmOnDeleteAlbum_Str,Browser.ConfirmOnDeleteAlbum);
}

/***********************************************************************/
void LoadBrowserPrefs()
/***********************************************************************/
{
    PictPubApp.GetLocalProfileString(ThumbnailDir_Str,Control.PouchPath,Browser.ThumbnailDir,sizeof(Browser.ThumbnailDir));
	FixPath(Lowercase(Browser.ThumbnailDir));
    PictPubApp.GetLocalProfileString(DBFInfoDir_Str,Control.PouchPath,Browser.DBFInfoDir,sizeof(Browser.DBFInfoDir));
	FixPath(Lowercase(Browser.DBFInfoDir));
    PictPubApp.GetLocalProfileString(AlbumFileDir_Str,Control.PouchPath,Browser.AlbumFileDir,sizeof(Browser.AlbumFileDir));
    GetDefaultString(DiskMarker_Str,NoDel_Str,Browser.DiskMarker,sizeof(Browser.DiskMarker));
	FixPath(Lowercase(Browser.AlbumFileDir));
    GetDefaultString(SearchAlbum_Str,FileSrch_Str,Browser.SearchAlbum,sizeof(Browser.SearchAlbum));
    Browser.AutoThumbnails=GetDefaultInt(AutoThumbnails_Str,NO);
    Browser.CreateThumbnailOnSave=GetDefaultInt(CreateThumbnailOnSave_Str,YES);
    Browser.ConfirmOnDelFile=GetDefaultInt(ConfirmOnDelFile_Str,YES);
    Browser.ConfirmOnMoveFile=GetDefaultInt(ConfirmOnMoveFile_Str,YES);
    Browser.ConfirmOnCopyFile=GetDefaultInt(ConfirmOnCopyFile_Str,YES);
    Browser.ConfirmOnRenameFile=GetDefaultInt(ConfirmOnRenameFile_Str,YES);
    Browser.ConfirmOnAddFile=GetDefaultInt(ConfirmOnAddFile_Str,YES);
    Browser.ConfirmOnCreateDir=GetDefaultInt(ConfirmOnCreateDir_Str,YES);
    Browser.ConfirmOnRenameDir=GetDefaultInt(ConfirmOnRenameDir_Str,YES);
    Browser.ConfirmOnDeleteDir=GetDefaultInt(ConfirmOnDeleteDir_Str,YES);
    Browser.ConfirmOnRenameAlbum=GetDefaultInt(ConfirmOnRenameAlbum_Str,YES);
    Browser.ConfirmOnDeleteAlbum=GetDefaultInt(ConfirmOnDeleteAlbum_Str,YES);
    Browser.OKtoSavePath=GetDefaultInt(OKtoSavePath_Str,YES);
    Browser.WarnReadOnly=GetDefaultInt(WarnReadOnly_Str,YES);
}

/***********************************************************************/
void SaveBrowserState()
/***********************************************************************/
{
	PutDefaultInt(ViewThumbnails_Str,Browser.ViewThumbnails);		
	PutDefaultInt(ViewAlbums_Str,Browser.ViewAlbums);			
	PutDefaultInt(bsrFileType_Str,Browser.bsrFileType-IDT_FIRSTFILETYPE);			
	PutDefaultInt(bsrImpFileType_Str,Browser.bsrImpFileType-IDT_FIRSTIMPORTFILETYPE);			
	PutDefaultString(LastAlbum_Str, Browser.LastAlbum);	
	// Pste browser
	PutDefaultInt(PasteAutoThumbnails_Str,ExtBrowser.PasteAutoThumbnails);			
	PutDefaultInt(PasteThumbCreate_Str,ExtBrowser.PasteThumbCreate);			
	PutDefaultInt(PasteViewThumbnails_Str,ExtBrowser.PasteViewThumbnails);			
}
/***********************************************************************/
void LoadBrowserState()
/***********************************************************************/
{
    GetDefaultString(LastAlbum_Str,Empty_Str,Browser.LastAlbum,sizeof(Browser.LastAlbum));
    Browser.bsrFileType=GetDefaultInt(bsrFileType_Str,1);
	Browser.bsrFileType += IDT_FIRSTFILETYPE;
    Browser.bsrImpFileType=GetDefaultInt(bsrImpFileType_Str,0);
	Browser.bsrImpFileType += IDT_FIRSTIMPORTFILETYPE;
    Browser.ViewThumbnails=GetDefaultInt(ViewThumbnails_Str,YES);
    Browser.ViewAlbums=GetDefaultInt(ViewAlbums_Str,NO);
    Browser.UseVolumeLabels=GetDefaultInt(UseVolumeLabels_Str,YES);
   
    ExtBrowser.PasteAutoThumbnails=GetDefaultInt(PasteAutoThumbnails_Str,NO);
    ExtBrowser.PasteThumbCreate=GetDefaultInt(PasteThumbCreate_Str,YES);
    ExtBrowser.PasteViewThumbnails=GetDefaultInt(PasteViewThumbnails_Str,YES);
}

/***********************************************************************/
void SaveSaveOptions()
/***********************************************************************/
{
	// EPS
	PutDefaultInt( PreviewBPP_Str, EPSOptions.PreviewBPP-IDC_PREVIEWNONE );
	PutDefaultInt(	CompositeBPP_Str, EPSOptions.CompositeBPP );
	PutDefaultInt(	bEPSPromptUser_Str, EPSOptions.bEPSPromptUser );
	PutDefaultInt(	bDCSPromptUser_Str, EPSOptions.bDCSPromptUser );
	PutDefaultString( ClipPath_Str, EPSOptions.ClipPath );
	// TIFF
	PutDefaultInt(	TIFFDiff_Str, TIFFOptions.TIFFDiff );
	PutDefaultInt(	TIFFCompress_Str, TIFFOptions.TIFFCompress );
	PutDefaultInt(	bTIFFPromptUser_Str, TIFFOptions.bTIFFPromptUser );
	PutDefaultInt(	TIFFbWritePT_Str, TIFFOptions.bWritePT );
	PutDefaultInt( TIFFSaveMask_Str, TIFFOptions.bSaveMask);
	// TARGA
	PutDefaultInt(	bTGAPromptUser_Str, TGAOptions.bTGAPromptUser );
	PutDefaultInt( TGASaveMask_Str, TGAOptions.bSaveMask);
	// JPEG		    
	PutDefaultInt(bJPEGPromptUser_Str, 	JPEGOptions.bJPEGPromptUser);
	// PPFF
	PutDefaultInt( bPPFFPromptUser_Str, PPFFOptions.bPPFFPromptUser);
	PutDefaultInt( PPFFCompress_Str,	PPFFOptions.PPFFCompress);
	PutDefaultInt( bSaveMask_Str,		PPFFOptions.bSaveMask);
	PutDefaultInt( bSaveProxy_Str,		PPFFOptions.bSaveProxy);
	PutDefaultInt( bSaveCmdList_Str,	PPFFOptions.bSaveCmdList);
	PutDefaultInt( bSaveLink_Str,		PPFFOptions.bSaveLink);
	PutDefaultInt( PPFFbWritePT_Str,	PPFFOptions.bWritePT);
	// AVI
	PutDefaultInt( bAVIPromptUser_Str,	AVIOptions.bAVIPromptUser); 	
	PutDefaultInt( iAVIPosType_Str,		AVIOptions.iAVIPosType);
	PutDefaultInt( iAVISizingType_Str, 	AVIOptions.iAVISizingType);
}

/***********************************************************************/
void LoadSaveOptions()
/***********************************************************************/
{

	// EPS
   EPSOptions.PreviewBPP=GetDefaultInt(PreviewBPP_Str,1);
   EPSOptions.PreviewBPP+=IDC_PREVIEWNONE;
   if (EPSOptions.PreviewBPP > IDC_PREVIEW24)
		EPSOptions.PreviewBPP = IDC_PREVIEWNONE;
   EPSOptions.CompositeBPP=GetDefaultInt(CompositeBPP_Str,NO);
   EPSOptions.bEPSPromptUser=GetDefaultInt(bEPSPromptUser_Str,YES);
   EPSOptions.bDCSPromptUser=GetDefaultInt(bDCSPromptUser_Str,YES);
GetDefaultString(ClipPath_Str,Empty_Str,EPSOptions.ClipPath,sizeof(EPSOptions.ClipPath));
	// TIFF
   TIFFOptions.TIFFCompress=GetDefaultInt(TIFFCompress_Str,NO);
   TIFFOptions.TIFFDiff=GetDefaultInt(TIFFDiff_Str,NO);
   TIFFOptions.bTIFFPromptUser=GetDefaultInt(bTIFFPromptUser_Str,NO);
   TIFFOptions.bWritePT=GetDefaultInt(TIFFbWritePT_Str,NO);
   TIFFOptions.bSaveMask=GetDefaultInt(TIFFSaveMask_Str,NO);
	// TARGA
   TGAOptions.bTGAPromptUser=GetDefaultInt(bTGAPromptUser_Str,NO);
   TGAOptions.bSaveMask=GetDefaultInt(TGASaveMask_Str,NO);
	// JPEG
   JPEGOptions.bJPEGPromptUser=GetDefaultInt(bJPEGPromptUser_Str,YES);
	// PPFF
   PPFFOptions.bPPFFPromptUser=GetDefaultInt(bPPFFPromptUser_Str,YES);
   PPFFOptions.PPFFCompress=GetDefaultInt(PPFFCompress_Str,NO);
   PPFFOptions.bSaveMask=GetDefaultInt(bSaveMask_Str,YES);
   PPFFOptions.bSaveProxy=GetDefaultInt(bSaveProxy_Str,YES);
   PPFFOptions.bSaveCmdList=GetDefaultInt(bSaveCmdList_Str,YES);
   PPFFOptions.bSaveLink=GetDefaultInt(bSaveLink_Str,NO);
   PPFFOptions.bWritePT=GetDefaultInt(PPFFbWritePT_Str,YES);
	// AVI
   AVIOptions.bAVIPromptUser=GetDefaultInt(bAVIPromptUser_Str,YES);
   AVIOptions.iAVIPosType=GetDefaultInt(iAVIPosType_Str,0);
   AVIOptions.iAVISizingType=GetDefaultInt(iAVISizingType_Str,0);
   AVIOptions.AVIPosition=GetDefaultInt(AVIPosition_Str,0);
   AVIOptions.AVILastPosition=GetDefaultInt(AVILastPosition_Str,99);
}

/************************************************************************/
static void HandleBrushSettings( int idTool, BOOL fPut )
/************************************************************************/
{
LPTSTR lp;
BRUSHSETTINGS FAR *Br;
STRING str;
STRING tmp;
LPTSTR pName;
LPTSTR lpStyle;
int brush;

switch ( idTool )
    {
    case IDC_ERASER:
	pName = Erase_Str;
	brush = ERASER_BRUSH;
	lpStyle = Retouch.RetouchStyleName[idTool - IDC_FIRST_RETOUCHTOOL];
	break;
    case IDC_PAINT:
	pName = Paint_Str;
	brush = PAINT_BRUSH;
	lpStyle = Retouch.RetouchStyleName[idTool - IDC_FIRST_RETOUCHTOOL];
	break;
    case IDC_PAINTON:
	pName = PaintOn_Str;
	brush = PAINTON_BRUSH;
	lpStyle = NULL;
	break;
    case IDC_SPRAY:
	pName = Spray_Str;
	brush = SPRAY_BRUSH;
	lpStyle = Retouch.RetouchStyleName[idTool - IDC_FIRST_RETOUCHTOOL];
	break;
    case IDC_CLONE:
	pName = Clone_Str;
	brush = CLONE_BRUSH;
	lpStyle = Retouch.RetouchStyleName[idTool - IDC_FIRST_RETOUCHTOOL];
	break;
    case IDC_TEXTURE:
	pName = TPaint_Str;
	brush = TEXTURE_BRUSH;
	lpStyle = Retouch.RetouchStyleName[idTool - IDC_FIRST_RETOUCHTOOL];
	break;
    case IDC_SMEAR:
	pName = Smear_Str;
	brush = SMEAR_BRUSH;
	lpStyle = Retouch.RetouchStyleName[idTool - IDC_FIRST_RETOUCHTOOL];
	break;
    case IDC_SHARP:
	pName = Sharpen_Str;
	brush = SHARPEN_BRUSH;
	lpStyle = Retouch.FilterStyleName[idTool - IDC_FIRST_FILTERTOOL];
	break;
    case IDC_SMOOTH:
	pName = Smooth_Str;
	brush = SMOOTH_BRUSH;
	lpStyle = Retouch.FilterStyleName[idTool - IDC_FIRST_FILTERTOOL];
	break;
    case IDC_LIGHTEN:
	pName = Lighten_Str;
	brush = LIGHTEN_BRUSH;
	lpStyle = Retouch.FilterStyleName[idTool - IDC_FIRST_FILTERTOOL];
	break;
    case IDC_DARKEN:
	pName = Darken_Str;
	brush = DARKEN_BRUSH;
	lpStyle = Retouch.FilterStyleName[idTool - IDC_FIRST_FILTERTOOL];
	break;
    case IDC_PENCIL:
	pName = DrawLine_Str;
	brush = DRAW_PENCIL_BRUSH;
	lpStyle = Retouch.DrawStyleName[idTool - IDC_FIRST_DRAWTOOL];
	break;
    case IDC_DRAWSQUARE:
	pName = DrawRect_Str;
	brush = DRAW_RECT_BRUSH;
	lpStyle = Retouch.DrawStyleName[idTool - IDC_FIRST_DRAWTOOL];
	break;
    case IDC_DRAWFREE:
	pName = DrawFree_Str;
	brush = DRAW_FREE_BRUSH;
	lpStyle = Retouch.DrawStyleName[idTool - IDC_FIRST_DRAWTOOL];
	break;
	case IDC_STROKEMASK:
	pName = StrokeMask_Str;
	brush = STROKEMASK_BRUSH;
	lpStyle = NULL;
	break;
    default:
	return;
    }

/* set basic portion of property name */
lstrcpy( str, pName );
lp = str + lstrlen( str);

if ( fPut && brush == RetouchBrushCurrent )
	SaveRetouchBrush( RetouchBrushCurrent );

/* set basic portion of property name */
lstrcpy( str, pName );
lp = str + lstrlen( str);
Br = &lpBrushSettings[brush];

lstrcpy( lp, BrushShape_Str);
if (fPut)
	PutDefaultString( str, itoa( Br->BrushShape - IDC_BRUSHCIRCLE, tmp, 10));
else
	Br->BrushShape = GetDefaultInt( str, 0 ) + IDC_BRUSHCIRCLE;
lstrcpy( lp, BrushSize_Str);
if (fPut)
	PutDefaultString( str, itoa( Br->BrushSize, tmp, 10));
else
	Br->BrushSize = GetDefaultInt( str, 15 );
lstrcpy( lp, Opacity_Str);
if (fPut)
	PutDefaultString( str, itoa( Br->Opacity, tmp, 10));
else
	{
	if (idTool == IDC_SMEAR || idTool == IDC_SPRAY)
		Br->Opacity = GetDefaultInt( str, 127 );
	else
		Br->Opacity = GetDefaultInt( str, 255 );
	}
lstrcpy( lp, Pressure_Str);
if (fPut)
	PutDefaultString( str, itoa( Br->Pressure, tmp, 10));
else
	{
	if (idTool == IDC_SMOOTH)
		Br->Pressure = GetDefaultInt( str, 127 );
	else
		Br->Pressure = GetDefaultInt( str, 64 );
	}
lstrcpy( lp, MergeMode_Str);
if (fPut)
	PutDefaultString( str, itoa( Br->MergeMode, tmp, 10));
else
	Br->MergeMode = (MERGE_MODE)GetDefaultInt( str, MM_NORMAL );
lstrcpy( lp, Feather_Str);
if (fPut)
	PutDefaultString( str, itoa( Br->Feather, tmp, 10));
else
	{
	if (idTool == IDC_PAINTON)
		Br->Feather = GetDefaultInt( str, 0 );
	else
		Br->Feather = GetDefaultInt( str, 100 );
	}
lstrcpy( lp, StyleName_Str );
if (lpStyle)
	{
	if ( fPut )
		PutDefaultString( str, lpStyle );
	else
		GetDefaultString( str, Empty_Str, lpStyle, sizeof(FNAME) );
	}
}

/************************************************************************/
void SetCommonSettings()
/************************************************************************/
{
SetCommonMode(Common.iModes);
SetCommonOpacity(Common.Opacity);
SetCommonMergeMode(Common.MergeMode);
SetCommonColorModel(Common.idColorModel);
SetCommonRange(Common.Range);
SetCommonWidth(Common.Width);
SetCommonHeight(Common.Height);
SetCommonConstrain(Common.Constrain);
SetCommonBrushShape(Common.BrushShape);
SetCommonBrushSize(Common.BrushSize);
SetCommonPressure(Common.Pressure);
SetCommonFeather(Common.Feather); 
SetCommonDoCircle(Common.DoCircle); 
SetCommonUnits(Common.CommonUnits); 
}

/************************************************************************/
void SetCommonMode(int iModes)
/************************************************************************/
{
Common.iModes = iModes;
if (Control.IndependentSettings)
	return;

Mask.iShapeModes = iModes;
Mask.PointIModes = iModes;
Mask.iFreeModes = iModes;
Mask.iPaintOnModes = iModes;
Mask.iWandModes = iModes;
Mask.iTransformModes = iModes;
}

/************************************************************************/
void SetCommonOpacity(int Opacity)
/************************************************************************/
{
int i;

Common.Opacity = Opacity;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].Opacity = Opacity;
	}
Vignette.VigOpacity = Opacity;
Fill.FillOpacity = Opacity;
Fill.FloodOpacity = Opacity;
Mask.TransformOpacity = Opacity;
TextForum.SetOpacity(Opacity);
Texture.TextureOpacity = Opacity;
}

/************************************************************************/
void SetCommonMergeMode(MERGE_MODE MergeMode)
/************************************************************************/
{
int i;

Common.MergeMode = MergeMode;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].MergeMode = MergeMode;
	}
Vignette.VigMergeMode = MergeMode;
Fill.FillMergeMode = MergeMode;
Fill.FloodMergeMode = MergeMode;
Mask.TransformMergeMode = MergeMode;
TextForum.SetMergeMode(MergeMode);
Texture.TextureMergeMode = MergeMode;
}

/************************************************************************/
void SetCommonColorModel(int idColorModel)
/************************************************************************/
{
Common.idColorModel = idColorModel;
if (Control.IndependentSettings)
	return;

Vignette.VigColorModel = idColorModel;
Fill.idFillColorModel = idColorModel;
Mask.idMagicColorModel = idColorModel;
Mask.idAutoColorModel = idColorModel;
}

/************************************************************************/
void SetCommonRange(int Range)
/************************************************************************/
{
Common.Range = Range;
if (Control.IndependentSettings)
	return;

Fill.FloodRange = Range;
Mask.WandRange = Range;
}

/************************************************************************/
void SetCommonWidth(LFIXED Width)
/************************************************************************/
{
Common.Width = Width;
if (Control.IndependentSettings)
	return;
Mask.ConstrainRectWidth = Width;
Mask.ConstrainCropWidth = Width;
}

/************************************************************************/
void SetCommonHeight(LFIXED Height)
/************************************************************************/
{
Common.Height = Height;
if (Control.IndependentSettings)
	return;
Mask.ConstrainRectHeight = Height;
Mask.ConstrainCropHeight = Height;
}

/************************************************************************/
void SetCommonConstrain(int Constrain)
/************************************************************************/
{
Common.Constrain = Constrain;
if (Control.IndependentSettings)
	return;
Mask.ConstrainRectAspect = Constrain;
Mask.ConstrainCropAspect = Constrain;
}

/************************************************************************/
void SetCommonBrushShape(int BrushShape)
/************************************************************************/
{
int i;

Common.BrushShape = BrushShape;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].BrushShape = BrushShape;
	}
}

/************************************************************************/
void SetCommonBrushSize(int BrushSize)
/************************************************************************/
{
int i;

Common.BrushSize = BrushSize;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].BrushSize = BrushSize;
	}
}

/************************************************************************/
void SetCommonPressure(int Pressure)
/************************************************************************/
{
int i;

Common.Pressure = Pressure;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].Pressure = Pressure;
	}
}

/************************************************************************/
void SetCommonFeather(int Feather)
/************************************************************************/
{
int i;

Common.Feather = Feather;
if (Control.IndependentSettings)
	return;

if (lpBrushSettings)
	{
	for (i = 0; i < TOTAL_BRUSHES; ++i)
		lpBrushSettings[i].Feather = Feather;
	}
}

/************************************************************************/
void SetCommonDoCircle(BOOL DoCircle)
/************************************************************************/
{
Common.DoCircle = DoCircle;
if (Control.IndependentSettings)
	return;

Mask.DoMaskCircle = DoCircle;
Draw.DoDrawCircle = DoCircle;
}

/************************************************************************/
void SetCommonUnits(int Units)
/************************************************************************/
{
Common.CommonUnits = Units;
if (Control.IndependentSettings)
	return;

Mask.ShapeUnits = Units;
Mask.CropUnits = Units;
}

/************************************************************************/
BOOL NeedSPAWarning()
/************************************************************************/
{
	FNAME szFileName;
	EXTNAME szSection;
	char key[20];
	int i;

	if ( GetWindowsDirectory( szFileName, sizeof(FNAME) ) <= 2)
		return(FALSE);

	FixPath( szFileName );
	lstrcat( szFileName, MGX_Ini );
	if ( !FileExists( szFileName ) )
		return(TRUE);

	lstrcpy(szSection, "Picture Publisher 5.0");
	lstrcpy(key, "Warning");
	if ( (i = GetPrivateProfileInt( szSection, key, -1, szFileName )) <= 0 )
	{
		if (!i)
			WritePrivateProfileString( szSection, key, "1", szFileName );
		return(TRUE);
	}
	return(FALSE);
}

#ifndef _DESIGNERFIX
/************************************************************************/
BOOL DesignerEditorInProgress ()
// This function checks the State the "EditorInprogress" entry of MGX.INI's
// "Micrografx" section.  If the value is 0 then we are NOT being used as
// and editor for their bitmaps at this moment.  If > 0 then YES.  
// NOTE!!!!! In Either case the function WILL ALWAYS reset the entry to FALSE
/************************************************************************/
{
	BOOL bResult = FALSE;

	FNAME szFileName;
	EXTNAME szSection;
	char key[20];
	int i;

	if ( GetWindowsDirectory( szFileName, sizeof(FNAME) ) <= 2)
		return bResult;

	FixPath( szFileName );
	lstrcat( szFileName, MGX_Ini );
	if ( !FileExists( szFileName ) )
		return bResult;

	lstrcpy(szSection, Mgxi_Str);
	lstrcpy(key, DesignerEditor_Str);
	if ( (i = GetPrivateProfileInt( szSection, key, 0, szFileName )) <= 0 )
	{
		bResult = (i>0);  // editor TRUE if i > 0
		WritePrivateProfileString( szSection, key, "0", szFileName );
						  // reset Section Entry to 0 (FALSE)	
	}
	else
		bResult = TRUE;
	return bResult;
}
#endif //_DESIGNERFIX

