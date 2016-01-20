// A Javascript class that implements picture cropping
// Sample calling HTML:
//
//	<link rel="stylesheet" type="text/css" href="MobilizeCrop.css">
//	<script type="text/javascript" src="MobilizeCrop.js"></script>
//	<script type="text/javascript">
//		var strOrigImageFileName = 'http://www.facetofacesoftware.com/mobilize/images/sample.jpg';
//		var MobilizeCrop = new CMobilizeCrop(
//			strOrigImageFileName, // the user's image filename
//			1734, 724,	// width and height of the user's image
//			176, 220,	// width and height of the user's phone display
//			400, 300,	// width and height of space available to (parent of) HTML element idCropDisplayContainer
//			128, 160,	// width and height of space available to (parent of) HTML element idPhoneDisplayContainer
//			0, 0);		// desired xy offset of image within the black phone display area
//	</script>
//...
//	<body onload="MobilizeCrop.Initialize()">
//		<table>
//		<tr>
//			<td style="width:400px; height:300px;">
//				<div id="idCropDisplayContainer" style="margin-right:10px;">
//					<img id="idCropDisplay" src="images/Dummy.jpg" alt="" />
//				</div>
//			</td>
//			<td style="vertical-align:top; text-align:center;">
//				<div style="text-align:left;">
//					<label for="Freeform"><input type="radio" name="EditOptions" id="Freeform" onclick="MobilizeCrop.OnFreeform()" checked="checked" />&nbsp;Freeform (Crop/resize)</label><br />
//					<label for="StretchToFit"><input type="radio" name="EditOptions" id="StretchToFit" onclick="MobilizeCrop.OnStretchToFit()" />&nbsp;Stretch to fit</label><br />
//					<label for="MaintainAspect"><input type="radio" name="EditOptions" id="MaintainAspect"  onclick="MobilizeCrop.OnMaintainAspect()" />&nbsp;Perfect fit (Keep shape)</label><br />
//				</div>
//				<div style="margin-top:20px; margin-bottom:10px;">
//					<b>My Sony Ericsson Z500</b>
//				</div>
//				<table cellpadding="0px" cellspacing="0px">
//					<tr>
//						<td style="padding:0px; width:11px; height:11px; background-image:url(images/GenericPhoneLT.jpg); background-repeat:no-repeat;" ></td>
//						<td style="padding:0px; width:11px; height:11px; background-image:url(images/GenericPhoneCT.jpg); background-repeat:repeat-x;" ></td>
//						<td style="padding:0px; width:11px; height:11px; background-image:url(images/GenericPhoneRT.jpg); background-repeat:no-repeat;" ></td>
//					</tr>
//					<tr>
//						<td style="padding:0px; width:11px; height:11px; background-image:url(images/GenericPhoneLC.jpg); background-repeat:repeat-y;" ></td>
//						<td style="padding:0px; background-color:black;">
//							<div style="width:128px; height:160px; text-align:left; vertical-align:top;">
//								<div id="idPhoneDisplayContainer">
//									<img id="idPhoneDisplay" src="images/Dummy.jpg" alt="" />
//								</div>
//							</div>
//						</td>
//						<td style="padding:0px; width:11px; height:11px; background-image:url(images/GenericPhoneRC.jpg); background-repeat:repeat-y;" ></td>
//					</tr>
//					<tr>
//						<td style="padding:0px; width:11px; height:16px; background-image:url(images/GenericPhoneLB.jpg); background-repeat:no-repeat;" ></td>
//						<td style="padding:0px; width:11px; height:16px; background-image:url(images/GenericPhoneCB.jpg); background-repeat:repeat-x;" ></td>
//						<td style="padding:0px; width:11px; height:16px; background-image:url(images/GenericPhoneRB.jpg); background-repeat:no-repeat;" ></td>
//					</tr>
//				</table>
//			</td>
//		</tr>
//		</table>
//		<div>
//			<input type="hidden" id="idCropIX" value="0" />
//			<input type="hidden" id="idCropIY" value="0" />
//			<input type="hidden" id="idCropDX" value="0" />
//			<input type="hidden" id="idCropDY" value="0" />
//			<input type="hidden" id="idSizeDX" value="0" />
//			<input type="hidden" id="idSizeDY" value="0" />
//		</div>
//	</body>

function CMobilizeCrop(
	strUserImageFileName,
	dxUserImage, dyUserImage,
	dxUserPhone, dyUserPhone,
	dxCropDisplay, dyCropDisplay,
	dxPhoneDisplay, dyPhoneDisplay,
	xPhoneDisplay, yPhoneDisplay)
{
	// Start of Program variables

	// Width of border around crop rectangle
	var m_iBorderSize = 3;

	// Size of handles used to resize crop rectangle
	var m_iHandleSize = 7;

	// User's image to crop
	var m_strUserImageFileName = strUserImageFileName;
	
	// Size of user's image
	var m_dxUserImage = dxUserImage;
	var m_dyUserImage = dyUserImage;

	// Size of user's phone display
	var m_dxUserPhone = dxUserPhone;
	var m_dyUserPhone = dyUserPhone;

	// Size of crop display window
	var m_dxCropDisplay = dxCropDisplay;
	var m_dyCropDisplay = dyCropDisplay;

	// Location of the display in the image of the phone
	var m_xPhoneDisplay = xPhoneDisplay;
	var m_yPhoneDisplay = yPhoneDisplay;

	// Size of the display in the image of the phone
	var m_dxPhoneDisplay = dxPhoneDisplay;
	var m_dyPhoneDisplay = dyPhoneDisplay;
	
	// Fixed aspect ratio; width of crop rectangle relative to height
	// Set to 0 for no fixed aspect ratio
	var m_fMaintainAspect = 0;

	var m_bStretchToFit = false;
	
	// End of program variables

	var m_iMinSize = (2 * (m_iHandleSize + m_iBorderSize)) + 1;	// Minimum width and height of crop area
	var m_bIsOpera = (navigator.userAgent.indexOf('Opera') >= 0);

	var m_oPhoneDisplayContainer = null;
	var m_oPhoneDisplayImage = null;
	var m_oCropRect = null;
	var m_oBorderRect = null;
	var m_oTransparentL = null;
	var m_oTransparentT = null;
	var m_oTransparentR = null;
	var m_oTransparentB = null;  
	var m_oHandle_lt = null;
	var m_oHandle_lc = null;
	var m_oHandle_lb = null;
	var m_oHandle_ct = null;
	var m_oHandle_cb = null;
	var m_oHandle_rt = null;
	var m_oHandle_rc = null;
	var m_oHandle_rb = null;
	var m_idHandle = null;
	var m_bHandleMouseDown = false;
	var m_bImageMouseDown = false;
	var m_xDownMouse = 0;
	var m_yDownMouse = 0;
	var m_ixDown = 0;
	var m_iyDown = 0;
	var m_dxDown = 0;
	var m_dyDown = 0;
	var m_bMouseMoveEventInProgress = false;
	
	this.Initialize = Initialize;
	function Initialize()
	{
		// Set events
		document.documentElement.ondragstart = OnElementCancelEvent;
		document.documentElement.onselectstart = OnElementCancelEvent;
		document.documentElement.onmousemove = OnElementMouseMove;		
		document.documentElement.onmouseup = OnElementMouseUp;		
		
		CreateElements();
		UpdatePhoneDisplay();
		SetHiddenCropValues();
	}

	this.CreateElements = CreateElements;
	function CreateElements()
	{
		m_dxCropDisplay -= (2 * m_iHandleSize);
		m_dyCropDisplay -= (2 * m_iHandleSize);
		var dxOrigCropDisplay = m_dxCropDisplay;
		var dyOrigCropDisplay = m_dyCropDisplay;

		// Use the phone display size as the minimum image size
		var dxImage = m_dxUserImage;
		var dyImage = m_dyUserImage;
		if (dxImage <= m_dxPhoneDisplay && dyImage <= m_dyPhoneDisplay)
		{
			dxImage = m_dxPhoneDisplay;
			dyImage = m_dyPhoneDisplay;
		}

		// If the image is smaller than the container, reduce the size of the container
		if (dxImage <= m_dxCropDisplay && dyImage <= m_dyCropDisplay)
		{
			m_dxCropDisplay = dxImage;
			m_dyCropDisplay = dyImage;
		}

		// Compute the scale factor that fits the user's image into the crop display window
		var fxScale = ScaleToFit(m_dxUserImage, m_dyUserImage, m_dxCropDisplay, m_dyCropDisplay, true/*bUseSmallerFactor*/);
		var fyScale = fxScale;

		m_dxCropDisplay = Math.round(m_dxUserImage * fxScale);
		m_dyCropDisplay = Math.round(m_dyUserImage * fyScale);
		var xOffset = Math.max(0, Half(dxOrigCropDisplay - m_dxCropDisplay));
		var yOffset = Math.max(0, Half(dyOrigCropDisplay - m_dyCropDisplay));

		var oCropDisplayContainer = document.getElementById('idCropDisplayContainer');
		oCropDisplayContainer.style.left = xOffset + 'px';
		// We don't need the yOffset becuase the HTML takes care of it
		// oCropDisplayContainer.style.top = yOffset + 'px';

		var oCropDisplay = document.getElementById('idCropDisplay');
		oCropDisplay.src = m_strUserImageFileName;
		oCropDisplay.style.width =  m_dxCropDisplay + 'px';
		oCropDisplay.style.height = m_dyCropDisplay + 'px';

		m_oPhoneDisplayContainer = document.getElementById('idPhoneDisplayContainer'); // Set size and location in UpdatePhoneDisplay()

		m_oPhoneDisplayImage = document.getElementById('idPhoneDisplay'); // Set size in UpdatePhoneDisplay()
		m_oPhoneDisplayImage.src = m_strUserImageFileName;

		m_oTransparentL = document.createElement('DIV');
		m_oTransparentL.className = 'clsTransparent'; 
		m_oTransparentL.style.left = '0px';
		m_oTransparentL.style.top = '0px';
		m_oTransparentL.style.height = m_dyCropDisplay + 'px';
		m_oTransparentL.style.width = '0px';
		m_oTransparentL.innerHTML = '<span></span>';
		oCropDisplayContainer.appendChild(m_oTransparentL);
		
		m_oTransparentT = document.createElement('DIV');
		m_oTransparentT.className = 'clsTransparent'; 
		m_oTransparentT.style.left = '0px';
		m_oTransparentT.style.top = '0px';
		m_oTransparentT.style.height = '0px';
		m_oTransparentT.style.width = m_dxCropDisplay + 'px';
		m_oTransparentT.innerHTML = '<span></span>';
		oCropDisplayContainer.appendChild(m_oTransparentT);
		
		m_oTransparentR = document.createElement('DIV');
		m_oTransparentR.className = 'clsTransparent'; 
		m_oTransparentR.style.left = m_dxCropDisplay + 'px';
		m_oTransparentR.style.top = '0px';
		m_oTransparentR.style.height = m_dyCropDisplay + 'px';
		m_oTransparentR.style.width = '0px';		
		m_oTransparentR.innerHTML = '<span></span>';
		oCropDisplayContainer.appendChild(m_oTransparentR); 
		
		m_oTransparentB = document.createElement('DIV');
		m_oTransparentB.className = 'clsTransparent'; 
		m_oTransparentB.style.left = '0px';
		m_oTransparentB.style.top = m_dyCropDisplay + 'px';
		m_oTransparentB.style.height = '0px';
		m_oTransparentB.style.width = m_dxCropDisplay + 'px';
		m_oTransparentB.innerHTML = '<span></span>';
		oCropDisplayContainer.appendChild(m_oTransparentB); 
		
		m_oBorderRect = document.createElement('DIV');
		m_oBorderRect.className = 'clsBorderRect';
		m_oBorderRect.style.left = 0 - m_iBorderSize + 'px';
		m_oBorderRect.style.top = 0 - m_iBorderSize + 'px';
		m_oBorderRect.style.width = m_dxCropDisplay + 'px';
		m_oBorderRect.style.height = m_dyCropDisplay + 'px';
		m_oBorderRect.innerHTML = '<div></div>'; 
		m_oBorderRect.style.cursor = 'move';
		m_oBorderRect.style.borderWidth = m_iBorderSize + 'px';
		oCropDisplayContainer.appendChild(m_oBorderRect); 
		
		m_oCropRect = document.createElement('DIV');
		m_oCropRect.className = 'clsCropRect';
		m_oCropRect.style.left = '0px';
		m_oCropRect.style.top = '0px';
		m_oCropRect.style.width = m_dxCropDisplay + 'px';
		m_oCropRect.style.height = m_dyCropDisplay + 'px';
		m_oCropRect.innerHTML = '<div></div>'; 
		m_oCropRect.style.cursor = 'move';
		m_oCropRect.onmousedown = OnImageMouseDown;
		oCropDisplayContainer.appendChild(m_oCropRect);
		
		if (m_bIsOpera)
		{
			var div = m_oCropRect.getElementsByTagName('DIV')[0];
			div.style.backgroundColor = 'transparent';
			m_oTransparentB.style.backgroundColor = 'transparent';
			m_oTransparentR.style.backgroundColor = 'transparent';
			m_oTransparentT.style.backgroundColor = 'transparent';
			m_oTransparentL.style.backgroundColor = 'transparent';
		}
		
		var xl = -m_iHandleSize;
		var xr = m_dxCropDisplay;
		var xc = Half(xr + xl);
		var yt = -m_iHandleSize;
		var yb = m_dyCropDisplay;
		var yc = Half(yb + yt);
		
		m_oHandle_lt = document.createElement('IMG');
		m_oHandle_lt.className = 'clsHandle'; 
		m_oHandle_lt.style.left = xl + 'px';
		m_oHandle_lt.style.top = yt + 'px';
		m_oHandle_lt.style.width = m_iHandleSize + 'px';
		m_oHandle_lt.style.height = m_iHandleSize + 'px';
		m_oHandle_lt.style.cursor = 'nw-resize';
		m_oHandle_lt.id = 'nw-resize';
		m_oHandle_lt.onmousedown = OnHandleMouseDown;
		m_oCropRect.appendChild(m_oHandle_lt);
		
		m_oHandle_rt = document.createElement('IMG');
		m_oHandle_rt.className = 'clsHandle'; 
		m_oHandle_rt.style.left = xr + 'px';
		m_oHandle_rt.style.top = yt + 'px';		
		m_oHandle_rt.style.width = m_iHandleSize + 'px';
		m_oHandle_rt.style.height = m_iHandleSize + 'px';
		m_oHandle_rt.style.cursor = 'ne-resize';
		m_oHandle_rt.id = 'ne-resize';
		m_oHandle_rt.onmousedown = OnHandleMouseDown;
		m_oCropRect.appendChild(m_oHandle_rt);
		
		m_oHandle_lb = document.createElement('IMG');
		m_oHandle_lb.className = 'clsHandle'; 
		m_oHandle_lb.style.left = xl + 'px';
		m_oHandle_lb.style.top = yb + 'px';
		m_oHandle_lb.style.width = m_iHandleSize + 'px';
		m_oHandle_lb.style.height = m_iHandleSize + 'px';
		m_oHandle_lb.style.cursor = 'sw-resize';
		m_oHandle_lb.id = 'sw-resize';
		m_oHandle_lb.onmousedown = OnHandleMouseDown;
		m_oCropRect.appendChild(m_oHandle_lb);
		
		m_oHandle_rb = document.createElement('IMG');
		m_oHandle_rb.className = 'clsHandle'; 
		m_oHandle_rb.style.left = xr + 'px';
		m_oHandle_rb.style.top = yb + 'px';
		m_oHandle_rb.style.width = m_iHandleSize + 'px';
		m_oHandle_rb.style.height = m_iHandleSize + 'px';
		m_oHandle_rb.style.cursor = 'se-resize';
		m_oHandle_rb.id = 'se-resize';
		m_oHandle_rb.onmousedown = OnHandleMouseDown;
		m_oCropRect.appendChild(m_oHandle_rb);
		
		m_oHandle_ct = document.createElement('IMG');
		m_oHandle_ct.className = 'clsHandle'; 
		m_oHandle_ct.style.left = xc + 'px';
		m_oHandle_ct.style.top = yt + 'px';
		m_oHandle_ct.style.width = m_iHandleSize + 'px';
		m_oHandle_ct.style.height = m_iHandleSize + 'px';
		m_oHandle_ct.style.cursor = 's-resize';
		m_oHandle_ct.id = 'n-resize';
		m_oHandle_ct.onmousedown = OnHandleMouseDown;
		m_oCropRect.appendChild(m_oHandle_ct);
		
		m_oHandle_cb = document.createElement('IMG');
		m_oHandle_cb.className = 'clsHandle'; 
		m_oHandle_cb.style.left = xc + 'px';
		m_oHandle_cb.style.top = yb + 'px';
		m_oHandle_cb.style.width = m_iHandleSize + 'px';
		m_oHandle_cb.style.height = m_iHandleSize + 'px';
		m_oHandle_cb.style.cursor = 's-resize';
		m_oHandle_cb.id = 's-resize';
		m_oHandle_cb.onmousedown = OnHandleMouseDown;
		m_oCropRect.appendChild(m_oHandle_cb);
		
		m_oHandle_lc = document.createElement('IMG');
		m_oHandle_lc.className = 'clsHandle'; 
		m_oHandle_lc.style.left = xl + 'px';
		m_oHandle_lc.style.top = yc + 'px';
		m_oHandle_lc.style.width = m_iHandleSize + 'px';
		m_oHandle_lc.style.height = m_iHandleSize + 'px';
		m_oHandle_lc.style.cursor = 'e-resize';
		m_oHandle_lc.id = 'w-resize';
		m_oHandle_lc.onmousedown = OnHandleMouseDown;
		m_oCropRect.appendChild(m_oHandle_lc);	
		
		m_oHandle_rc = document.createElement('IMG');
		m_oHandle_rc.className = 'clsHandle'; 
		m_oHandle_rc.style.left = xr + 'px';
		m_oHandle_rc.style.top = yc + 'px';
		m_oHandle_rc.style.width = m_iHandleSize + 'px';
		m_oHandle_rc.style.height = m_iHandleSize + 'px';
		m_oHandle_rc.style.cursor = 'e-resize';
		m_oHandle_rc.id = 'e-resize';
		m_oHandle_rc.onmousedown = OnHandleMouseDown;
		m_oCropRect.appendChild(m_oHandle_rc);		
	}

	this.OnImageMouseDown = OnImageMouseDown;
	function OnImageMouseDown(e)
	{
		if (document.all)
			e = event;
		if (e.target)
			source = e.target;
		else
		if (e.srcElement)
			source = e.srcElement;
		if (source.nodeType == 3) // defeat Safari bug
			source = source.parentNode;
		if (source.id && source.id.indexOf('resize') >= 0)
			return;	
		
		m_xDownMouse = e.clientX;
		m_yDownMouse = e.clientY;		
		m_ixDown = Int(m_oCropRect.style.left);
		m_iyDown = Int(m_oCropRect.style.top);
		m_dxDown = Int(m_oCropRect.style.width);
		m_dyDown = Int(m_oCropRect.style.height);		
		
		m_bImageMouseDown = true;
		return false;
	}

	this.OnHandleMouseDown = OnHandleMouseDown;
	function OnHandleMouseDown(e)
	{
		if (document.all)
			e = event;
		
		m_oCropRect.style.cursor = 'default';
		m_idHandle = this.id;

		m_xDownMouse = e.clientX;
		m_yDownMouse = e.clientY;
		m_ixDown = Int(m_oCropRect.style.left);
		m_iyDown = Int(m_oCropRect.style.top);
		m_dxDown = Int(m_oCropRect.style.width);
		m_dyDown = Int(m_oCropRect.style.height);
		
		if (document.all)
		{
			var div = m_oCropRect.getElementsByTagName('DIV')[0];
			div.style.display = 'none';
		}
				
		m_bHandleMouseDown = true;
		return false;
	}
	
	this.OnElementCancelEvent = OnElementCancelEvent;
	function OnElementCancelEvent(e)
	{
		if (document.all)
			e = event;
		if (e.target)
			source = e.target;
		else
		if (e.srcElement)
			source = e.srcElement;
		if (source.nodeType == 3) // defeat Safari bug
			source = source.parentNode;
						
		if (source.tagName && source.tagName.toLowerCase() == 'input')
			return true;

		return false;
	}
	
	this.OnElementMouseMove = OnElementMouseMove;
	function OnElementMouseMove(e)
	{
		if (m_bMouseMoveEventInProgress)
			return;
		if (!m_bImageMouseDown && !m_bHandleMouseDown)
			return;
		if (document.all)
			m_bMouseMoveEventInProgress = true;
		if (document.all)
			e = event;

		var dxMouse = e.clientX - m_xDownMouse;
		var dyMouse = e.clientY - m_yDownMouse;

		// Move the crop handle
		if (m_bHandleMouseDown)
		{
			var L = Int(m_oCropRect.style.left);
			var T = Int(m_oCropRect.style.top);
			var R = Int(m_oCropRect.style.width) + L;
			var B = Int(m_oCropRect.style.height) + T;
			
			if (m_idHandle == 'w-resize' || m_idHandle == 'sw-resize' || m_idHandle == 'nw-resize')
			{
				var LDown = m_ixDown;
				L = LDown + dxMouse;
				if (L > R - m_iMinSize)
					L = R - m_iMinSize;
				if (L < 0)
					L = 0;
			}
			
			if (m_idHandle == 'e-resize' || m_idHandle == 'ne-resize' || m_idHandle == 'se-resize')
			{
				var RDown = L + m_dxDown;
				R = RDown + dxMouse;
				if (R < L + m_iMinSize)
					R = L + m_iMinSize;
				if (R > m_dxCropDisplay)
					R = m_dxCropDisplay;
			}

			if (m_idHandle == 'n-resize' || m_idHandle == 'nw-resize' || m_idHandle == 'ne-resize')
			{
				var TDown = m_iyDown;
				T = TDown + dyMouse;
				if (T > B - m_iMinSize)
					T = B - m_iMinSize;
				if (T < 0)
					T = 0;
			}
			
			if (m_idHandle == 's-resize' || m_idHandle == 'sw-resize' || m_idHandle == 'se-resize')
			{
				var BDown = T + m_dyDown;
				B = BDown + dyMouse;
				if (B < T + m_iMinSize)
					B = T + m_iMinSize;
				if (B > m_dyCropDisplay)
					B = m_dyCropDisplay;
			}
			
			if (m_fMaintainAspect) // Preserve Aspect Ratio
			{
				var dx = R - L;
				var dy = B - T;
				var fAspect = dx / dy;
				if (fAspect < m_fMaintainAspect)
					dy = Math.round(dx / m_fMaintainAspect); // Reduce dy
				else
					dx = Math.round(dy * m_fMaintainAspect); // Reduce dx

				if (m_idHandle == 'se-resize')
				{ // Anchor is LT
					R = L + dx;
					B = T + dy;
				}
				
				if (m_idHandle == 'nw-resize')
				{ // Anchor is RB
					L = R - dx;
					T = B - dy;
				}
	
				if (m_idHandle == 'sw-resize')
				{ // Anchor is RT
					L = R - dx;
					B = T + dy;
				}
				
				if (m_idHandle == 'ne-resize')
				{ // Anchor is LB
					R = L + dx;
					T = B - dy;
				}
			}

			m_oCropRect.style.left = L + 'px';
			m_oCropRect.style.top = T + 'px';
			m_oCropRect.style.width = (R - L) + 'px';
			m_oCropRect.style.height = (B - T) + 'px';
		}
		
		// Move the crop window
		if (m_bImageMouseDown)
		{
			var ix = Int(m_oCropRect.style.left);
			var iy = Int(m_oCropRect.style.top);

			ix = m_ixDown + dxMouse;
			if (ix < 0)
				ix = 0;
			if (ix > m_dxCropDisplay - m_dxDown)
				ix = m_dxCropDisplay - m_dxDown;
			iy = m_iyDown + dyMouse;
			if (iy < 0)
				iy = 0;
			if (iy > m_dyCropDisplay - m_dyDown)
				iy = m_dyCropDisplay - m_dyDown;

			m_oCropRect.style.left = ix + 'px';
			m_oCropRect.style.top = iy + 'px';
		}
		
		if (m_bHandleMouseDown || m_bImageMouseDown)
		{
			RepositionElements();
			UpdatePhoneDisplay();
			SetHiddenCropValues();
		}

		m_bMouseMoveEventInProgress = false;
	}
	
	this.OnElementMouseUp = OnElementMouseUp;
	function OnElementMouseUp()
	{
		m_bHandleMouseDown = false;
		m_bImageMouseDown = false;
		m_oCropRect.style.cursor = 'move';
		if (document.all)
		{
			var div = m_oCropRect.getElementsByTagName('DIV')[0];
			div.style.display = 'block';
		}
	}
	
	this.OnFreeform = OnFreeform;
	function OnFreeform()
	{
		m_fMaintainAspect = 0;
		m_bStretchToFit = false;
		RepositionElements();
		UpdatePhoneDisplay();
		SetHiddenCropValues();
	}

	this.OnStretchToFit = OnStretchToFit;
	function OnStretchToFit()
	{
		m_fMaintainAspect = 0;
		m_bStretchToFit = true;
		RepositionElements();
		UpdatePhoneDisplay();
		SetHiddenCropValues();
	}

	this.OnMaintainAspect = OnMaintainAspect;
	function OnMaintainAspect()
	{
		m_fMaintainAspect = m_dxPhoneDisplay / m_dyPhoneDisplay;
		m_bStretchToFit = false;
		CorrectAspect();
		RepositionElements();
		UpdatePhoneDisplay();
		SetHiddenCropValues();
	}

	this.CorrectAspect = CorrectAspect;
	function CorrectAspect()
	{
		if (!m_fMaintainAspect)
			return;

		var dx = Int(m_oCropRect.style.width);
		var dy = Int(m_oCropRect.style.height);

		var fAspect = dx / dy;
		if (fAspect < m_fMaintainAspect)
			dy = Math.round(dx / m_fMaintainAspect); // Reduce dy
		else
			dx = Math.round(dy * m_fMaintainAspect); // Reduce dx

		m_oCropRect.style.width = dx + 'px';
		m_oCropRect.style.height = dy + 'px';
	}

	this.RepositionElements = RepositionElements;
	function RepositionElements()
	{
		var ix = Int(m_oCropRect.style.left);
		var iy = Int(m_oCropRect.style.top);
		var dx = Int(m_oCropRect.style.width);
		var dy = Int(m_oCropRect.style.height);

		// Repostion the transparent rectangles
		m_oTransparentL.style.width = ix + 'px';

		m_oTransparentR.style.left = (dx + ix) + 'px';
		m_oTransparentR.style.width = (m_dxCropDisplay - dx - ix) + 'px';

		m_oTransparentT.style.left = ix + 'px';
		m_oTransparentT.style.height = iy + 'px';
		m_oTransparentT.style.width = dx + 'px' ;

		m_oTransparentB.style.left = ix + 'px';
		m_oTransparentB.style.top = (dy + iy) + 'px';
		m_oTransparentB.style.height = (m_dyCropDisplay - dy - iy) + 'px';
		m_oTransparentB.style.width = dx + 'px' ;
		
		m_oTransparentL.style.visibility = (m_oTransparentL.style.width == '0px' ? 'hidden' : 'visible');
		m_oTransparentR.style.visibility = (m_oTransparentR.style.width == '0px' ? 'hidden' : 'visible');
		m_oTransparentT.style.visibility = (m_oTransparentT.style.width == '0px' ? 'hidden' : 'visible');
		m_oTransparentB.style.visibility = (m_oTransparentB.style.width == '0px' ? 'hidden' : 'visible');

		// Repostion the border rectangle
		m_oBorderRect.style.left = ix - m_iBorderSize + 'px';
		m_oBorderRect.style.top = iy - m_iBorderSize + 'px';
		m_oBorderRect.style.width = dx + 'px';
		m_oBorderRect.style.height = dy + 'px';

		// Reposition the handles
		var xl = Int(m_oHandle_lt.style.left);
		var yt = Int(m_oHandle_lt.style.top);

		var xr = dx;
		m_oHandle_rt.style.left = xr + 'px';
		m_oHandle_rc.style.left = xr + 'px';
		m_oHandle_rb.style.left = xr + 'px';

		var yb = dy;
		m_oHandle_rb.style.top = yb + 'px';
		m_oHandle_cb.style.top = yb + 'px';
		m_oHandle_lb.style.top = yb + 'px';

		var xc = Half(xr + xl);
		m_oHandle_ct.style.left = xc + 'px';
		m_oHandle_cb.style.left = xc + 'px';

		var yc = Half(yb + yt);
		m_oHandle_lc.style.top = yc + 'px';
		m_oHandle_rc.style.top = yc + 'px';
		
		m_oHandle_lc.style.visibility = (m_fMaintainAspect ? 'hidden' : 'visible');
		m_oHandle_rc.style.visibility = (m_fMaintainAspect ? 'hidden' : 'visible');
		m_oHandle_ct.style.visibility = (m_fMaintainAspect ? 'hidden' : 'visible');
		m_oHandle_cb.style.visibility = (m_fMaintainAspect ? 'hidden' : 'visible');
	}
	
	this.UpdatePhoneDisplay = UpdatePhoneDisplay;
	function UpdatePhoneDisplay()
	{
		var ix = Int(m_oCropRect.style.left);
		var iy = Int(m_oCropRect.style.top);
		var dx = Int(m_oCropRect.style.width);
		var dy = Int(m_oCropRect.style.height);

		// Compute the scale factor that fits the cropped image into the phone display
		var fxScale = 0;
		var fyScale = 0;
		if (m_bStretchToFit)
		{
			fxScale = Scale(dx, m_dxPhoneDisplay);
			fyScale = Scale(dy, m_dyPhoneDisplay);
		}
		else
			fxScale = fyScale = ScaleToFit(dx, dy, m_dxPhoneDisplay, m_dyPhoneDisplay, true/*bUseSmallerFactor*/);

		var ixPhoneDisplayContainer = Math.round(ix * fxScale);
		var iyPhoneDisplayContainer = Math.round(iy * fyScale);
		var dxPhoneDisplayContainer = Math.round(dx * fxScale);
		var dyPhoneDisplayContainer = Math.round(dy * fyScale);

		// Hide the Container and the Image (to avoid flicker when changing display attributes)
		m_oPhoneDisplayContainer.style.visibility = 'hidden';
		m_oPhoneDisplayImage.style.visibility = 'hidden';

		var dxPhoneDisplayImage = Math.round(m_dxCropDisplay * fxScale);
		var dyPhoneDisplayImage = Math.round(m_dyCropDisplay * fyScale);
		m_oPhoneDisplayImage.style.width = dxPhoneDisplayImage + 'px';
		m_oPhoneDisplayImage.style.height = dyPhoneDisplayImage + 'px';

		// Since we are centering the container in the parent, the parent should use style="text-align:left; vertical-align:top;"
		m_oPhoneDisplayContainer.style.left = m_xPhoneDisplay + Half(m_dxPhoneDisplay - dxPhoneDisplayContainer) + 'px';
		m_oPhoneDisplayContainer.style.top = m_yPhoneDisplay + Half(m_dyPhoneDisplay - dyPhoneDisplayContainer) + 'px';
		m_oPhoneDisplayContainer.style.width = dxPhoneDisplayContainer + 'px';
		m_oPhoneDisplayContainer.style.height = dyPhoneDisplayContainer + 'px';
		m_oPhoneDisplayContainer.scrollLeft = ixPhoneDisplayContainer;
		m_oPhoneDisplayContainer.scrollTop = iyPhoneDisplayContainer;

		// Show the Container and the Image
		m_oPhoneDisplayContainer.style.visibility = 'visible';
		m_oPhoneDisplayImage.style.visibility = 'visible';
	}
	
	this.SetHiddenCropValues = SetHiddenCropValues;
	function SetHiddenCropValues()
	{
		var ix = Int(m_oCropRect.style.left);
		var iy = Int(m_oCropRect.style.top);
		var dx = Int(m_oCropRect.style.width);
		var dy = Int(m_oCropRect.style.height);

		var fxScale = m_dxUserImage / m_dxCropDisplay;
		var fyScale = m_dyUserImage / m_dyCropDisplay;

		document.getElementById('idCropIX').value = Math.round(ix * fxScale);
		document.getElementById('idCropIY').value = Math.round(iy * fyScale);
		document.getElementById('idCropDX').value = Math.round(dx * fxScale);
		document.getElementById('idCropDY').value = Math.round(dy * fyScale);

		if (m_bStretchToFit)
		{
			document.getElementById('idSizeDX').value = m_dxUserPhone;
			document.getElementById('idSizeDY').value = m_dyUserPhone;
		}
		else
		{
			fxScale = fyScale = ScaleToFit(dx, dy, m_dxUserPhone, m_dyUserPhone, true/*bUseSmallerFactor*/);
			document.getElementById('idSizeDX').value = Math.round(dx * fyScale);
			document.getElementById('idSizeDY').value = Math.round(dy * fyScale);
		}
	}	
	
	this.ScaleToFit = ScaleToFit;
	function ScaleToFit(dxSrc, dySrc, dxDst, dyDst, bUseSmallerFactor)
	{
		// Scale the source to fit the destination
		var fxScale = dxDst / dxSrc;
		var fyScale = dyDst / dySrc;
		return (bUseSmallerFactor ? Math.min(fxScale, fyScale) : Math.max(fxScale, fyScale));
	}

	this.Scale = Scale;
	function Scale(dxSrc, dxDst)
	{
		// Scale the source to fit the destination
		return dxDst / dxSrc;
	}

	this.Int = Int;
	function Int(str)
	{
		return str.replace('px','')/1;
	}

	this.Half = Half;
	function Half(val)
	{
		return Math.floor(val / 2);
	}
}
