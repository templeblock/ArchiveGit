<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ManageMembers.aspx.cs" Inherits="TrumpBubbles.Web.ManageMembers" %>
<%@ Register TagPrefix="telerik" Namespace="Telerik.Web.UI" Assembly="Telerik.Web.UI" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
     <asp:ScriptManager ID="scriptManager" runat="server">
                <Scripts>
                    <asp:ScriptReference Path="~/includes/js/jquery-1.3.2.min.js" />
                    <asp:ScriptReference Path="~/includes/js/toggleReportLegend.js" />
                </Scripts>
            </asp:ScriptManager>
    <div style="height:500px; min-height:500px;">
    Manage Members!
    <telerik:RadComboBox ID="ddlProduct" runat="server" Width="300px">
        <Items>
            <telerik:RadComboBoxItem Text="Test" Value="1" />
        </Items>
    </telerik:RadComboBox>
    </div>
    </form>
</body>
</html>
