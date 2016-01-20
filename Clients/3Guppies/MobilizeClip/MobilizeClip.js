// Author: Jim McCurdy, Face To Face Software, jmccurdy@facetofacesoftware.com
//
// A Javascript class that implements the ringtone clipping user interface
// Sample calling HTML:
//
//	<link rel="stylesheet" type="text/css" href="MobilizeClip.css" />
//	<script type="text/javascript" src="MobilizeClip.js"></script>
//	<script type="text/javascript">
//		var MobilizeClip = new CMobilizeClip(
//			100 * 1000,			// length of the clip in milliseconds
//			30000,				// length of the maximum selection in milliseconds
//			'images/Background.gif',
//			500, 70,			// width and height of the background image
//			'images/HandleLeft.gif',
//			'images/HandleRight.gif',
//			16, 90,				// width and height of the handle image
//			6,					// width of the overhang on each side of the handle in the image
//			2, 80, 65, 60);		// width of the selection border, and the width of the length, start, and end labels
//			OnClipChanged);		// Callback function when the clip data changes
//
//		if (window.isLoaded)
//			Initialize();
//		else
//			addLoadEvent(Initialize);
//
//		function Initialize()
//		{
//			MobilizeClip.Initialize('idClipDisplayContainer', 'idClipStart', 'idClipEnd', 0/*msNewClipLength*/);
//		}
//	</script>
//	<link rel="stylesheet" type="text/css" href="MobilizeClip.css">
//	<body onload="MobilizeClip.Initialize()">
//		<table><tr>
//			<td>
//				<div id="idClipDisplayContainer" style="width:530px; height:130px;" />
//				<input type="hidden" id="idClipStart" value="0" />
//				<input type="hidden" id="idClipEnd" value="0" />
//			</td>
//		</tr></table>

function CMobilizeClip(msClipLength, msMaxSelection,
	strBackgroundImage, dxBackgroundImage, dyBackgroundImage,
	strStartHandleImage, strEndHandleImage, dxHandleImage, dyHandleImage, 
	dxHandleOverhang,
	iBorderSize, dxLengthLabel, dxStartLabel, dxEndLabel,
	fnOnClipChanged)
{
	// Start of Program variables
	// Length of the clip
	var m_msClipLength = msClipLength;

	var m_msMaxSelection = msMaxSelection;
	
	// Names of the images
	var m_strBackgroundImage = strBackgroundImage;
	var m_strStartHandleImage = strStartHandleImage;
	var m_strEndHandleImage = strEndHandleImage;

	// Size of clip display window
	var m_dxClipDisplay = dxBackgroundImage;
	var m_dyClipDisplay = dyBackgroundImage;

	// Width of the handles used to resize the clip rectangle
	var m_dxHandleImage = dxHandleImage;
	var m_dyHandleImage = dyHandleImage;
	var m_dxHandleOverhang = dxHandleOverhang;

	// Width of border around the clip rectangle
	var m_iBorderSize = iBorderSize;
	var m_dxLengthLabel = dxLengthLabel;
	var m_dxStartLabel = dxStartLabel;
	var m_dxEndLabel = dxEndLabel;

	// Callback function when the clip data changes
	var m_fnOnClipChanged = fnOnClipChanged;

	// End of program variables
	
	var m_bInitialized = false;
	var m_dxClipMax = 0;
	var m_iMinLabelGap = 5;
	var m_iMinSize = Half(dxHandleImage) + m_iMinLabelGap; // Minimum width and height of clip area
	var m_bIsOpera = (navigator.userAgent.indexOf('Opera') >= 0);

	var m_oClipRect = null;
	var m_oBorderRect = null;
	var m_oTransparentL = null;
	var m_oTransparentR = null;
	var m_oPlayPosition = null;
	var m_oStartHandle = null;
	var m_oEndHandle = null;
	var m_oLengthLabel = null;
	var m_oStartLabel = null;
	var m_oEndLabel = null;
	var m_oHiddenClipStart = null;
	var m_oHiddenClipEnd = null;
	var m_idHandleGrabbed = null;
	var m_bHandleMouseDown = false;
	var m_bImageMouseDown = false;
	var m_xDownMouse = 0;
	var m_ixDown = 0;
	var m_dxDown = 0;
	var m_bMouseMoveEventInProgress = false;
	
	// public
	this.Initialize = Initialize;
	function Initialize(idClipDisplayContainer, idHiddenClipStart, idHiddenClipEnd, msNewClipLength)
	{
		if (m_bInitialized)
			return;

		if (msNewClipLength)
			m_msClipLength = msNewClipLength;
		if (!m_msClipLength)
			return;

		// Convert max selection milliseconds to pixels
		m_dxClipMax = (m_msMaxSelection && m_msMaxSelection < m_msClipLength ? m_msMaxSelection : m_msClipLength);
		m_dxClipMax = Math.round(m_dxClipDisplay * (m_dxClipMax / m_msClipLength));
		if (!m_dxClipMax || m_dxClipMax > m_dxClipDisplay)
			m_dxClipMax = m_dxClipDisplay;

		// Set events
		document.documentElement.ondragstart = OnElementCancelEvent;
		document.documentElement.onselectstart = OnElementCancelEvent;
		document.documentElement.onmousemove = OnElementMouseMove;		
		document.documentElement.onmouseup = OnElementMouseUp;		
		
		CreateElements(idClipDisplayContainer);

		m_oHiddenClipStart = document.getElementById(idHiddenClipStart);
		m_oHiddenClipEnd = document.getElementById(idHiddenClipEnd);
		SetHiddenClipValues();

		m_bInitialized = true;
	}

	// public
	this.IsInitialized = IsInitialized;
	function IsInitialized()
	{
		return m_bInitialized;
	}

	// public
	this.SetPlayPosition = SetPlayPosition;
	function SetPlayPosition(iPlayPosition)
	{
		if (!m_msClipLength)
			return;

		var fxScale = m_dxClipDisplay / m_msClipLength;
		var xPlayPosition = Math.round(iPlayPosition * fxScale);
		m_oPlayPosition.style.left = xPlayPosition + 'px';
	}
	
	// private
	this.CreateElements = CreateElements;
	function CreateElements(idClipDisplayContainer)
	{
		// Offset all of the created elements inside another container div
		// This is so the m_oClipRect element is located at offset 0,0
		var yOffsetLengthLabel = -20;
		var xOffsetStartHandle = m_dxHandleOverhang - m_dxHandleImage - m_iBorderSize;
		var xOffsetEndHandle = m_dxClipDisplay + m_iBorderSize + 2 - m_dxHandleOverhang;
		var yOffset = -yOffsetLengthLabel;
		var xOffset = -xOffsetStartHandle;

		// Create an inside container element that will host all the other elements
		// Append this as a child element to idClipDisplayContainer after all other elements are created
		var oClipDisplayContainer = document.createElement('DIV');
		oClipDisplayContainer.className = 'clsBackgroundImage'; 
		oClipDisplayContainer.src = m_strBackgroundImage;
		oClipDisplayContainer.style.left = xOffset + 'px';
		oClipDisplayContainer.style.top = yOffset + 'px';

		var oBackgroundImage = document.createElement('IMG');
		oBackgroundImage.className = 'clsBackgroundImage'; 
		oBackgroundImage.src = m_strBackgroundImage;
		oBackgroundImage.style.left = '0px';
		oBackgroundImage.style.top = '0px';
		oBackgroundImage.style.width = m_dxClipDisplay + 'px';
		oBackgroundImage.style.height = m_dyClipDisplay + 'px';
		oClipDisplayContainer.appendChild(oBackgroundImage);
		
		m_oTransparentL = document.createElement('DIV');
		m_oTransparentL.className = 'clsTransparentRect'; 
		m_oTransparentL.style.left = '0px';
		m_oTransparentL.style.top = '0px';
		m_oTransparentL.style.width = '0px';
		m_oTransparentL.style.height = m_dyClipDisplay + 'px';
		oClipDisplayContainer.appendChild(m_oTransparentL);
		
		m_oTransparentR = document.createElement('DIV');
		m_oTransparentR.className = 'clsTransparentRect'; 
		m_oTransparentR.style.left = m_dxClipMax + 'px';
		m_oTransparentR.style.top = '0px';
		m_oTransparentR.style.width = m_dxClipDisplay - m_dxClipMax + 'px';		
		m_oTransparentR.style.height = m_dyClipDisplay + 'px';
		oClipDisplayContainer.appendChild(m_oTransparentR); 
		
		var yBorderTop = -m_iBorderSize;
		m_oBorderRect = document.createElement('DIV');
		m_oBorderRect.className = 'clsBorderRect';
		m_oBorderRect.style.left = 0 - m_iBorderSize + 'px';
		m_oBorderRect.style.top = yBorderTop + 'px';
		m_oBorderRect.style.width = m_dxClipMax + 2 + 'px';
		m_oBorderRect.style.height = m_dyClipDisplay + 2 + 'px';
		m_oBorderRect.style.borderWidth = m_iBorderSize + 'px';
		oClipDisplayContainer.appendChild(m_oBorderRect); 
		
		m_oPlayPosition = document.createElement('DIV');
		m_oPlayPosition.className = 'clsPlayPosition'; 
		m_oPlayPosition.style.left = '0px';
		m_oPlayPosition.style.top = '0px';
		m_oPlayPosition.style.width = m_iBorderSize + 'px';
		m_oPlayPosition.style.height = m_dyClipDisplay + 2 + 'px';
		oClipDisplayContainer.appendChild(m_oPlayPosition);		

		CreateTimelineElements(oClipDisplayContainer);

		m_oClipRect = document.createElement('DIV');
		m_oClipRect.className = 'clsClipRect';
		m_oClipRect.style.left = '0px';
		m_oClipRect.style.top = '0px';
		m_oClipRect.style.width = m_dxClipMax + 'px';
		m_oClipRect.style.height = m_dyClipDisplay + 'px';
		m_oClipRect.innerHTML = '<div></div>'; // This gives us something to move the clip rect with
		m_oClipRect.onmousedown = OnImageMouseDown;
		
		if (m_bIsOpera)
		{
			var div = m_oClipRect.getElementsByTagName('DIV')[0];
			div.style.backgroundColor = 'transparent';
			m_oTransparentL.style.backgroundColor = 'transparent';
			m_oTransparentR.style.backgroundColor = 'transparent';
		}
		
		m_oStartHandle = document.createElement('IMG');
		m_oStartHandle.src = m_strStartHandleImage;
		m_oStartHandle.className = 'clsHandle'; 
		m_oStartHandle.style.left = xOffsetStartHandle + 'px';
		m_oStartHandle.style.top = yBorderTop + 'px';
		m_oStartHandle.style.width = m_dxHandleImage + 'px';
		m_oStartHandle.style.height = m_dyHandleImage + 'px';
		m_oStartHandle.style.cursor = 'e-resize';
		m_oStartHandle.id = 'w-resize';
		m_oStartHandle.onmousedown = OnHandleMouseDown;
		m_oClipRect.appendChild(m_oStartHandle);	

		m_oEndHandle = document.createElement('IMG');
		m_oEndHandle.src = m_strEndHandleImage; 
		m_oEndHandle.className = 'clsHandle'; 
		m_oEndHandle.style.left = xOffsetEndHandle - m_dxClipDisplay + m_dxClipMax + 'px';
		m_oEndHandle.style.top = yBorderTop + 'px';
		m_oEndHandle.style.width = m_dxHandleImage + 'px';
		m_oEndHandle.style.height = m_dyHandleImage + 'px';
		m_oEndHandle.style.cursor = 'e-resize';
		m_oEndHandle.id = 'e-resize';
		m_oEndHandle.onmousedown = OnHandleMouseDown;
		m_oClipRect.appendChild(m_oEndHandle);		

		var ix = Int(m_oClipRect.style.left);
		var hl = ix + Int(m_oStartHandle.style.left);
		var hr = ix + Int(m_oEndHandle.style.left);
		var xLengthLabel = Half(hl + hr + m_dxHandleImage - m_dxLengthLabel);
		var bStartLabelAlignLeft = (Int(m_oTransparentL.style.width) < m_dxStartLabel);
		var bEndLabelAlignLeft = (Int(m_oTransparentR.style.width) > m_dxEndLabel);
		var xStartLabel = (bStartLabelAlignLeft ? xOffsetStartHandle : hl - m_dxStartLabel + m_dxHandleImage);
		var xEndLabel = (bEndLabelAlignLeft ? hr : xOffsetEndHandle - m_dxEndLabel + m_dxHandleImage);

		m_oLengthLabel = document.createElement('SPAN');
		m_oLengthLabel.className = 'clsLengthLabel'; 
		m_oLengthLabel.style.textAlign = 'center';
		m_oLengthLabel.style.left = xLengthLabel + 'px';
		m_oLengthLabel.style.top = yBorderTop + yOffsetLengthLabel + 'px';
		m_oLengthLabel.style.width = m_dxLengthLabel + 'px';
		oClipDisplayContainer.appendChild(m_oLengthLabel);

		m_oStartLabel = document.createElement('SPAN');
		m_oStartLabel.className = 'clsStartLabel'; 
		m_oStartLabel.style.textAlign = (bStartLabelAlignLeft ? 'left' : 'right');
		m_oStartLabel.style.left = xStartLabel + 'px';
		m_oStartLabel.style.top = yBorderTop + m_dyHandleImage + 'px';
		m_oStartLabel.style.width = m_dxStartLabel + 'px';
		oClipDisplayContainer.appendChild(m_oStartLabel);		

		m_oEndLabel = document.createElement('SPAN');
		m_oEndLabel.className = 'clsEndLabel'; 
		m_oEndLabel.style.textAlign = (bEndLabelAlignLeft ? 'left' : 'right');
		m_oEndLabel.style.left = xEndLabel + 'px';
		m_oEndLabel.style.top = yBorderTop + m_dyHandleImage + 'px';
		m_oEndLabel.style.width = m_dxEndLabel + 'px';
		oClipDisplayContainer.appendChild(m_oEndLabel);		

		oClipDisplayContainer.appendChild(m_oClipRect);

		// Attach all the newly created elements to the document
		var oClipDisplayContainerContainer = document.getElementById(idClipDisplayContainer);
		oClipDisplayContainerContainer.appendChild(oClipDisplayContainer);
	}

	// private
	this.CreateTimelineElements = CreateTimelineElements;
	function CreateTimelineElements(oClipDisplayContainer)
	{
		if (!m_msClipLength)
			return;

		// Timeline Constants
		var iTickFrequency = 25; // pixels
		var iLabelFrequency = 100; // pixels
		var iTickHeightPercent = 8; // percent
		var iLabelWidth = 25; // pixels
		var iLabelHeight = 25; // pixels

		// Calculate the tick spacing and the time per tick
		var fxScale = m_dxClipDisplay / m_msClipLength;
		var fTickSpacing = 0; // pixels
		var msTimePerTick = 0;
		while (fTickSpacing < iTickFrequency)
		{
			if (msTimePerTick < 5000)
				msTimePerTick += 1000; // 1 second increments
			else
			if (msTimePerTick < 10000)
				msTimePerTick += 5000; // 5 second increments
			else
			if (msTimePerTick < 15000)
				msTimePerTick += 10000; // 10 second increments
			else
			if (msTimePerTick < 30000)
				msTimePerTick += 15000; // 15 second increments
			else
			if (msTimePerTick < 60000)
				msTimePerTick += 30000; // 30 second increments
			else
				msTimePerTick += 60000; // 60 second increments
			fTickSpacing = msTimePerTick * fxScale;
		}

		// Add the timeline elements; ticks and labels
		var iTickHeight = Math.round(m_dyClipDisplay * (iTickHeightPercent / 100));
		var iTicksPerLabel = Math.round(iLabelFrequency / fTickSpacing);
		var fTickPosition = 0;
		var iTickCount = 0;
		var msTime = 0;
		while ((fTickPosition += fTickSpacing) < m_dxClipDisplay)
		{
			// Create a tick
			var oTick = document.createElement('DIV');
			oTick.className = 'clsTicks'; 
			oTick.style.left = Math.round(fTickPosition) + 'px';
			oTick.style.top = '0px';
			oTick.style.width = '1px';
			oTick.style.height = iTickHeight + 'px';
			oClipDisplayContainer.appendChild(oTick);	

			msTime += msTimePerTick;
			if (++iTickCount % iTicksPerLabel)
				continue;

			// Create a label
			var iLabelLeft = Math.round(fTickPosition - (iLabelWidth / 2));
			if (iLabelLeft + iLabelWidth > m_dxClipDisplay)
				continue;

			var oTickLabel = document.createElement('DIV');
			oTickLabel.className = 'clsTickLabels'; 
			oTickLabel.style.left = iLabelLeft + 'px';
			oTickLabel.style.top = iTickHeight + 2 + 'px';
			oTickLabel.style.width = iLabelWidth + 'px';
			oTickLabel.style.height = iLabelHeight + 'px';
			oTickLabel.innerHTML = FormatTime(msTime, false/*bShowTenths*/); 
			oClipDisplayContainer.appendChild(oTickLabel);	
		}
	}

	// private
	this.OnImageMouseDown = OnImageMouseDown;
	function OnImageMouseDown(evt)
	{
		if (typeof(evt) == 'undefined')
			evt = event;
		if (evt.target)
			source = evt.target;
		else
		if (evt.srcElement)
			source = evt.srcElement;
		if (source.nodeType == 3) // defeat Safari bug
			source = source.parentNode;
		if (source.id && source.id.indexOf('resize') >= 0)
			return;	
		
		m_xDownMouse = evt.clientX;
		m_ixDown = Int(m_oClipRect.style.left);
		m_dxDown = Int(m_oClipRect.style.width);
		
		m_bImageMouseDown = true;
		return false;
	}

	// private
	this.OnHandleMouseDown = OnHandleMouseDown;
	function OnHandleMouseDown(evt)
	{
		if (typeof(evt) == 'undefined')
			evt = event;
		m_oClipRect.style.cursor = 'e-resize';
		m_idHandleGrabbed = this.id;

		m_xDownMouse = evt.clientX;
		m_ixDown = Int(m_oClipRect.style.left);
		m_dxDown = Int(m_oClipRect.style.width);
		
		m_bHandleMouseDown = true;
		return false;
	}
	
	// private
	this.OnElementCancelEvent = OnElementCancelEvent;
	function OnElementCancelEvent(evt)
	{
		if (typeof(evt) == 'undefined')
			evt = event;
		if (evt.target)
			source = evt.target;
		else
		if (evt.srcElement)
			source = evt.srcElement;
		if (source.nodeType == 3) // defeat Safari bug
			source = source.parentNode;
						
		if (source.tagName && source.tagName.toLowerCase() == 'input')
			return true;

		return false;
	}
	
	// private
	this.OnElementMouseMove = OnElementMouseMove;
	function OnElementMouseMove(evt)
	{
		if (typeof(evt) == 'undefined')
			evt = event;
		if (m_bMouseMoveEventInProgress)
			return;
		if (!m_bImageMouseDown && !m_bHandleMouseDown)
			return;
		if (document.all)
			m_bMouseMoveEventInProgress = true;

		var dxMouse = evt.clientX - m_xDownMouse;

		// Move one of the handles
		if (m_bHandleMouseDown)
		{
			var L = Int(m_oClipRect.style.left);
			var R = Int(m_oClipRect.style.width) + L;
			
			if (m_idHandleGrabbed == 'w-resize')
			{
				var LDown = m_ixDown;
				L = LDown + dxMouse;
				if (L > R - m_iMinSize)
					L = R - m_iMinSize;
				if (L < 0)
					L = 0;
				var bBypassClipMax = evt.shiftKey || evt.ctrlKey;
				if (!bBypassClipMax)
				{ // move the other handle
					if (R - L > m_dxClipMax)
						R = L + m_dxClipMax;
				}
			}
			else
			if (m_idHandleGrabbed == 'e-resize')
			{
				var RDown = m_ixDown + m_dxDown;
				R = RDown + dxMouse;
				if (R < L + m_iMinSize)
					R = L + m_iMinSize;
				if (R > m_dxClipDisplay)
					R = m_dxClipDisplay;
				var bBypassClipMax = evt.shiftKey || evt.ctrlKey;
				if (!bBypassClipMax)
				{ // move the other handle
					if (R - L > m_dxClipMax)
						L = R - m_dxClipMax;
				}
			}

			m_oClipRect.style.left = L + 'px';
			m_oClipRect.style.width = (R - L) + 'px';
		}
		else
		// Move the entire clip window
		if (m_bImageMouseDown)
		{
			var ix = Int(m_oClipRect.style.left);

			ix = m_ixDown + dxMouse;
			if (ix < 0)
				ix = 0;
			if (ix > m_dxClipDisplay - m_dxDown)
				ix = m_dxClipDisplay - m_dxDown;

			m_oClipRect.style.left = ix + 'px';
		}
		
		if (m_bHandleMouseDown || m_bImageMouseDown)
		{
			RepositionElements();
			SetHiddenClipValues();
		}

		m_bMouseMoveEventInProgress = false;
	}
	
	// private
	this.OnElementMouseUp = OnElementMouseUp;
	function OnElementMouseUp(evt)
	{
		if (typeof(evt) == 'undefined')
			evt = event;
		if (!m_bHandleMouseDown && !m_bImageMouseDown)
			return;
			
		m_bHandleMouseDown = false;
		m_bImageMouseDown = false;
		m_oClipRect.style.cursor = 'move';

		SetHiddenClipValues();
		if (m_fnOnClipChanged)
			m_fnOnClipChanged();
	}
	
	// private
	this.RepositionElements = RepositionElements;
	function RepositionElements()
	{
		var ix = Int(m_oClipRect.style.left);
		var dx = Int(m_oClipRect.style.width);

		// Repostion the transparent rectangles
		m_oTransparentL.style.width = ix + 'px';

		m_oTransparentR.style.left = (ix + dx) + 'px';
		m_oTransparentR.style.width = (m_dxClipDisplay - dx - ix) + 'px';

		m_oTransparentL.style.visibility = (m_oTransparentL.style.width == '0px' ? 'hidden' : 'visible');
		m_oTransparentR.style.visibility = (m_oTransparentR.style.width == '0px' ? 'hidden' : 'visible');

		// Repostion the border rectangle
		m_oBorderRect.style.left = ix - m_iBorderSize + 'px';
		m_oBorderRect.style.width = dx + 2 + 'px';

		// Reposition the handles
		m_oEndHandle.style.left = (dx + m_iBorderSize + 2 - m_dxHandleOverhang) + 'px';

		// Reposition the labels
		var xOffsetStartHandle = m_dxHandleOverhang - m_dxHandleImage - m_iBorderSize;
		var xOffsetEndHandle = m_dxClipDisplay + m_iBorderSize + 2 - m_dxHandleOverhang;
		var hl = ix + Int(m_oStartHandle.style.left);
		var hr = ix + Int(m_oEndHandle.style.left);
		var xLengthLabel = Half(hl + hr + m_dxHandleImage - m_dxLengthLabel);
		var bStartLabelAlignLeft = (Int(m_oTransparentL.style.width) < m_dxStartLabel);
		var bEndLabelAlignLeft = (Int(m_oTransparentR.style.width) > m_dxEndLabel);
		var xStartLabel = (bStartLabelAlignLeft ? xOffsetStartHandle : hl - m_dxStartLabel + m_dxHandleImage);
		var xEndLabel = (bEndLabelAlignLeft ? hr : xOffsetEndHandle - m_dxEndLabel + m_dxHandleImage);

		// Make adjustments to the start and end labels if they overlap
		if (xEndLabel < xStartLabel + m_dxStartLabel + m_iMinLabelGap)
		{
			if (Int(m_oTransparentL.style.width) < Int(m_oTransparentR.style.width))
				xEndLabel = xStartLabel + m_dxStartLabel + m_iMinLabelGap; // adjust the end label
			else
				xStartLabel = xEndLabel - m_dxStartLabel - m_iMinLabelGap; // adjust the start label
		}

		m_oLengthLabel.style.left = xLengthLabel + 'px';
		m_oStartLabel.style.textAlign = (bStartLabelAlignLeft ? 'left' : 'right');
		m_oStartLabel.style.left = xStartLabel + 'px';
		m_oEndLabel.style.textAlign = (bEndLabelAlignLeft ? 'left' : 'right');
		m_oEndLabel.style.left = xEndLabel + 'px';
	}
	
	// private
	this.SetHiddenClipValues = SetHiddenClipValues;
	function SetHiddenClipValues()
	{
		if (!m_msClipLength)
			return;

		var ix = Int(m_oClipRect.style.left);
		var dx = Int(m_oClipRect.style.width);

		var fxScale = m_msClipLength / m_dxClipDisplay;
		var iLength = Math.round(dx * fxScale);
		var iStart = Math.round(ix * fxScale);
		var iEnd = iStart + iLength;

		// Mixxer does not want to show tenths
		var bShowStartTenths = false; //j (m_bImageMouseDown || (m_bHandleMouseDown && m_idHandleGrabbed == 'w-resize'));
		var bShowEndTenths = false; //j (m_bImageMouseDown || (m_bHandleMouseDown && m_idHandleGrabbed == 'e-resize'));
		var bShowLengthTenths = false; //j (bShowStartTenths || bShowEndTenths);

		m_oLengthLabel.innerHTML = 'Length ' + FormatTime(iLength, bShowLengthTenths);
		m_oStartLabel.innerHTML = 'Start ' + FormatTime(iStart, bShowStartTenths);
		m_oEndLabel.innerHTML = 'End ' + FormatTime(iEnd, bShowEndTenths);

		if (m_oHiddenClipStart && m_oHiddenClipEnd)
		{
			m_oHiddenClipStart.value = iStart;
			m_oHiddenClipEnd.value = iEnd;
		}
	}
	
	// private
	this.Scale = Scale;
	function Scale(dxSrc, dxDst)
	{
		// Scale the source to fit the destination
		return dxDst / dxSrc;
	}

	// private
	this.FormatTime = FormatTime;
	function FormatTime(iMilliSeconds, bShowTenths)
	{
		var iTenths = Math.round(iMilliSeconds / 100);
		var iSeconds = Math.floor(iTenths / 10);
		var iMinutes = Math.floor(iSeconds / 60);
		iSeconds %= 60;
		iTenths %= 10;
		var sMinutes = (!iMinutes ? '0' : iMinutes);
		var sSeconds = (iSeconds < 10 ? '0' + iSeconds : iSeconds);
		var sTenths = (bShowTenths ? '.' + iTenths : '');
		return sMinutes + ':' + sSeconds + sTenths;
	}

	// private
	this.Int = Int;
	function Int(str)
	{
		return str.replace('px','')/1;
	}

	// private
	this.Half = Half;
	function Half(val)
	{
		return Math.floor(val / 2);
	}
}
