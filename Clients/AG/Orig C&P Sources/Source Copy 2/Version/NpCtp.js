
// Conditional alert.
function cAlert(message)
{
	if (!this.silent)
		alert(message);
}

// Variable indicating whether or not installation should proceed.
var bInstall = true;

// Make sure Java is enabled before doing anything else.
if (! navigator.javaEnabled())
{
	bInstall = false;
	cAlert("Java must be enabled to install.");
}

// Make sure installation is attempted on correct machine architecture.
else if (navigator.platform != "Win32")
{
	bInstall = false;
	cAlert("This plug-in only runs on Win32 platforms.");
}

// If all conditions look good, proceed with the installation.
if (bInstall)
{
	// Create a version object and a software update object
	var vi = new netscape.softupdate.VersionInfo(1, 0, 0, 0);
	var su = new netscape.softupdate.SoftwareUpdate(this, "American Greetings Create and Print Plug-in");

	// Initialize bAbort.
	var bAbort = false;

	// Start the install process
	var err = su.StartInstall("plugins/AmericanGreetings/", vi,
		netscape.softupdate.SoftwareUpdate.FULL_INSTALL);
	
	if (err != 0)
		bAbort = true;
	else
	{
		// Find the plug-ins directory on the user's machine
		PIFolder = su.GetFolder("Plugins");

		// Install the files. Unpack them and list where they go
		err = su.AddSubcomponent("CTP DLL", vi, "NpCtp.dll",
			PIFolder, "", this.force);
		
		bAbort = bAbort || (err != 0);
	}

	// Unless there was a problem, move files to final location
	// and update the Client Version Registry
	if (bAbort)
	{
		cAlert("Installation error. Aborting.");
		su.AbortInstall();
	}
	else
	{
		err = su.FinalizeInstall();
	
		// Refresh list of available plug-ins
		if (err == 0)
			navigator.plugins.refresh(true);
		else if (err == 999)
			cAlert("You must reboot to finish this installation.");
		else
			cAlert("Installation error. Aborting.");
	}
}
