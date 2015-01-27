<%@ Page Language="C#" MasterPageFile="~/Main.Master" AutoEventWireup="true" CodeBehind="Resume.aspx.cs"
	Inherits="FaceToFace.Web.Resume" Title="Face To Face Software Credentials" %>

<asp:Content ID="Content1" ContentPlaceHolderID="x_HeadContent" runat="server">
	<meta name="description" content="Face To Face Software offers contract software development for the Web or the Desktop. Jim McCurdy operates Face to Face Software, where he works designing, developing, and managing software development projects for a variety of clients. Jim has worked in the software industry since 1977 and has broad expertise in Web, Windows, and systems technologies. He has been a team player on many agile development projects throughout his career, and is a founding member at software startups Astral Development and Powerhouse Entertainment. Jim currently specializes in development projects for Silverlight, .NET, ASP.NET, and WPF platforms. One such project is a complete web site using Silverlight, .NET, and C#; a unique financial and lifestyle planning web application at YinYangMoney.com. In the process of developing YinYangMoney, Jim has become expert in developing for the Silverlight platform. Over his career, Jim has gained unique experience in the application areas of Imaging, Video, Graphics, User Experience (UX), and Animation.  Jim's blog can be found at YinYangMe.com."></meta>
	<meta name="keywords" content="silverlight, consulting, contractor, software, development, application, Windows, C#, C++, imaging, video, graphics, user experience, ux, animation, project, management, yinyangmoney, yinyangme, Jim McCurdy, Andover, Massachusetts"></meta>
	<asp:Literal id="x_HeadContent" runat="server" />
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="x_BodyContent" runat="server">
	<div>
		<table width="100%">
			<tr>
				<td width="100%">
					<div class="div_content">
						<asp:Literal id="x_BodyContent" runat="server" />
					</div>
				</td>
			</tr>
		</table>
	</div>
</asp:Content>
