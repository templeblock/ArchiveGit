<?xml version="1.0" encoding="iso-8859-1" ?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1" omit-xml-declaration="yes" /> 
	<xsl:variable name="g_ahost" select="'http://www.americangreetings.com'" />
	<xsl:variable name="g_ahostname" select="'AmericanGreetings.com'" />

	<xsl:template match="/topics">
		<html lang="en" xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
			<head>
				<title>Workspace Help</title>
				<style type="text/css">
					body {font-family: Verdana; font-size: 10pt; color: black; background-color: white;}
					h1 {font-family: Verdana; font-size: 16pt; color: blue;}
					h2 {font-family: Verdana; font-size: 14pt; color: red;}
					h3 {font-family: Verdana; font-size: 12pt; color: green;}
					a:link {text-decoration: none; color: #000080;}
					a:visited {text-decoration: none; color: #000080;}
					a:active {text-decoration: none; color: #000080;}
					a:hover {text-decoration: underline; color: #FF4400;}
				</style>
				<script language="JavaScript" src="cphelp.js"></script>
				<script language="JavaScript">
					var strContext = GetContext();
					var strTopic = GetOption(strContext, 'topic');
					if (strTopic == null <![CDATA[||]]> strTopic == '') strTopic = '0';
					SetTopic(strTopic, ''/*strBackTopic*/);

					function OnLoad()
					{
						GoToTopic(strTopic);
					}
				</script>
			</head> 

			<body onload="OnLoad();" marginheight="0" marginwidth="0" topmargin="10" leftmargin="10">
				<xsl:apply-templates select="topic">
				</xsl:apply-templates>
			</body>
		</html>
	</xsl:template>

	<xsl:template name="replace">
		<xsl:param name="strIn"/>
		<xsl:param name="charsIn"/>
		<xsl:param name="charsOut"/>
		<xsl:choose>
			<xsl:when test="contains($strIn, $charsIn)">
				<xsl:copy-of select="concat(substring-before($strIn, $charsIn), $charsOut)"/>
				<xsl:call-template name="replace">
					<xsl:with-param name="strIn" select="substring-after($strIn, $charsIn)"/>
					<xsl:with-param name="charsIn" select="$charsIn"/>
					<xsl:with-param name="charsOut" select="$charsOut"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy-of select="$strIn"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="topic">
		<xsl:variable name="strContentOut">
			<xsl:call-template name="replace">
				<xsl:with-param name="strIn" select="content"/>
				<xsl:with-param name="charsIn" select="'g_ahostname'"/>
				<xsl:with-param name="charsOut" select="$g_ahostname"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="strContentOut2">
			<xsl:call-template name="replace">
				<xsl:with-param name="strIn" select="$strContentOut"/>
				<xsl:with-param name="charsIn" select="'g_ahost'"/>
				<xsl:with-param name="charsOut" select="$g_ahost"/>
			</xsl:call-template>
		</xsl:variable>
		<div id="{@name}" prev="{@prev}" next="{@next}" style="visibility:hidden; display:none; position:relative">
			<h1><xsl:value-of select="@title" /></h1>
			<div><xsl:copy-of select="content" /></div>
		</div>
	</xsl:template>
	  
</xsl:stylesheet>
