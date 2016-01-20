using System;
using System.Collections;
using System.ComponentModel;
using System.Web;
using System.Web.SessionState;
using System.IO;
using System.Data;
using System.Data.Odbc;

/// <summary>
/// Summary description for Global.
/// </summary>
public partial class MyApplicationBase : System.Web.HttpApplication
{
	/// <summary>
	/// Required designer variable.
	/// </summary>
	private System.ComponentModel.IContainer components = null;

	public MyApplicationBase()
	{
		InitializeComponent();
	}	
	
	protected void Application_Start(Object sender, EventArgs e)
	{
		Application["MyValue"] = 1;
	}

	protected void Session_Start(Object sender, EventArgs e)
	{
		Session["BackColor"] = "beige";
		Session["ForeColor"] = "black";
		Session["LinkColor"] = "blue";
		Session["FontSize"] = "8pt";
		Session["FontName"] = "verdana";
	}

	protected void Application_BeginRequest(Object sender, EventArgs e)
	{

	}

	protected void Application_EndRequest(Object sender, EventArgs e)
	{

	}

	protected void Application_AuthenticateRequest(Object sender, EventArgs e)
	{

	}

	protected void Application_Error(Object sender, EventArgs e)
	{
	}

	protected void Session_End(Object sender, EventArgs e)
	{

	}

	protected void Application_End(Object sender, EventArgs e)
	{

	}
}
