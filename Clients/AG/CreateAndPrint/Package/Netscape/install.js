var strVersion = '3.0.0.0';

function ErrorMessage(bXPI, msg)
{
	if (bXPI)
		logComment(msg + '\n');
	else
	if (!this.silent)
		alert(msg);
}

function InstallPlugin()
{
	var packageDisplayName = 'AmericanGreetings.com Create & Print Plugin';
	var packageRegName = 'plugins/americangreetings/Ctp';
	var bXPI = (this.force == undefined ? true : false);
	var su = (bXPI ? null : new netscape.softupdate.SoftwareUpdate(this, packageDisplayName));
	var version = (bXPI ? strVersion : new netscape.softupdate.VersionInfo(strVersion));
	var err = (bXPI ? 
		initInstall(packageDisplayName, packageRegName, version) :
		su.StartInstall(packageRegName, version, netscape.softupdate.SoftwareUpdate.FULL_INSTALL));

	if (err)
	{
		ErrorMessage(bXPI, 'StartInstall error ' + err);
		(bXPI ? cancelInstall(err) : su.AbortInstall());
		return false;
	}

	var pluginsFolder = (bXPI ? getFolder('Plugins') : su.GetFolder('Plugins'));
	var jarSourcePath = 'PluginDir';
	if (bXPI)
	{
		err = addDirectory(
			'',				// registryName
			version,		// version
			jarSourcePath,	// dir name in jar to extract
			pluginsFolder,	// Where to put the files locally
			'',				// subdir name to create relative to pluginsFolder
			true);			// Force Flag
	}
	else
	{
		err = su.AddDirectory(
			'',				// registryName
			version,		// version
			jarSourcePath,	// dir name in jar to extract
			pluginsFolder,	// Where to put the files locally
			'',				// subdir name to create relative to pluginsFolder
			this.force);	// Force Flag
	}

	if (err)
	{
		ErrorMessage(bXPI, 'AddDirectory error ' + err);
		(bXPI ? cancelInstall(err) : su.AbortInstall());
		return false;
	}

	err = (bXPI ? performInstall() : su.FinalizeInstall());
	(bXPI ? refreshPlugins() : navigator.plugins.refresh(false));
	if (err)
	{
		if (err == 999)
			ErrorMessage(bXPI, 'You must restart your browser in order to complete the installation of the Create & Print plugin for Netscape');
		else
		{
			ErrorMessage(bXPI, 'FinalizeInstall error ' + err);
			return false;
		}
	}

	return true;
}

InstallPlugin();
