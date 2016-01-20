<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1" omit-xml-declaration="yes" /> 
	<xsl:include href="variables.xsl"/>
	<!--xsl:variable name="product-path" select="''" /-->
	<!--xsl:variable name="application-path" select="''" /-->

	<xsl:template match="/thumbs">
		<html lang="en" xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
			<head>
				<title><xsl:value-of select="@title" /></title>
				<style type="text/css">
					body
						{ font-family:Verdana,sans-serif; font-size:8pt; margin:0px; padding:3px; }
					img
						{ border-width: 0px; }
					.thumb
						{ background-color:#E2E7EE; margin:3px; padding:6px; width:180px; height:194px; float:left; text-align:center; }
					.label
						{ font-family:Verdana,sans-serif; font-size:11px; font-weight: bold; color:#1A4C8F;  margin:10px; }
				</style>
		        <script language="javascript" src="{$application-path}navigation.js"></script>
				<script language="javascript">
					function OnLoad()
					{
						RemoveFocus();
					}
					function OnThumb(product, pw, ph, name, verse)
					{
						var amp = ampersand();
						var strOptions = '?';
						strOptions += 'name=' + name + amp;
						strOptions += 'product=' + product + amp;
						strOptions += 'pw=' + pw + amp;
						strOptions += 'ph=' + ph + amp;
						strOptions += 'apppath=' + '<xsl:value-of select="$application-path" />' + amp;
						strOptions += 'productpath=' + '<xsl:value-of select="$product-path" />' + amp;
						strOptions += 'verse=' + verse + amp;
						var strUrl = '<xsl:value-of select="$application-path" />preview.htm' + strOptions;
						ThumbsSetUrl(strUrl);
					}
				</script>
			</head>
			<body onload="OnLoad()">
				<div class="label">
					<span>Click on a project to display its preview image.</span>
				</div>
				<div>
					<xsl:apply-templates select="thumb">
						<xsl:sort select="@seq" order='ascending' data-type='number' />
					</xsl:apply-templates>
				</div>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="thumb">
		<div class="thumb">
			<a href="#" onclick="var name = Encode64(unescape(&quot;{@name}&quot;));var verse = Encode64(unescape(&quot;{@verse}&quot;));OnThumb('{@product}', '{@pw}', '{@ph}', name, verse);">
				<img src="{$product-path}{@product}/{@product}t.gif" width="{@tw}" height="{@th}" alt="{@name}" ></img>
			</a>
			<br />
			<script language="javascript">document.write(unescape("<xsl:value-of select="@name" />"));</script>
		</div>
	</xsl:template>
	  
</xsl:stylesheet>
