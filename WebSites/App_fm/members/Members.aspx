<%--For ASP 2.0 Compiled pages, use CodeFile="", and "public partial class" in the .cs file --%>
<%--For ASP 1.1 Dynamic pages, use Src="", and "public class" in the .cs file --%>
<%@ Page Language="c#" Src="Members.aspx.cs" Inherits="Members" %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" > 

<html>
<body>
<h1>
Hello member, (EmailAddress=<%=Page.User.Identity.Name%>) (LoggedIn=<%=Page.User.Identity.IsAuthenticated%>)
</h1>
</body>
</html> 
