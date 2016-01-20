	function EmbedPlugin(strVersion, strCpHost, strImgHost, strContext, strProduct)
	{
		var WIDTH = '100%';
		var HEIGHT = '100%';

		var str = '<object id="ctp" hspace="0" vspace="0" border="0" width="' + WIDTH + '" height="' + HEIGHT + '"';
		str += ' classid="clsid:38578BF0-0ABB-11D3-9330-0080C6F796A1"';
		str += ' codebase="' + strImgHost + '/cp/install/AxCtp.cab#version=' + strVersion + '">';
		str += '<param name="src"     value="' + strProduct + '">';
		str += '<param name="context" value="' + strContext + '">';
		str += '<iframe width="100%" height="100%" frameborder=0 src="' + strCpHost + '/noactivex.htm"></iframe>';
		str += '</object>';
		document.write(str);
	}

	function IconOnEx(strImgPath, IMGNAME)
	{
		var DIVID = 'd_' + IMGNAME;
		SetImage(DIVID, IMGNAME, strImgPath + IMGNAME + '.gif');
	}

	function IconOffEx(strImgPath, IMGNAME)
	{
		var DIVID = 'd_' + IMGNAME;
		SetImage(DIVID, IMGNAME, strImgPath + 'space.gif');
	}

	function SetImage(DIVID, IMGNAME, FILENAME)
	{
		if (document.images == null)
			return;

		var strCommand = 'document.images.';
		strCommand += IMGNAME + '.src';
		strCommand += ' = "' + FILENAME + '"';
		eval(strCommand);
	}

	function GetPlugin()
	{
		var nFrames = parent.frames.length;
		var doc = (nFrames > 0 ? parent.frames[nFrames-1].document : document);
		if (doc)
		{
			if (doc.ctp)
				return doc.ctp;
			if (doc.embeds && doc.embeds.ctp)
				return doc.embeds.ctp;
		}

		alert('Cannot access the Create & Print plugin');
		return null;
	}

	function GetCommand(strCommand)
	{
		var plugin = GetPlugin();
		return (plugin ? plugin.GetCommand(strCommand) : 0);
	}

	function OnCommand(strCommand)
	{
		var plugin = GetPlugin();
		if (plugin)
			plugin.DoCommand(strCommand);
	}

	function PreloadWorkspaceImages(strImgPath)
	{
		// List the images
		var ImageList = new Array (
			'bgleft.gif',
			'bgtop.gif',
			'bgtopleft.gif',
			'space.gif',

			'toolsleft.gif',
			'help.gif',
			'addphoto.gif',
			'addshape.gif',
			'addtextbox.gif',
			'zoomout.gif',
			'zoomin.gif',
			'transform.gif',
			'fliph.gif',
			'flipv.gif',
			'rotateccw.gif',
			'rotatecw.gif',
			'poscenterh.gif',
			'postopleft.gif',
			'postop.gif',
			'postopright.gif',
			'posleft.gif',
			'poscenter.gif',
			'posright.gif',
			'posbottomleft.gif',
			'posbottom.gif',
			'posbottomright.gif',
			'poscenterv.gif',
			'layerdown.gif',
			'layerback.gif',
			'layerfront.gif',
			'layerup.gif',
			'selectnext.gif',
			'selectprev.gif',

			'toolstop.gif',
			'print.gif',
			'open.gif',
			'close.gif',
			'saveas.gif',
			'undo.gif',
			'redo.gif',
			'cut.gif',
			'copy.gif',
			'paste.gif',
			'deletex.gif',
			'spellcheck.gif',
			'grid.gif',
			'extras.gif'
		);

		return LoadImageArray(strImgPath, ImageList);
	}
