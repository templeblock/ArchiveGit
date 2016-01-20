import netscape.plugin.Plugin;
//j public class NpCtp extends netscape.plugin.Plugin
public class NpCtp extends Plugin
{
	// properties

	// methods
	public native void PutSrc(java.lang.String value);
	public native java.lang.String GetSrc();
	public native void PutContext(java.lang.String value);
	public native java.lang.String GetContext();
	public native int DoCommand(java.lang.String strCommand);
	public native int GetCommand(java.lang.String strCommand);
	public native void FileDownload(java.lang.String strCommand);
}
