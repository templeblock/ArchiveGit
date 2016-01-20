<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1" omit-xml-declaration="yes" /> 
	<xsl:variable name="drawicons" select="'0'" />
	<xsl:variable name="filename" select="'Thumbnails'" />
	<xsl:variable name="folder-indent" select="'12'" />
	<xsl:variable name="line-height" select="'14'" />
	<xsl:variable name="pad-filename" select="'20'" />
	<xsl:variable name="pad-iconplus" select="'0'" />
	<xsl:variable name="pad-icon" select="'5'" />
	<xsl:variable name="img-folder" select="'images'" />
	<xsl:variable name="thumbslist-path" select="'paths/'" />

	<xsl:template match="/tree">
		<html lang="en" xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
			<head>
				<title><xsl:value-of select="@title" /></title>
				<style type="text/css">
					body
						{ font-family:Verdana,sans-serif; font-size:8pt; margin:0px; border:6px solid #cccccc; overflow:hidden; }
					body, div
						{ background-color:#ffffff; }
					img
						{ border-width:0; }
					div, span, a, td
						{ white-space:nowrap; }
					.clickable
						{ cursor:hand; }
					.item
						{ height:16; line-height:<xsl:value-of select="$line-height" />px; padding:0px; cursor:hand; color:#1A4C8F; text-decoration:none; }
					.itemhover
						{ height:16; line-height:<xsl:value-of select="$line-height" />px; padding:0px; cursor:hand; color:#1A4C8F; text-decoration:underline; }
					.iconplus
						{ width:16; height:16; margin-right:<xsl:value-of select="$pad-iconplus" />px; }
					.icon
						{ width:16; height:16; margin-right:<xsl:value-of select="$pad-icon" />px; }
				</style>
		        <script language="javascript" src="navigation.js"></script>
				<script language="javascript">
					var LastActiveFolderNode = null;
					var DefaultBGColor = -1;
					function OnLoad()
					{
						RemoveFocus();
					}
					function Over(Node)
					{
						Node.className = 'itemhover';
					}
					function Out(Node)
					{
						Node.className = 'item';
					}
					function OnFolder(LinkNode)
					{
						var FolderNode = GetFolderFromLink(LinkNode);
						ToggleNode(FolderNode);
						SetActiveFolder(FolderNode);
						AutoClick(FolderNode);
					}
					function OnFile(LinkNode, strName)
					{
						OnAutoFile(LastActiveFolderNode, strName);
					}
					function OnAutoFile(FolderNode, strName)
					{
						var strCategory	= BuildFolderPath(FolderNode);
						SetCategoryLabel(strCategory);
						var strUrl = '<xsl:value-of select="$thumbslist-path" />' + strName + '.xml';
						ThumbsSetUrl(strUrl);
					}
					function OnSetCategory(strPath)
					{ // Set the category when using Back and Forward
						var FolderNode = document.body;
						var iLast = 0;
						var i = 0;
						var strSeparator = GetSeparator();
						while ((i = strPath.indexOf(strSeparator, iLast)) != -1)
						{
							var strCategory = strPath.substring(iLast, i);
							iLast = i + strSeparator.length;
							FolderNode = FindNode(FolderNode, strCategory);
							if (FolderNode)
								ExpandNode(FolderNode);
						}

						if (FolderNode)
							SetActiveFolder(FolderNode);
					}
					function FindNode(RootNode, name)
					{
						if (!RootNode)
							return null;

						for (var i = 0; i != RootNode.childNodes.length; i++)
						{
							var Node = RootNode.childNodes(i);
							if (Node.id == 'folder' || Node.id == 'folderx')
							{
								if (Node.className == name)
									return Node;
							}
							if (Node.childNodes.length)
							{
								var NewNode = FindNode(Node, name);
								if (NewNode)
									return NewNode;
							}
						}

						return null;
					}
					function SetActiveFolder(FolderNode)
					{
						HiliteNode(LastActiveFolderNode, false);
						HiliteNode(FolderNode, true);
					//j	Uncomment the following line if you want the previously expanded node to be collapsed first
						CollapsePrevious(LastActiveFolderNode, FolderNode);
						LastActiveFolderNode = FolderNode;
					}
					function HiliteNode(FolderNode, bOn)
					{
						if (!FolderNode)
							return;

						var bIsExpanded = (FolderNode.style.display != 'none');
						var Node = FolderNode.parentNode;
						if (DefaultBGColor == -1)
						{
							if (!bOn)
								return;
							DefaultBGColor = Node.style.backgroundColor;
						}

						Node.style.backgroundColor = (!bOn ? DefaultBGColor : '#e0e0e0');
					}
					function BuildFolderPath(FolderNode)
					{
						var strPath = '';
						var strSeparator = GetSeparator();
						while (FolderNode)
						{
							if (FolderNode.id == 'folder' || FolderNode.id == 'folderx')
								strPath = FolderNode.className + strSeparator + strPath;
							if (FolderNode.id == 'file')
								strPath = '<xsl:value-of select="$filename" />' + strPath;
							FolderNode = FolderNode.parentNode;
						}
						
						return strPath;
					}
					function IsParent(TargetNode, FolderNode)
					{
						while (FolderNode)
						{
							if (FolderNode == TargetNode)
								return true;
							FolderNode = FolderNode.parentNode;
						}

						return false;
					}
					function CollapsePrevious(FolderNode, newFolderNode)
					{
						while (FolderNode)
						{
							if (IsParent(FolderNode, newFolderNode))
								break;

							CollapseNode(FolderNode);
							FolderNode = FolderNode.parentNode;
						}
					}
					function CollapseNode(FolderNode)
					{
						if (!FolderNode || FolderNode.id != 'folder')
							return;

						var bCollapsed = (FolderNode.style.display == 'none');
						if (bCollapsed)
							return;

						ToggleNode(FolderNode);
					}
					function ExpandNode(FolderNode)
					{
						if (!FolderNode || FolderNode.id != 'folder')
							return;

						var bCollapsed = (FolderNode.style.display == 'none');
						if (!bCollapsed)
							return;

						ToggleNode(FolderNode);
					}
					function ToggleNode(FolderNode)
					{
						if (!FolderNode || FolderNode.id != 'folder')
							return;

						var bCollapsed = (FolderNode.style.display == 'none');
						FolderNode.style.display = (bCollapsed ? 'block' : 'none');
						bCollapsed = !bCollapsed;

						var LinkNode = GetLinkFromFolder(FolderNode);
						if (!LinkNode)
							return;

						if (LinkNode.childNodes.length)
						{
							var object = LinkNode.childNodes.item(0);
							if (object.nodeName == 'IMG')
								object.src = GetPath(object.src) + (bCollapsed ? 'plus.gif' : 'minus.gif');
						}
					}
					function AutoClick(FolderNode)
					{
						if (!FolderNode)
							return;

						var FileNode = FolderNode.childNodes.item(0);
						if (!FileNode || FileNode.id != 'file')
							return;

						OnAutoFile(FolderNode, FileNode.className);
					}
					function GetFolderFromLink(LinkNode)
					{
						if (!LinkNode)
							return null;

						var FolderNode = LinkNode.nextSibling;
						while (FolderNode)
						{
							if (FolderNode.id == 'folder' || FolderNode.id == 'folderx')
								return FolderNode;

							FolderNode = FolderNode.nextSibling;
						}

						return null;
					}
					function GetLinkFromFolder(FolderNode)
					{
						if (!FolderNode)
							return null;

						var LinkNode = FolderNode.parentNode.firstChild;
						if (!LinkNode)
							return null;

						while (LinkNode)
						{
							if (LinkNode.tagName == 'SPAN')
								return LinkNode;
							LinkNode = LinkNode.nextSibling;
						}

						return null;
					}
				</script>
			</head>
			<body onload="OnLoad()">
			<div style="background-color:#999999; width:190px; height:20px; padding:3px; color:#ffffff; font-weight:bold;">Projects</div>
			<div style="border:3px solid #eaeaea; height:100%;">
			<div>
				<xsl:apply-templates select="folder">
					<xsl:sort select="@name" />
					<xsl:with-param name="depth" select="0" />
				</xsl:apply-templates>
			</div>
				</div>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="folder">
		<xsl:param name="depth" />
		<div>
			<xsl:variable name="indent" select="$depth * $folder-indent" />
			<span class="clickable" onclick="OnFolder(this)" style="margin-left:{$indent}; ">
				<xsl:if test="@type='folder'">
					<img class="iconplus" src="{$img-folder}/plus.gif" />
				</xsl:if>
				<xsl:if test="@type='folderx'">
					<img class="iconplus" src="{$img-folder}/space.gif" />
				</xsl:if>
				<xsl:if test="$drawicons='1'">
					<img class="icon" src="{$img-folder}/{@type}.gif" />
				</xsl:if>
				<span class="item" onmouseover="Over(this)" onmouseout="Out(this)"><xsl:value-of select="@name" /></span>
			</span>
			<div id="{@type}" class="{@name}" style="display:none;">
				<xsl:apply-templates select="folder">
					<xsl:sort select="@name" />
					<xsl:with-param name="depth" select="$depth+1" />
				</xsl:apply-templates>
				<xsl:apply-templates select="file">
					<xsl:sort select="@name" />
					<xsl:with-param name="depth" select="$depth" />
				</xsl:apply-templates>
			</div>
		</div>
	</xsl:template>
	  
	<xsl:template match="file">
		<xsl:param name="depth" />
		<div id="{@type}" class="{@name}">
			<xsl:variable name="indent" select="($depth * $folder-indent) + $pad-filename" />
			<span class="clickable" onclick="OnFile(this, '{@name}')" style="margin-left:{$indent}; ">
				<xsl:if test="$drawicons='1'">
					<img class="icon" src="{$img-folder}/{@type}.gif" />
				</xsl:if>
				<span class="item" onmouseover="Over(this)" onmouseout="Out(this)"><xsl:value-of select="$filename" /></span>
			</span>
		</div>
	</xsl:template>
	  
</xsl:stylesheet>
