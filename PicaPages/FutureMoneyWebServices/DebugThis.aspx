<%@ Page Language="C#" %>
<script language="C#" runat=server>

void Page_Load(Object sender , EventArgs e) 
{
	int zero = 0;
	int intCounter = 0;
	for (intCounter = 0; intCounter < 11; intCounter++)
		lblCount.Text = intCounter.ToString();
	intCounter /= zero;
}

</Script>

<html>
<head><title>DebugThis.aspx</title></head>
<body>

<asp:Label
  id="lblCount"
  Runat="Server" />

</body>
</html>